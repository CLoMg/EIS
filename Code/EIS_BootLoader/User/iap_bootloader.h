#ifndef IAP_BOOTLOADER_H 
#define IAP_BOOTLOADER_H

typedef void (*iapfun)(void);
void MSR_MSP(unsigned int addr);
void iap_load_app(unsigned int app_addr);
#endif 
