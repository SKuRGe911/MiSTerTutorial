///////////////////////////////////////////////////////////////////////
// GERANIUM DEMO CORE FOR MISTER													//
///////////////////////////////////////////////////////////////////////

module
other_io
(
	input		wire									hardware_reset,
	input		wire									system_reset,
	input		wire									system_clk,

	input		wire		[27:0]					hardware_addr,
	input		wire		[31:0]					hardware_data_in,
	output	wire		[31:0]					hardware_data_out,
	input		wire		[3:0]						hardware_data_sel,
	input		wire									hardware_write,
	input		wire									hardware_ready,
	output	wire									hardware_ack,
	
	input		wire									boot_phase_mod,
	output	logic									boot_phase_done					=		1'b0,
	output	logic									boot_phase_ready					=		1'b0,
	output	logic		[4:0]						counter_sel							=		5'b00100,
	output	logic									phase_enable						=		1'b0,
	input		wire									phase_done,
	output	logic									phase_up_down						=		1'b1,
	
	output	logic									pll_reset							=		1'b0,
	input		wire									pll_locked

);

///////////////////////////////////////////////////////////////////////
// IO SELECTORS																		//
///////////////////////////////////////////////////////////////////////

localparam									PHASE_SHIFT_SEL							=		7'b0000000;
localparam									PLL_RESET_SEL								=		7'b0000001;

///////////////////////////////////////////////////////////////////////
// VARIABLES																			//
///////////////////////////////////////////////////////////////////////

wire											sel_phase_shift;
wire											sel_pll_reset;

logic			[31:0]						phase_shift_data							=			32'd0;
logic			[15:0]						phase_shift_up								=			16'd0;
logic			[15:0]						phase_shift_down							=			16'd0;
logic											phase_shift_ack							=			1'b0;
logic			[1:0]							phase_counter								=			2'b11;

logic											do_pll_reset								=			1'b0;
logic											pll_reset_ack								=			1'b0;

logic			[2:0]							boot_phase_count							=			3'b111;
logic											boot_phase_started						=			1'b0;
logic											boot_phase_ended							=			1'b0;

///////////////////////////////////////////////////////////////////////
// ASSIGNMENTS																			//
///////////////////////////////////////////////////////////////////////

assign										hardware_ack					=		phase_shift_ack | pll_reset_ack;
assign										hardware_data_out				=		phase_shift_ack ? phase_shift_data :
																							pll_reset_ack ? 32'd0 :
																							32'd0;

///////////////////////////////////////////////////////////////////////
// PHASE SHIFT																			//
///////////////////////////////////////////////////////////////////////

assign										sel_phase_shift				=		hardware_addr[6:0]==PHASE_SHIFT_SEL ? 1'b1 : 1'b0;
																							
always @(posedge system_clk) begin
	if(system_reset || ~pll_locked) begin
		phase_enable															<=			1'b0;
		phase_counter															<=			2'b11;
		boot_phase_started													<=			1'b0;
		boot_phase_ready														<=			1'b0;
	end else begin
		boot_phase_ready														<=			1'b1;
		if(phase_counter<2'b10) begin
			phase_counter														<=			phase_counter + 2'b01;
		end else begin
			phase_enable														<=			1'b0;
		end
		if(boot_phase_mod) begin
			boot_phase_started												<=			1'b1;
			boot_phase_ended													<=			1'b0;
			boot_phase_done													<=			1'b0;
			boot_phase_count													<=			3'b111;
		end
		if(boot_phase_started && phase_done && ~boot_phase_ended && ~boot_phase_mod && ~phase_enable) begin
			counter_sel															<=			5'b00001;
			phase_enable														<=			1'b1;
			phase_counter														<=			2'b00;
			phase_up_down														<=			1'b0;
			boot_phase_count													<=			boot_phase_count - 1'b1;
			if(boot_phase_count==3'b000) begin
				boot_phase_ended												<=			1'b1;
				boot_phase_done												<=			1'b1;
			end
		end
		if(boot_phase_done) begin
			if(hardware_ready) begin
				if(~phase_shift_ack) begin
					if(sel_phase_shift) begin
						if(hardware_write) begin
							if(phase_done) begin
								phase_shift_ack										<=		1'b1;
								counter_sel												<=		hardware_data_in[4:0];
								phase_enable											<=		1'b1;
								phase_counter											<=		2'b00;
								phase_up_down											<=		hardware_data_in[5];
								if(hardware_data_in[5]) begin
									phase_shift_up										<=		phase_shift_up + 1'b1;
								end else begin
									phase_shift_down									<=		phase_shift_down + 1'b1;
								end
							end
						end else begin
							phase_shift_ack											<=		1'b1;
							phase_shift_data											<=		{ phase_shift_up , phase_shift_down };
						end
					end
				end
			end else begin
				phase_shift_ack																	<=		1'b0;
			end
		end
	end
end

///////////////////////////////////////////////////////////////////////
// PHASE SHIFT																			//
///////////////////////////////////////////////////////////////////////

assign										sel_pll_reset				=		hardware_addr[6:0]==PLL_RESET_SEL ? 1'b1 : 1'b0;
																							
always @(posedge system_clk) begin
	do_pll_reset															<=		1'b0;
	if(hardware_ready) begin
		if(~pll_reset_ack) begin
			if(sel_pll_reset) begin
				pll_reset_ack												<=		1'b1;
				if(hardware_write) begin
					do_pll_reset											<=		hardware_data_in[0];
				end
			end
		end
	end else begin
		pll_reset_ack														<=		1'b0;
	end
end

always @(posedge do_pll_reset,negedge pll_locked) begin
	if(~pll_locked) begin
		pll_reset															<=		1'b0;
	end else begin
		pll_reset															<=		1'b1;
	end
end

endmodule
