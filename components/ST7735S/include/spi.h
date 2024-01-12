//spi.h
#ifndef __SPI_H
#define __SPI_H






#define u8					unsigned char
#define u16					unsigned int



#define     SPI_SOFT        1
#define     SPI_HALDE       0


#if SPI_HALDE
#include "driver/spi_master.h"
#include "driver/spi_common.h"
    #define Flash_SPI								SPI2_HOST
    #define Flash_SPI_MISO							SPI_DATA_IN_PIN  	
    #define Flash_SPI_MOSI							SPI_DATA_OUT_PIN	
    #define Flash_SPI_SCLK							SPI_SCLK_PIN	
    #define Flash_SPI_CS							SPI_CS_PIN		
    #define Flash_SPI_WP							-1
    #define Flash_SPI_HD							-1
    #define Flash_SPI_DMA							SPI_DMA_CH_AUTO

    //定义设备参数
    #define Flash_CLK_SPEED							6 * 1000 * 1000  //6M的时钟
    #define Flash_Address_Bits						1*8				//地址位长度
    #define Flash_Command_Bits						1*8				//命令位长度
    #define Flash_Dummy_Bits						0*8				//dummy位长度
    esp_err_t bsp_spi_flash_init(spi_device_handle_t* handle);
    
#endif 





#define SPI_DATA_IN_PIN		0



#define SPI_DATA_OUT_PIN	17



#define SPI_SCLK_PIN		18



#define SPI_CS_PIN			14




#define SPI_SCLK_HIGH_KEEP	0	//时钟信号低电平维持时间
#define SPI_SCLK_LOW_KEEP	0	//时钟信号高电平维持时间

#define SPI_DATA_VALID		1	//0-低电平有效， 1-高电平有效
#define SPI_EDGE_TRIGGERED	1	//0-下降沿有效， 1-上升沿有效

#define SPI_WRITE_ENABLE	1	//0-禁止SPI写入数据，1-使能SPI读取数据
#define SPI_READ_ENABLE		0	//0-禁止SPI读取数据，1-使能SPI写入数据

void spi_init(void);
void spi_start(void);
void spi_write_place(u8 data);
void spi_write(u8 data);
u8 spi_read_place(void);
u8 spi_read(void);
void spi_stop(void);

#endif	//__SPI_H
