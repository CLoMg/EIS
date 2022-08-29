#include "iap_bootloader.h"
#include "gd32f10x_fmc.h"
#include "gd32f10x.h"
#include "stdint.h"


iapfun jump2app;
/*设置栈顶地址*/
void MSR_MSP(uint32_t addr)
{
		__ASM volatile("MSR MSP, r0");
		__ASM volatile("BX r14");
} 
void iap_load_app(uint32_t  app_addr){
	if(((*(uint32_t *)app_addr)&0x2FFE0000) == 0x20000000) //检查栈顶地址是否合法
	{
		jump2app = (iapfun)*(uint32_t *)(app_addr+4);
		MSR_MSP(*(uint32_t *)app_addr);
		jump2app();
	}
}

