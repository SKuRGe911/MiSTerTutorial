///////////////////////////////////////////////////////////////////////
// KEYBOARD IO																			//
///////////////////////////////////////////////////////////////////////

`timescale 1ps / 1ps

module
keyboard_io
(
	input			wire											system_reset,
	input			wire											system_clock,
	
	input			wire		[10:0]							ps2_key,
	
	output		logic											interupt_req				=			1'b0,
	output		wire		[7:0]								key_pressed
	
);

///////////////////////////////////////////////////////////////////////
// KEYBOARD PS2 SCAN CODES FROM													//
// https://techdocs.altium.com/display/FPGA/PS2+Keyboard+Scan+Codes	//
// ASCII CODES FROM																	//
// http://www.asciitable.com/														//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// KEYBOARD VARIABLES																//
///////////////////////////////////////////////////////////////////////

wire											ps2_extended;
wire											ps2_pressed;
wire											ps2_toggle;
wire		[7:0]								ps2_character;

logic		[7:0]								key_value					=		8'd0;
logic		[7:0]								key_value_a					=		8'd0;
logic		[7:0]								key_value_b					=		8'd0;
logic		[1:0]								key_req						=		2'b00;
logic		[2:0]								key_toggle					=		3'b000;
logic		[1:0]								key_extended				=		2'b00;

///////////////////////////////////////////////////////////////////////
// KEYBOARD ASSIGNMENTS																//
///////////////////////////////////////////////////////////////////////

assign										ps2_extended				=		ps2_key[8];
assign										ps2_pressed					=		ps2_key[9];
assign										ps2_toggle					=		ps2_key[10];
assign										ps2_character				=		ps2_key[7:0];

assign										key_pressed					=		key_value;

///////////////////////////////////////////////////////////////////////
// KEYBOARD LOGIC																		//
///////////////////////////////////////////////////////////////////////

always @(posedge system_clock) begin
	interupt_req															<=		1'b0;
	if(system_reset) begin
		key_req																<=		2'b00;
		key_toggle															<=		3'b000;
		key_extended														<=		2'b00;
		key_value_a															<=		8'd0;
		key_value_b															<=		8'd0;
		key_value															<=		8'd0;
	end else begin
		key_req																<=		{ key_req[0] , ps2_pressed };
		key_toggle															<=		{ key_toggle[1:0] , ps2_toggle };
		key_extended														<=		{ key_extended[0] , ps2_extended };
		key_value_a															<=		ps2_character;
		key_value_b															<=		key_value_a;
		if(~interupt_req) begin
			if((key_toggle[2:1]==2'b01) || (key_toggle[2:1]==2'b10)) begin
				if(key_req[1]) begin
					interupt_req												<=		1'b1;
					if(key_extended[1]) begin
						case(key_value_b)
						8'h75: begin
							key_value												<=		8'd17;		// DEVICE CONTROL 1 (UP ARROW KEY)
						end
						8'h6B: begin
							key_value												<=		8'd18;		// DEVICE CONTROL 2 (LEFT ARROW KEY)
						end
						8'h72: begin
							key_value												<=		8'd19;		// DEVICE CONTROL 3 (DOWN ARROW KEY)
						end
						8'h74: begin
							key_value												<=		8'd20;		// DEVICE CONTROL 4 (RIGHT ARROW KEY)
						end
						default: begin
							key_value												<=		8'd0;			// NULL
						end
						endcase
					end else begin
						case(key_value_b)
						8'h29: begin
							key_value												<=		8'd32;		//SPACEBAR
						end
						8'h16: begin
							key_value												<=		8'd49;		//1
						end
						8'h1E: begin
							key_value												<=		8'd50;		//2
						end
						8'h26: begin
							key_value												<=		8'd51;		//3
						end
						8'h25: begin
							key_value												<=		8'd52;		//4
						end
						8'h2E: begin
							key_value												<=		8'd53;		//5
						end
						8'h36: begin
							key_value												<=		8'd54;		//6
						end
						8'h3D: begin
							key_value												<=		8'd55;		//7
						end
						8'h3E: begin
							key_value												<=		8'd56;		//8
						end
						8'h46: begin
							key_value												<=		8'd57;		//9
						end
						8'h45: begin
							key_value												<=		8'd48;		//0
						end
						8'h4E: begin
							key_value												<=		8'd45;		//-
						end
						8'h55: begin
							key_value												<=		8'd61;		//=
						end
						8'h15: begin
							key_value												<=		8'd81;		//Q
						end
						8'h1D: begin
							key_value												<=		8'd87;		//W
						end
						8'h24: begin
							key_value												<=		8'd69;		//E
						end
						8'h2D: begin
							key_value												<=		8'd82;		//R
						end
						8'h2C: begin
							key_value												<=		8'd84;		//T
						end
						8'h35: begin
							key_value												<=		8'd89;		//Y
						end
						8'h3C: begin
							key_value												<=		8'd85;		//U
						end
						8'h43: begin
							key_value												<=		8'd73;		//I
						end
						8'h44: begin
							key_value												<=		8'd79;		//O
						end
						8'h4D: begin
							key_value												<=		8'd80;		//P
						end
						8'h54: begin
							key_value												<=		8'd91;		//[
						end
						8'h5B: begin
							key_value												<=		8'd93;		//]
						end
						8'h5D: begin
							key_value												<=		8'd92;		//\
						end
						8'h1C: begin
							key_value												<=		8'd65;		//A
						end
						8'h1B: begin
							key_value												<=		8'd83;		//S
						end
						8'h23: begin
							key_value												<=		8'd68;		//D
						end
						8'h2B: begin
							key_value												<=		8'd70;		//F
						end
						8'h34: begin
							key_value												<=		8'd71;		//G
						end
						8'h33: begin
							key_value												<=		8'd72;		//H
						end
						8'h3B: begin
							key_value												<=		8'd74;		//J
						end
						8'h42: begin
							key_value												<=		8'd75;		//K
						end
						8'h4B: begin
							key_value												<=		8'd76;		//L
						end
						8'h4C: begin
							key_value												<=		8'd59;		//;
						end
						8'h52: begin
							key_value												<=		8'd39;		//'
						end
						8'h5A: begin
							key_value												<=		8'd10;		//ENTER (CR)
						end
						8'h1A: begin
							key_value												<=		8'd90;		//Z
						end
						8'h22: begin
							key_value												<=		8'd88;		//X
						end
						8'h21: begin
							key_value												<=		8'd67;		//C
						end
						8'h2A: begin
							key_value												<=		8'd86;		//V
						end
						8'h32: begin
							key_value												<=		8'd66;		//B
						end
						8'h31: begin
							key_value												<=		8'd78;		//N
						end
						8'h3A: begin
							key_value												<=		8'd77;		//M
						end
						8'h41: begin
							key_value												<=		8'd44;		//,
						end
						8'h49: begin
							key_value												<=		8'd46;		//.
						end
						8'h4A: begin
							key_value												<=		8'd47;		///
						end
						8'h76: begin
							key_value												<=		8'd255;		//ESC
						end
						8'h05: begin
							key_value												<=		8'd01;		//F1
						end
						8'h06: begin
							key_value												<=		8'd02;		//F2
						end
						8'h04: begin
							key_value												<=		8'd03;		//F3
						end
						8'h0C: begin
							key_value												<=		8'd04;		//F4
						end
						8'h03: begin
							key_value												<=		8'd05;		//F5
						end
						8'h0B: begin
							key_value												<=		8'd06;		//F6
						end
						8'h83: begin
							key_value												<=		8'd07;		//F7
						end
						8'h0A: begin
							key_value												<=		8'd08;		//F8
						end
						8'h01: begin
							key_value												<=		8'd09;		//F9
						end
						8'h09: begin
							key_value												<=		8'd10;		//F10
						end
						8'h78: begin
							key_value												<=		8'd11;		//F11
						end
						8'h07: begin
							key_value												<=		8'd12;		//F12
						end
						8'h0E: begin
							key_value												<=		8'd126;		//~
						end
						default: begin
							key_value												<=		8'd0;			//NULL
						end
						endcase
					end
				end
			end
		end
	end
end

endmodule
