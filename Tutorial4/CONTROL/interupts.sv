`timescale 1ps/1ps
///////////////////////////////////////////////////////////////////////
// INTERUPT MODULE																	//
///////////////////////////////////////////////////////////////////////

module
interupts
(
	input				wire								system_reset,
	
	input				wire								interupt_a,
	output			wire								interupt_a_ack,
	input				wire								interupt_b,
	output			wire								interupt_b_ack,
	input				wire								interupt_c,
	output			wire								interupt_c_ack,
	input				wire								interupt_d,
	output			wire								interupt_d_ack,
	input				wire								interupt_e,
	output			wire								interupt_e_ack,
	input				wire								interupt_f,
	output			wire								interupt_f_ack,
	input				wire								interupt_g,
	output			wire								interupt_g_ack,
	input				wire								interupt_h,
	output			wire								interupt_h_ack,
	
	output			wire		[31:0]				cpu_interupt_out,
	input				wire		[31:0]				cpu_interupt_ack

);

assign													cpu_interupt_out			=	system_reset ? 32'd0 :
																								{	interupt_h ,
																									interupt_g ,
																									interupt_f ,
																									interupt_e ,
																									interupt_d ,
																									interupt_c ,
																									interupt_b ,
																									interupt_a ,
																									24'd0				};

assign													interupt_a_ack				=	cpu_interupt_ack[24];
assign													interupt_b_ack				=	cpu_interupt_ack[25];
assign													interupt_c_ack				=	cpu_interupt_ack[26];
assign													interupt_d_ack				=	cpu_interupt_ack[27];
assign													interupt_e_ack				=	cpu_interupt_ack[28];
assign													interupt_f_ack				=	cpu_interupt_ack[29];
assign													interupt_g_ack				=	cpu_interupt_ack[30];
assign													interupt_h_ack				=	cpu_interupt_ack[31];


endmodule
