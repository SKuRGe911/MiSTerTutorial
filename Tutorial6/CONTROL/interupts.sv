`timescale 1ps/1ps
///////////////////////////////////////////////////////////////////////
// INTERUPT MODULE																	//
///////////////////////////////////////////////////////////////////////

module
interupts
(
	input				wire								system_reset,
	
	input				wire								interupt_00,
	output			wire								interupt_00_ack,
	input				wire								interupt_01,
	output			wire								interupt_01_ack,
	input				wire								interupt_02,
	output			wire								interupt_02_ack,
	input				wire								interupt_03,
	output			wire								interupt_03_ack,
	input				wire								interupt_04,
	output			wire								interupt_04_ack,
	input				wire								interupt_05,
	output			wire								interupt_05_ack,
	input				wire								interupt_06,
	output			wire								interupt_06_ack,
	input				wire								interupt_07,
	output			wire								interupt_07_ack,
	input				wire								interupt_08,
	output			wire								interupt_08_ack,
	input				wire								interupt_09,
	output			wire								interupt_09_ack,
	input				wire								interupt_10,
	output			wire								interupt_10_ack,
	input				wire								interupt_11,
	output			wire								interupt_11_ack,
	input				wire								interupt_12,
	output			wire								interupt_12_ack,
	input				wire								interupt_13,
	output			wire								interupt_13_ack,
	input				wire								interupt_14,
	output			wire								interupt_14_ack,
	input				wire								interupt_15,
	output			wire								interupt_15_ack,
	
	output			wire		[31:0]				cpu_interupt_out,
	input				wire		[31:0]				cpu_interupt_ack

);

assign													cpu_interupt_out			=	system_reset ? 32'd0 :
																								{	16'd0			,
																									interupt_15 ,
																									interupt_14 ,
																									interupt_13 ,
																									interupt_12 ,
																									interupt_11 ,
																									interupt_10 ,
																									interupt_09 ,
																									interupt_08 ,
																									interupt_07 ,
																									interupt_06 ,
																									interupt_05 ,
																									interupt_04 ,
																									interupt_03 ,
																									interupt_02 ,
																									interupt_01 ,
																									interupt_00 
																								};

assign													interupt_15_ack				=	cpu_interupt_ack[15];
assign													interupt_14_ack				=	cpu_interupt_ack[14];
assign													interupt_13_ack				=	cpu_interupt_ack[13];
assign													interupt_12_ack				=	cpu_interupt_ack[12];
assign													interupt_11_ack				=	cpu_interupt_ack[11];
assign													interupt_10_ack				=	cpu_interupt_ack[10];
assign													interupt_09_ack				=	cpu_interupt_ack[9];
assign													interupt_08_ack				=	cpu_interupt_ack[8];
assign													interupt_07_ack				=	cpu_interupt_ack[7];
assign													interupt_06_ack				=	cpu_interupt_ack[6];
assign													interupt_05_ack				=	cpu_interupt_ack[5];
assign													interupt_04_ack				=	cpu_interupt_ack[4];
assign													interupt_03_ack				=	cpu_interupt_ack[3];
assign													interupt_02_ack				=	cpu_interupt_ack[2];
assign													interupt_01_ack				=	cpu_interupt_ack[1];
assign													interupt_00_ack				=	cpu_interupt_ack[0];


endmodule
