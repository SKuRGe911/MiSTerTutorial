////////////////////////////////////////////////////////
// GERANIUM DMA CACHE											//
////////////////////////////////////////////////////////

`timescale 1ps / 1ps

`ifndef REAL_BUILD
`include "../SYSTEM/system_include.sv"
`endif

module
dma_cache
(
	input		wire							clock,
	input		wire							reset,
	
	// DMA channel address strobes
	input		dma_channels_from_host 	channels_from_host,
	output	dma_channels_to_host		channels_to_host,

	//SDRAM interface
	output	wire		[27:0]			sdram_addr,
	output	logic							sdram_req							=		1'b0,
	output	wire		[31:0]			sdram_size,
	input		wire							sdram_ack, // Set when the request has been acknowledged.
	input		wire							sdram_nak, // Set when bank collisions prevent the request being serviced
	input		wire							sdram_fill,
	input		wire		[15:0]			sdram_data,
	
	input		wire							debug

);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// THE DMA CACHE USES ON CHIP MEMORY FOR EACH CHANNEL AND A GENERATE STATEMENT TO CREATE LOGIC FOR	//
// EACH CHANNEL. IT READS DATA FROM SDRAM WHICH IS REQUESTED FROM THE USER COMPONENT.					//
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE																												//
/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef
enum
{
	rd1,
	rcv1,
	rcv2
}												inputstate_t;

inputstate_t								inputstate					=		rd1;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// DMA CHANNELS																												//
/////////////////////////////////////////////////////////////////////////////////////////////////////

localparam									dma_cache_max_cache_bit		=		9;

typedef
struct
{
	logic	[dma_cache_max_cache_bit:0]		wrptr;
	logic	[dma_cache_max_cache_bit:0]		wrptr_next;
	logic	[dma_cache_max_cache_bit:0]		rdptr;
	logic	[27:0]									addr;
	logic	[32:0]									count;
	logic 											pending;
	logic 											fill;
	logic 											full;
	logic 											drain;
	logic 											empty;
}														dma_channel_internal;

typedef dma_channel_internal					dma_channels_internal[TOTAL_DMA_CHANNELS:0];

dma_channels_internal							internals;


logic			[9:0]									cache_wraddr;
logic													cache_wren;
logic			[15:0]								data_from_ram;

logic			[4:0]									activechannel						=				5'b00000;

genvar												CHANNEL;
integer												I;

/////////////////////////////////////////////////////////////////////////////////////////////////////
assign					sdram_addr				=			internals[activechannel].addr;
assign					sdram_size				=			internals[activechannel].count[31:0];
/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// FIFO COUNTER															//
//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
generate

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	for(CHANNEL=0;CHANNEL<TOTAL_DMA_CHANNELS;CHANNEL=CHANNEL+1) begin : fifo_counters

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		wire	[9:0]									cache_rdaddr						=				internals[CHANNEL].rdptr;
		wire	[15:0]								cache_data;
		wire											cache_write							=				activechannel==CHANNEL[4:0] ? cache_wren : 1'b0;
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		fifo_counter
		#(
			.maxbit			(	dma_cache_max_cache_bit														),
			.lowbit			(	0																					),
			.increment		(	1																					)
		)
		fifocounter
		(
			.clock			(	clock																				),
			.reset			(	channels_from_host[CHANNEL].setaddr										),
			.fill				(	internals[CHANNEL].fill														),
			.drain			(	internals[CHANNEL].drain													),
			.full				(	internals[CHANNEL].full														),
			.empty			(	internals[CHANNEL].empty													)
		);
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		dma_ram
		dma_ram
		(
			.clock					(			clock																		),
			.data						(			data_from_ram															),
			.rdaddress				(			cache_rdaddr															),
			.wraddress				(			cache_wraddr															),
			.wren						(			cache_write																),
			.q							(			cache_data																)

		);
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		// READING																	//
		//////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		always @(posedge clock) begin
			internals[CHANNEL].drain								<=						1'b0;
			channels_to_host[CHANNEL].valid						<=						1'b0;
			if(channels_from_host[CHANNEL].req && channels_from_host[CHANNEL].active) begin
				internals[CHANNEL].pending							<=						1'b1;
			end
			if(internals[CHANNEL].pending && ~internals[CHANNEL].empty) begin
				internals[CHANNEL].rdptr							<=						internals[CHANNEL].rdptr + 9'd1;
				channels_to_host[CHANNEL].data					<=						cache_data;
				channels_to_host[CHANNEL].valid					<=						1'b1;
				internals[CHANNEL].drain							<=						1'b1;
				internals[CHANNEL].pending							<=						channels_from_host[CHANNEL].req && channels_from_host[CHANNEL].active ? 1'b1 : 1'b0;
			end
			if(channels_from_host[CHANNEL].setaddr) begin
				internals[CHANNEL].rdptr							<=						'0;
				internals[CHANNEL].pending							<=						1'b0;
			end
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////

	end
	/////////////////////////////////////////////////////////////////////////////////////////////////////

endgenerate
/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// WRITING																	//
//////////////////////////////////////////////////////////////

always @(posedge clock) begin

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if(reset) begin
	
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		inputstate														<=						rd1;
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		for(I=0;I<TOTAL_DMA_CHANNELS;I=I+1) begin
			internals[I].count										<=						'0;
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		
	end else begin

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		cache_wren														<=						1'b0;
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		if(sdram_ack==1'b1) begin
			sdram_req													<=						1'b0;
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		for(I=0;I<TOTAL_DMA_CHANNELS;I=I+1) begin
			internals[I].fill											<=						1'b0;
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		case(inputstate)
		rd1: begin
			for(I=TOTAL_DMA_CHANNELS-1;I>=0;I=I-1) begin
				if(channels_from_host[I].active) begin
					if(~internals[I].full && internals[I].count[31:0]!=32'h00000000 && ~internals[I].count[32]) begin
						activechannel									<=						I[4:0];
						sdram_req										<=						1'b1;
						inputstate										<=						rcv1;
						if(debug) begin
							$display("[DMACACHE] DMA READ RAM REQUEST FOR CHANNEL %01d AT %t",I,$time);
						end
					end
				end
			end
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		rcv1: begin
			if(sdram_nak) begin
				sdram_req												<=						1'b0;
				inputstate												<=						rd1;
				if(debug) begin
					$display("[DMACACHE] DMA READ RAM REQUEST NAK AT %t",$time);
				end
			end
			if(sdram_fill) begin
				data_from_ram											<=						sdram_data;
				cache_wren												<=						1'b1;
				inputstate												<=						rcv2;
				cache_wraddr											<=						internals[activechannel].wrptr;
				internals[activechannel].wrptr					<=						internals[activechannel].wrptr_next;
				internals[activechannel].wrptr_next				<=						internals[activechannel].wrptr_next + 1'b1;
				internals[activechannel].count					<=						internals[activechannel].count - 1'b1;
				internals[activechannel].addr						<=						internals[activechannel].addr + 28'd2;
				internals[activechannel].fill						<=						1'b1;
				if(debug) begin
					$display("[DMACACHE] DMA READ RAM FILL DATA %04x FOR CHANNEL %01d AT %t",sdram_data,activechannel,$time);
				end
			end
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		rcv2: begin
			if(sdram_fill) begin
				data_from_ram											<=						sdram_data;
				cache_wren												<=						1'b1;
				inputstate												<=						rcv2;
				cache_wraddr											<=						internals[activechannel].wrptr;
				internals[activechannel].wrptr					<=						internals[activechannel].wrptr_next;
				internals[activechannel].wrptr_next				<=						internals[activechannel].wrptr_next + 1'b1;
				internals[activechannel].count					<=						internals[activechannel].count - 1'b1;
				internals[activechannel].addr						<=						internals[activechannel].addr + 28'd2;
				internals[activechannel].fill						<=						1'b1;
				if(debug) begin
					$display("[DMACACHE] DMA READ RAM FILL DATA %04x FOR CHANNEL %01d AT %t",sdram_data,activechannel,$time);
				end
			end else begin
				inputstate												<=						rd1;
				for(I=TOTAL_DMA_CHANNELS-1;I>=0;I=I-1) begin
					if(channels_from_host[I].active) begin
						if(~internals[I].full && internals[I].count[31:0]!=32'h00000000 && ~internals[I].count[32]) begin
							activechannel								<=						I[4:0];
							sdram_req									<=						1'b1;
							inputstate									<=						rcv1;
							if(debug) begin
								$display("[DMACACHE] DMA READ RAM REQUEST FOR CHANNEL %01d AT %t",I,$time);
							end
						end
					end
				end
			end
		end
		endcase
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		for(I=0;I<TOTAL_DMA_CHANNELS;I=I+1) begin
			if(channels_from_host[I].active) begin
				if(channels_from_host[I].setaddr) begin
					internals[I].addr									<=						channels_from_host[I].addr;
					internals[I].wrptr								<=						'0;
					internals[I].wrptr_next							<=						'0;
					internals[I].wrptr_next[0]						<=						1'b1;
					internals[I].count								<=						'0;
					if(debug) begin
						$display("[DMACACHE] DMA SET ADDRESS %04x FOR CHANNEL %01d AT %t",channels_from_host[I].addr,I,$time);
					end
				end
				if(channels_from_host[I].setreqlen) begin
					internals[I].count[31:0]						<=						channels_from_host[I].reqlen;
					internals[I].count[32]							<=						1'b0;
					if(debug) begin
						$display("[DMACACHE] DMA SET REQUEST LENGTH %04x FOR CHANNEL %01d AT %t",channels_from_host[I].reqlen,I,$time);
					end
				end
			end
		end
		/////////////////////////////////////////////////////////////////////////////////////////////////////
	end
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	
end

endmodule
