////////////////////////////////////////////////////////
// CHARACTER DISPLAY												//
////////////////////////////////////////////////////////

`timescale 1ps / 1ps

module
character_display
(
	input				wire				reset,
	input				wire				system_clk,
	
	input				wire	[9:0]		x_pos,
	input				wire	[9:0]		y_pos,
	input				wire				pixel_clk,
	output			logic				pixel						=		1'b0,
	output			logic				window					=		1'b0,
	
	input				wire	[7:0]		insert_char,
	input				wire				insert,
	output			logic				insert_ack				=		1'b0,
	input				wire				insert_clear_screen,
	input				wire				insert_clear_line,
	
	input				wire				debug_a,
	input				wire				debug_b,
	input				wire				debug_c
	
);

////////////////////////////////////////////////////////
// PARAMETERS														//
////////////////////////////////////////////////////////

parameter		[9:0]				xstart						=			10'd0;
parameter		[9:0]				ystart						=			10'd0;
parameter		[9:0]				xstop							=			10'd640;
parameter		[9:0]				ystop							=			10'd480;
parameter		[9:0]				border						=			10'd4;

parameter		[9:0]				screen_y_size				=			16'd480;

parameter		[31:0]			x_screen_size				=			32'd640;
parameter		[31:0]			x_char_size					=			32'd8;
parameter		[31:0]			y_screen_size				=			32'd480;
parameter		[31:0]			y_char_size					=			32'd8;

////////////////////////////////////////////////////////
// REGISTERS														//
////////////////////////////////////////////////////////

wire		[10:0]		romaddr;
logic		[12:0]		messageaddr								=			13'd0;
logic		[12:0]		rowaddr									=			13'd0;
wire		[7:0]			messagechar;
wire		[7:0]			chardata;
logic		[7:0]			chardatashift							=			8'd0;
logic		[2:0]			ycounter									=			3'b000;

logic		[2:0]			insert_clear_screendd				=			3'b000;
logic		[2:0]			insert_clear_linedd					=			3'b000;
logic		[2:0]			insert_writedd							=			3'b000;
logic		[7:0]			insert_write_char						=			8'd0;
logic		[7:0]			insert_write_chard					=			8'd0;
logic		[7:0]			insert_write_chardd					=			8'd0;

logic		[12:0]		insert_pos_addr						=			13'd0;
logic						insert_write							=			1'b0;
logic						clear										=			1'b0;
logic						clear_line								=			1'b0;
logic						write_char								=			1'b0;
logic						move_up									=			1'b0;
logic						write										=			1'b0;
logic		[7:0]			write_out_data							=			8'd0;
logic		[12:0]		write_rowaddr							=			13'd0;
logic		[12:0]		read_rowaddr							=			13'd0;

wire		[12:0]		write_addr;
wire		[7:0]			write_data;
wire						write_enable;
wire		[12:0]		read_addr;
wire		[7:0]			read_data;

wire		[31:0]		x_row_length;
wire		[12:0]		x_row_length_size;
wire		[31:0]		y_row_length;
wire		[12:0]		y_row_length_size;
wire		[12:0]		total_rows_start;
wire		[12:0]		total_rows_length_size;
wire		[12:0]		total_rows_length_size_sub_row;
wire		[12:0]		first_row_length_size;

wire		[12:0]		current_row_start;
wire		[12:0]		current_row_end;

////////////////////////////////////////////////////////
// TOP LEVEL ASSIGNMENTS										//
////////////////////////////////////////////////////////

assign					x_row_length							=			x_screen_size / x_char_size;
assign					x_row_length_size						=			x_row_length[12:0];
assign					y_row_length							=			y_screen_size / y_char_size;
assign					y_row_length_size						=			y_row_length[12:0];
assign					total_rows_start						=			13'd0;
assign					total_rows_length_size				=			x_row_length_size * y_row_length_size;
assign					total_rows_length_size_sub_row	=			total_rows_length_size - x_row_length_size;
assign					first_row_length_size				=			x_row_length_size + 13'd1;

assign					current_row_start						=			( insert_pos_addr / x_row_length_size) * x_row_length_size;
assign					current_row_end						=			current_row_start + x_row_length_size;

assign					romaddr									=			{ read_data[7:0] , ycounter };

assign					write_addr								=			( move_up | clear | clear_line )		?		write		?		write_rowaddr		:		read_rowaddr		:		insert_pos_addr;
assign					read_addr								=			move_up										?		read_rowaddr							:		rowaddr;
assign					write_data								=			( clear | clear_line )					?		8'd0		:		move_up				?		write_out_data		:		insert_write_char;
assign					write_enable							=			( move_up | clear | clear_line )		?		write		?		1'b1					:		1'b0					:		write_char;

////////////////////////////////////////////////////////
// CHARACTER ROM													//
////////////////////////////////////////////////////////

char_rom
char_rom
(
	.address				(	romaddr							),
	.clock				(	system_clk						),
	.q						(	chardata							)
	
);

////////////////////////////////////////////////////////
// TEXT RAM															//
////////////////////////////////////////////////////////

char_ram
char_ram
(
	.clock					(				system_clk																					),
	.wraddress				(				write_addr																					),
	.wren						(				write_enable																				),
	.data						(				write_data																					),
	.rdaddress				(				read_addr																					),
	.q							(				read_data																					)
);

////////////////////////////////////////////////////////
// CHARACTER DISPLAY LOGIC										//
////////////////////////////////////////////////////////

always @(posedge system_clk) begin
	if(reset) begin
		messageaddr					<=			13'd0;
		rowaddr						<=			13'd0;
		chardatashift				<=			8'd0;
		ycounter						<=			3'b000;
		move_up						<=			1'b0;
		write							<=			1'b0;
		clear							<=			1'b0;
		clear_line					<=			1'b0;
		insert_clear_screendd	<=			3'b000;
		insert_clear_linedd		<=			3'b000;
		insert_writedd				<=			3'b000;
		write_char					<=			1'b0;
		insert_write_char			<=			8'd0;
		insert_pos_addr			<=			13'd0;
	end else begin
		////////////////////////////////////////////////////////////////////////////
		insert_writedd				<=			{ insert_writedd[1:0] , insert };
		insert_clear_screendd	<=			{ insert_clear_screendd[1:0] , insert_clear_screen };
		insert_clear_linedd		<=			{ insert_clear_linedd[1:0] , insert_clear_line };
		insert_write_chardd		<=			insert_write_chard;
		insert_write_chard		<=			insert_char;
		////////////////////////////////////////////////////////////////////////////
		if(insert_clear_linedd[2:1]==2'b01) begin
			write_rowaddr			<=			current_row_start;
			clear_line				<=			1'b1;
			write_out_data			<=			8'd0;
			write						<=			1'b1;
		end
		////////////////////////////////////////////////////////////////////////////
		if(insert_clear_linedd[2:1]==2'b10) begin
			if(insert_ack) begin
				insert_ack			<=			1'b0;
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(insert_clear_screendd[2:1]==2'b01) begin
			write_rowaddr			<=			total_rows_start;
			clear						<=			1'b1;
			write_out_data			<=			8'd0;
			write						<=			1'b1;
		end
		////////////////////////////////////////////////////////////////////////////
		if(insert_clear_screendd[2:1]==2'b10) begin
			if(insert_ack) begin
				insert_ack			<=			1'b0;
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(insert_writedd[2:1]==2'b01) begin
			insert_write_char		<=			insert_write_chardd;
			insert_write			<=			1'b1;
		end
		////////////////////////////////////////////////////////////////////////////
		if(insert_writedd[2:1]==2'b10) begin
			if(insert_ack) begin
				insert_ack			<=			1'b0;
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(insert_write) begin
			if(insert_pos_addr==total_rows_length_size) begin
				insert_pos_addr	<=			total_rows_length_size_sub_row;
				move_up				<=			1'b1;
				read_rowaddr		<=			first_row_length_size;
				write_rowaddr		<=			total_rows_start;
				write					<=			1'b0;
				insert_write		<=			1'b0;
			end else begin
				write_char			<=			1'b1;
				insert_ack			<=			1'b1;
				insert_write		<=			1'b0;
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(write_char) begin
			insert_pos_addr		<=			insert_pos_addr + 1'b1;
			write_char				<=			1'b0;
		end
		////////////////////////////////////////////////////////////////////////////
		if(clear_line) begin
			if(write_rowaddr<current_row_end) begin
				write_out_data		<=			8'd0;
				write					<=			1'b1;
				write_rowaddr		<=			write_rowaddr + 1'b1;
			end else begin
				write					<=			1'b0;
				clear_line			<=			1'b0;
				insert_ack			<=			1'b1;
				insert_pos_addr	<=			current_row_start;
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(clear) begin
			if(write_rowaddr<total_rows_length_size) begin
				write_out_data		<=			8'd0;
				write					<=			1'b1;
				write_rowaddr		<=			write_rowaddr + 1'b1;
			end else begin
				write					<=			1'b0;
				clear					<=			1'b0;
				insert_ack			<=			1'b1;
				insert_pos_addr	<=			total_rows_start;
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(move_up) begin
			if(read_rowaddr>=total_rows_length_size || write_rowaddr>=total_rows_length_size_sub_row) begin
				if(write_rowaddr<total_rows_length_size) begin
					write_out_data	<=			8'd0;
					write				<=			1'b1;
					write_rowaddr	<=			write_rowaddr + 1'b1;
				end else begin
					write				<=			1'b0;
					move_up			<=			1'b0;
					insert_write	<=			1'b1;
				end
			end else begin
				if(~write) begin
					read_rowaddr		<=			read_rowaddr + 1'b1;
					write_out_data		<=			read_data;
					write					<=			1'b1;
				end else begin
					write_rowaddr		<=			write_rowaddr + 1'b1;
					write					<=			1'b0;
				end
			end
		end
		////////////////////////////////////////////////////////////////////////////
		if(pixel_clk) begin
			////////////////////////////////////////////////////////////////////////////
			window					<=			1'b0;
			if(x_pos>=(xstart+border) && x_pos<(xstop-border) && y_pos>=(ystart+border) && y_pos<(ystop-border)) begin
				window				<=			1'b1;
			end
			////////////////////////////////////////////////////////////////////////////
			pixel						<=			1'b0;
			if(x_pos==0 && y_pos==screen_y_size) begin
				messageaddr			<=			13'd0;
				rowaddr				<=			13'd0;
				ycounter				<=			3'b000;
			end
			////////////////////////////////////////////////////////////////////////////
			if(y_pos>=(ystart+border) && y_pos<(ystop-border)) begin
				if(x_pos==(xstop-border)) begin
					ycounter			<=			ycounter + 3'd1;
					if(ycounter==3'b111) begin
						messageaddr	<=			rowaddr;
					end else begin
						rowaddr		<=			messageaddr;
					end
				end
				if(x_pos>=(xstart+border) && x_pos<(xstop-border)) begin // new pixel
					if(x_pos[2:0]==3'b000) begin
						rowaddr			<=			rowaddr + 13'd1;					
						pixel				<=			chardata[7];
						chardatashift	<=			{ chardata[6:0] , 1'b0 };
					end else begin
						pixel				<=			chardatashift[7];
						chardatashift	<=			{ chardatashift[6:0] , 1'b0 };
					end
				end
			end
			////////////////////////////////////////////////////////////////////////////
		end
	end
end

endmodule
