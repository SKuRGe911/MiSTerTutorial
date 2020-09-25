`timescale 1ps/1ps
////////////////////////////////////////////////////////
// GERANIUM SDRAM CONTROLLER									//
////////////////////////////////////////////////////////

module
sdram_init
(
	input								sd_clk,
	input								sd_rst,
	output	logic 	[3:0] 	sd_cmd				=		4'b1111,
	output 	logic		[12:0]	sd_a					=		13'd0,
	output	logic		[1:0]		sd_dqm				=		2'b11,
	output	logic					sd_cke				=		1'b0,
	output 							sd_rdy
);

localparam BURST_LENGTH1			=		3'b000;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTH2			=		3'b001;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTH4			=		3'b010;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTH8			=		3'b011;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTHC			=		3'b111;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam ACCESS_TYPE_SEQ			=		1'b0;		// 0=sequential, 1=interleaved
localparam ACCESS_TYPE_INT			=		1'b0;		// 0=sequential, 1=interleaved
localparam CAS_LATENCY2				=		3'd2;		// 2/3 allowed
localparam CAS_LATENCY3				=		3'd3;		// 2/3 allowed
localparam OP_MODE					=		2'b00;	// only 00 (standard operation) allowed
localparam WRITE_BURST_ON			=		1'b0;		// 0= write burst enabled, 1=only single access write
localparam WRITE_BURST_OFF			=		1'b1;		// 0= write burst enabled, 1=only single access write

// all possible commands
localparam CMD_INHIBIT				=		4'b1111;
localparam CMD_NOP					=		4'b0111;
localparam CMD_ACTIVE				=		4'b0011;
localparam CMD_READ					=		4'b0101;
localparam CMD_WRITE					=		4'b0100;
localparam CMD_BURST_TERMINATE	=		4'b0110;
localparam CMD_PRECHARGE			=		4'b0010;
localparam CMD_AUTO_REFRESH		=		4'b0001;
localparam CMD_LOAD_MODE			=		4'b0000;

parameter MODE							=		0;

logic [3:0] t							=		4'd0;
logic [4:0] reset						=		5'h1f;
logic		[15:0]								start_delay;
assign	sd_rdy						=		reset == 5'd0;

always @(posedge sd_clk) begin
	sd_cmd								<=		CMD_NOP;  // default: idle
	if(sd_rst) begin 
		t									<=		4'd0;
		reset								<=		5'h1f;
		sd_a								<=		13'd0;
		sd_dqm							<=		2'b11;
		sd_cke							<=		1'b0;
		start_delay						<=		16'h59D8;
	end else if (!sd_rdy) begin
		if(start_delay>16'd0) begin
			start_delay					<=				start_delay - 1'b1;
		end else begin
			sd_cke							<=			1'b1;
			t									<=		t + 4'd1;
			if(t==4'hF) begin 
				reset							<=		reset - 5'd1;		
			end
			if(t==4'h0) begin 
				if(reset == 13) begin
					$display("[SD_RESET] PRECHARGING ALL BANKS");
					sd_cmd					<=		CMD_PRECHARGE;
					sd_a[10] 				<= 	1'b1;
				end
				if(reset==11) begin
					$display("[SD_RESET] LOADING MODE");
					sd_cmd 					<=		CMD_LOAD_MODE;
					sd_a 						<=		MODE;
				end
				if(reset==9 || reset==7) begin
					$display("[SD_RESET] AUTOREFRESH");
					sd_cmd 					<=		CMD_AUTO_REFRESH;
				end
			end
		end
	end
end

endmodule

