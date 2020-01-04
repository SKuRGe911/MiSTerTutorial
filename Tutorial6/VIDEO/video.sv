///////////////////////////////////////////////////////////////////////
// GERANIUM TUTORIAL CORE 2020													//
///////////////////////////////////////////////////////////////////////

`timescale 1ps / 1ps

`ifndef REAL_BUILD
`include "../SYSTEM/system_include.sv"
`endif

module
video
(
	input		wire																					reset,
	input		wire																					video_clock,
	input		wire																					system_clock,
	input		wire																					sdram_clock,

	output	dma_channels_from_host															video_dma_from_host,
	input		dma_channels_to_host																video_dma_to_host,
	
	input		wire																					char_display_on,
	input		wire																					insert_char,
	input		wire						[7:0]														insert_char_data,
	input		wire																					clear_screen,
	input		wire																					clear_line,
	output	wire																					char_ack,

	output	logic																					video_vbi							=		1'b0,

   output	wire						[7:0]														red,
   output	wire						[7:0]														green,
   output	wire						[7:0]														blue,
   output	logic																					hsync									=		1'b0,
   output	logic																					vsync									=		1'b0,
	output	logic																					hblank								=		1'b0,
	output	logic																					vblank								=		1'b0,
	
	input		wire																					debug_a,
	input		wire																					debug_b,
	input		wire																					debug_c

);

///////////////////////////////////////////////////////////////////////
// http://tinyvga.com/vga-timing/640x480@60Hz								//
// VIDEO PARAMETERS																	//
///////////////////////////////////////////////////////////////////////

parameter 				HSTRT						=				10'd0;   				// START OF HORIZONTAL VISIBLE AREA
parameter 				H   						=				10'd640;   				// WIDTH OF VISIBLE AREA
parameter 				HFP 						=				10'd16;     			// UNUSED TIME BEFORE HSYNC
parameter 				HS  						=				10'd96;    				// WIDTH OF HSYNC
parameter 				HBP 						=				10'd48;    				// UNUSED TIME AFTER HSYNC
parameter				HSYNC 					=				1'b0;						// NEGATIVE SYNC SIGNAL (1 NEG, 0 POS)

parameter 				VSTRT						=				10'd0;   				// START OF VERITICAL VISIBLE AREA
parameter 				V   						=				10'd480;    			// HEIGHT OF VISIBLE AREA
parameter 				VFP 						=				10'd10;     			// UNUSED TIME BEFORE VSYNC
parameter 				VS  						=				10'd2;      			// WIDTH OF VSYNC
parameter 				VBP 						=				10'd33;     			// UNUSED TIME AFTER VSYNC
parameter				VSYNC 					=				1'b0;						// NEGATIVE SYNC SIGNAL (1 NEG, 0 POS)

parameter				TEXT_BORDER				=				10'd0;					// TEXT BORDER PIXELS

///////////////////////////////////////////////////////////////////////
// VIDEO MODES																			//
// WE ARE GOING TO IMPLEMENT MORE THAN 1 VIDEO MODE SO LETS DEFINE	//
// SOME MODE CONSTANTS																//
///////////////////////////////////////////////////////////////////////

parameter				MODE0			=			32'd0;
parameter				MODE1			=			32'd1;
parameter				MODE2			=			32'd2;
parameter				MODE3			=			32'd3;

///////////////////////////////////////////////////////////////////////
// WIRES AND REGISTERS																//
///////////////////////////////////////////////////////////////////////

logic		[9:0]  													h_cnt									=			HSTRT;
logic		[9:0]  													v_cnt									=			VSTRT;

wire		[15:0]													pixel_color_out;
wire		[23:0]													pixel_out;
wire																	char_pixel;
wire																	console_window_pixel;

logic		[1:0]														new_pixel_d							=			2'b00;
logic																	new_pixel							=			1'b0;

///////////////////////////////////////////////////////////////////////
// BOTH COUNTERS COUNT FROM THE BEGIN OF THE VISIBLA AREA				//
///////////////////////////////////////////////////////////////////////
// HORIZONTAL PIXEL COUNTER														//
///////////////////////////////////////////////////////////////////////

always@(posedge video_clock) begin
	if(reset) begin
		h_cnt								<=			HSTRT;
		hblank							<=			1'b0;
	end else begin
		if(h_cnt==H+HFP+HS+HBP-1) begin
			h_cnt							<= 		HSTRT;
		end else begin
			h_cnt							<= 		h_cnt + 10'd1;
		end
		if(h_cnt == H+HFP-1) begin
			hsync							<= 		HSYNC ? 1'b0 : 1'b1;
		end
		if(h_cnt == H+HFP+HS-1) begin
			hsync							<=			HSYNC ? 1'b1 : 1'b0;
		end
		if(h_cnt == H-1) begin
			hblank						<= 		1'b1;
		end
		if(h_cnt == H+HFP+HS+HBP-1) begin
			hblank						<= 		1'b0;
		end
	end
end

///////////////////////////////////////////////////////////////////////
// VERITICAL PIXEL COUNTER															//
///////////////////////////////////////////////////////////////////////

always@(posedge video_clock) begin
	if(reset) begin
`ifndef REAL_BUILD
		v_cnt								<=			V-10'd1;
`else
		v_cnt								<=			VSTRT;
`endif
		vblank							<=			1'b0;
	end else begin
		if(h_cnt == H+HFP+HS+HBP-1) begin
			if(v_cnt==VS+VBP+V+VFP-1) begin
				v_cnt						<= 		VSTRT; 
			end else begin
				v_cnt						<=			v_cnt + 10'd1;
			end
			if(v_cnt == V+VFP-1) begin
				vsync						<= 		VSYNC ? 1'b0 : 1'b1;
			end
			if(v_cnt == V+VFP+VS-1) begin
				vsync						<=			VSYNC ? 1'b1 : 1'b0;
			end
		end
		if(h_cnt==H+HFP+HS+HBP-1) begin
			if(v_cnt == V+VFP+VS+VBP-1) begin
				vblank					<= 		1'b0;
			end
		end
		if(h_cnt==H-1) begin
			if(v_cnt == V-1) begin
				vblank					<= 		1'b1;
			end
		end
	end
end

///////////////////////////////////////////////////////////////////////
// VERITICAL BLANKING INTERVAL													//
///////////////////////////////////////////////////////////////////////

always @(posedge video_clock) begin
	if(reset) begin
		video_vbi								<=		1'b0;
	end else begin
		if(v_cnt==V-1 && h_cnt==H) begin
			video_vbi							<=		1'b1;
		end else begin
			video_vbi							<=		1'b0;
		end
	end
end

///////////////////////////////////////////////////////////////////////
// ASSIGN PIXELS																		//
// HERE WE DIVIDE UP THE PIXEL COLOR REGISTER INTO ITS COMPONENTS OF	//
// RED GREEN AND BLUE																//
///////////////////////////////////////////////////////////////////////

assign					red								=			pixel_out[23:16];
assign					green								=			pixel_out[15:8];
assign					blue								=			pixel_out[7:0];

///////////////////////////////////////////////////////////////////////
// NEW PIXEL LOGIC																	//
// THIS IS TO FIND THE CLOCK EDGE OF THE PIXEL CLOCK ON THE SYSTEM	//
// CLOCK																					//
///////////////////////////////////////////////////////////////////////

always @(posedge system_clock) begin
	new_pixel												<=							1'b0;
	if(reset) begin
		new_pixel_d											<=							2'b00;
	end else begin
		new_pixel_d											<=							{ new_pixel_d[0] , video_clock };
		if(new_pixel_d[1:0]==2'b01) begin
			new_pixel										<=							1'b1;
		end
	end
end

///////////////////////////////////////////////////////////////////////
// CHARACTER OVERLAY																	//
///////////////////////////////////////////////////////////////////////

character_display
#(
	.xstart					(			HSTRT														),
	.ystart					(			VSTRT														),
	.xstop					(			H															),
	.ystop					(			V															),
	.border					(			TEXT_BORDER												),
	
	.screen_y_size			(			V															),
	
	.x_screen_size			(			{ 22'd0 , H }											),
	.x_char_size			(			32'd8														),
	.y_screen_size			(			{ 22'd0 , V }											),
	.y_char_size			(			32'd8														)

)
character_display
(
	.reset					(			reset														),
	.system_clk				(			system_clock											),

	.x_pos					(			h_cnt														),
	.y_pos					(			v_cnt														),
	.pixel_clk				(			new_pixel												),
	.pixel					(			char_pixel												),
	.window					(			console_window_pixel									),

	.insert_char			(			insert_char_data										),
	.insert					(			insert_char												),
	.insert_clear_screen	(			clear_screen											),
	.insert_clear_line	(			clear_line												),
	.insert_ack				(			char_ack													)
	
);

///////////////////////////////////////////////////////////////////////
// VIDEO MULTIPLEXER																	//
// FOR OUR MULIPLE VIDEO MODES AND TO MIX IN THE CHARACTER DISPLAY	//
///////////////////////////////////////////////////////////////////////

pixel_multiplexer
#(
	.MODE0							(			MODE0																			),
	.MODE1							(			MODE1																			),
	.MODE2							(			MODE2																			),
	.MODE3							(			MODE3																			)
)
pixel_multiplexer
(
	.selector						(			MODE0																			),			// SELECT THE INPUT TO USE

	.console_on						(			char_display_on															),			// TO TURN CONSOLE ON OR OFF
	.char_pixel						(			char_pixel																	),			// CONSOLE CHARACTER BIT
	.window_pixel					(			console_window_pixel														),			// CONSOLE WINDOW BIT

	.source_mode_0					(			pixel_color_out															),			// A 16BIT VIDEO MODE INPUT
	.source_mode_1					(			16'd0																			),			// A 16BIT VIDEO MODE INPUT
	.source_mode_2					(			24'd0																			),			// A 24BIT VIDEO MODE INPUT
	.source_mode_3					(			24'd0																			),			// A 24BIT VIDEO MODE INPUT
	.hi_lo_empty_bits				(			1'b0																			),			// THIS SELECTS IF UNUSED BITS ARE HIGH OR LOW
	
	.destination					(			pixel_out																	)			// OUR OUTPUT DATA

);

///////////////////////////////////////////////////////////////////////
// MODE 0 VIDEO																		//
///////////////////////////////////////////////////////////////////////

mode_0_video
#(
	.H   						(			H																			),
	.HFP 						(			HFP																		),
	.HS  						(			HS																			),
	.HBP 	 					(			HBP																		),

	.V   			 			(			V																			),
	.VFP 		 				(			VFP																		),
	.VS  		 				(			VS																			),
	.VBP  					(			VBP																		)

)
mode_0_video
(
	.reset						(		reset																				),
	.ram_clock					(		sdram_clock																		),
	.pixel_clock				(		video_clock																		),
	.active						(		1'b1																				),

	.buffer_start_a			(		28'h01000000																	),
	.buffer_start_b			(		28'h01800000																	),
	.video_buffer_sel			(		1'b0																				),
	.video_buffer_swap		(		1'b0																				),
	.video_buffer_swap_ack	(																							),

	.v_cnt						(		v_cnt																				),
	.h_cnt						(		h_cnt																				),

	.video_dma_from_host		(		video_dma_from_host[0]														),
	.video_dma_to_host		(		video_dma_to_host[0]															),

	.pixel_out					(		pixel_color_out																),

	.debug						(		debug_a																			)

);

endmodule
