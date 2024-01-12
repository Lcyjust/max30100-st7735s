#ifndef __IICCOM_H
#define __IICCOM_H


#include <stdint.h>

#define MAX30100_SDA_PIN            8
#define MAX30100_SCLK_PIN           4

#define FALSE 0
#define TRUE	1

void I2C_GPIO_Config(void);                                //IIC引脚初始化

void I2C_delay(void);

uint8_t I2C_Start(void); //IIC开始
void I2C_Stop(void);//IIC停止

void I2C_Ack(void);//IIC发送
void I2C_NoAck(void);

uint8_t I2C_WaitAck(void); //返回为:=1有ACK,=0无ACK  等待接收
void I2C_SendByte(uint8_t SendByte);//数据从高位到低位//发送数据
unsigned char I2C_RadeByte(void);//数据从高位到低位//读取字节
uint8_t Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);//单字节写入
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);//单字节读取


#endif /*__IICCOM_H*/

