
`timescale 1ps / 1ps

module
_SYSTEM_TB;

	localparam								ROM_SIZE							=		25'h08000;
	localparam								ddram_debug						=		1'b1;
	localparam								ddram_debug_hw					=		1'b0;
	
	wire										clock_locked;
	wire										clk_sys;
	wire										clk_video;
	logic										CLK_50M							=		1'b0;

	logic										reset								=		1'b1;
	logic		[7:0]							system_reset_count			=		8'd0;
	wire										system_reset;
	logic		[7:0]							download_reset_count			=		8'd0;
	wire										download_reset;
	logic										download_reset_done			=		1'b0;
	wire										hardware_reset					=		reset | ~clock_locked;

	logic											rom_download										=			1'b0;
	logic			[31:0]						rom_download_data									=			32'd0;
	logic			[27:0]						rom_download_addr									=			28'd0;
	logic											rom_download_write								=			1'b0;
	logic											rom_download_count								=			1'b0;
	wire											rom_download_ack;

	wire				[26:0]		ddram_addr;
	wire				[31:0]		ddram_dout;
	wire				[31:0]		ddram_din;
	wire				[3:0]			ddram_dqm;
	wire								ddram_req;
	wire								ddram_rw;
	wire								ddram_ack;
	
	wire		[7:0]							ioctl_index;
	wire		[31:0]						ioctl_file_ext;
	wire										ioctl_download;
	wire		[24:0]						ioctl_addr;
	wire		[15:0]						ioctl_dout;
	wire										ioctl_wr;
	wire										ioctl_wait;
	logic										ioctl_wait_reset				=		1'b0;
	logic										ioctl_wait_download			=		1'b0;
	logic										ioctl_reset						=		1'b1;

	logic			[2:0]							ioctl_wr_reg										=			3'b000;
	logic			[15:0]						ioctl_data_a										=			16'd0;
	logic			[15:0]						ioctl_data_b										=			16'd0;
	logic			[31:0]						ioctl_addr_a										=			32'd0;
	logic			[31:0]						ioctl_addr_b										=			32'd0;

	wire										DDRAM_CLK;
	wire										DDRAM_BUSY;
	wire		[7:0]							DDRAM_BURSTCNT;
	wire		[28:0]						DDRAM_ADDR;
	wire		[63:0]						DDRAM_DOUT;
	wire										DDRAM_DOUT_READY;
	wire										DDRAM_RD;
	wire		[63:0]						DDRAM_DIN;
	wire		[7:0]							DDRAM_BE;
	wire										DDRAM_WE;

	wire															SDRAM_CLK;
	wire															SDRAM_CKE;
	wire			[12:0]										SDRAM_A;
	wire			[1:0]											SDRAM_BA;
	wire			[15:0]										SDRAM_DQ;
	wire															SDRAM_DQML;
	wire															SDRAM_DQMH;
	wire															SDRAM_nCS;
	wire															SDRAM_nCAS;
	wire															SDRAM_nRAS;
	wire															SDRAM_nWE;

	wire		[7:0]							VGA_R;
	wire		[7:0]							VGA_G;
	wire		[7:0]							VGA_B;
	wire										VGA_DE;
	wire										VGA_HS;
	wire										VGA_VS;
	wire										CE_PIXEL;

	wire				[10:0]								ps2_key;
	wire				[31:0]								status;
	wire				[1:0]									scanlines;

	wire				[31:0]								sd_lba;
	wire														sd_rd;
	wire														sd_wr;
	wire														sd_ack;
	wire				[7:0]									sd_buff_addr;
	wire				[15:0]								sd_buff_dout;
	wire				[15:0]								sd_buff_din;
	wire														sd_buff_wr;

	wire				[15:0]								audio_left;
	wire				[15:0]								audio_right;
	wire				[1:0]									audio_mix;
	wire														audio_signed;
	
	///////////////////////////////////////////////////////////////////////
	// DMA																					//
	///////////////////////////////////////////////////////////////////////

	wire		[27:0]						dma_reserve_addr;
	wire										dma_reserve;
	wire		[27:0]						dma_addr_0;
	wire										dma_req_0;
	wire		[31:0]						dma_size_0;
	wire										dma_ack_0;
	wire										dma_nak_0;
	wire										dma_fill_0;
	wire		[27:0]						dma_addr_1;
	wire										dma_req_1;
	wire		[31:0]						dma_size_1;
	wire										dma_ack_1;
	wire										dma_nak_1;
	wire										dma_fill_1;
	wire		[15:0]						dma_data_from_ram;
	logic										dma_allow_reserve				=		1'b1;

	///////////////////////////////////////////////////////////////////////
	// SDRAM																					//
	///////////////////////////////////////////////////////////////////////

	wire		[27:0]						sdram_addr;
	wire		[31:0]						sdram_data_out;
	wire		[3:0]							sdram_dqm;
	wire										sdram_req;
	wire										sdram_ack;
	wire										sdram_rw;

	wire													sdram_ready;
	wire													video_refresh;

	///////////////////////////////////////////////////////////////////////
	// TIMING																				//
	///////////////////////////////////////////////////////////////////////

	initial begin
		CLK_50M = 1'b0;
		forever #10000 CLK_50M = ~CLK_50M;
	end

	///////////////////////////////////////////////////////////////////////
	// RESET																					//
	///////////////////////////////////////////////////////////////////////

	initial begin
		reset = 1'b1;
		#100000
		while(clock_locked==1'b0) begin
			#10 reset						<=					1'b1;
		end
		reset = 1'b0;
	end
	
	always @(posedge clk_sys) begin
		if(reset==1'b1||(ioctl_download==1'b1&&ioctl_reset==1'b1)) begin
			download_reset_done							<=		1'b0;
			download_reset_count							<=		8'd0;
			ioctl_reset										<=		1'b0;
		end else if(reset==1'b0 && ioctl_download==1'b1 && download_reset_done==1'b0) begin
			download_reset_done							<=		1'b1;
			ioctl_wait_download							<=		1'b1;
		end
		if(download_reset_done==1'b1) begin
			if(download_reset_count != 8'd255) begin
				download_reset_count						<=		download_reset_count	+	8'd1;
			end else begin
				ioctl_wait_download						<=		1'b0;
			end
		end
		if(download_reset==1'b1||ioctl_download==1'b1) begin
			system_reset_count							<=		8'd0;
		end else begin
			if(system_reset_count != 8'd255) begin
				system_reset_count						<=		system_reset_count	+	8'd1;
			end else begin
				ioctl_reset									<=		1'b1;
			end
		end
	end

	assign									system_reset					=		(	system_reset_count	!=	8'd255	);
	assign									download_reset					=		(	download_reset_count	!=	8'd255	);

	assign									ioctl_wait						=		ioctl_wait_download | ioctl_wait_reset;

	assign DDRAM_CLK = clk_sys;
	
	pll
	pll
	(
		.refclk									(		CLK_50M												),			// 50MHz SOURCE CLOCK
		.rst										(		1'b0													),			// A LINE TO RESET THE PLL WHICH IS JUST SET LOW FOR THIS PROJECT
		.outclk_0								(		clk_sys												),			// 100MHz FOR GENERAL SYSTEM CLOCK
		.outclk_1								(		clk_video											),			// 25.175MHz FOR STANDARD 640x480 VGA SYSTEM
		.locked									(		clock_locked										)			// GOES HIGH WHEN THE PHASE LOCKED LOOP IS STABLE
		
	);

	///////////////////////////////////////////////////////////////////////
	// DATA LOADER																			//
	///////////////////////////////////////////////////////////////////////

	data_io
	#(
			.ROM_SIZE						(					ROM_SIZE											)
	
	)
	data_rom
	(
		.downloading						(					ioctl_download									),
		.index								(																		),
		.rst									(					download_reset									),
		.clk									(					CLK_50M											),
		.wr									(					ioctl_wr											),
		.addr									(					ioctl_addr										),
		.data									(					ioctl_dout										),
		.dwait								(					ioctl_wait										)
	);
	
	///////////////////////////////////////////////////////////////////////
	// DDRAM																					//
	///////////////////////////////////////////////////////////////////////

	ddram_ctrl
	ddram_ctrl
	(
		.DDRAM_CLK					(			DDRAM_CLK																			),
		.DDRAM_BUSY					(			DDRAM_BUSY																			),
		.DDRAM_BURSTCNT			(			DDRAM_BURSTCNT																		),
		.DDRAM_ADDR					(			DDRAM_ADDR																			),
		.DDRAM_DOUT					(			DDRAM_DOUT																			),
		.DDRAM_DOUT_READY			(			DDRAM_DOUT_READY																	),
		.DDRAM_RD					(			DDRAM_RD																				),
		.DDRAM_DIN					(			DDRAM_DIN																			),
		.DDRAM_BE					(			DDRAM_BE																				),
		.DDRAM_WE					(			DDRAM_WE																				),

		.addr							(			ddram_addr																			),
		.dout							(			ddram_dout																			),
		.din							(			ddram_din																			),
		.din_byte_en				(			ddram_dqm																			),
		.req							(			ddram_req																			),
		.rw							(			ddram_rw																				),
		.ack							(			ddram_ack																			),
		
		.debug						(			ddram_debug																			)
		
	);

	///////////////////////////////////////////////////////////////////////
	// ROM DOWNLOAD																		//
	///////////////////////////////////////////////////////////////////////


	always @(posedge DDRAM_CLK) begin
		if(hardware_reset==1'b1) begin
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
			if(ioctl_download==1'b1) begin
				rom_download														<=		1'b1;
				if(rom_download_write==1'b0 && ioctl_wr_reg[2:1]==2'b01) begin
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
			if(rom_download_write==1'b1 && rom_download_ack==1'b1) begin
				ioctl_wait_reset													<=		1'b0;
				rom_download_count												<=		1'b0;
				rom_download_write												<=		1'b0;
			end
		end
	end

	///////////////////////////////////////////////////////////////////////
	// SYSTEM																				//
	///////////////////////////////////////////////////////////////////////

	geranium_system
	geranium_system
	(

		.hardware_reset									(					hardware_reset																							),
		.system_reset										(					system_reset																							),
		
		.system_clock										(					clk_sys																									),
		.video_clock										(					clk_video																								),
		
		.status												(					status																									),

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
		
		.led_a												(					led_a																										),
		.led_b												(					led_b																										),
		.led_c												(					led_c																										)

	);
	
	ddram_tb_sim
	avalonslavetestbench
	(
		.clk_ram								(		DDRAM_CLK															),
		.mem_wait_request					(		DDRAM_BUSY															),
		.mem_write							(		DDRAM_WE																),
		.mem_read							(		DDRAM_RD																),
		.mem_address						(		DDRAM_ADDR															),
		.mem_byte_enable					(		DDRAM_BE																),
		.mem_burst_count					(		DDRAM_BURSTCNT														),
		.mem_data_out						(		DDRAM_DOUT															),
		.mem_read_data_valid				(		DDRAM_DOUT_READY													),
		.mem_data_in						(		DDRAM_DIN															),
		.reset								(		hardware_reset														),
		.debug								(		ddram_debug_hw														)

	);

endmodule

