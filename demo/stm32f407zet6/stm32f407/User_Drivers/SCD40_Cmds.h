#ifndef __SCD40_CMDS_H__
#define __SCD40_CMDS_H__



typedef enum
{
//#####基础命令 (Basic commands)
	
	//描述： 让传感器进入连续工作模式。默认情况下，传感器大约每 5 秒更新一次 CO2、温度和湿度数据。
	SCD40_start_periodic_measurement = 0x21b1, //开始周期性测量

	//描述： 让传感器周期不断的输出 CO2 浓度、温度和湿度数值。我们监测读取就行
	//通常在检测到数据就绪后调用,会读出9个字节，每三个字节分别代表CO2浓度，温度，湿度
	SCD40_read_measurement = 0xec05, 					//读取测量结果

	//描述： 让传感器停止自动测量，进入空闲模式。在修改某些配置（如设置海拔）之前，通常需要先停止测量。
	SCD40_stop_periodic_measurement = 0x3f86,	//停止周期性测量

//#####片上输出信号补偿 (On-chip output signal compensation)
//#####由于物理环境（压力、热量）会影响 CO2 读数，这些命令用于校准误差。
	
	//描述： 如果传感器安装在发热的 PCB 上，实测温度会偏高。通过此命令设置补偿值，以获得准确的环境温度。
	SCD40_set_temperature_offset = 0x241d,		//设置温度偏移量
	SCD40_get_temperature_offset = 0x2318,		//获取温度偏移量
	
	//描述： 气压会影响 CO2 的物理测量。设置当前所在地的海拔（米），传感器会自动调整算法以补偿气压偏差。
	SCD40_set_sensor_altitude = 0x2427,				//设置传感器海拔高度
	SCD40_get_sensor_altitude = 0x2322,				//获取海拔高度
	
	//描述： 比设置海拔更精确。如果你手头有气压计，可以直接输入当前的实时气压值（hPa）进行补偿。
	SCD40_set_ambient_pressure = 0xe000,			//设置环境压力，读写位为写
	SCD40_get_ambient_pressure = 0xe000,			//获取环境压力，读写位为读
	
	
//#####现场校准 (Field calibration)
//#####用于确保传感器在长期运行后依然保持准确。

	//描述： 人为给传感器一个参考值。例如你把传感器放在新鲜空气中（已知约 400ppm），调用此命令告诉它“现在就是 400ppm”，它会立即修正偏差。
	SCD40_perform_forced_recalibration = 0x362f,	//执行强制重新校准 (FRC)

	//描述： 开启后，传感器会根据过去几天的最低读数（通常认为是背景空气）自动进行校准。
	SCD40_set_automatic_self_calibration_enabled = 0x2416, //设置自动自校准 (ASC) 启用状态
	SCD40_get_automatic_self_calibration_enabled = 0x2313, //获取自动自校准状态
	
	//描述： 设置自动校准参考的基准值，默认通常是 400ppm（室外大气水平）。
	SCD40_set_automatic_self_calibration_target = 0x243a,		//设置自动自校准目标值
	SCD40_get_automatic_self_calibration_target = 0x233f,		//获取目标值
	
	
//#####高级功能 (Advanced features)
//#####系统维护和设备信息。

	//描述：重要！ 之前设置的温度偏移、海拔等参数在掉电后会丢失。调用此命令将设置永久保存到传感器内部。
	SCD40_persist_settings = 0x3615, 	//保存设置到非易失性存储器

	//描述： 读取传感器的唯一身份 ID。
	SCD40_get_serial_number = 0x3682, 	//获取序列号

	//描述： 传感器检查内部硬件是否正常工作，大约需要 10 秒。
	SCD40_perform_self_test = 0x3639,		//执行自检

	//描述： 清除所有自定义配置，恢复到默认状态。
	SCD40_perform_factory_reset = 0x3632, 	//执行恢复出厂设置

	//描述： 软件复位，重新加载设置。
	SCD40_reinit = 0x3646,	//重新初始化

	//描述： 识别是 SCD40, SCD41 还是 SCD43。
	SCD40_get_sensor_variant = 0x202f, //获取传感器型号
	
//#####低功耗周期性测量模式 (Low power periodic measurement mode)
//#####适用于电池供电的场景。

	//描述： 测量频率大大降低（约每 30 秒一次），从而显著降低平均功耗。
	SCD40_start_low_power_periodic_measurement = 0x21ac, //开始低功耗周期性测量

	//描述： 检查传感器是否已经完成了当前周期的测量并准备好数据。
	SCD40_get_data_ready_status = 0xe4b8,	//获取数据就绪状态

} SCD4x_CommandTypeDef;


#endif
