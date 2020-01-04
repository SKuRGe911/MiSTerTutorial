///////////////////////////////////////////////////////////////////////
// GERANIUM TUTORIAL CORE 2020													//
///////////////////////////////////////////////////////////////////////

`timescale 1ps / 1ps

module
pixel_multiplexer
(
	input						wire				[31:0]						selector,
	
	input						wire												console_on,
	input						wire												char_pixel,
	input						wire												window_pixel,
	
	input						wire				[15:0]						source_mode_0,
	input						wire				[15:0]						source_mode_1,
	input						wire				[23:0]						source_mode_2,
	input						wire				[23:0]						source_mode_3,
	input						wire												hi_lo_empty_bits,
	
	output					wire				[23:0]						destination
	
);

///////////////////////////////////////////////////////////////////////
// VIDEO MODES																			//
///////////////////////////////////////////////////////////////////////

parameter				MODE0			=			32'd0;
parameter				MODE1			=			32'd1;
parameter				MODE2			=			32'd2;
parameter				MODE3			=			32'd3;

///////////////////////////////////////////////////////////////////////
// MULTIPLEXER																			//
///////////////////////////////////////////////////////////////////////

wire						[23:0]					pixel_0					=				{ source_mode_0[15:11] , hi_lo_empty_bits ? 3'b111 : 3'b000 , source_mode_0[10:5] , hi_lo_empty_bits ? 2'b11 : 2'b00 , source_mode_0[4:0] , hi_lo_empty_bits ? 3'b111 : 3'b000 };
wire						[23:0]					pixel_1					=				{ source_mode_1[15:11] , hi_lo_empty_bits ? 3'b111 : 3'b000 , source_mode_1[10:5] , hi_lo_empty_bits ? 2'b11 : 2'b00 , source_mode_1[4:0] , hi_lo_empty_bits ? 3'b111 : 3'b000 };
wire						[23:0]					pixel_2					=				source_mode_2;
wire						[23:0]					pixel_3					=				source_mode_3;
wire						[23:0]					pixel_out				=				selector==MODE0 ? pixel_0 : selector==MODE1 ? pixel_1 : selector==MODE2 ? pixel_2 : selector==MODE3 ? pixel_3 : 24'd0;

assign												destination				=				(char_pixel & console_on) ? pixel_out | 24'b001111110011111100111111 : (window_pixel & console_on) ? pixel_out & 24'b000111110001111100011111 : pixel_out;

endmodule
