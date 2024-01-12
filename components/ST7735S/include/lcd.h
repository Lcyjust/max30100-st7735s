//lcd.h
#ifndef __LCD_H
#define __LCD_H


#include "spi.h"



#define u8					unsigned char
#define u16					unsigned int





#define X_MAX_PIXEL	        160
#define Y_MAX_PIXEL	        160
#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰

#define LCD_RESET_PIN			16



#define LCD_CMD_DATA_PIN		15



#define LCD_BACK_LIGHT_PIN		13




#define LCD_NOP			0x00	//空命令
#define LCD_SWRESET		0x01	//软件复位，在睡眠和显示模式下，重置软件后需等待120ms后方可执行下一条指令
#define LCD_RDDID		0x04	//读取LCD的制造商ID（8位）、驱动版本ID（最高位为1，7位）、驱动程序ID（8位）
#define LCD_RDDST		0x09	//读取显示屏所有状态参数
#define LCD_RDDPM		0x0A	//读取显示屏能量模式
#define LCD_RDDMADCTL	0x0B	//读取显示屏MADCTL
#define LCD_RDDCOLMOD	0x0C	//读取显示屏像素定义
#define LCD_RDDIM		0x0D	//读取显示屏图片模式
#define LCD_RDDSM		0x0E	//读取显示屏单信号模式
#define LCD_RDDSDR		0x0F	//读取显示屏自我诊断结果
#define LCD_SLPIN		0x10	//进入最小功耗模式
#define LCD_SLPOUT 		0x11	//关闭睡眠模式
#define LCD_PTLON		0x12	//打开Partial模式
#define LCD_NORON		0x13	//恢复到正常模式
#define LCD_INVOFF		0x20	//显示反转模式中恢复
#define LCD_INVON		0x21	//进入反向显示模式
#define LCD_GAMSET		0x26	//当前显示选择所需的伽马曲线
#define LCD_DISPOFF		0x28	//关闭显示，帧内存的输出被禁用
#define LCD_DISPON		0x29	//开启显示，帧内存的输出被启用
#define LCD_CASET		0x2A	//列地址设置，每个值代表帧内存中的一列
#define LCD_RASET		0x2B	//行地址设置，每个值代表帧内存中的一列
#define LCD_RAMWR		0x2C	//写入内存
#define LCD_RGBSET		0x2D	//颜色模式设置
#define LCD_RAMRD		0x2E	//读取内存
#define LCD_PTLAR		0x30	//部分模式的显示区域设置
#define LCD_SCRLAR		0x33	//定义垂直滚动区域的显示
#define LCD_TEOFF		0x34	//关闭(Active Low) TE信号线的撕裂效应输出信号
#define LCD_TEON		0x35	//打开TE信号线的撕裂效果输出信号
#define LCD_MADCTL		0x36	//定义帧内存的读写扫描方向
#define LCD_VSCSAD		0x37	//设置垂直滚动起始地址，此命令与垂直滚动定义(33h)一起使用
#define LCD_IDMOFF		0x38	//关闭空闲模式
#define LCD_IDMON		0x39	//开启空闲模式
#define LCD_COLMOD		0x3A	//定义通过MCU接口传输的RGB图片数据的格式
#define LCD_FRMCTR1		0xB1	//设置全色正常模式的帧频
#define LCD_FRMCTR2 	0xB2	//设置空闲模式的帧频
#define LCD_FRMCTR3 	0xB3	//设置部分模式/全色的帧频率
#define LCD_INVCRT 		0xB4	//反转模式控制
#define LCD_PWCTR1 		0xC0	//设置AVDD、MODE、VRHP、VRHN
#define LCD_PWCTR2 		0xC1	//设置VGH与VGL的供电功率
#define LCD_PWCTR3 		0xC2	//设置正常模式/全色模式下的运放的电流
#define LCD_PWCTR4 		0xC3	//设置空闲模式/八色模式下的运放的电流
#define LCD_PWCTR5 		0xC4	//设置部分模式/全色模式下的运放的电流
#define LCD_VMCTR1 		0xC5	//设置VCOM电压电平以减少闪烁问题
#define LCD_VMOFCTR		0xC7	//VCOM偏移控制，在使用命令0xC7之前，命令0xD9的位VMF_EN必须启用(设置为1)
#define LCD_WRID2		0xD1	//写入LCD模块版本的7位数据，保存到NVM
#define LCD_WRID3		0xD2	//写入项目代码模块的8位数据，保存到NVM
#define LCD_NVFCTR1		0xD9	//NVM状态控制
#define LCD_RDID1		0xDA	//读字节返回8位LCD模块的制造商ID
#define LCD_RDID2		0xDB	//读字节返回8位LCD模块/驱动程序版本ID
#define LCD_RDID3		0xDC	//读字节返回8位LCD模块/驱动ID
#define LCD_NVFCTR2		0xDE	//NVM读取命令
#define LCD_NVFCTR3		0xDF	//NVM写取命令
#define LCD_GMCTRP1		0xE0	//Gamma ‘+’ Polarity Correction Characteristics Setting
#define LCD_GMCTRN1		0xE1	//Gamma ‘+’ Polarity Correction Characteristics Setting
#define LCD_GCV			0xFC	//自动调节门泵时钟，节省功耗

#define X_CENTER 		63-24
#define Y_CENTER 		63

enum TFT_DATA_TYPE {
	TFT_CMD = 0,
	TFT_DATA
};

void lcd_init(void);
void lcd_config(void);
void lcd_reset(void);
void lcd_write(u8 c_d, u8 data);
void set_background(u16 color);
void set_scan_direction(u8 data);
void set_color_format(u8 data);
void set_frame_size(u16 x_start, u16 x_end, u16 y_start, u16 y_end);
void set_point(u16 x, u16 y, u16 color);
void set_rectangle(u16 x_start, u16 x_end, u16 y_start, u16 y_end, u16 color);
void DispGrayHor16(void);
void DispFrame(void);
void DispBand(void);
void DispPic(u8 x,u8 y,u8 w, u8 h,const unsigned char *p);
void Lcd_Clear(u16 Color)  ;
void Lcd_Init(void);
void set_line(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void LCD_WriteData_16Bit(u16 Data);
void set_sin(u16 A,u16 w, u16 color,int t);
#endif	//__LCD_H
