///////////////////////////////////////////////////////////////////////
// GERANIUM DEMO CORE FOR MISTER DDRAM INTERFACE							//
///////////////////////////////////////////////////////////////////////

`timescale 1ps/1ps

module
ddram_ctrl
(
	input			wire								DDRAM_CLK,
	input			wire								DDRAM_BUSY,
	output		wire				[7:0]			DDRAM_BURSTCNT,
	output		wire				[28:0]		DDRAM_ADDR,
	input			wire				[63:0]		DDRAM_DOUT,
	input			wire								DDRAM_DOUT_READY,
	output		wire								DDRAM_RD,
	output		wire				[63:0]		DDRAM_DIN,
	output		wire				[7:0]			DDRAM_BE,
	output		wire								DDRAM_WE,

	input			wire				[26:0]		addr,
	output		wire				[31:0]		dout,
	input			wire				[31:0]		din,
	input			wire				[3:0]			din_byte_en,
	input			wire								req,
	input			wire								rw,
	output		logic								ack							=		1'b0,
	
	input			wire								debug
	
);


logic			[7:0]			ram_burst								=		8'b00000000;
logic			[7:0]			ram_byte_sel							=		8'b00000000;
logic			[63:0]		ram_q										=		64'd0;
logic			[63:0]		next_q									=		64'd0;
logic			[63:0]		ram_data									=		64'd0;
logic			[26:0]		ram_address								=		27'd0;
logic			[26:0]		cache_addr								=		27'd10000;
logic							ram_read									=		1'b0;
logic							ram_write								=		1'b0;

localparam	[26:0]		ram_address_inc						=		27'b000000000000000000000000010;

localparam	[1:0]
	state_idle										=	2'b00,
	state_write										=	2'b01,
	state_read1										=	2'b10,
	state_read2										=	2'b11;

logic			[1:0]			state										=		state_idle;

assign						DDRAM_BURSTCNT							=		ram_burst;
assign						DDRAM_BE									=		ram_byte_sel;
assign						DDRAM_ADDR								=		{6'b001100, ram_address[23:1]};
assign						DDRAM_RD									=		ram_read;
assign						DDRAM_DIN								=		ram_data;
assign						DDRAM_WE									=		ram_write;

assign						dout										=		addr[0]==1'b0 ? ram_q[31:0] : ram_q[63:32];

always @(posedge DDRAM_CLK) begin

	if(req==1'b0 && ack==1'b1) begin
		ack																<=		1'b0;
		if(debug) begin
			$display("DDRAM: ack negated - %t ps",$time);
		end
	end
	
	if(!DDRAM_BUSY) begin

		ram_write														<=		1'b0;
		ram_read															<=		1'b0;

		case(state)
		state_idle: begin 
			if(req==1'b1 && ack==1'b0 && rw==1'b1) begin
				ram_data													<=		{2{din}};
				ram_address												<=		addr;
				if(addr[0]==1'b0) begin
					ram_byte_sel										<=		{ 4'b0000 , din_byte_en };
				end else begin
					ram_byte_sel										<=		{ din_byte_en , 4'b0000 };
				end
				ram_write												<=		1'b1;
				ram_burst												<=		8'd1;
				state														<=		state_write;
				if(debug) begin
					$display("DDRAM: write req asserted - %t ps",$time);
				end
			end else if(req==1'b1 && ack==1'b0 && rw==1'b0) begin
				ram_byte_sel											<=		8'b11111111;
				if(cache_addr[26:1] == addr[26:1]) begin
					ack													<=		1'b1;
					if(debug) begin
						$display("DDRAM: read ack asserted - %t ps",$time);
					end
				end else if((cache_addr[26:1]+ram_address_inc[26:1]) == addr[26:1]) begin
					ack													<=		1'b1;
					ram_q													<=		next_q;
					cache_addr											<=		{ addr[26:1] , 1'b0 };
					ram_address											<=		{ addr[26:1] + ram_address_inc[26:1] , 1'b0 };
					ram_read												<=		1'b1;
					ram_burst											<=		8'd1;
					state													<=		state_read2;
					if(debug) begin
						$display("DDRAM: read req asserted (BURST 1) - %t ps",$time);
					end
				end else begin
					ram_address											<=		{ addr[26:1] , 1'b0 };
					cache_addr											<=		{ addr[26:1] , 1'b0 };
					ram_read												<=		1'b1;
					ram_burst											<=		8'd2;
					state													<=		state_read1;
					if(debug) begin
						$display("DDRAM: read req asserted (BURST 2) - %t ps",$time);
					end
				end
			end
		end
		state_write: begin
			ack															<=		1'b1;
			state															<=		state_idle;
			if(debug) begin
				$display("DDRAM: write ack asserted - %t ps",$time);
			end
		end
		state_read1: begin
			if(DDRAM_DOUT_READY) begin
				ram_q														<=		DDRAM_DOUT;
				ack														<=		1'b1;
				state														<=		state_read2;
				if(debug) begin
					$display("DDRAM: read ack asserted - %t ps",$time);
				end
			end
		end
		state_read2: begin
			if(DDRAM_DOUT_READY) begin
				next_q													<=		DDRAM_DOUT;
				state														<=		state_idle;
				if(debug) begin
					$display("DDRAM: read next completed - %t ps",$time);
				end
			end
		end
		endcase
	end
end

endmodule
