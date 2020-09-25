////////////////////////////////////////////////////////
// GERANIUM DMA CACHE											//
////////////////////////////////////////////////////////

`timescale 1ps / 1ps

module
fifo_counter
(

	input				wire						clock,			// RAM CLOCK
	input				wire						reset,			// RESET ACTIVE HIGH

	input				wire						fill,				// FIFO HAS RECEIVED AN ENTRY
	input				wire						drain,			// AN ENTRY HAS BEEN READ
	output			wire						full,				// FIFO IS FULL
	output			wire						empty				// FIFO IS EMPTY

);

parameter integer								maxbit			=							5;
parameter integer								lowbit			=							2;
parameter integer								increment		=							1;

logic			[maxbit+1:0]					counter			=							'0;

localparam	[maxbit:lowbit] 				hightide			=							'1;
localparam	[maxbit+1:0]					lowtide			=							'0;

assign											full				=							( counter[maxbit+1] || counter[maxbit:lowbit]==hightide ) ? 1'b1 : 1'b0;
assign											empty				=							( counter[maxbit+1:0]==lowtide ) ? 1'b1 : 1'b0;

always @(posedge clock) begin
	if(reset) begin
		counter														<=							'0;
	end else begin
		if(fill && ~drain) begin
			counter													<=							counter + increment;
		end else if(~fill && drain) begin
			counter													<=							counter - increment;
		end
	end
end


endmodule
