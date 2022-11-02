#ifndef __I2C_H__
#define __I2C_H__

#include "sys.h"
#include "stm32f4xx_conf.h"
 
static void I2Cx_GPIO_Config(I2C_TypeDef* I2Cx);
void I2Cx_Init(I2C_TypeDef* I2Cx);
Error_Code_t I2Cx_Write_Single(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *write_buff);
Error_Code_t I2Cx_Write_Multiple(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *write_buff,u8 length);
u8 I2Cx_Read_Single(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *read_buff);
u8 I2Cx_Read_Multiple(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *read_buff,uint16_t length);

Error_Code_t I2Cx_Acknowled_Check(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT,uint16_t Count);
#endif
