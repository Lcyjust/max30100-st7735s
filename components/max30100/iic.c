/**
  ******************************************************************************
  * @file    IICcom.c
  * @author  张东
  * @version V1.0.0
  * @date    2019-7-10
  * @brief   软件IIC通信
  ******************************************************************************
  */
	
/*--Include-start-------------------------------------------------------------*/
#include "iic.h"
#include "driver/gpio.h"
#include "delay.h"
/*--Include-end---------------------------------------------------------------*/


/*模拟IIC端口输出输入定义*/


#define SCL_H         gpio_set_level(MAX30100_SCLK_PIN,1)
#define SCL_L         gpio_set_level(MAX30100_SCLK_PIN,0)
   
#define SDA_H         gpio_set_level(MAX30100_SDA_PIN,1)
#define SDA_L         gpio_set_level(MAX30100_SDA_PIN,0)

#define SCL_read      gpio_get_level(MAX30100_SCLK_PIN)
#define SDA_read      gpio_get_level(MAX30100_SDA_PIN)



/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)                                //IIC引脚初始化
{
    gpio_config_t ioConfig = {
		.pin_bit_mask = (1ull << MAX30100_SDA_PIN)|(1ull << MAX30100_SCLK_PIN),
		.mode = GPIO_MODE_INPUT_OUTPUT,
	};
	gpio_config(&ioConfig);
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{        
	delay_us(1);
	//uint16_t i=50; //这里可以优化速度        ，经测试最低到5还能写入
//    while(i) 
//    { 
//      i--; 
//    }  
}
void delay5ms(void)
{
                
	delay_ms(5);
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather         Start
****************************************************************************** */
uint8_t I2C_Start(void)                        //IIC开始
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return FALSE;        //SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;        //SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)                                //IIC停止
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)                //IIC发送
{        
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)        
{        
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather         Reserive Slave Acknowledge Single
****************************************************************************** */
uint8_t I2C_WaitAck(void)          //返回为:=1有ACK,=0无ACK  等待接收
{
	uint8_t re;
	
	SCL_L;
	I2C_delay();
	SDA_H;                        
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	SCL_L;
	I2C_delay();
	return re;
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(uint8_t SendByte) //数据从高位到低位//发送数据
{
	uint8_t i=8;
	while(i--)
	{
		SCL_L;
		I2C_delay();
		
		if(SendByte&0x80)
			SDA_H;  
		else 
			SDA_L;
		
		SendByte<<=1;
		I2C_delay();
		SCL_H;
		I2C_delay();
	}
	SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_RadeByte(void)  //数据从高位到低位//读取字节
{ 
	uint8_t i=8;
	uint8_t ReceiveByte=0;

	SDA_H;                                
	while(i--)
	{
		ReceiveByte<<=1;      
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();        
		if(SDA_read)
		{
			ReceiveByte|=0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
} 
//ZRX          
//单字节写入*******************************************

uint8_t Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)                     //void 
{
	if(!I2C_Start())return FALSE;

	I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
	if(!I2C_WaitAck())
	{
		I2C_Stop(); return FALSE;
	}
	I2C_SendByte(REG_Address );   //设置低起始地址      
	I2C_WaitAck();        
	I2C_SendByte(REG_data);
	I2C_WaitAck();   
	I2C_Stop(); 
	delay5ms();
	return TRUE;
}

//单字节读取*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   
	unsigned char REG_data;    
	
	if(!I2C_Start())return FALSE;
	
	I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
	
	if(!I2C_WaitAck())
	{
		I2C_Stop();
		//test=1; 
		return FALSE;
	}
	I2C_SendByte((uint8_t) REG_Address);   //设置低起始地址      
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(SlaveAddress+1);
	I2C_WaitAck();

	REG_data= I2C_RadeByte();
	I2C_NoAck();
	I2C_Stop();
	
	//return TRUE;
	return REG_data;

}             

