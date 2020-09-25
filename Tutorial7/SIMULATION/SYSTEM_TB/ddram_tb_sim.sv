`timescale 1ps/1ps

module
ddram_tb_sim
(
	input		wire										clk_ram,
	output	wire										mem_wait_request,
	input		wire										mem_write,
	input		wire										mem_read,
	input		wire		[28:0]						mem_address,
	input		wire		[7:0]							mem_byte_enable,
	input		wire		[7:0]							mem_burst_count,
	output	wire		[63:0]						mem_data_out,
	output	wire										mem_read_data_valid,
	input		wire		[63:0]						mem_data_in,
	input		wire										reset,
	input		wire										debug

);

//TB DDR3 RAM
`define NUM_MASTERS  1
`define NUM_SLAVES   1

`define SLAVE0    avalonslavetestbench.mm_slave_bfm_0

import verbosity_pkg::*;
import avalon_mm_pkg::*;

parameter mem_start =   805306368;
parameter mem_end =   1073741823;

//---------------------------------------------------
// Constants
//---------------------------------------------------
localparam ADDR_W                   = 32;
			
localparam SYMBOL_W                 = 8;
localparam NUM_SYMBOLS              = 8;
localparam DATA_W                   = NUM_SYMBOLS * SYMBOL_W;
			
localparam BURST_W                  = 8;
localparam MAX_BURST                = 8;

localparam SLAVE_SPAN               = 32'h1000;

localparam MAX_COMMAND_IDLE         = 5;
localparam MAX_COMMAND_BACKPRESSURE = 2;
localparam MAX_DATA_IDLE            = 3;

//---------------------------------------------------
// Data structures
//---------------------------------------------------
typedef logic [BURST_W-1:0]      Burstcount;

typedef enum bit
{
	 WRITE = 0,
	 READ  = 1
} Transaction;

typedef enum bit
{
	 NOBURST = 0,
	 BURST   = 1
} Burstmode;

typedef struct 
{
	 Transaction                  trans;
	 Burstcount                   burstcount;
	 logic [ADDR_W-1: 0]          addr;
	 logic [DATA_W-1:0]           data       [MAX_BURST-1:0];
	 logic [NUM_SYMBOLS-1:0]      byteenable [MAX_BURST-1:0];
	 bit [31:0]                   cmd_delay;
	 bit [31:0]                   data_idles [MAX_BURST-1:0];
} Command;

typedef struct
{
	Burstcount                    burstcount;
	logic [DATA_W-1:0]            data     [MAX_BURST-1:0];
	bit [31:0]                    latency  [MAX_BURST-1:0];
} Response;

reg [DATA_W-1:0] ram_data [*];//[mem_start:mem_end];
	
//---------------------------------------------------
// Command and Response Queues
//---------------------------------------------------

// slave command queue
Command  write_command_queue_slave[`NUM_SLAVES][$];
Command  read_command_queue_slave[`NUM_SLAVES][$];

// slave response queue
Response read_response_queue_slave[`NUM_SLAVES][$];

//---------------------------------------------------
// Macro Definitions
//---------------------------------------------------

// Get command received by slave, verify command.
// If command is read command, send out response
`define MACRO_SLAVE_THREAD(SLAVE_ID) \
   always @(`SLAVE``SLAVE_ID.signal_command_received) begin \
\
      Command     actual_cmd, exp_cmd; \
      Response    rsp; \
\
      automatic int backpressure_cycles; \
\
      // set random backpressure cycles for next command \
      for (int i = 0; i < MAX_BURST; i++) begin \
         backpressure_cycles = $urandom_range(0, MAX_COMMAND_BACKPRESSURE); \
         `SLAVE``SLAVE_ID.set_interface_wait_time(backpressure_cycles, i); \
      end \
\
      actual_cmd = get_command_from_slave_``SLAVE_ID(); \
\
      // set read response \
      if (actual_cmd.trans == READ) begin \
         rsp = create_response(actual_cmd.burstcount,actual_cmd.addr); \
         configure_and_push_response_to_slave_``SLAVE_ID(rsp); \
         read_response_queue_slave[``SLAVE_ID].push_back(rsp); \
      end \
   end

`define MACRO_GET_COMMAND_FROM_SLAVE(SLAVE_ID) \
   function automatic Command get_command_from_slave_``SLAVE_ID (); \
\
      Command cmd; \
\
      `SLAVE``SLAVE_ID.pop_command(); \
      cmd.burstcount          = `SLAVE``SLAVE_ID.get_command_burst_count(); \
      cmd.addr                = `SLAVE``SLAVE_ID.get_command_address(); \
\
      if (`SLAVE``SLAVE_ID.get_command_request() == REQ_WRITE) begin \
         cmd.trans = WRITE; \
         for(int i = 0; i < cmd.burstcount; i++) begin \
            cmd.data[i]       =`SLAVE``SLAVE_ID.get_command_data(i); \
            cmd.byteenable[i] =`SLAVE``SLAVE_ID.get_command_byte_enable(i); \
				if(ram_data.exists(cmd.addr+i)) begin \
					ram_data[cmd.addr+i][7:0]=cmd.byteenable[i][0]==1'b1 ? cmd.data[i][7:0] : ram_data[cmd.addr+i][7:0]; \
					ram_data[cmd.addr+i][15:8]=cmd.byteenable[i][1]==1'b1 ? cmd.data[i][15:8] : ram_data[cmd.addr+i][15:8]; \
					ram_data[cmd.addr+i][23:16]=cmd.byteenable[i][2]==1'b1 ? cmd.data[i][23:16] : ram_data[cmd.addr+i][23:16]; \
					ram_data[cmd.addr+i][31:24]=cmd.byteenable[i][3]==1'b1 ? cmd.data[i][31:24] : ram_data[cmd.addr+i][31:24]; \
					ram_data[cmd.addr+i][39:32]=cmd.byteenable[i][4]==1'b1 ? cmd.data[i][39:32] : ram_data[cmd.addr+i][39:32]; \
					ram_data[cmd.addr+i][47:40]=cmd.byteenable[i][5]==1'b1 ? cmd.data[i][47:40] : ram_data[cmd.addr+i][47:40]; \
					ram_data[cmd.addr+i][55:48]=cmd.byteenable[i][6]==1'b1 ? cmd.data[i][55:48] : ram_data[cmd.addr+i][55:48]; \
					ram_data[cmd.addr+i][63:56]=cmd.byteenable[i][7]==1'b1 ? cmd.data[i][63:56] : ram_data[cmd.addr+i][63:56]; \
				end else begin \
					ram_data[cmd.addr+i]=cmd.data[i]; \
				end \
				if(debug) begin \
					$display("%t ps: %08x set to %016x",$time,cmd.addr+i,ram_data[cmd.addr+i]); \
				end \
         end \
      end else begin \
         cmd.trans = READ; \
      end \
\
      return cmd; \
   endfunction
   
`define MACRO_PENDING_READ_CYCLES(SLAVE_ID) \
   int pending_read_cycles_slave_``SLAVE_ID = 0; \
   always @(posedge clk_ram) begin \
      if (pending_read_cycles_slave_``SLAVE_ID > 0) begin \
         pending_read_cycles_slave_``SLAVE_ID--; \
      end \
   end

`define MACRO_CONFIGURE_AND_PUSH_RESPONSE_TO_SLAVE(SLAVE_ID) \
task automatic configure_and_push_response_to_slave_``SLAVE_ID ( \
      Response rsp \
   ); \
\
      int read_response_latency; \
\
      `SLAVE``SLAVE_ID.set_response_request(REQ_READ); \
      `SLAVE``SLAVE_ID.set_response_burst_size(rsp.burstcount); \
      for (int i = 0; i < rsp.burstcount; i++) begin \
         `SLAVE``SLAVE_ID.set_response_data(rsp.data[i], i); \
\
         if (i == 0) begin \
            `SLAVE``SLAVE_ID.set_response_latency(rsp.latency[i] + pending_read_cycles_slave_``SLAVE_ID, i); \
            read_response_latency = rsp.latency[i]; \
         end else begin \
            `SLAVE``SLAVE_ID.set_response_latency(rsp.latency[i], i); \
            read_response_latency = rsp.latency[i] + read_response_latency; \
         end \
\
      end \
      `SLAVE``SLAVE_ID.push_response(); \
      pending_read_cycles_slave_``SLAVE_ID = pending_read_cycles_slave_``SLAVE_ID + read_response_latency + rsp.burstcount + 2; \
   endtask

//---------------------------------------------------
// Macro Instantiations
//---------------------------------------------------

// slave 0
`MACRO_SLAVE_THREAD(0)
`MACRO_GET_COMMAND_FROM_SLAVE(0)
`MACRO_PENDING_READ_CYCLES(0)
`MACRO_CONFIGURE_AND_PUSH_RESPONSE_TO_SLAVE(0)

///////////////////////////////////////////////////////////////////////
// MEMORY																				//
///////////////////////////////////////////////////////////////////////

avalonslavetestbench
avalonslavetestbench
(
	.clk_clk(clk_ram),
	.avalon_waitrequest(mem_wait_request),
	.avalon_write(mem_write),
	.avalon_read(mem_read),
	.avalon_address(mem_address),
	.avalon_byteenable(mem_byte_enable),
	.avalon_burstcount(mem_burst_count),
	.avalon_readdata(mem_data_out),
	.avalon_readdatavalid(mem_read_data_valid),
	.avalon_writedata(mem_data_in),
	.reset_reset(reset)

);

task automatic queue_command (
	Command  cmd,
	int      slave_id
);
	
	save_command_slave(cmd, slave_id);
endtask

task automatic save_command_slave(
	Command  cmd,
	int      slave_id
);

	if (cmd.trans == WRITE) begin
		write_command_queue_slave[slave_id].push_back(cmd);
	end else begin
		read_command_queue_slave[slave_id].push_back(cmd);
	end

endtask

function automatic Response create_response (Burstcount burstcount,bit [ADDR_W-1: 0] address   );

	Response rsp;
	
	rsp.burstcount       = burstcount;
	for (int i = 0;i < burstcount; i++) begin
		if(ram_data.exists(address+i)) begin
			rsp.data[i]       = ram_data[address+i];
		end else begin
			rsp.data[i]			=	64'd0;
		end
		rsp.latency[i]    = $urandom_range(0, MAX_DATA_IDLE);
		if(debug) begin
			$display("%t ps: %08x get is %016x",$time,address,rsp.data[i]);
		end
	end
	
	return rsp;
endfunction

task automatic print_mem(integer addr);

	if(ram_data.exists(addr)) begin
		$display("DDRAM ADDR: %08x - %016x",addr,ram_data[addr]);
	end else begin
		$display("DDRAM ADDR: %08x - NOT WRITTEN TO YET!",addr);
	end
	
endtask

endmodule
