///////////////////////////////////////////////////////////////////////
// GERANIUM TUTORIAL CORE 2020													//
///////////////////////////////////////////////////////////////////////

`timescale 1ps / 1ps

module
io_control
(
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	input		wire								hardware_reset,
	input		wire								system_clock,
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	input		wire		[27:0]				hardware_addr,
	input		wire		[31:0]				hardware_data_in,
	output	wire		[31:0]				hardware_data_out,
	input		wire		[3:0]					hardware_data_sel,
	input		wire								hardware_write,
	input		wire								hardware_ready,
	output	wire								hardware_ack,
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	output	logic								char_display_on						=		1'b0,
	output	logic								char_write								=		1'b0,
	output	logic		[7:0]					char_out									=		8'd0,
	output	logic								char_clear								=		1'b0,
	output	logic								char_clear_line						=		1'b0,
	input		wire								char_done,
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	input		wire		[63:0]				status,
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	output	logic		[31:0]				sd_lba									=		32'd0,
	output	logic								sd_rd										=		1'b0,
	output	logic								sd_wr										=		1'b0,
	input		wire								sd_ack,
	input		wire		[7:0]					sd_buff_addr,
	input		wire		[15:0]				sd_buff_dout,
	output	wire		[15:0]				sd_buff_din,
	input		wire								sd_buff_wr,
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	input		wire								img_mounted,
	input		wire		[63:0]				img_size,
	input		wire								img_readonly,
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	input		wire		[7:0]					keyboard_in
	/////////////////////////////////////////////////////////////////////////////////////////////////////
);

///////////////////////////////////////////////////////////////////////
// IO SELECTORS																		//
///////////////////////////////////////////////////////////////////////

localparam									CHAR_SEL										=		7'b0000000;
localparam									SD_LBA_SEL									=		7'b0000001;
localparam									SD_CONTROL_SEL								=		7'b0000010;
localparam									SD_DATA_SEL									=		7'b0000011;
localparam									STATUS_SEL									=		7'b0000100;
localparam									SD_IMG_MOUNTED_SEL						=		7'b0000101;
localparam									SD_IMG_HI_SEL								=		7'b0000110;
localparam									SD_IMG_LO_SEL								=		7'b0000111;
localparam									KEY_SEL										=		7'b0001000;
localparam									CONSOLE_CONTROL_SEL						=		7'b0001001;

localparam									STATUS_LO_SEL								=		1'b0;
localparam									STATUS_HI_SEL								=		1'b1;

///////////////////////////////////////////////////////////////////////
// VARIABLES																			//
///////////////////////////////////////////////////////////////////////

wire											sel_key;
wire											sel_status;
wire											sel_status_hi;
wire											sel_status_lo;
wire											sel_char;
wire											sel_console_ctrl;
wire											sel_sd_lba;
wire											sel_sd_control;
wire											sel_sd_data;
wire											sel_sd_img_hi;
wire											sel_sd_img_lo;
wire											sel_sd_img_mounted;
wire											sel_video_buffer;

logic			[31:0]						status_data											=			32'd0;
logic											status_ack											=			1'b0;

logic											char_ack												=			1'b0;
logic			[31:0]						char_data											=			32'd0;

logic											sd_hardware_ack									=			1'b0;
logic											sd_image_mounted									=			1'b0;
logic			[31:0]						sd_io_data_out										=			32'd0;

logic			[6:0]							sd_read_addr										=			7'd0;
logic			[31:0]						sd_write_word										=			32'd0;
logic											sd_write												=			1'b0;
logic											sd_done												=			1'b0;
wire			[31:0]						sd_read_word;
logic											sd_write_flag										=			1'b0;

logic			[31:0]						keyboard_key										=			32'd0;
logic											keyboard_ack										=			1'b0;

logic											video_ack											=			1'b0;
logic			[31:0]						video_data_out										=			32'd0;

///////////////////////////////////////////////////////////////////////
// ASSIGNMENTS																			//
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
assign										hardware_ack					=		char_ack | sd_hardware_ack | keyboard_ack | status_ack;
assign										hardware_data_out				=		char_ack ? char_data :
																							status_ack ? status_data :
																							keyboard_ack ? keyboard_key :
																							sd_hardware_ack ? sd_io_data_out :
																							32'd0;
/////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// DISPLAY BUFFER TEXT IO															//
// THIS HARDWARE ADDRESS IS USED TO IMPLEMENT A SIMPLE CHARACTER		//
// DISPLAY CONSOLE IN THE VIDEO CONTROLLER									//
///////////////////////////////////////////////////////////////////////

assign										sel_char							=		hardware_addr[6:0]==CHAR_SEL					?	1'b1	:	1'b0;
assign										sel_console_ctrl				=		hardware_addr[6:0]==CONSOLE_CONTROL_SEL	?	1'b1	:	1'b0;

always @(posedge system_clock) begin
	if(hardware_reset) begin
		char_ack																			<=			1'b0;
		char_write																		<=			1'b0;
		char_clear																		<=			1'b0;
		char_clear_line																<=			1'b0;
	end else begin
		if(char_write) begin
			if(char_done) begin
				char_write																<=			1'b0;
			end
		end
		if(char_clear) begin
			if(char_done) begin
				char_clear																<=			1'b0;
			end
		end
		if(char_clear_line) begin
			if(char_done) begin
				char_clear_line														<=			1'b0;
			end
		end
		if(hardware_ready) begin
			if(~char_ack) begin
				if(sel_char) begin
					char_ack																<=			1'b1;
					if(hardware_write) begin
						char_write														<=			1'b1;
						char_out															<=			hardware_data_in[7:0];
					end else begin
						char_data														<=			{ 24'd0 , char_out };
					end
				end else if(sel_console_ctrl) begin
					char_ack																<=			1'b1;
					if(hardware_write) begin
						if(hardware_data_in[1]) begin													// THIS BIT WILL CLEAR THE CONSOLE SCREEN OF ALL TEXT
							char_clear													<=			1'b1;
						end else if(hardware_data_in[2]) begin										// THIS BIT WILL CLEAR THE CURRENT LINE OF ALL TEXT
							char_clear_line											<=			1'b1;
						end
						if(hardware_data_in[0]) begin													// THIS BIT WILL TURN OFF THE TEXT DISPLAY
							char_display_on											<=			1'b1;
						end else begin
							char_display_on											<=			1'b0;
						end
					end else begin
						char_data														<=			{ 29'd0 , char_clear_line , char_clear , char_display_on };
					end
				end
			end
		end else begin
			char_ack																		<=			1'b0;
		end
	end
end

///////////////////////////////////////////////////////////////////////
// SD CARD ACCESS																		//
///////////////////////////////////////////////////////////////////////

assign										sel_sd_lba						=		hardware_addr[6:0]==SD_LBA_SEL				?	1'b1	:	1'b0;
assign										sel_sd_control					=		hardware_addr[6:0]==SD_CONTROL_SEL			?	1'b1	:	1'b0;
assign										sel_sd_data						=		hardware_addr[6:0]==SD_DATA_SEL				?	1'b1	:	1'b0;
assign										sel_sd_img_mounted			=		hardware_addr[6:0]==SD_IMG_MOUNTED_SEL		?	1'b1	:	1'b0;
assign										sel_sd_img_hi					=		hardware_addr[6:0]==SD_IMG_HI_SEL			?	1'b1	:	1'b0;
assign										sel_sd_img_lo					=		hardware_addr[6:0]==SD_IMG_LO_SEL			?	1'b1	:	1'b0;

always @(posedge system_clock) begin
	if(hardware_reset) begin
		sd_read_addr															<=		7'd0;
		sd_write_word															<=		32'd0;
		sd_write																	<=		1'b0;
		sd_rd																		<=		1'b0;
		sd_wr																		<=		1'b0;
		sd_lba																	<=		32'd0;
		sd_write_flag															<=		1'b0;
		sd_done																	<=		1'b1;
		sd_io_data_out															<=		32'd0;
		sd_hardware_ack														<=		1'b0;
	end else begin
		if(sd_ack) begin
			sd_rd																	<=		1'b0;
			sd_wr																	<=		1'b0;
			if(&sd_buff_addr) begin
				sd_done															<=		1'b1;
			end
		end
		if(hardware_ready) begin
			if(~sd_hardware_ack) begin
				//////////////////////////////////////////////////////////////////////////////
				if(sel_sd_img_mounted) begin
					sd_hardware_ack											<=		1'b1;
					if(~hardware_write) begin
						sd_io_data_out											<=		{ 31'd0 , sd_image_mounted };
					end
				//////////////////////////////////////////////////////////////////////////////
				end else if(sel_sd_img_hi) begin
					sd_hardware_ack											<=		1'b1;
					if(~hardware_write) begin
						sd_io_data_out											<=		img_size[63:32];
					end
				//////////////////////////////////////////////////////////////////////////////
				end else if(sel_sd_img_lo) begin
					sd_hardware_ack											<=		1'b1;
					if(~hardware_write) begin
						sd_io_data_out											<=		img_size[31:0];
					end
				//////////////////////////////////////////////////////////////////////////////
				end else if(sel_sd_lba) begin
					sd_hardware_ack											<=		1'b1;
					if(~hardware_write) begin
						sd_io_data_out											<=		sd_lba;
					end else begin
						sd_lba													<=		hardware_data_in;
					end
				//////////////////////////////////////////////////////////////////////////////
				end else if(sel_sd_control) begin
					sd_hardware_ack											<=		1'b1;
					if(~hardware_write) begin
						sd_io_data_out											<=		{ 28'd0 , sd_done , sd_ack , sd_wr , sd_rd };
					end else begin
						if(sd_done) begin
							if(hardware_data_in[0]) begin
								sd_rd												<=		1'b1;
								sd_read_addr									<=		7'd0;
								sd_done											<=		1'b0;
							end else if(hardware_data_in[1]) begin
								sd_wr												<=		1'b1;
								sd_read_addr									<=		7'd0;
								sd_write_flag									<=		1'b0;
								sd_done											<=		1'b0;
							end
						end
					end
				//////////////////////////////////////////////////////////////////////////////
				end else if(sel_sd_data) begin
					if(~hardware_write) begin
						if(sd_done) begin
							if(~&sd_read_addr) begin
								sd_read_addr									<=		sd_read_addr + 1'b1;
							end
							sd_io_data_out										<=		sd_read_word;
							sd_hardware_ack									<=		1'b1;
						end
					end else begin
						if(sd_done) begin
							if(~&sd_read_addr && sd_write_flag) begin
								sd_read_addr									<=		sd_read_addr + 1'b1;
							end
							sd_write_flag										<=		1'b1;
							sd_write												<=		1'b1;
							sd_write_word										<=		hardware_data_in;
							sd_hardware_ack									<=		1'b1;
						end
					end
				//////////////////////////////////////////////////////////////////////////////
				end
			end
		end else begin
			sd_hardware_ack													<=		1'b0;
		end
		if(sd_write) begin
			sd_write																<=		1'b0;
		end
	end
end

///////////////////////////////////////////////////////////////////////
// RAM FOR THE SDCARD INTERFACE													//
///////////////////////////////////////////////////////////////////////

sdcardram
sdcardram
(
	.clock_a						(			system_clock													),
	.address_a					(			sd_buff_addr													),
	.data_a						(			sd_buff_dout													),
	.wren_a						(			sd_buff_wr & sd_ack											),
	.q_a							(			sd_buff_din														),

	.clock_b						(			system_clock													),
	.address_b					(			sd_read_addr													),
	.data_b						(			sd_write_word													),
	.wren_b						(			sd_write															),
	.q_b							(			sd_read_word													)

);

///////////////////////////////////////////////////////////////////////
// SD IMAGE MOUNTED																	//
///////////////////////////////////////////////////////////////////////

always_ff @(posedge img_mounted or negedge hardware_reset) begin
	if(img_mounted) begin
		sd_image_mounted														<=		1'b1;
	end else begin
		sd_image_mounted														<=		1'b0;
	end
end

///////////////////////////////////////////////////////////////////////
// KEYBOARD																				//
///////////////////////////////////////////////////////////////////////

assign										sel_key							=		hardware_addr[6:0]==KEY_SEL ? 1'b1 : 1'b0;

always @(posedge system_clock) begin
	keyboard_key[7:0]															<=		keyboard_in;
	keyboard_key[31:8]														<=		24'd0;
	if(hardware_ready) begin
		if(~keyboard_ack) begin
			if(sel_key) begin
				keyboard_ack													<=		1'b1;
			end
		end
	end else begin
		keyboard_ack															<=		1'b0;
	end
end

///////////////////////////////////////////////////////////////////////
// STATUS																				//
///////////////////////////////////////////////////////////////////////

assign										sel_status						=		hardware_addr[6:0]==STATUS_SEL	?	1'b1	:	1'b0;
assign										sel_status_lo					=		hardware_addr[7]==STATUS_LO_SEL	?	1'b1	:	1'b0;
assign										sel_status_hi					=		hardware_addr[7]==STATUS_HI_SEL	?	1'b1	:	1'b0;

always @(posedge system_clock) begin
	if(hardware_ready) begin
		if(~status_ack) begin
			if(sel_status) begin
				status_ack														<=		1'b1;
				if(sel_status_lo) begin
					status_data													<=		status[31:0];
				end else begin
					status_data													<=		status[63:32];
				end
			end
		end
	end else begin
		status_ack																<=		1'b0;
	end
end



endmodule
