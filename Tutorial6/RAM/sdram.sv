////////////////////////////////////////////////////////
// GERANIUM SDRAM CONTROLLER									//
////////////////////////////////////////////////////////

`timescale 10ns / 1ps

module
sdram
(
	// INTERFACE TO THE MT48LC16M16 CHIP
`ifdef REAL_BUILD
	inout 	trireg			[15:0]  				sd_data,    				// 16 BIT BIDIRECTIONAL DATA BUS
`else
	inout 	wire				[15:0]  				sd_data,    				// 16 BIT BIDIRECTIONAL DATA BUS
`endif
	output 	wire				[12:0]				sd_addr,    				// 13 BIT MULTIPLEXED ADDRESS BUS
	output 	wire				[1:0]					sd_dqm,     				// TWO BYTE MASKS
	output 	wire				[1:0] 				sd_ba,      				// TWO BANKS
	output 	wire										sd_cs,      				// A SINGLE CHIP SELECT
	output 	wire										sd_we,      				// WRITE ENABLE
	output 	wire										sd_ras,     				// ROW ADDRESS SELECT
	output 	wire										sd_cas,     				// COLUMNS ADDRESS SELECT
	output	wire										sd_cke,						// CLOCK ENABLE

	// SYSTEM
	input		wire										ram_clock,					// SDRAM CLOCK
	input		wire										reset,						// ASYNCRONOUS RESET (HIGH ASSERT, LOW NEGATE)
	output	wire										ready,						// SDRAM IS READY FOR NORMAL OPERATION (HIGH ASSERTED, LOW NEGATED)
	input		wire										debug,						// HIGH WHEN DISPLAY MSG ACTIVE

	// DMA INTERFACE
	input		wire										dma_req,						// DMA REQUEST OPERATION
	output	logic										dma_ack=1'b0,				// DMA ACKNOWLEGE (HIGH ASSERTED, LOW NEGATED)
	output	logic										dma_nack=1'b0,				// DMA NOT ACKNOWLEGE (HIGH ASSERTED, LOW NEGATED)
	output	wire										dma_fill,					// DMA FILL (HIGH ASSERTED, LOW NEGATED)
	input		wire				[31:0]				dma_size,					// DMA SIZE REQUEST
	input		wire				[27:0]				dma_addr,					// DMA ADDRESS
	output	wire				[15:0]				dma_data,					// DMA DATA OUT
	
	// SYSTEM INTERFACE
	input		wire										sys_req,
	output	wire										sys_ack,
	input		wire				[27:0]				sys_addr,
	input		wire				[31:0]				sys_data,
	input		wire				[3:0]					sys_dqm,
	input		wire										sys_rw
	
);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// THE SDRAM CONTROLLER IS A BIT COMPLICATED. I GOT THE INSPIRATION FROM OTHERS WORKS. IT RUNS ON	//
// A STATE MACHINE AND HAS 2 SLOTS AVAILABLE FOR USE BEING MUTUALLY EXCLUSIVE FROM THE SAME USER	//
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 150MHz - 6.666ns PERIOD CHIP DEFAULTS AND TIMINGS																//
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
localparam		tRC			=		60;			//10
localparam		tRFC			=		60;			//10
localparam		tRCD			=		18;			//3
localparam		tRP			=		18;			//3
localparam		tRRD			=		12;			//2
localparam		tMRD			=		12;			//2
localparam		tRAS_MIN		=		42;			//7
localparam		tRAS_MAX		=		120000;		//18000
localparam		tWR			=		12;			//2
localparam		tCK_CL2		=		10;			//2
localparam		tCK_CL3		=		6;				//1
localparam		tCH			=		2;
localparam		tCL			=		2;
localparam		tAC_CL2		=		6;				//1
localparam		tAC_CL3		=		5;				//1
localparam		tOH			=		2.5;
localparam		tLZ			=		0;
localparam		tHZ			=		5;
localparam		tIS			=		1.5;
localparam		tIH			=		0.8;
localparam		tPDE			=		tIS + tCK_CL3;
localparam		tREFI			=		7.8;
localparam		tXSR			=		tRC + tIS;
/////////////////////////////////////////////////////////////////////////////////////////////////////

localparam BURST_LENGTH1	=	3'b000;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTH2	=	3'b001;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTH4	=	3'b010;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTH8	=	3'b011;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam BURST_LENGTHC	=	3'b111;	// 000=1, 001=2, 010=4, 011=8, 111 = continuous.
localparam ACCESS_TYPE_SEQ	=	1'b0;		// 0=sequential, 1=interleaved
localparam ACCESS_TYPE_INT	=	1'b1;		// 0=sequential, 1=interleaved
localparam CAS_LATENCY2		=	3'd2;		// 2/3 allowed
localparam CAS_LATENCY3		=	3'd3;		// 2/3 allowed
localparam OP_MODE			=	2'b00;	// only 00 (standard operation) allowed
localparam WRITE_BURST_ON	=	1'b0;		// 0= write burst enabled, 1=only single access write
localparam WRITE_BURST_OFF	=	1'b1;		// 0= write burst enabled, 1=only single access write

// all possible commands
localparam CMD_INHIBIT         = 4'b1111;
localparam CMD_NOP             = 4'b0111;
localparam CMD_ACTIVE          = 4'b0011;
localparam CMD_READ            = 4'b0101;
localparam CMD_WRITE           = 4'b0100;
localparam CMD_BURST_TERMINATE = 4'b0110;
localparam CMD_PRECHARGE       = 4'b0010;
localparam CMD_AUTO_REFRESH    = 4'b0001;
localparam CMD_LOAD_MODE       = 4'b0000;

////////////////////////////////////////////////////////////////////////////////
//SDRAM MODE																						//
////////////////////////////////////////////////////////////////////////////////

localparam MODE									=	{ 3'b000, WRITE_BURST_ON, OP_MODE, CAS_LATENCY3, ACCESS_TYPE_SEQ, BURST_LENGTH8 }; 

////////////////////////////////////////////////////////////////////////////////
//STATE VALUES																						//
////////////////////////////////////////////////////////////////////////////////

localparam [3:0]
	ph0 												=	4'b0000,
	ph1 												=	4'b0001,
	ph2 												=	4'b0010,
	ph3 												=	4'b0011,
	ph4												=	4'b0100,
	ph5 												=	4'b0101,
	ph6 												=	4'b0110,
	ph7 												=	4'b0111,
	ph8 												=	4'b1000,
	ph9 												=	4'b1001,
	ph10 												=	4'b1010,
	ph11 												=	4'b1011,
	ph12 												=	4'b1100,
	ph13 												=	4'b1101,
	ph14 												=	4'b1110,
	ph15 												=	4'b1111;

logic	[3:0]		ram_state;

// PORT SLOTS
localparam [2:0]
	REFRESH											= 	3'b000,		// REFRESH
	DMA												= 	3'b001,		// DMA PORT
	PORT0												= 	3'b010,		// PORT0
	PORT1												= 	3'b011,		// PORT1
	PORT2												= 	3'b100,		// PORT2
	PORT3												=	3'b101,		// PORT3
	WRITECACHE										=	3'b110,		// WRITE CACHE
	IDLE												= 	3'b111;		// SLOT IS IDLE

//SLOT 1
logic	[2:0] 			slot1_type;
logic	[1:0]				slot1_bank;
logic						slot1_fill;
logic						slot1_rw;
logic	[3:0]				slot1_count;

//SLOT 2
logic	[2:0] 			slot2_type;
logic	[1:0]				slot2_bank;
logic						slot2_fill;
logic						slot2_rw;
logic	[3:0]				slot2_count;

//INITIALIZATION
wire	[3:0]				sd_init_cmd;
wire	[12:0]			sd_init_addr;
wire	[1:0]				sd_init_dqm;
wire						sd_init_cke;

// CURRENT COMMAND SENT TO SDRAM
logic	[3:0] 			ram_cmd;

// DRIVE CONTROL SIGNALS ACCORDING TO CURRENT COMMAND
assign 					sd_cs						=	ready	?	ram_cmd[3]	:	sd_init_cmd[3];
assign					sd_ras					=	ready	?	ram_cmd[2]	:	sd_init_cmd[2];
assign					sd_cas					=	ready	?	ram_cmd[1]	:	sd_init_cmd[1];
assign					sd_we						=	ready	?	ram_cmd[0]	:	sd_init_cmd[0];
assign					sd_cke					=	ready	?	1'b1			:	sd_init_cke;

// DATA DRIVE
logic	[15:0]			ram_data_to_ram;					// DATA GOING INTO THE SDRAM
logic	[15:0]			ram_data_from_ram;				// DATA COMMING OUT OF THE SDRAM
logic						ram_write;							// DATA WRITE TO SDRAM FLAG

assign					dma_data					=		ram_data_from_ram;

////////////////////////////////////////////////////////////////////////////////
// DRIVE RAM INPUTS AND OUTPUTS																//
////////////////////////////////////////////////////////////////////////////////

`ifdef REAL_BUILD

/////////////////////////////////////////////////////////////////////////////////////////////////////
always @(ram_clock,ram_write,ram_data_to_ram) begin
	sd_data													<=			16'bZZZZZZZZZZZZZZZZ;
	if(ram_write) begin
		sd_data												<=			ram_data_to_ram;
	end else begin
		ram_data_from_ram									<=			sd_data;
	end
end
/////////////////////////////////////////////////////////////////////////////////////////////////////

`else

/////////////////////////////////////////////////////////////////////////////////////////////////////
assign					sd_data							=			ram_write ? ram_data_to_ram : 16'bZZZZZZZZZZZZZZZZ;
/////////////////////////////////////////////////////////////////////////////////////////////////////
always @(posedge ram_clock) begin
	ram_data_from_ram										<=			sd_data;
end
/////////////////////////////////////////////////////////////////////////////////////////////////////

`endif

// RAM READY
logic						ram_ready					=	1'b0;

// RAM CONTROL
logic	[12:0]					ram_addr;
logic	[1:0]						ram_bank;
logic	[1:0]						ram_dqm;

assign sd_addr											=	ready	?	ram_addr	:	sd_init_addr;
assign sd_ba											=	ram_bank;
assign sd_dqm											=	ready	?	ram_dqm : sd_init_dqm;

// RAM REFRESH CONTROL
logic			[9:0]						refreshcnt			=	10'd0;
localparam	[9:0]						refreshcntmax		=	10'b1111111111;
logic										auto_refresh		=	1'b0;

// HOLDING REGISTERS
logic	[27:0]					cas_addr				=	28'd0;

// WRITE CACHE
localparam
		WC_WAITWRITE				=	1'b0,
		WC_FILL						=	1'b1;

logic					writecache_state			=	WC_WAITWRITE;
logic	[27:0]		writecache_addr			=	28'd100;
logic	[15:0]		writecache_word0;
logic	[15:0]		writecache_word1;
logic	[3:0]			writecache_dqm				=	4'b1111;
logic					writecache_req				=	1'b0;
logic					writecache_dirty			=	1'b0;
logic					writecache_dtack			=	1'b0;
logic					writecache_burst			=	1'b0;

assign				dma_fill						=	((slot1_fill && slot1_count>4'b0000 && slot1_type==DMA) || (slot2_fill && slot2_count>4'b0000 && slot2_type==DMA)) ? 1'b1 : 1'b0;

//RAM INITIALIZATION

sdram_init
#(
	.MODE(MODE)
)
INIT
(
	
	.sd_clk			(	ram_clock								),
	.sd_rst			(	reset										),
	.sd_cmd			(	sd_init_cmd								),
	.sd_a				(	sd_init_addr							),
	.sd_dqm			(	sd_init_dqm								),
	.sd_cke			(	sd_init_cke								),
	.sd_rdy			(	ready										)

);

always @(posedge ram_clock) begin

	ram_cmd											<=		CMD_INHIBIT;	// DEFAULT: IDLE
	ram_write										<=		1'b0;
	ram_bank											<=		2'b00;
	ram_dqm											<=		2'b00;

	dma_ack											<=		1'b0;
	dma_nack											<=		1'b0;
	slot1_count										<=		slot1_fill ? slot1_count > 4'b0000 ? slot1_count - 4'b0001 : 4'b0000 : slot1_count;
	slot2_count										<=		slot2_fill ? slot2_count > 4'b0000 ? slot2_count - 4'b0001 : 4'b0000 : slot2_count;
	
	if(ram_ready) begin
	
		// REFRESH COUNTER
		if(~|refreshcnt) begin 
			auto_refresh							<=		1'b1;
		end else begin
			refreshcnt								<=		refreshcnt - 10'd1;
		end
		
		//////////////////////////////////////////////////////////////////////////
		case(ram_state)
		ph0 : begin

		end
		ph1 : begin

			//SLOT2
			if(slot2_type==WRITECACHE) begin
				ram_addr						<=		{ 4'b0000 , cas_addr[9:1] }; //NO AUTO PRECHARGE
				ram_bank						<=		slot2_bank;
				ram_cmd						<=		CMD_WRITE;
				ram_write					<=		1'b1;
				ram_data_to_ram			<=		writecache_word0;
				ram_dqm						<=		writecache_dqm[1:0];
			end

		end
		ph2 : begin

			//SLOT2
			if(slot2_type==WRITECACHE) begin
				ram_write					<=		1'b1;
				ram_data_to_ram			<=		writecache_word1;
				ram_dqm						<=		writecache_dqm[3:2];
				writecache_burst			<=		1'b0;
			end
		
			//////////////////////////////////////////////////////////////////////////
			//SLOT1 ALLOC																				//
			//////////////////////////////////////////////////////////////////////////
			
			// AUTO REFRESH
			if(auto_refresh && slot2_type==IDLE) begin
				slot1_type					<=	REFRESH;
				ram_cmd						<=	CMD_AUTO_REFRESH;
				refreshcnt          		<= refreshcntmax;
				auto_refresh				<=	1'b0;
				dma_nack						<=	1'b1;
				if(debug) begin
					$display("[SDRAM] SLOT1 ALLOC REFRESH AT %t",$time);
				end
			end else
			
			// DMA READ
			if(dma_req && slot2_type!=DMA && (dma_addr[24:23]!=slot2_bank || slot2_type==IDLE)) begin
				slot1_type					<=	DMA;
				ram_addr						<=	dma_addr[22:10];
				ram_bank						<=	dma_addr[24:23];
				slot1_bank					<=	dma_addr[24:23];
				slot1_count					<=	9'b111111111 - dma_addr[9:1] < 9'd8 ? (4'b1111 - dma_addr[4:1]) + 4'b0001 : 4'b1000;
				cas_addr						<=	dma_addr;
				ram_cmd						<=	CMD_ACTIVE;
				dma_ack						<=	1'b1;
				slot1_rw						<=	1'b0;
				if(debug) begin
					$display("[SDRAM] SLOT1 ALLOC DMA READ AT %t",$time);
				end
			end else
			
			//CPU WRITE CACHE
			if(writecache_req && slot2_type!=WRITECACHE && (writecache_addr[24:23]!=slot2_bank || slot2_type==IDLE)) begin
				slot1_type					<= WRITECACHE;
				ram_addr						<= writecache_addr[22:10];
				ram_bank						<=	writecache_addr[24:23];
				slot1_bank					<=	writecache_addr[24:23];
				cas_addr						<= writecache_addr;
				ram_cmd						<=	CMD_ACTIVE;
				dma_nack						<=	1'b1;
				slot1_rw						<=	1'b1;
				if(debug) begin
					$display("[SDRAM] SLOT1 ALLOC WRITE CACHE AT %t",$time);
				end
			end else
			
			//IDLE
			begin
				slot1_type					<= IDLE;
				ram_cmd						<= CMD_NOP;
			end

		end
		ph3 : begin
		
			//SLOT2
			if(slot2_type==WRITECACHE) begin
				ram_dqm						<=		2'b11;
			end

		end
		ph4 : begin
		
			//SLOT2
			if(slot2_type==WRITECACHE) begin
				ram_dqm						<=		2'b11;
				ram_cmd						<=		CMD_PRECHARGE;
				ram_addr[10]				<=		1'b0;
				ram_bank						<=		slot2_bank;
			end

		end
		ph5 : begin

			//SLOT1
			if(slot1_type==DMA) begin
				ram_addr						<=		{ 4'b0010 , cas_addr[9:1] }; //AUTO PRECHARGE
				ram_bank						<=		slot1_bank;
				ram_cmd						<=		CMD_READ;
			end
		
		end
		ph6 : begin
			
		end
		ph7 : begin
			
			//SLOT1
			if(slot1_type==WRITECACHE) begin
				writecache_burst			<=		1'b1;
			end
			
		end
		ph8 : begin
			
		end
		ph9 : begin
			
			//SLOT1
			if(slot1_type==WRITECACHE) begin
				ram_addr						<=		{ 4'b0000 , cas_addr[9:1] }; //NO AUTO PRECHARGE
				ram_bank						<=		slot1_bank;
				ram_write					<=		1'b1;
				ram_data_to_ram			<=		writecache_word0;
				ram_dqm						<=		writecache_dqm[1:0];
				ram_cmd						<=		CMD_WRITE;
			end
			
		end
		ph10 : begin

			//SLOT1
			if(slot1_type==WRITECACHE) begin
				ram_write					<=		1'b1;
				ram_data_to_ram			<=		writecache_word1;
				ram_dqm						<=		writecache_dqm[3:2];
				writecache_burst			<=		1'b0;
			end

			//////////////////////////////////////////////////////////////////////////
			//SLOT2 ALLOC																				//
			//////////////////////////////////////////////////////////////////////////
			
			if(auto_refresh || slot1_type==REFRESH) begin
				slot2_type					<=	IDLE;
				dma_nack						<=	1'b1;
				if(debug) begin
					$display("[SDRAM] SLOT2 ALLOC REFRESH AT %t",$time);
				end
			end else

			// DMA READ
			if(dma_req && slot1_type!=DMA && (dma_addr[24:23]!=slot1_bank || slot1_type==IDLE)) begin
				slot2_type					<=	DMA;
				ram_addr						<=	dma_addr[22:10];
				ram_bank						<=	dma_addr[24:23];
				slot2_bank					<=	dma_addr[24:23];
				slot2_count					<=	9'b111111111 - dma_addr[9:1] < 9'd8 ? (4'b1111 - dma_addr[4:1]) + 4'b0001 : 4'b1000;
				cas_addr						<=	dma_addr;
				ram_cmd						<=	CMD_ACTIVE;
				dma_ack						<=	1'b1;
				slot2_rw						<=	1'b0;
				if(debug) begin
					$display("[SDRAM] SLOT2 ALLOC DMA READ AT %t",$time);
				end
			end else
			
			//CPU WRITE CACHE
			if(writecache_req && slot1_type!=WRITECACHE && (writecache_addr[24:23]!=slot1_bank || slot1_type==IDLE)) begin
				slot2_type					<= WRITECACHE;
				ram_addr						<= writecache_addr[22:10];
				ram_bank						<=	writecache_addr[24:23];
				slot2_bank					<=	writecache_addr[24:23];
				cas_addr						<= writecache_addr;
				ram_cmd						<=	CMD_ACTIVE;
				dma_nack						<=	1'b1;
				slot2_rw						<=	1'b1;
				if(debug) begin
					$display("[SDRAM] SLOT2 ALLOC WRITE CACHE AT %t",$time);
				end
			end else
			
			//IDLE
			begin
				slot2_type					<= IDLE;
				ram_cmd						<= CMD_NOP;
				dma_nack						<=	1'b1;
			end

		end
		ph11 : begin

			//SLOT1
			if(slot1_type==WRITECACHE) begin
				ram_dqm						<=		2'b11;
			end
			
		end
		ph12 : begin
			
			//SLOT1
			if(slot1_type==WRITECACHE) begin
				ram_dqm						<=		2'b11;
				ram_bank						<=		slot1_bank;
				ram_addr[10]				<=		1'b0;
				ram_cmd						<=		CMD_PRECHARGE;
			end

		end
		ph13 : begin
			
			//SLOT2
			if(slot2_type==DMA) begin
				ram_addr						<=		{ 4'b0010 , cas_addr[9:1] }; //AUTO PRECHARGE
				ram_bank						<=		slot2_bank;
				ram_dqm						<=		2'b00;
				ram_cmd						<=		CMD_READ;
			end

		end
		ph14 : begin

		end
		ph15 : begin
			
			if(slot2_type==WRITECACHE) begin
				writecache_burst			<=		1'b1;  // CLOSE THE DOOR ON NEW WRITE DATA
			end
			
		end
		endcase
		//////////////////////////////////////////////////////////////////////////
	end
end

////////////////////////////////////////////////////////////////////////////////
// WRITE CACHE																						//
////////////////////////////////////////////////////////////////////////////////

assign							sys_ack			=		writecache_dtack;

always @(posedge ram_clock) begin
	if(~ram_ready) begin
		writecache_req								<=	1'b0;
		writecache_dirty							<=	1'b0;
		writecache_dqm								<=	4'b1111;
		writecache_state							<=	WC_WAITWRITE;
		writecache_dtack							<=	1'b0;
	end else begin
		if(~sys_req && writecache_dtack) begin
			writecache_dtack										<=		1'b0;
		end
		case(writecache_state)
		WC_WAITWRITE: begin
			if(sys_req && sys_rw && ~writecache_dtack) begin
				if(writecache_dirty==1'b0 || sys_addr[27:2]==writecache_addr[27:2]) begin
					writecache_addr						<=		sys_addr;
					if(sys_dqm[0]) begin
						writecache_word0[7:0]			<=		sys_data[7:0];
						writecache_dqm[0]					<=		1'b0;
					end
					if(sys_dqm[1]) begin
						writecache_word0[15:8]			<=		sys_data[15:8];
						writecache_dqm[1]					<=		1'b0;
					end
					if(sys_dqm[2]) begin
						writecache_word1[7:0]			<=		sys_data[23:16];
						writecache_dqm[2]					<=		1'b0;
					end
					if(sys_dqm[3]) begin
						writecache_word1[15:8]			<=		sys_data[31:24];
						writecache_dqm[3]					<=		1'b0;
					end
					writecache_req									<=		1'b1;
					writecache_dirty								<=		1'b1;
					writecache_dtack								<=		1'b1;
				end
			end
			if(writecache_burst && writecache_dirty) begin
				writecache_req										<=		1'b0;
				writecache_state									<=		WC_FILL;
			end
		end
		WC_FILL: begin
			if(~writecache_burst) begin
				writecache_dirty								<=		1'b0;
				writecache_dqm									<=		4'b1111;
				writecache_state								<=		WC_WAITWRITE;
			end
		end
		endcase
	end
end

////////////////////////////////////////////////////////////////////////////////
// RAM STATE MACHINE																				//
////////////////////////////////////////////////////////////////////////////////

logic		ram_init_done										=		1'b0;

always @(posedge ram_clock) begin
	if(ready && ~ram_init_done) begin
		ram_ready							<=			1'b1;
		ram_init_done						<=			1'b1;
		if(debug) begin
			$display("[SDRAM] RAM READY AT %t",$time);
		end
	end else if(~ready) begin
		ram_ready							<=			1'b0;
		ram_init_done						<=			1'b0;
	end
	if(~ram_ready) begin
		ram_state							<=			ph2;
		slot1_fill							<=			1'b0;
		slot2_fill							<=			1'b0;
	end else begin
		ram_state							<=			ram_state + 4'd1;
		case(ram_state)
		ph0: begin
`ifndef REAL_BUILD
			slot1_fill						<=			1'b0;
			slot2_fill						<=			1'b1;
`endif
		end
		ph1: begin
`ifdef REAL_BUILD
			slot1_fill						<=			1'b0;
			slot2_fill						<=			1'b1;
`endif
		end
		ph2: begin
		
		end
		ph3: begin
		
		end
		ph4: begin

		end
		ph5: begin
		
		end
		ph6: begin
		
		end
		ph7: begin

		end
		ph8: begin
`ifndef REAL_BUILD
			slot1_fill						<=			1'b1;
			slot2_fill						<=			1'b0;
`endif
		end
		ph9: begin
`ifdef REAL_BUILD
			slot1_fill						<=			1'b1;
			slot2_fill						<=			1'b0;
`endif
		end
		ph10: begin

		end
		ph11: begin

		end
		ph12: begin

		end
		ph13: begin

		end
		ph14: begin
		
		end
		ph15: begin
		
		end
		endcase
	end
end

endmodule
