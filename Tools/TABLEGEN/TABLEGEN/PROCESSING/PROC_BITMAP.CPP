#include "GRNMPCH.H"
//////////////////////////////////////////////////////////////////////////
#define VDP_COLOR_TRANSPARENT			0x00
#define VDP_COLOR_BLACK					0x01
#define VDP_COLOR_MEDIUM_GREEN			0x02
#define VDP_COLOR_LIGHT_GREEN			0x03
#define VDP_COLOR_DARK_BLUE				0x04
#define VDP_COLOR_LIGHT_BLUE			0x05
#define VDP_COLOR_DARK_RED				0x06
#define VDP_COLOR_CYAN					0x07
#define VDP_COLOR_MEDIUM_RED			0x08
#define VDP_COLOR_LIGHT_RED				0x09
#define VDP_COLOR_DARK_YELLOW			0x0A
#define VDP_COLOR_LIGHT_YELLOW			0x0B
#define VDP_COLOR_DARK_GREEN			0x0C
#define VDP_COLOR_MAGENTA				0x0D
#define VDP_COLOR_GRAY					0x0E
#define VDP_COLOR_WHITE					0x0F
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE pat[8];
} PATTERN_VALUE;
bool operator==(PATTERN_VALUE& a,PATTERN_VALUE& b)
{
	if((a.pat[0]==b.pat[0])&&(a.pat[1]==b.pat[1])&&(a.pat[2]==b.pat[2])&&(a.pat[3]==b.pat[3])&&(a.pat[4]==b.pat[4])&&(a.pat[5]==b.pat[5])&&(a.pat[6]==b.pat[6])&&(a.pat[7]==b.pat[7]))
	{
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
BOOL ProcBitmap(CString outnamepath,CString innamepath)
{

//////////////////////////////////////////////////////////////////////////
	LPBITMAPFILEHEADER file_header;
	LPBITMAPINFOHEADER info_header;
	vector<char> buffer;
	vector<char> outpic;
	vector<char> outpattern;
	vector<PATTERN_VALUE> outpattern_store;
	CStringA OutputString;
	CFile outfile;
	CFile infile;
	DWORD len;
	BYTE bitcount;
	WORD color;
	WORD color_red;
	WORD color_blue;
	WORD color_green;
	DWORD x = 0;
	DWORD y = 0;

	DWORD RED_CUTOFF_LOW = 4;
	DWORD GREEN_CUTOFF_LOW = 4;
	DWORD BLUE_CUTOFF_LOW = 4;
	DWORD RED_CUTOFF_MID = 18;
	DWORD GREEN_CUTOFF_MID = 10;
	DWORD BLUE_CUTOFF_MID = 10;
	DWORD RED_CUTOFF_HIGH = 31;
	DWORD GREEN_CUTOFF_HIGH = 31;
	DWORD BLUE_CUTOFF_HIGH = 31;
	//////////////////////////////////////////////////////////////////////////
	if(outfile.Create(outnamepath,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL)==FALSE)
	{
		MessageBoxW(NULL,L"COULD NOT OPEN OUTPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	if(infile.Open(innamepath,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL)==FALSE)
	{
		MessageBoxW(NULL,L"COULD NOT OPEN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	CStringA title;
	title.Format(";%S\n",innamepath.Right(innamepath.GetLength()-innamepath.ReverseFind(_T('\\'))-1));
	outfile.Write(title.GetBuffer(title.GetLength()),title.GetLength());
	title.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	len = infile.GetSize();
	buffer.resize(len);
	if(infile.Read(&buffer[0],len)==FALSE)
	{
		MessageBoxW(NULL,L"COULD NOT READ INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	file_header = (LPBITMAPFILEHEADER)(&buffer[0]);
	info_header = (LPBITMAPINFOHEADER)(&buffer[0]+sizeof(BITMAPFILEHEADER));
	//////////////////////////////////////////////////////////////////////////
	if(info_header->biCompression!=BI_RGB&&info_header->biCompression!=BI_BITFIELDS)
	{
		MessageBoxW(NULL,L"UNKNOWN COMPRESSION IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	bitcount = 0;
	switch(info_header->biBitCount)
	{
		case 0:
		{
			MessageBoxW(NULL,L"UNKNOWN BITCOUNT IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
			return FALSE;
		}
		case 1:
		{
			MessageBoxW(NULL,L"UNKNOWN BITCOUNT IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
			bitcount = 1;
			return FALSE;
		}
		case 4:
		{
			MessageBoxW(NULL,L"UNKNOWN BITCOUNT IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
			bitcount = 4;
			return FALSE;
		}
		case 8:
		{
			MessageBoxW(NULL,L"UNKNOWN BITCOUNT IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
			bitcount = 8;
			return FALSE;
		}
		case 16:
		{
			bitcount = 16;
			RED_CUTOFF_LOW = 4;
			GREEN_CUTOFF_LOW = 4;
			BLUE_CUTOFF_LOW = 4;
			RED_CUTOFF_MID = 18;
			GREEN_CUTOFF_MID = 18;
			BLUE_CUTOFF_MID = 18;
			RED_CUTOFF_HIGH = 32;
			GREEN_CUTOFF_HIGH = 32;
			BLUE_CUTOFF_HIGH = 32;
			break;
		}
		case 24:
		{
			bitcount = 24;
			RED_CUTOFF_LOW = 32;
			GREEN_CUTOFF_LOW = 32;
			BLUE_CUTOFF_LOW = 32;
			RED_CUTOFF_MID = 128;
			GREEN_CUTOFF_MID = 128;
			BLUE_CUTOFF_MID = 128;
			RED_CUTOFF_HIGH = 255;
			GREEN_CUTOFF_HIGH = 255;
			BLUE_CUTOFF_HIGH = 255;

			break;
		}
		case 32:
		{
			MessageBoxW(NULL,L"UNKNOWN BITCOUNT IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
			bitcount = 32;
			return FALSE;
		}
		default:
		{
			MessageBoxW(NULL,L"UNKNOWN BITCOUNT IN INPUT FILE",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	outpic.resize((info_header->biWidth*info_header->biHeight)*1);
	outpattern.resize((info_header->biWidth*info_header->biHeight)*1);
	outpattern_store.resize((info_header->biWidth*info_header->biHeight)*1);
	//////////////////////////////////////////////////////////////////////////
	DWORD width_x = info_header->biWidth;
	DWORD height_y = info_header->biHeight;
	if(height_y<0)
	{
		MessageBoxW(NULL,L"ONLY NEGATIVE BITMAPS SUPPORTED",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	BYTE* pColor = ((BYTE*)info_header+(WORD)(info_header->biSizeImage));
	//////////////////////////////////////////////////////////////////////////
	BYTE*bits = (BYTE*)(&buffer[0]+file_header->bfOffBits);
	//////////////////////////////////////////////////////////////////////////
	DWORD modheight = height_y%8;
	if(modheight!=0)
	{
		MessageBoxW(NULL,L"HEIGHT MUST BE DIVISIBLE BY 8",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	modheight = height_y/8;
	//////////////////////////////////////////////////////////////////////////
	DWORD modwidth = width_x%2;
	if(modwidth!=0)
	{
		MessageBoxW(NULL,L"WIDTH MUST BE DIVISIBLE BY 2",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	modwidth = width_x/2;
	//////////////////////////////////////////////////////////////////////////
	OutputString.Format("\n\n.START:\n\n");
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
	OutputString.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	for(DWORD j = height_y-1; j>=0; j--)
	{
		for(DWORD k = 0; k<width_x; k++)
		{
			switch(bitcount)
			{
				case 16:
				{
					color = bits[(k+j*width_x)*2];
					break;
				}
				case 24:
				{
					for(DWORD l = 0; l<3; l++)
					{
						switch(l)
						{
							case 0:
							{
								color_blue = bits[(k*3+j*width_x*3+l)];
								break;
							}
							case 1:
							{
								color_green = bits[(k*3+j*width_x*3+l)];
								break;
							}
							case 2:
							{
								color_red = bits[(k*3+j*width_x*3+l)];
								break;
							}
							default:
							{
								break;
							}
						}
					}
					break;
				}
				default:
				{
					MessageBoxW(NULL,L"COLOR DEPTH NOT SUPPORTED",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
					return FALSE;
				}
			}
			BYTE blue = bitcount==16 ? (color&0x07C00)>>10 : bitcount==24 ? color_blue : 0;
			BYTE green = bitcount==16 ? (color&0x03E0)>>5 : bitcount==24 ? color_green : 0;
			BYTE red = bitcount==16 ? (color&0x01F) : bitcount==24 ? color_red : 0;
			BYTE outcolor;
			if(blue>=0&&blue<BLUE_CUTOFF_LOW)
			{
				if(green>=0&&green<GREEN_CUTOFF_LOW)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_BLACK;			//BLACK
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_DARK_RED;			//DARK RED
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_MEDIUM_RED;			//MEDIUM RED
					} else
					{
				  //error
						goto color_error;
					}
				} else if(green>=GREEN_CUTOFF_LOW&&green<=GREEN_CUTOFF_MID)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_DARK_GREEN;			//DARK GREEN
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_DARK_YELLOW;			//DARK YELLOW
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_DARK_YELLOW;			//DARK YELLOW
					} else
					{
				  //error
						goto color_error;
					}
				} else if(green>=GREEN_CUTOFF_MID&&green<=GREEN_CUTOFF_HIGH)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_MEDIUM_GREEN;			//MEDIUM GREEN
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_LIGHT_GREEN;			//LIGHT GREEN
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_LIGHT_YELLOW;			//LIGHT YELLOW
					} else
					{
				  //error
						goto color_error;
					}
				} else
				{
			  //error
					goto color_error;
				}
			} else if(blue>=BLUE_CUTOFF_LOW&&blue<=BLUE_CUTOFF_MID)
			{
				if(green>=0&&green<GREEN_CUTOFF_LOW)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_DARK_BLUE;			//DARK BLUE
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_MAGENTA;			//MAGENTA
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_MAGENTA;			//MAGENTA
					} else
					{
				  //error
						goto color_error;
					}
				} else if(green>=GREEN_CUTOFF_LOW&&green<=GREEN_CUTOFF_MID)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_CYAN;			//CYAN
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_GRAY;			//GREY
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_LIGHT_RED;			//LIGHT RED
					} else
					{
				  //error
						goto color_error;
					}
				} else if(green>=GREEN_CUTOFF_MID&&green<=GREEN_CUTOFF_HIGH)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_MEDIUM_GREEN;			//MEDIUM GREEN
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_LIGHT_GREEN;			//LIGHT GREEN
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_DARK_YELLOW;			//DARK YELLOW
					} else
					{
				  //error
						goto color_error;
					}
				} else
				{
			  //error
					goto color_error;
				}
			} else if(blue>=BLUE_CUTOFF_MID&&blue<=BLUE_CUTOFF_HIGH)
			{
				if(green>=0&&green<GREEN_CUTOFF_LOW)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_DARK_BLUE;			//DARK BLUE
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_MAGENTA;			//MAGENTA
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_MAGENTA;			//MAGENTA
					} else
					{
				  //error
						goto color_error;
					}
				} else if(green>=GREEN_CUTOFF_LOW&&green<=GREEN_CUTOFF_MID)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_LIGHT_BLUE;			//LIGHT BLUE
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_LIGHT_BLUE;			//LIGHT BLUE
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_MAGENTA;			//MAGENTA
					} else
					{
				  //error
						goto color_error;
					}
				} else if(green>=GREEN_CUTOFF_MID&&green<=GREEN_CUTOFF_HIGH)
				{
					if(red>=0&&red<RED_CUTOFF_LOW)
					{
						outcolor = VDP_COLOR_LIGHT_BLUE;			//LIGHT BLUE
					} else if(red>=RED_CUTOFF_LOW&&red<=RED_CUTOFF_MID)
					{
						outcolor = VDP_COLOR_GRAY;			//GREY
					} else if(red>=RED_CUTOFF_MID&&red<=RED_CUTOFF_HIGH)
					{
						outcolor = VDP_COLOR_WHITE;			//WHITE
					} else
					{
				  //error
						goto color_error;
					}
				} else
				{
			  //error
					goto color_error;
				}
			} else
			{
		  //error
				goto color_error;
			}

			outpic[x+y*width_x] = outcolor;

			x++;
			if(x>=width_x)
			{
				x = 0;
				y++;
				if(y>=height_y)
				{
					goto color_copy_done;
				}
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////

	color_error:

		//////////////////////////////////////////////////////////////////////////
	MessageBoxW(NULL,L"COLOR OUT OF BOUNDS",L"CONVERT ERROR!",MB_OK|MB_ICONERROR);
	return FALSE;
	//////////////////////////////////////////////////////////////////////////

	color_copy_done:


		//////////////////////////////////////////////////////////////////////////
	OutputString.Format("\n\n;X-%d BY Y-%d\n\n",width_x,height_y);
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
	OutputString.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	BYTE color_write;
	BYTE pattern_number = 0;
	BYTE pattern_number_last = 0;
	BYTE pattern_count = 0;
	BYTE pattern_total = 0;
	PATTERN_VALUE patval;
	patval = {0,0,0,0,0,0,0,0};
	BYTE patvalpos = 0;
	y = 0;
	x = 0;
	//////////////////////////////////////////////////////////////////////////
	while(1)
	{
		OutputString.Format("\n\tDB\t");
		for(DWORD ycount = 0; ycount<8; ycount++)
		{
			for(DWORD xcount = 0; xcount<2; xcount++)
			{
				if(xcount==0)
				{
					color_write = (outpic[x+xcount+(y+ycount)*width_x])<<4;
				} else
				{
					color_write = color_write|(outpic[x+xcount+(y+ycount)*width_x]);
					patval.pat[patvalpos] = color_write;
					patvalpos++;
					if(patvalpos>=8)
					{
						patvalpos = 0;
					}
				}
			}
			if(ycount==0)
			{
				OutputString.Format("%s\t$%02X",OutputString,color_write);
			} else
			{
				OutputString.Format("%s,$%02X",OutputString,color_write);
			}
		}
		if(patvalpos==0)
		{
			for(BYTE p = 0; p<pattern_total; p++)
			{
				if(outpattern_store[p]==patval)
				{
					pattern_number = p;
					outpattern[pattern_count] = pattern_number;
					pattern_count++;
					goto pattern_found;
				}
			}
		}
		outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
		OutputString.ReleaseBuffer();
		pattern_number = pattern_number_last;
		outpattern[pattern_count] = pattern_number;
		outpattern_store[pattern_total] = patval;
		pattern_total++;
		pattern_count++;
		pattern_number_last++;

		pattern_found:

		x = x+2;
		if(x>=width_x)
		{
			x = 0;
			y = y+8;
			if(y>=height_y)
			{
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	OutputString.Format("\n.PATTERN:\n");
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
	OutputString.ReleaseBuffer();
	OutputString = "";
	//////////////////////////////////////////////////////////////////////////
	BYTE p;
	for(p = 0; p<pattern_count; p++)
	{
		if(p%16==0)
		{
			outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
			OutputString.ReleaseBuffer();
			OutputString.Format("\n\tDB\t$%02X",(BYTE)outpattern[p]);
		} else
		{
			OutputString.Format("%s,$%02X",OutputString,(BYTE)outpattern[p]);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
	OutputString.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	OutputString.Format("\n.END:\n\n\0");
	outfile.Write(OutputString.GetBuffer(OutputString.GetLength()),OutputString.GetLength());
	OutputString.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////
	outfile.Close();
	infile.Close();
	//////////////////////////////////////////////////////////////////////////
	return TRUE;
	//////////////////////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////////////////////
