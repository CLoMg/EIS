#ifndef __SYS_H
#define __SYS_H	 
#include "stm32f4xx.h" 

//获取产品唯一ID
#ifdef __STM32F10x_H
#define ID_ADDR1 0x1FFFF7E8
#endif
#ifdef __STM32F2xx_H
#define ID_ADDR1 0x1FFF7A10
#endif
#ifdef __STM32F4xx_H
#define ID_ADDR1 0x1FFF7A10
#endif

/***定义设备类型 
0--魏家山/龙溪隧道A型: U1:CH4（炜盛） U4:NH3(炜盛) U5:CO(炜盛) U6:PM2.5/PM10
1--魏家山/龙溪隧道B型：U1:SO2（炜盛） U4:H2S(炜盛) U5:NO2(炜盛) U6:CO2(炜盛)
2--龙池/成德南 I型 U1:CH4（UESTC） U4:CH4(HW) U5:NH3(HW) U6:PM2.5/PM10
3--龙池       II型 U1:CO（UESTC）  U4:CO(HW)  U5:H2S(HW) U6:H2S(UESTC)
4--          III型                 U4:SO2(炜盛)  U5:NO2(炜盛） U6:CO2(炜盛) 
**/
#define gateway 0
#define iwatch_dog_work 1
extern uint8_t device_type_num;
//#define device_type 3

//定义传感器5v 供电使能引脚 旧版 PA3 新版PD9
//#define Sensor_5V_PowerEN PAout(3)
#define Sensor_5V_PowerEN PDout(9)

#define serial_port1_debug 0
//char device_info[][60]={"U1:CH4（WS） U4:NH3(WS) U5:CO(WS) U6:PM2.5/PM10",
//											"U1:SO2（WS） U4:H2S(WS) U5:NO2(WS) U6:CO2(WS)",
//											"U1:CH4（UESTC） U4:CH4(HW) U5:NH3(HW) U6:PM2.5/PM10",
//											"U1:CO（UESTC）  U4:CO(HW)  U5:H2S(HW) U6:H2S(UESTC)",
//											"U1:CO（UESTC）  U4:CO(HW)  U5:H2S(HW) U6:CO2(UESTC)",
//											"U1:H2S（UESTC） U4:CO(HW)  U5:H2S(HW) U6:CO2(UESTC)"};
enum gas_type
{
	CH4=4,CO=4,SO2=4,NH3=5,H2S=5,NO2=5,CO2=6,
};
//本网关在所管理终端设备
extern uint8_t terminal_id[9][8];
	
//定义device_id获取方式 旧方式为手动定义，新方式为获取stm32 96位mac地址，取低64位作为device_id
#define New_ID_Access 1
#define data_simulated 1


//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_UCOS		0		//定义系统文件夹是否支持UCOS
//错误检查函数
#define CHECK_ERROR(e) if(e != 1) return e;
#define Error_Code_t _Bool
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
void Get_Device_ID(uint32_t *id);//获取设备ID
void Sensor_5V_PowerEN_Init(void);//
#endif











