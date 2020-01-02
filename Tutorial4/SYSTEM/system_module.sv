///////////////////////////////////////////////////////////////////////
// GERANIUM TUTORIAL CORE FOR MISTER											//
///////////////////////////////////////////////////////////////////////

module
geranium_system
(

	input				wire															hardware_reset,
	input				wire															system_reset,
	
	input				wire															system_clock,
	input				wire															video_clock,

	input				wire				[63:0]									status,

	input				wire															rom_download,
	input				wire				[31:0]									rom_data_in,
	input				wire				[27:0]									rom_data_addr,
	input				wire															rom_data_wr,
	output			wire															rom_ack,

	output			wire				[26:0]									ddram_addr,
	output			wire				[31:0]									ddram_data_out,
	input				wire				[31:0]									ddram_data_in,
	output			wire				[3:0]										ddram_data_sel,
	output			wire															ddram_req,
	output			wire															ddram_rw,
	input				wire															ddram_ack,

	output			wire				[7:0]										VGA_R,
	output			wire				[7:0]										VGA_G,
	output			wire				[7:0]										VGA_B,
	output			wire															VGA_VS,
	output			wire															VGA_HS,
	output			wire															VGA_DE,
	output			wire															CE_PIXEL,
	
	output			wire				[31:0]									sd_lba,
	output			wire															sd_rd,
	output			wire															sd_wr,
	input				wire															sd_ack,
	input				wire				[7:0]										sd_buff_addr,
	input				wire				[15:0]									sd_buff_dout,
	output			wire				[15:0]									sd_buff_din,
	input				wire															sd_buff_wr,

	input				wire															img_mounted,
	input				wire				[63:0]									img_size,
	input				wire															img_readonly,

	output			logic															led_a										=			1'b0,
	output			logic															led_b										=			1'b0,
	output			logic															led_c										=			1'b0

);

///////////////////////////////////////////////////////////////////////
// TOP LEVEL VARIABLES																//
///////////////////////////////////////////////////////////////////////

wire		[1:0]							scanlines;												// VIDEO SCAN LINES
wire		[31:0]						video_addr;
wire		[31:0]						video_data;
wire										video_write;
wire										video_ack;

///////////////////////////////////////////////////////////////////////
// MULTIPLEXER SELECTORS FOR ADDRESS LINE FROM CPU							//
// ONLY USED FOR DDRAM AND IO HERE BUT WILL ADD OTHER ADDRESS			//
// MAPPINGS TO	FUTURE TUTORIALS													//
///////////////////////////////////////////////////////////////////////

localparam																					DDRAM_SEL									=		2'b00;
localparam																					SDRAM_SEL									=		2'b01;
localparam																					IO_SEL										=		2'b10;
localparam																					OTHER_SEL									=		2'b11;

wire																							sel_io;
wire																							sel_ddrram;
wire																							sel_sdram;
wire																							sel_other;

///////////////////////////////////////////////////////////////////////
// HARDWARE IO																			//
///////////////////////////////////////////////////////////////////////

wire											hardware_io_ack;
wire			[27:0]						hardware_io_addr;
wire			[31:0]						hardware_io_data_out;
wire			[31:0]						hardware_io_data_in;
wire			[3:0]							hardware_io_dqm;
wire											hardware_io_write;

logic											io_active									=			1'b0;
logic			[31:0]						io_data_in									=			32'd0;
logic			[31:0]						io_data_out									=			32'd0;
logic			[27:0]						io_addr										=			28'd0;
logic			[3:0]							io_dqm										=			4'b0000;
logic											io_ack										=			1'b0;
logic											io_wr											=			1'b0;

///////////////////////////////////////////////////////////////////////
// RAM																					//
// DDRAM WILL BE USED FOR STORAGE FOR INTERFACE WITH THE CPU			//
///////////////////////////////////////////////////////////////////////

logic											ddram_ack_data								=			1'b0;
logic			[31:0]						ddram_data_in_d							=			32'd0;

///////////////////////////////////////////////////////////////////////
// VIDEO																					//
// VARIABLES TO USE ON THE VGA SIGNAL GENERATION OBJECT					//
///////////////////////////////////////////////////////////////////////

wire			[7:0]							R;
wire			[7:0]							G;
wire			[7:0]							B;
wire											HSync;
wire											VSync;
wire											HBlank;
wire											VBlank;

///////////////////////////////////////////////////////////////////////
// CPU																					//
// WE ARE GOING TO USE A RISCV CPU IMPLEMENTATION							//
///////////////////////////////////////////////////////////////////////

wire			[31:0]						riscv_cpu_addr;
wire			[31:0]						riscv_cpu_data_out;
wire			[3:0]							riscv_cpu_data_select;
wire			[31:0]						riscv_cpu_data_in;
wire											riscv_cpu_addr_valid;
wire											riscv_cpu_instruction;
wire											riscv_cpu_addr_ready;
wire											riscv_cpu_write;
wire			[31:0]						riscv_cpu_interupts						=				32'd0;
wire			[31:0]						riscv_cpu_interupts_ack;

///////////////////////////////////////////////////////////////////////
// CHARACTER OUTPUT																	//
///////////////////////////////////////////////////////////////////////

wire											char_write;
wire			[7:0]							char_out;
wire											char_clear;
wire											char_clear_line;
wire											char_ack;
wire											char_display_on;

///////////////////////////////////////////////////////////////////////
// ASSIGNMENTS																			//
///////////////////////////////////////////////////////////////////////

assign										scanlines						=				status[12:11];
assign										rom_ack							=				ddram_ack;

assign										sel_ddrram						=				rom_download	?	1'b1	:	riscv_cpu_addr[31:30]==DDRAM_SEL	?	1'b1	:	1'b0;
assign										sel_sdram						=				rom_download	?	1'b0	:	riscv_cpu_addr[31:30]==SDRAM_SEL	?	1'b1	:	1'b0;
assign										sel_io							=				rom_download	?	1'b0	:	riscv_cpu_addr[31:30]==IO_SEL		?	1'b1	:	1'b0;
assign										sel_other						=				rom_download	?	1'b0	:	riscv_cpu_addr[31:30]==OTHER_SEL	?	1'b1	:	1'b0;

assign										ddram_addr						=				sel_ddrram ? rom_download ? rom_data_addr : riscv_cpu_addr[28:2] : 27'd0;
assign										ddram_data_out					=				sel_ddrram ? rom_download ? rom_data_in : riscv_cpu_data_out : 32'd0;
assign										ddram_data_sel					=				sel_ddrram ? rom_download ? 4'b1111 : riscv_cpu_data_select : 4'b0000;
assign										ddram_req						=				sel_ddrram ? rom_download ? rom_data_wr : riscv_cpu_addr_valid ? 1'b1 : 1'b0 : 1'b0;
assign										ddram_rw							=				sel_ddrram ? rom_download ? 1'b1 : riscv_cpu_write : 1'b0;

assign										video_addr						=				sel_other ? { 5'd0 , riscv_cpu_addr[28:2] } : 32'd0;
assign										video_data						=				riscv_cpu_data_out;
assign										video_write						=				sel_other ? riscv_cpu_addr_valid ? riscv_cpu_write ? 1'b1 : 1'b0 : 1'b0 : 1'b0;

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
	.clk_vid									(		video_clock											),
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
	.reset									(		hardware_reset										),
	.video_clock							(		video_clock											),
	.system_clock							(		system_clock										),

	.data_in									(		video_data											),			// HERE WE SEND THE DOWNLOADED DATA
	.addr_in									(		video_addr											),			// AND ADDRESS
	.data_write								(		video_write											),			// WRITE DATA WHEN HIGH
	.data_ack								(		video_ack											),			// RECEIVE ACKNOWLEDGE

	.char_display_on						(		char_display_on									),
	.insert_char							(		char_write											),
	.insert_char_data						(		char_out												),
	.clear_screen							(		char_clear											),
	.clear_line								(		char_clear_line									),
	.char_ack								(		char_ack												),

	.red										(		R														),
	.green									(		G														),
	.blue										(		B														),
	.hsync									(		HSync													),
	.vsync									(		VSync													),
	.hblank									(		HBlank												),
	.vblank									(		VBlank												)
	
);

///////////////////////////////////////////////////////////////////////
// DRIVE LINES FOR CPU																//
///////////////////////////////////////////////////////////////////////

assign									riscv_cpu_write				=		riscv_cpu_data_select[0] | riscv_cpu_data_select[1] | riscv_cpu_data_select[2] | riscv_cpu_data_select[3];
assign									riscv_cpu_addr_ready			=		ddram_ack_data | io_ack | video_ack;
assign									riscv_cpu_data_in				=		io_active ?
																						io_data_out :
																						ddram_ack_data ?
																						ddram_data_in_d :
																						32'd0;

///////////////////////////////////////////////////////////////////////
// LATCH INPUT DATA FROM DDRAM FOR CPU											//
///////////////////////////////////////////////////////////////////////

always @(posedge system_clock) begin
	if(system_reset) begin
		ddram_ack_data															<=		1'b0;
	end else begin
		if(~ddram_ack_data) begin
			if(riscv_cpu_addr_valid) begin
				if(ddram_ack) begin
					ddram_ack_data												<=		1'b1;
					ddram_data_in_d											<=		ddram_data_in;
				end
			end
		end else begin
			if(~riscv_cpu_addr_valid) begin
				ddram_ack_data													<=		1'b0;
			end
		end
	end
end

///////////////////////////////////////////////////////////////////////
// CPU																					//
// https://github.com/cliffordwolf/picorv32 IS THE CPU					//
// RISCV IMPLEMENTATION WE WILL BE USING. SO WE WILL NEED AN			//
// APPROPRIATE COMPILER CHAIN FOR IT											//
///////////////////////////////////////////////////////////////////////

picorv32
#(
	.ENABLE_COUNTERS					(					1													),
	.ENABLE_COUNTERS64				(					1													),
	.ENABLE_REGS_16_31				(					1													),
	.ENABLE_REGS_DUALPORT			(					1													),
	.LATCHED_MEM_RDATA				(					0													),
	.TWO_STAGE_SHIFT					(					1													),
	.BARREL_SHIFTER					(					1													),
	.TWO_CYCLE_COMPARE				(					1													),
	.TWO_CYCLE_ALU						(					1													),
	.COMPRESSED_ISA					(					1													),
	.CATCH_MISALIGN					(					1													),
	.CATCH_ILLINSN						(					1													),
	.ENABLE_PCPI						(					1													),
	.ENABLE_MUL							(					1													),
	.ENABLE_FAST_MUL					(					1													),
	.ENABLE_DIV							(					1													),
	.ENABLE_IRQ							(					1													),
	.ENABLE_IRQ_QREGS					(					1													),
	.ENABLE_IRQ_TIMER					(					1													),
	.ENABLE_TRACE						(					0													),
	.REGS_INIT_ZERO					(					1													),
	.MASKED_IRQ							(					32'h00FF_FFFF									),
	.LATCHED_IRQ						(					32'hFFFF_FFFF									),
	.PROGADDR_RESET					(					32'h0000_0000									),
	.PROGADDR_IRQ						(					32'h0000_0010									),
	.STACKADDR							(					32'h3FFF_FF00									)
	
)
cpu
(
	.trap									(																		),
	.irq									(			riscv_cpu_interupts									),
	.eoi									(			riscv_cpu_interupts_ack								),

	.clk									(			system_clock											),
	.resetn								(			~system_reset											),
	
	.mem_valid							(			riscv_cpu_addr_valid									),			//HIGH ON VALID READ OR WRITE ADDRESS
	.mem_instr							(			riscv_cpu_instruction								),
	.mem_ready							(			riscv_cpu_addr_ready									),			//SET WHEN MEMORY OPERATION IS DONE

	.mem_addr							(			riscv_cpu_addr											),
	.mem_rdata							(			riscv_cpu_data_in										),
	.mem_wdata							(			riscv_cpu_data_out									),
	.mem_wstrb							(			riscv_cpu_data_select								),
	
	.mem_la_read						(																		),
	.mem_la_write						(																		),
	.mem_la_addr						(																		),
	.mem_la_wdata						(																		),
	.mem_la_wstrb						(																		),

	.pcpi_valid							(																		),
	.pcpi_insn							(																		),
	.pcpi_rs1							(																		),
	.pcpi_rs2							(																		),
	.pcpi_wr								(																		),
	.pcpi_rd								(																		),
	.pcpi_wait							(																		),
	.pcpi_ready							(																		),
	
	.trace_valid						(																		),
	.trace_data							(																		)
	
);

///////////////////////////////////////////////////////////////////////
// IO FOR HARDWARE																	//
// THE TOP 2 BITS OF THE CPU OUTPUT ADDRESS IS USED TO SELECT 4		//
// ADDRESS MAPPINGS. HOW THE REST OF THE ADDRESS IS USED IS DONE		//
// IN THE IO MODULE.																	//
///////////////////////////////////////////////////////////////////////

assign										hardware_io_addr							=		riscv_cpu_addr[29:2];
assign										hardware_io_data_in						=		riscv_cpu_data_out;
assign										hardware_io_dqm							=		riscv_cpu_data_select;
assign										hardware_io_write							=		riscv_cpu_write;

always @(posedge system_clock) begin
	if(system_reset) begin
		io_active																				<=		1'b0;
		io_ack																					<=		1'b0;
	end else begin
		if(riscv_cpu_addr_valid) begin
			if(~io_active) begin
				if(sel_io) begin
					io_active																	<=		1'b1;
					io_data_in																	<=		hardware_io_data_in;
					io_addr																		<=		hardware_io_addr;
					io_dqm																		<=		hardware_io_dqm;
					io_wr																			<=		hardware_io_write;
				end
			end else begin
				if(~io_ack) begin
					if(hardware_io_ack) begin
						io_data_out																<=		hardware_io_data_out;
						io_ack																	<=		1'b1;
					end
				end
			end
		end else begin
			io_active																			<=		1'b0;
			io_ack																				<=		1'b0;
		end
	end
end

io_control
io_control
(
	.hardware_reset				(			hardware_reset																	),			// RESET
	.sys_clk							(			system_clock																	),			// SYSTEM CLOCK

	.hardware_addr					(			io_addr																			),			// IO ADDRESS
	.hardware_data_in				(			io_data_in																		),			// IO DATA IN
	.hardware_data_out			(			hardware_io_data_out															),			// IO DATA OUT
	.hardware_data_sel			(			io_dqm																			),			// IO DATA MASK
	.hardware_write				(			io_wr																				),			// IO WRITE
	.hardware_ready				(			io_active																		),			// IO REQUEST
	.hardware_ack					(			hardware_io_ack																),			// IO ACKNOWLEDGE

	.char_display_on				(			char_display_on																),			// ALLOWS US TO TURN ON OR OFF THE CONSOLE IN THE VIDEO CONTROLLER
	.char_write						(			char_write																		),			// CONSOLE CHARACTER WRITE
	.char_out						(			char_out																			),			// CONSOLE CHARACTER TO DISPLAY
	.char_clear						(			char_clear																		),			// CONSOLE CLEAR SCREEN
	.char_clear_line				(			char_clear_line																),			// CONSOLE CLEAR CURRENT LINE
	.char_done						(			char_ack																			),			// CONSOLE CHARACTER OUT ACKNOWLEDGE

	.status							(			status																			),			// SYSTEM STATUS REGISTER

	.sd_lba							(			sd_lba																			),			// SD CARD LOGICAL BLOCK ADDRESS
	.sd_rd							(			sd_rd																				),			// SD CARD READ
	.sd_wr							(			sd_wr																				),			// SD CARD WRITE
	.sd_ack							(			sd_ack																			),			// SD CARD ACKNOWLEDGE
	.sd_buff_addr					(			sd_buff_addr																	),			// SD BUFFER ADDRESS
	.sd_buff_dout					(			sd_buff_dout																	),			// SD DATA OUT
	.sd_buff_din					(			sd_buff_din																		),			// SD DATA IN
	.sd_buff_wr						(			sd_buff_wr																		),			// SD BUFFER WRITE

	.img_mounted					(			img_mounted																		),			// SD IMAGE MOUNTED
	.img_size						(			img_size																			),			// SD IMAGE SIZE
	.img_readonly					(			img_readonly																	),			// SD READ ONLY

	.keyboard_in					(			keyboard_key																	)

);

endmodule
