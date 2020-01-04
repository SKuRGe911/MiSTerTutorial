///////////////////////////////////////////////////////////////////////
// GERANIUM DEMO CORE 2019															//
///////////////////////////////////////////////////////////////////////

`timescale 1ps / 1ps

`ifndef REAL_BUILD
`include "../SYSTEM/system_include.sv"
`endif

module
mode_0_video
(
	// SYSTEM
	input		wire										reset,
	input		wire										ram_clock,
	input		wire										pixel_clock,
	input		wire										active,

	input		wire	[27:0]							buffer_start_a,
	input		wire	[27:0]							buffer_start_b,
	input		wire										video_buffer_sel,
	input		wire										video_buffer_swap,
	
	input		wire	[9:0]								v_cnt,
	input		wire	[9:0]								h_cnt,

	// DMA
	output	dma_channel_fromhost					video_dma_from_host,
	input		dma_channel_tohost					video_dma_to_host,

	// OUTPUT
	output	logic		[15:0]						pixel_out							=				16'd0,
	output	logic										video_buffer_swap_ack			=				1'b0,
	
	input		wire										debug


);

///////////////////////////////////////////////////////////////////////
// http://tinyvga.com/vga-timing/640x480@60Hz								//
// VIDEO PARAMETERS																	//
///////////////////////////////////////////////////////////////////////

parameter 		H   									=			10'd640;
parameter 		HFP 									=			10'd16;
parameter 		HS  									=			10'd96;
parameter 		HBP 									=			10'd48;

parameter 		V   									=			10'd480;
parameter 		VFP 									=			10'd10;
parameter 		VS  									=			10'd2;
parameter 		VBP 									=			10'd33;

wire		[31:0]													video_line_req_length			=			{ 22'd0 , H };
logic		[27:0]													video_buffer_current				=			28'h01000000;
logic																	video_double_buffer_active		=			1'b0;
logic																	new_pixel							=			1'b0;
logic		[2:0]														new_pixel_reg						=			3'b000;

///////////////////////////////////////////////////////////////////////
// DETERMINE WHEN A NEW PIXEL IS READY											//
///////////////////////////////////////////////////////////////////////

always @(posedge ram_clock) begin
	new_pixel												<=		1'b0;
	new_pixel_reg											<=		{ new_pixel_reg[1:0] , pixel_clock };
	if(new_pixel_reg[2:1]==2'b01) begin
		new_pixel											<=		1'b1;
	end
end

///////////////////////////////////////////////////////////////////////
// GET CURRENT PIXEL DATA															//
///////////////////////////////////////////////////////////////////////

always @(posedge ram_clock) begin
	video_dma_from_host.req										<=			1'b0;
	video_dma_from_host.setaddr								<=			1'b0;
	video_dma_from_host.setreqlen								<=			1'b0;
	video_dma_from_host.active									<=			active;
	if(reset) begin
		video_buffer_swap_ack									<=			1'b0;
		video_double_buffer_active								<=			1'b0;
	end else begin
		if(video_dma_to_host.valid) begin
			pixel_out												<=			video_dma_to_host.data;
		end
		if(new_pixel) begin
			if((v_cnt<V && h_cnt<H)||(v_cnt<V-1 && h_cnt==H+HFP+HS+HBP-1)||(v_cnt==V+VFP+VS+VBP-1 && h_cnt==H+HFP+HS+HBP-1)) begin
				if(active) begin
					video_dma_from_host.req						<=			1'b1;
				end
			end else begin
				if(v_cnt==V && h_cnt==10'd0) begin
					if(video_buffer_swap) begin
						video_buffer_swap_ack					<=			1'b1;
						video_double_buffer_active				<=			video_buffer_sel ? 1'b1 : 1'b0;
						video_buffer_current						<=			video_buffer_sel ? buffer_start_b : buffer_start_a;
						video_dma_from_host.addr				<=			video_buffer_sel ? buffer_start_b : buffer_start_a;
					end else begin
						video_dma_from_host.addr				<=			video_buffer_current;
					end
					if(active) begin
						video_dma_from_host.setaddr			<=			1'b1;
					end
				end
				if(v_cnt==V+VFP+VS+VBP-1 || v_cnt<V-1) begin
					if(h_cnt==H) begin
						if(active) begin
							video_dma_from_host.reqlen			<=			video_line_req_length;
							video_dma_from_host.setreqlen		<=			1'b1;
						end
					end
				end
			end
		end
		if(~video_buffer_swap && video_buffer_swap_ack) begin
			video_buffer_swap_ack								<=			1'b0;
		end
	end
end

endmodule
