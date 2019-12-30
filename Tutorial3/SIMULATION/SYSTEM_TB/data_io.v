`timescale 1ns/1ps
////////////////////////////////////////////////////////
// GERANIUM DEMO CORE FOR MIST FPGA BOARD					//
////////////////////////////////////////////////////////
// DATA_IO FROM MIST SOURCE									//
////////////////////////////////////////////////////////

module
data_io
(
	////////////////////////////////////////////////////////
	output wire						downloading,   // signal indicating an active download
   output reg [4:0]				index= 5'b00000,     // menu index used to upload the file
	////////////////////////////////////////////////////////
	input wire						rst,
	input wire 						clk,
	output reg						wr=1'b0,
	output wire [24:0]			addr,
	output wire [15:0]			data,
	input	wire						dwait
	////////////////////////////////////////////////////////

);

parameter ROM_SIZE	=		25'h200;

reg download=1'b0;
reg [24:0] read_addr=25'd0;
wire [15:0] read_data;
assign data_debug_out = read_data;
assign addr_debug_out = read_addr;
assign downloading = download;
assign data = read_data;
assign addr = read_addr;

always @(posedge clk) begin
	if(rst==1'b1) begin
		index = 5'b00000;
		download <= 1'b1;
		read_addr <= 25'd0;
		wr <= 1'b0;
	end else if(download==1'b1) begin
		if(read_addr>=ROM_SIZE) begin
			wr <= 1'b0;
			download <= 1'b0;
		end else begin
			if(dwait==1'b0) begin
				if(wr==1'b0) begin
					wr <= 1'b1;
				end else begin
					if(wr==1'b1) begin
						wr				<=		1'b0;
						read_addr	<= 	read_addr + 25'd2;
					end
				end
			end else begin
				wr						<=		1'b0;
			end
		end
	end

end

data_io_rom
data_io_rom
(
	.address({7'd0 , read_addr}),
	.clock(clk),
	.q(read_data)
	
);

endmodule
