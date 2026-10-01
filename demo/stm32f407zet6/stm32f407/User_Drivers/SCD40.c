//回调用模块宏文件
#include "SCD40.H"

/**
	* @brief		接收一个SCD4x_CommandTypeDef枚举类型的命令，会自动发送给传感器
	* @note			阻塞发送，如果项目有要求慎重
	* @param		SCD4x_CommandTypeDef枚举类型，命令细节可在SCD40_Cmds.h里查
	* @retval		无
	*/
void SCD40_Send_Cmd(SCD4x_CommandTypeDef cmd){
	//命令缓存区初始化
	uint8_t data[2]={0};
	data[0] = (uint8_t)(cmd>>8)&0xFF;	//取出高八位
	data[1] = (uint8_t)cmd&0xFF;			//取出低八位
	
	HAL_I2C_Master_Transmit(I2C_Prot,SCD40_Address+1,data,2,HAL_MAX_DELAY);
	
}


/**
	* @brief		初始化传感器，开启周期输出，我们只需要监控读取就行
	* @note			连续工作模式大概3秒更新一次数据，并且发送
	* @param		无
	* @retval		无
	*/
void SCD40_Init(void){
	//让传感器进入连续工作模式，不断更新数据
	SCD40_Send_Cmd(SCD40_start_periodic_measurement);
	//让传感器周期输出数据
	SCD40_Send_Cmd(SCD40_read_measurement);
}


/**
	* @brief		读取传感器数据包并且解析，连续工作模式大概3秒更新一次数据，并且发送
	* @note			阻塞式读取，采用轮询方式
	* @param		传入一个SCD40_data_TypeDef类型的结构体指针
	* @retval		结构体返回，CO2浓度ppm，温度摄氏度，湿度百分比
* @retval		返回0表示读取不成功，返回1表示读取成功
	*/
bool SCD40_Read_Data(SCD40_Data_TypeDef* SCD40_Data){
	//定义数据缓存区
	uint8_t data[9]={0};
	//定义CRC校验数据
	uint8_t crc=0xFF;
	//定义读取成功标志位,默认值失败
	HAL_StatusTypeDef status = HAL_ERROR;
	
	
	//发送准备读取指令
	SCD40_Send_Cmd(SCD40_read_measurement);
	//阻塞接收I2C数据
	status = HAL_I2C_Master_Receive(I2C_Prot,0xC4,data,9,HAL_MAX_DELAY);
	//如果读取失败
	if(status != HAL_OK){
		//退出读取程序
		return 0;
	}
	
	//数据校验程序
	//j表示每三个数据为一组
	for(uint16_t j = 0; j < 3;j++){
		crc=0xFF;
		//i会取出每组每个要校验的数据
		for (uint16_t i = 0; i < 2; i++) {
			crc ^= data[j*3+i];
			//校验算法部分
			for (uint8_t bit = 8; bit > 0; --bit) {
				if (crc & 0x80) {
					crc = (crc << 1) ^ 0x31; // 多项式 0x31
				} else {
					crc = (crc << 1);
				}
			}
		}
		//如果校验算法后结果不等于原始数据里的校验值
		if(crc!=data[j*3+2]){
			//退出读取程序
			return 0;
		}
	}
	
	//数据处理
	SCD40_Data -> CO2=data[0]<<8|data[1];														//二氧化碳
	SCD40_Data -> Temperature=-45+(175*(data[3]<<8|data[4]))/65535;	//温度
	SCD40_Data -> Humidity=100*(data[6]<<8|data[7])/65535;					//湿度
	
	//数据处理完成后返回读取成功
	return 1;
}


/**
	* @brief		均值滤波算法，提供SCD40_Data_TypeDef类型的数组，和数组长度，返回一个SCD40_Data_TypeDef类型的变量，存储了均值滤波后的数据
	* @note			没有校验程序，传入的数组长度必须合法
	* @param		SCD40_Data_TypeDef类型的数组，存储多组数据
	* @param		数组长度，传入的数组中数据的个数
	* @retval		传入的多个数据均值滤波后的平均值
	*/
SCD40_Data_TypeDef SCD40_Mean_Filter(SCD40_Data_TypeDef* SCD40_Dataset,uint16_t Length){
	//定义均值缓存区
	SCD40_Data_TypeDef SCD40_Data={0};
	

	//定义数据处理需要用到的独立变量
	uint32_t	CO2=0;
	uint32_t	Temperature=0;
	uint32_t	Humidity=0;

	//开始遍历数据求和
	for(uint16_t i=0;i<Length;i++){
		CO2+=SCD40_Dataset[i].CO2;
		Temperature+=SCD40_Dataset[i].Temperature;
		Humidity+=SCD40_Dataset[i].Humidity;
	}
	
	//把求和后的数据均分赋值
	SCD40_Data.CO2=(uint16_t)CO2/Length;
	SCD40_Data.Temperature=(uint8_t)Temperature/Length;
	SCD40_Data.Humidity=(uint8_t)Humidity/Length;
	
	//赋值后输出结构体
	return SCD40_Data;
}

