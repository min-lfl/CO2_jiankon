#ifndef __SCD40_H__
#define __SCD40_H__

//#########模块导入区##########
//导入I2C库
#include <i2c.h>
//导命令库
#include "SCD40_Cmds.h"
//导入布尔类型变量库
#include <stdbool.h>

//#########宏定义区域############
//传感器I2C定义,加1写，加0读
#define SCD40_Address 0xC4
#define I2C_Prot &hi2c1

//#########数据结构定义区##########
//存储传感器数据的结构体
typedef struct{
	uint16_t	CO2;					//CO2浓度的成员，单位ppm
	uint8_t		Temperature;	//温度成员变量，单位摄氏度oC
	uint8_t		Humidity;			//湿度成员结构体变量，单位%
}SCD40_Data_TypeDef;



//#########应用函数区##########
//发送命令指令，参数为命令枚举，在SCD40_Cmd.h里可查
void SCD40_Send_Cmd(SCD4x_CommandTypeDef cmd);
//初始化SCD40
void SCD40_Init(void);
//获取数据，存入结构体
bool SCD40_Read_Data(SCD40_Data_TypeDef* SCD40_Data);
//均值滤波算法,传入多组结构体，传出一组滤波后的结构体
SCD40_Data_TypeDef SCD40_Mean_Filter(SCD40_Data_TypeDef* SCD40_Dataset,uint16_t Length);

#endif
