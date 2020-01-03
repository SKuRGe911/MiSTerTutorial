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
	"F,ROM,Load Rom;",
	"S,VHDIMGIMARAW,Mount Disk;",
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
wire										system_clock;																	// A 100MHZ SYSTEM CLOCK
wire										video_clock;																	// VIDEO CLOCK
wire										hardware_reset;																// A SYSTEM RESET SIGNAL
logic		[7:0]							system_reset_count								=			8'd0;			// RESET COUNT FOR AFTER DOWNLOAD OF ROM
wire										system_reset;																	// RESET GOES LOW WHEN RESET COUNT IS 255
logic		[7:0]							download_reset_count								=			8'd0;			// DOWNLOAD COUNT FOR AFTER SYSTEM RESET OR INITIALIZATION
wire										download_reset;																// RESET GOES LOW WHEN RESET COUNT IS 255
logic										download_reset_done								=			1'b0;			// SET WHEN PROCESS OF RESET IS FINISHED

///////////////////////////////////////////////////////////////////////
// SYSTEM VARIABLES																	//
///////////////////////////////////////////////////////////////////////

wire										led_a;													// USER LED
wire										led_b;													// DISK LED
wire										led_c;													// POWER LED

///////////////////////////////////////////////////////////////////////
// ROM DOWNLOAD																		//
///////////////////////////////////////////////////////////////////////

logic		[2:0]							ioctl_wr_reg										=			3'b000;
logic		[15:0]						ioctl_data_a										=			16'd0;
logic		[15:0]						ioctl_data_b										=			16'd0;
logic		[26:0]						ioctl_addr_a										=			27'd0;
logic		[26:0]						ioctl_addr_b										=			27'd0;

logic										rom_download										=			1'b0;
logic		[31:0]						rom_download_data									=			32'd0;
logic		[26:0]						rom_download_addr									=			27'd0;
logic										rom_download_write								=			1'b0;
logic										rom_download_count								=			1'b0;
wire										rom_download_ack;

///////////////////////////////////////////////////////////////////////
// DDRAM																					//
///////////////////////////////////////////////////////////////////////

wire		[26:0]						ddram_addr;
wire		[31:0]						ddram_dout;
wire		[31:0]						ddram_din;
wire		[3:0]							ddram_dqm;
wire										ddram_req;
wire										ddram_rw;
wire										ddram_ack;

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
// SYSTEM VARIABLE ASSIGNMENTS													//
///////////////////////////////////////////////////////////////////////

assign									pll_reset						=		1'b0;
assign									hardware_reset					=		RESET | buttons[1] | status[0] | ~clock_locked;
assign									system_reset					=		(	system_reset_count	!=	8'd255	);
assign									download_reset					=		(	download_reset_count	!=	8'd255	);
assign									CLK_VIDEO						=		video_clock;
assign									DDRAM_CLK						=		system_clock;
assign									ioctl_wait						=		ioctl_wait_download | ioctl_wait_reset;

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
	.outclk_0								(		system_clock										),			// 100MHz FOR GENERAL SYSTEM CLOCK
	.outclk_1								(		video_clock											),			// 25.175MHz FOR STANDARD 640x480 VGA SYSTEM
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
// THIS IS THE TOP LEVEL ABSTRACTION WE WILL USE FOR THE SYSTEM		//
// IT WILL CONTAIN THE VIDEO CONTROLLER, CPU AND DDRAM CONTROLLER		//
///////////////////////////////////////////////////////////////////////

geranium_system
geranium_system
(

	.hardware_reset									(					hardware_reset																							),
	.system_reset										(					system_reset																							),
	
	.system_clock										(					system_clock																							),
	.video_clock										(					video_clock																								),
	
	.status												(					status																									),
	.ps2_key												(					ps2_key																									),

	.rom_download										(					rom_download																							),
	.rom_data_in										(					rom_download_data																						),
	.rom_data_addr										(					rom_download_addr																						),
	.rom_data_wr										(					rom_download_write																					),
	.rom_ack												(					rom_download_ack																						),

	.ddram_addr											(					ddram_addr																								),
	.ddram_data_out									(					ddram_din																								),
	.ddram_data_in										(					ddram_dout																								),
	.ddram_data_sel									(					ddram_dqm																								),
	.ddram_req											(					ddram_req																								),
	.ddram_rw											(					ddram_rw																									),
	.ddram_ack											(					ddram_ack																								),

	.VGA_R												(					VGA_R																										),
	.VGA_G												(					VGA_G																										),
	.VGA_B												(					VGA_B																										),
	.VGA_VS												(					VGA_VS																									),
	.VGA_HS												(					VGA_HS																									),
	.VGA_DE												(					VGA_DE																									),
	.CE_PIXEL											(					CE_PIXEL																									),
	
	.sd_lba												(					sd_lba																									),
	.sd_rd												(					sd_rd																										),
	.sd_wr												(					sd_wr																										),
	.sd_ack												(					sd_ack																									),
	.sd_buff_addr										(					sd_buff_addr																							),
	.sd_buff_dout										(					sd_buff_dout																							),
	.sd_buff_din										(					sd_buff_din																								),
	.sd_buff_wr											(					sd_buff_wr																								),

	.img_mounted										(					img_mounted																								),
	.img_size											(					img_size																									),
	.img_readonly										(					img_readonly																							),

	.led_a												(					led_a																										),
	.led_b												(					led_b																										),
	.led_c												(					led_c																										)

);

///////////////////////////////////////////////////////////////////////
// DDRAM																					//
///////////////////////////////////////////////////////////////////////

ddram_ctrl
ddram_ctrl
(
	.DDRAM_CLK											(					DDRAM_CLK																								),
	.DDRAM_BUSY											(					DDRAM_BUSY																								),
	.DDRAM_BURSTCNT									(					DDRAM_BURSTCNT																							),
	.DDRAM_ADDR											(					DDRAM_ADDR																								),
	.DDRAM_DOUT											(					DDRAM_DOUT																								),
	.DDRAM_DOUT_READY									(					DDRAM_DOUT_READY																						),
	.DDRAM_RD											(					DDRAM_RD																									),
	.DDRAM_DIN											(					DDRAM_DIN																								),
	.DDRAM_BE											(					DDRAM_BE																									),
	.DDRAM_WE											(					DDRAM_WE																									),

	.addr													(					ddram_addr																								),
	.dout													(					ddram_dout																								),
	.din													(					ddram_din																								),
	.din_byte_en										(					ddram_dqm																								),
	.req													(					ddram_req																								),
	.rw													(					ddram_rw																									),
	.ack													(					ddram_ack																								),
	
	.debug												(					1'b0																										)
	
);

///////////////////////////////////////////////////////////////////////
// RESET AND ROM DOWNLOAD PROCESS LOGIC										//
// THIS IS FOR ROM DOWNLOADING AND SYSTEM RESET WHICH WILL BE USED	//
// IN FUTURE TUTORIALS																//
// IT PROVIDES A RESET LINE FOR WHEN DOWNLOAD OF THE ROM IS COMPLETE	//
///////////////////////////////////////////////////////////////////////

always @(posedge system_clock) begin
	if(hardware_reset||(ioctl_download&&ioctl_reset)) begin
		download_reset_done							<=		1'b0;
		download_reset_count							<=		8'd0;
		ioctl_reset										<=		1'b0;
	end else if(~hardware_reset && ioctl_download && ~download_reset_done) begin
		download_reset_done							<=		1'b1;
		ioctl_wait_download							<=		1'b1;
	end
	if(download_reset_done) begin
		if(download_reset_count != 8'd255) begin
			download_reset_count						<=		download_reset_count	+	8'd1;
		end else begin
			ioctl_wait_download						<=		1'b0;
		end
	end
	if(download_reset||ioctl_download) begin
		system_reset_count							<=		8'd0;
	end else begin
		if(system_reset_count != 8'd255) begin
			system_reset_count						<=		system_reset_count	+	8'd1;
		end else begin
			ioctl_reset									<=		1'b1;
		end
	end
end

///////////////////////////////////////////////////////////////////////
// ROM DOWNLOAD																		//
// WE USE MULTIPLE REGISTERS FOR METASTABILITY REASONS BECAUSE WE		//
// CROSS CLOCK DOMAINS (VERY IMPORTANT)										//
// THIS BLOCK BASICLY GETS THE DATA FROM THE IO CONTROL AND MAKES IT	//
// 32BIT WORDS INSTEAD OF 16BIT WORDS											//
///////////////////////////////////////////////////////////////////////

always @(posedge system_clock) begin
	if(hardware_reset) begin
		ioctl_wait_reset														<=		1'b0;
		ioctl_wr_reg															<=		3'b000;
		ioctl_data_a															<=		16'd0;
		ioctl_data_b															<=		16'd0;
		ioctl_addr_a															<=		27'd0;
		ioctl_addr_b															<=		27'd0;
		rom_download															<=		1'b0;
		rom_download_addr														<=		27'd0;
		rom_download_count													<=		1'b0;
		rom_download_write													<=		1'b0;
		rom_download_data														<=		32'd0;
	end else begin
		ioctl_wr_reg															<=		{ ioctl_wr_reg[1:0] , ioctl_wr };
		ioctl_data_a															<=		ioctl_dout;
		ioctl_data_b															<=		ioctl_data_a;
		ioctl_addr_a															<=		ioctl_addr;
		ioctl_addr_b															<=		ioctl_addr_a;
		if(ioctl_download) begin
			rom_download														<=		1'b1;
			if(~rom_download_write && ioctl_wr_reg[2:1]==2'b01) begin
				rom_download_count											<=		rom_download_count + 1'b1;
				case(rom_download_count)
				1'b0: begin
					rom_download_data[15:0]									<=		ioctl_data_b[15:0];
					rom_download_addr											<=		{ 2'b00 , ioctl_addr_b[26:2] };
				end
				1'b1: begin
					rom_download_data[31:16]								<=		ioctl_data_b[15:0];
					ioctl_wait_reset											<=		1'b1;
					rom_download_write										<=		1'b1;
				end
				endcase
			end
		end else begin
			rom_download														<=		1'b0;
			rom_download_count												<=		1'b0;
			rom_download_write												<=		1'b0;
			ioctl_wait_reset													<=		1'b0;
		end
		if(rom_download_write && rom_download_ack) begin
			ioctl_wait_reset													<=		1'b0;
			rom_download_count												<=		1'b0;
			rom_download_write												<=		1'b0;
		end
	end
end


endmodule
