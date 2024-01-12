//spi.c
#include "spi.h"
#include "driver/gpio.h"

#include "esp_log.h"



static const char* TAG = "spi";

#if SPI_HALDE
	static spi_device_handle_t* handle;
#endif

#define SPI_DATA_IN 		gpio_get_level(SPI_DATA_IN_PIN)
#define SPI_DATA_OUT(x) 	gpio_set_level(SPI_DATA_OUT_PIN,x)
#define SPI_SCLK(x)			gpio_set_level(SPI_SCLK_PIN,x)
#define SPI_CS(x)			gpio_set_level(SPI_CS_PIN,x)


void spi_init() {
	
#if SPI_SOFT 
	gpio_config_t ioConfig = {
		.pin_bit_mask = (1ull << SPI_DATA_OUT_PIN)|(1ull << SPI_SCLK_PIN)|(1ull << SPI_CS_PIN),
		.mode = GPIO_MODE_OUTPUT,
	};
	gpio_config(&ioConfig);
	SPI_CS(1);	//拉高SPI片选引脚，使通信无效
	SPI_SCLK(0);
	SPI_DATA_OUT(0);
#endif

#if SPI_HALDE
	handle =(spi_device_handle_t*)malloc(sizeof(spi_device_handle_t));
	bsp_spi_flash_init(handle);
#endif

}



/**
*  @breif 初始化spi,包括初始化总线结构体和设备结构体
*  @param[out] handle: 获取配置SPI的操作句柄
*  @retval esp_err_t
**/
#if SPI_HALDE
esp_err_t bsp_spi_flash_init(spi_device_handle_t* handle)
{
	//00 定义错误标志
	esp_err_t e;

	//01 配置总线初始化结构体
	static spi_bus_config_t bus_cfg;    //总线配置结构体

	bus_cfg.miso_io_num		= Flash_SPI_MISO;				 //miso
	bus_cfg.mosi_io_num		= Flash_SPI_MOSI;				 //mosi
	bus_cfg.sclk_io_num		= Flash_SPI_SCLK;				 //sclk
	bus_cfg.quadhd_io_num	= Flash_SPI_HD;					// HD
	bus_cfg.quadwp_io_num	= Flash_SPI_WP;					// WP
	bus_cfg.max_transfer_sz = 4092;							//非DMA最大64bytes,DMA最大4092bytes
	//bus_cfg.intr_flags = 0;								//这个用于设置中断优先级的，0是默认
	bus_cfg.flags = SPICOMMON_BUSFLAG_MASTER;
	//这个用于设置初始化的时候要检测哪些选项。比如这里设置的是spi初始化为主机模式是否成功。检测结果通过spi_bus_initialize函数的
	//返回值进行返回。如果初始化为主机模式成功，就会返回esp_ok

	//02 初始化总线配置结构体
	e = spi_bus_initialize(Flash_SPI, &bus_cfg, Flash_SPI_DMA);

	if (e != ESP_OK)
	{
		printf("bus initialize failed!\n");
		return e;
	}

	//03 配置设备结构体
	static spi_device_interface_config_t interface_cfg; //设备配置结构体


   interface_cfg.address_bits = Flash_Address_Bits;   //配置地址位长度
   //(1)如果设置为0，在通讯的时候就不会发送地址位。
   //(2)如果设置了非零值，就会在spi通讯的地址发送阶段发送指定长度的address数据。
   //如果设置了非零值并且在后面数据发送结构体中没有定义addr的值，会默认发送指定长度0值
   //(3)我们后面发送数据会使用到spi_transaction_t结构体，这个结构体会使用spi_device_interface_config_t中定义好address、command和dummy的长度
   //如果想使用非固定长度，就要使用spi_transaction_ext_t结构体了。这个结构体包括了四个部分，包含了一个spi_transaction_t和address、command、dummy的长度。
   //我们要做的就是在spi_transaction_ext_t.base.flags中设置SPI_TRANS_VARIABLE_ADDR/CMD/DUMMY
   //然后定义好这三部分数据的长度，然后用spi_transaction_ext_t.base的指针代替spi_transaction_t的指针即可


	 
	interface_cfg.command_bits = Flash_Command_Bits;  //配置命令位长度
	//与address_bits是一样的


	interface_cfg.dummy_bits = Flash_Dummy_Bits;  //配置dummy长度
	//这里的配置方法与address_bits是一样的。但是要着重说一下这个配置的意义，后面会再说一遍
	//(1)dummy_bits是用来用来补偿输入延迟。
	//(2)在read phase开始阶段之前被插入进去。在dummy_bits的时钟下，并不进行数据读取的工作
	//相当于这段时间发送的clock都是虚拟的时钟，并没有功能。在输入延迟最大允许时间不够的时候，可以通过这种方法进行配置，从而
	//能够使得系统工作在更高的时钟频率下。
	//(3)如果主机设备只进行write操作，可以在flags中设置SPI_DEVICE_NO_DUMMY，关闭dummy bits的发送。只有写操作的话，即使使用了gpio交换矩阵,时钟周期也可以工作在80MHZ


	//interface_cfg.input_delay_ns = 0;  //配置输入延时的允许范围
	//时钟发出信号到miso进行输入直接会有延迟，这个参数就是配置这个允许的最大延迟时间。
	//如果主机接收到从机时钟，但是超过这个时间没有收到miso发来的输入信号，就会返回通讯失败。
	//这个时间即使设置为0，也能正常工作，但是最好通过手册或逻辑分析仪进行估算。能够实现更好的通讯。
	//超过8M的通讯都应该认真设置这个数字


	interface_cfg.clock_speed_hz = Flash_CLK_SPEED;  //配置时钟频率
	//配置通讯的时钟频率。
	//这个频率受到io_mux和input_delay_ns限制。
	//如果是io直连的，时钟上限是80MHZ，如果是gpio交换矩阵连接进来的,时钟上限是40MHZ。
	//如果是全双工，时钟上限是26MHZ。并且还要考虑输入延时。在相同输入延时的条件下，使用gpio交换矩阵会比使用io mux最大允许的时钟频率小。可以通过
	//spi_get_freq_limit()来计算能够允许的最大时钟频率是多少
	//有关SPI通讯时钟极限和配置的问题，后面会详细说一下。




	interface_cfg.mode = 0; //设置SPI通讯的相位特性和采样边沿。包括了mode0-3四种。要看从设备能够使用哪种模式

	interface_cfg.spics_io_num = Flash_SPI_CS; //配置片选线


	interface_cfg.duty_cycle_pos = 0;  //配置占空比
	//设置时钟的占空比，比例是 pos*1/256,默认为0，也就是50%占空比


	//interface_cfg.cs_ena_pretrans; //在传输之前，片选线应该保持激活状态多少个时钟，只有全双工的时候才需要配置
	//interface_cfg.cs_ena_posttrans; //在传输之后，片选线应该保持激活状态多少个时钟，只有全双工的时候才需要配置


	interface_cfg.queue_size = 6; //传输队列的长度，表示可以在通讯的时候挂起多少个spi通讯。在中断通讯模式的时候会把当前spi通讯进程挂起到队列中


	//interface_cfg.flags; //配置与从机有关的一些参数，比如MSB还是LSB，使不使用三线SPI

	//interface_cfg.pre_cb; 
	//配置通讯前中断。比如不在这里配置cs片选线，把片选线作为自行控制的线，把片选线拉低放在通讯前中断中


	//interface_cfg.post_cb;
	//配置通讯后中断。比如不在这里配置cs片选线，把片选线作为自行控制的线，把片选线拉高放在通讯前中断中


	//04 设备初始化
	e = spi_bus_add_device(Flash_SPI, &interface_cfg, handle);
	if (e != ESP_OK)
	{
		printf("device config error\n");
		return e;
	}
	

	return ESP_OK;
}
#endif




void spi_start(){
	SPI_CS(0);
}



void spi_write(u8 Data){
#if SPI_SOFT
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
			SPI_DATA_OUT(1); //输出数据
		else 
			SPI_DATA_OUT(0);
		SPI_SCLK(0);
		SPI_SCLK(1);
		Data<<=1; 
	}
#endif
#if SPI_HALDE
	spi_transaction_t *transaction_config = (spi_transaction_t* )malloc(sizeof(spi_transaction_t)) ;//定义数据结构体
	//memset(&transaction_config, 0, sizeof(transaction_config));  //为数据结构体分配内存
	//transaction_config.cmd = 0x9F; //因为是固定内存地址，使用的是nterface_config的配置，也就是8bit cmd，0bit address
	transaction_config->length = 1 * 8;  //要发送或者接收的数据的长度，不算前面的cmd/address/dummy的长度
	transaction_config->tx_buffer = &Data;  //发送没有指定内部空间，使用的是外部区域，因此要自己指定
	transaction_config->rx_buffer = NULL; //接收定义了SPI_TRANS_USE_RXDATA，使用的是内部空间。
	transaction_config->flags = SPI_TRANS_USE_RXDATA;

	spi_device_polling_transmit(handle, transaction_config);

#endif
}





void spi_stop() {
	SPI_CS(1);
}
