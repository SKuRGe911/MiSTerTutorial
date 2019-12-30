///////////////////////////////////////////////////////////////////////
// GERANIUM TUTORIAL CORE															//
///////////////////////////////////////////////////////////////////////

`timescale 1ps / 1ps

module
video
(
	input		wire																					reset,
	input		wire																					video_clock,

   output	wire						[7:0]														red,
   output	wire						[7:0]														green,
   output	wire						[7:0]														blue,
   output	logic																					hsync									=		1'b0,
   output	logic																					vsync									=		1'b0,
	output	logic																					hblank								=		1'b0,
	output	logic																					vblank								=		1'b0

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

///////////////////////////////////////////////////////////////////////
// WIRES AND REGISTERS																//
///////////////////////////////////////////////////////////////////////

logic		[9:0]  													h_cnt									=			HSTRT;
logic		[9:0]  													v_cnt									=			VSTRT;

logic		[23:0]													pixel_color_out					=			24'd0;

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
		v_cnt								<=			VSTRT;
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
// HERE WE GENERATE A SIMPLE PATTERN TO TEST THE VGA COUNTERS			//
///////////////////////////////////////////////////////////////////////

always @(posedge video_clock) begin
	if(reset) begin
		pixel_color_out											<=			24'b000000000000000000000000;
	end else begin
		if(h_cnt>=H || v_cnt>=V) begin
			// THIS SETS THE COLOR TO BLACK WHEN THE SIGNAL IS NOT IN THE VISIBLE AREA OF THE CLOCK COUNTERS
			pixel_color_out										<=			24'b000000000000000000000000;
		end else if(h_cnt<H/2) begin
			// HERE WE SET THE COLOR BASED ON THE HORIZONTAL COUNTER BEING ABOVE OR BELLOW HALF OF THE HORIZONTAL AREA
			// AND THE VERITICAL COUNTER BEING ABOVE OR BELLOW THE VERITICAL AREA
			if(v_cnt<V/2) begin
				pixel_color_out									<=			24'b111111110000000000000000;
			end else begin
				pixel_color_out									<=			24'b000000000000000011111111;
			end
		end else begin
			if(v_cnt<V/2) begin
				pixel_color_out									<=			24'b000000000000000011111111;
			end else begin
				pixel_color_out									<=			24'b111111110000000000000000;
			end
		end
	end
end

///////////////////////////////////////////////////////////////////////
// ASSIGN PIXELS																		//
// HERE WE DIVIDE UP THE PIXEL COLOR REGISTER INTO ITS COMPONENTS OF	//
// RED GREEN AND BLUE																//
///////////////////////////////////////////////////////////////////////

assign					red								=			pixel_color_out[23:16];
assign					green								=			pixel_color_out[15:8];
assign					blue								=			pixel_color_out[7:0];

endmodule
