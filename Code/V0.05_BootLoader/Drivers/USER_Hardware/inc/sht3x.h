#ifndef SHT3x_H
#define SHT3x_H



extern unsigned char humiture_buff1[20];
extern unsigned char humiture_buff2[20];
extern unsigned char Refresh_SHT30_Data;
extern unsigned char send_data_fleg;
extern unsigned char Temperature_L;
extern unsigned char Humidity_L;
extern unsigned char Temperature_H;
extern unsigned char Humidity_H;

extern float Temperature;
extern float Humidity;

unsigned char SHT30_Acquire (unsigned char slave_addr,unsigned char *data_buff);
unsigned char SHT30_read_result(unsigned char slave_addr,unsigned char *data_buff);
void SHT30_Init(void);

 

#endif

 
