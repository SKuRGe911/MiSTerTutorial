///////////////////////////////////////////////////////////////////////
// GERANIUM TUTORIAL CORE FOR MISTER											//
///////////////////////////////////////////////////////////////////////

module emu
(
	// MASTER INPUT CLOCK
	input		wire															CLK_50M,

	// ASYNC RESET FROM TOP-LEVEL MODULE
	// CAN BE USED AS INITIAL RESET
	input		wire															RESET,

	// MUST BE PASSED TO HPS_IO MODULE
	inout		wire			[45:0]										HPS_BUS,

	// BASE VIDEO CLOCK USUALLY EQUALS TO CLK_CPUTEM
	output	wire															CLK_VIDEO,

	// MULTIPLE RESOLUTIONS ARE SUPPORTED USING DIFFERENT CE_PIXEL RATES
	// MUST BE BASED ON CLK_VIDEO
	output	wire															CE_PIXEL,

	// ADC
	inout		wire			[3:0]											ADC_BUS,

	// VIDEO ASPECT RATIO FOR HDMI MOST RETRO SYSTEMS HAVE RATIO 4:3
	output	wire			[7:0]											VIDEO_ARX,
	output	wire			[7:0]											VIDEO_ARY,

	// VIDEO OUTPUT
	output	wire			[7:0]											VGA_R,
	output	wire			[7:0]											VGA_G,
	output	wire			[7:0]											VGA_B,
	output   wire     													VGA_HS,
	output   wire     													VGA_VS,
	output   wire     													VGA_DE,    // = ~(VBlank | HBlank)
	output   wire     													VGA_F1,
	output	wire			[1:0]											VGA_SL,

	// LED OUTPUT
	output	wire															LED_USER,  // 1 - ON, 0 - OFF.
	output	wire			[1:0]											LED_POWER,
	output	wire			[1:0]											LED_DISK,

	// I/O board button press simulation (active high)
	// b[1]: user button
	// b[0]: osd button
	output	wire			[1:0]											BUTTONS,
	
	// AUDIO OUTPUT
	output	wire			[15:0]										AUDIO_L,
	output	wire			[15:0]										AUDIO_R,
	output	wire															AUDIO_S, // 1 - signed audio samples, 0 - unsigned
	output	wire			[1:0]											AUDIO_MIX, // 0 - no mix, 1 - 25%, 2 - 50%, 3 - 100% (mono)
	
	// TAPE INPUT
	input		wire															TAPE_IN,

	// SD-SPI
	output	wire															SD_SCK,
	output	wire															SD_MOSI,
	input		wire															SD_MISO,
	output	wire															SD_CS,
	input		wire															SD_CD,

	// HIGH LATENCY DDR3 RAM INTERFACE
	// USE FOR NON-CRITICAL TIME PURPOSES
	output	wire															DDRAM_CLK,
	input		wire															DDRAM_BUSY,
	output	wire			[7:0]											DDRAM_BURSTCNT,
	output	wire			[28:0]										DDRAM_ADDR,
	input		wire			[63:0]										DDRAM_DOUT,
	input		wire															DDRAM_DOUT_READY,
	output	wire															DDRAM_RD,
	output	wire			[63:0]										DDRAM_DIN,
	output	wire			[7:0]											DDRAM_BE,
	output	wire															DDRAM_WE,

	// SDRAM INTERFACE WITH LOWER LATENCY
	output	wire															SDRAM_CLK,
	output	wire															SDRAM_CKE,
	output	wire			[12:0]										SDRAM_A,
	output	wire			[1:0]											SDRAM_BA,
	inout		wire			[15:0]										SDRAM_DQ,
	output	wire															SDRAM_DQML,
	output	wire															SDRAM_DQMH,
	output	wire															SDRAM_nCS,
	output	wire															SDRAM_nCAS,
	output	wire															SDRAM_nRAS,
	output	wire															SDRAM_nWE,

	// UART
	input		wire															UART_CTS,
	output	wire															UART_RTS,
	input		wire															UART_RXD,
	output	wire															UART_TXD,
	output	wire															UART_DTR,
	input		wire															UART_DSR,

	// OPEN-DRAIN USER PORT
	// SET USER_OUT TO 1 TO READ FROM USER_IN
	input		wire			[5:0]											USER_IN,
	output	wire			[5:0]											USER_OUT,

	// ON SCREEN DISPLAY STATUS
	input		wire															OSD_STATUS

);

///////////////////////////////////////////////////////////////////////
// VERSION INCLUDE																	//
///////////////////////////////////////////////////////////////////////

`include "build_id.v"

///////////////////////////////////////////////////////////////////////
// TOP LEVEL ASSIGNMENTS															//
// THESE ASSIGNMENTS ARE FOR THE BASIC SYSTEM INTERFACE					//
///////////////////////////////////////////////////////////////////////

assign									USER_OUT							=		'1;
assign									UART_RTS							=		1'b0;
assign									UART_TXD							=		1'b0;
assign									UART_DTR							=		1'b0;

assign									LED_USER							=		led_a;
assign									LED_DISK							=		{ 1'b1 , led_b };
assign									LED_POWER						=		{ 1'b1 , led_c };

assign									BUTTONS							=		2'b00;

assign									VIDEO_ARX						=		status[10] ? 8'd16 : 8'd4;
assign									VIDEO_ARY						=		status[10] ? 8'd9  : 8'd3;

assign									ADC_BUS							=		'Z;

assign									SD_SCK							=		1'bZ;
assign									SD_MOSI							=		1'bZ;
assign									SD_CS								=		1'bZ;

assign									VGA_F1							=		1'b0;
assign									VGA_SL							=		2'b00;

///////////////////////////////////////////////////////////////////////
// CORE CONFIGURATION STRING														//
// THIS STRING IS SHOWN IN THE MISTER MENU WHEN F12 IS PRESSED			//
///////////////////////////////////////////////////////////////////////

parameter
CONF_STR = {
	"GERANIUM;;",
	"-;",
	"OA,Aspect ratio,4:3,16:9;",
	"OBC,Scan Lines,Off,%25,%50,%75;",
	"R0,Reset;",
	"-;",
	"V,v",`BUILD_DATE
};

///////////////////////////////////////////////////////////////////////
// RESET AND CLOCK VARIABLES														//
///////////////////////////////////////////////////////////////////////

wire										pll_reset;																		// PHASE LOCKED LOOP RESET
wire										clock_locked;																	// PHASE LOCKED LOOP STABLE
wire										clk_video;																		// VIDEO CLOCK
wire										hardware_reset;																// A SYSTEM RESET SIGNAL

///////////////////////////////////////////////////////////////////////
// SYSTEM VARIABLES																	//
///////////////////////////////////////////////////////////////////////

wire										led_a;													// USER LED
wire										led_b;													// DISK LED
wire										led_c;													// POWER LED

wire		[1:0]							scanlines;												// VIDEO SCAN LINES

///////////////////////////////////////////////////////////////////////
// HPSIO																					//
// THIS IS THE MISTER HARD PROCESSOR SYSTEM IO INTERFACE STUFF			//
///////////////////////////////////////////////////////////////////////

wire		[1:0]							buttons;
wire		[63:0]						status;
wire										forced_scandoubler;

wire		[31:0]						sd_lba;
wire										sd_rd;
wire										sd_wr;
wire										sd_ack;
wire		[7:0]							sd_buff_addr;
wire		[15:0]						sd_buff_dout;
wire		[15:0]						sd_buff_din;
wire										sd_buff_wr;

wire										img_mounted;
wire										img_readonly;
wire		[63:0]						img_size;

wire		[7:0]							ioctl_index;
wire		[31:0]						ioctl_file_ext;
wire										ioctl_download;
wire		[24:0]						ioctl_addr;
wire		[15:0]						ioctl_dout;
wire										ioctl_wr;
wire										ioctl_wait;
logic										ioctl_wait_reset									=			1'b0;
logic										ioctl_wait_download								=			1'b0;
logic										ioctl_reset											=			1'b1;

wire		[31:0]						joy0;
wire		[31:0]						joy1;
wire		[31:0]						joy2;
wire		[31:0]						joy3;
wire		[31:0]						joy4;
wire		[31:0]						joy5;

wire		[15:0]						analog_joy0;
wire		[15:0]						analog_joy1;
wire		[15:0]						analog_joy2;
wire		[15:0]						analog_joy3;
wire		[15:0]						analog_joy4;
wire		[15:0]						analog_joy5;

wire		[24:0]						ps2_mouse;
wire		[10:0]						ps2_key;

logic										new_vmode											=			1'b0;

wire		[63:0]						real_time_clock;
wire		[31:0]						time_stamp;

///////////////////////////////////////////////////////////////////////
// VIDEO																					//
// VARIABLES TO USE ON THE VGA SIGNAL GENERATION OBJECT					//
///////////////////////////////////////////////////////////////////////

wire		[7:0]							R;
wire		[7:0]							G;
wire		[7:0]							B;
wire										HSync;
wire										VSync;
wire										HBlank;
wire										VBlank;

///////////////////////////////////////////////////////////////////////
// SYSTEM VARIABLE ASSIGNMENTS													//
///////////////////////////////////////////////////////////////////////

assign									pll_reset						=		1'b0;
assign									scanlines						=		status[12:11];
assign									hardware_reset					=		RESET | buttons[1] | status[0] | ~clock_locked;
assign									CLK_VIDEO						=		clk_video;
assign									led_a								=		1'b0;
assign									led_b								=		1'b0;
assign									led_c								=		1'b0;

///////////////////////////////////////////////////////////////////////
// PHASE LOCKED LOOP CLOCKS														//
// THE PHASE LOCKED LOOP OBJECT GENERATES CLOCKS FOR THE SYSTEM		//
// BASED ON THE 50MHZ MASTER CLOCK												//
///////////////////////////////////////////////////////////////////////

pll
pll
(
	.refclk									(		CLK_50M												),			// 50MHz SOURCE CLOCK
	.rst										(		pll_reset											),			// A LINE TO RESET THE PLL WHICH IS JUST SET LOW FOR THIS PROJECT
	.outclk_0								(		clk_video											),			// 25.175MHz FOR STANDARD 640x480 VGA SYSTEM
	.locked									(		clock_locked										)			// GOES HIGH WHEN THE PHASE LOCKED LOOP IS STABLE
	
);

///////////////////////////////////////////////////////////////////////
// HPSIO																					//
// THIS IS THE HARD PROCESSOR IO OBJECT WHICH IS THERE TO INTERFACE	//
// WITH THE SPECIFIC MISTER STUFF												//
///////////////////////////////////////////////////////////////////////

hps_io
#(
	.STRLEN									(		$size(CONF_STR)>>3								),
	.WIDE										(		1														)
)
hps_io
(
	.clk_sys									(		CLK_50M												),
	.HPS_BUS									(		HPS_BUS												),
	.conf_str								(		CONF_STR												),

	.buttons									(		buttons												),
	.forced_scandoubler					(		forced_scandoubler								),
	.new_vmode								(		new_vmode											),

	.joystick_0								(		joy0													),
	.joystick_1								(		joy1													),
	.joystick_2								(		joy2													),
	.joystick_3								(		joy3													),
	.joystick_4								(		joy4													),
	.joystick_5								(		joy5													),
	.joystick_analog_0					(		analog_joy0											),
	.joystick_analog_1					(		analog_joy1											),
	.joystick_analog_2					(		analog_joy2											),
	.joystick_analog_3					(		analog_joy3											),
	.joystick_analog_4					(		analog_joy4											),
	.joystick_analog_5					(		analog_joy5											),
	
	.ps2_kbd_clk_out						(																),
	.ps2_kbd_data_out						(																),
	.ps2_kbd_clk_in						(																),
	.ps2_kbd_data_in						(																),
	.ps2_kbd_led_status					(																),
	.ps2_kbd_led_use						(																),
	
	.ps2_mouse_clk_out					(																),
	.ps2_mouse_data_out					(																),
	.ps2_mouse_clk_in						(																),
	.ps2_mouse_data_in					(																),
	
	.ps2_key									(		ps2_key												),
	.ps2_mouse								(		ps2_mouse											),

	.status									(		status												),

	.ioctl_index							(		ioctl_index											),
	.ioctl_file_ext						(		ioctl_file_ext										),
	.ioctl_addr								(		ioctl_addr											),
	.ioctl_dout								(		ioctl_dout											),
	.ioctl_wr								(		ioctl_wr												),
	.ioctl_download						(		ioctl_download										),
	.ioctl_wait								(		ioctl_wait											),

	.sd_lba									(		sd_lba												),
	.sd_rd									(		sd_rd													),
	.sd_wr									(		sd_wr													),
	.sd_ack									(		sd_ack												),
	.sd_buff_addr							(		sd_buff_addr										),
	.sd_buff_dout							(		sd_buff_dout										),
	.sd_buff_din							(		sd_buff_din											),
	.sd_buff_wr								(		sd_buff_wr											),

	.img_mounted							(		img_mounted											),
	.img_readonly							(		img_readonly										),
	.img_size								(		img_size												),
	
	.RTC										(		real_time_clock									),
	.TIMESTAMP								(		time_stamp											),
	
	.uart_mode								(		16'd0													)

);

///////////////////////////////////////////////////////////////////////
// VIDEO MIXER																			//
// A PART OF THE HPSIO SYSTEM WHICH HANDLES SOME DISPLAY STUFF			//
///////////////////////////////////////////////////////////////////////

`ifdef REAL_BUILD // THIS DEFINITION IS USED IN FUTURE TUTORIALS WHEN WE NEED TO SIMULATE THINGS

video_mixer
#(
	.LINE_LENGTH							(		800													),
	.HALF_DEPTH								(		0														)
)
video_mixer
(
	.clk_vid									(		clk_video											),
	.ce_pix_out								(		CE_PIXEL												),
	.ce_pix									(		1'b1													),

	.scandoubler							(		1'b0													),
	.scanlines								(		scanlines											),
	.hq2x										(		1'b0													),
	.mono										(		1'b0													),
	.gamma_bus								(																),

	.R											(		R														),
	.G											(		G														),
	.B											(		B														),

	.HSync									(		HSync													),
	.VSync									(		VSync													),
	.HBlank									(		HBlank												),
	.VBlank									(		VBlank												),

	.VGA_R									(		VGA_R													),
	.VGA_G									(		VGA_G													),
	.VGA_B									(		VGA_B													),
	.VGA_VS									(		VGA_VS												),
	.VGA_HS									(		VGA_HS												),
	.VGA_DE									(		VGA_DE												)

);

`endif

///////////////////////////////////////////////////////////////////////
// VIDEO																					//
// THIS IS THE INSTANCE OF OUR VIDEO GENERATION MODULE					//
///////////////////////////////////////////////////////////////////////

video
video
(
	.reset									(		reset													),
	.video_clock							(		clk_video											),
	
	.red										(		R														),
	.green									(		G														),
	.blue										(		B														),
	.hsync									(		HSync													),
	.vsync									(		VSync													),
	.hblank									(		HBlank												),
	.vblank									(		VBlank												)
	
);

endmodule
