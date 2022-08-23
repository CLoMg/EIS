#ifndef DIRECT_H
#define DIRECT_H

typedef enum{
	all_off = 0x00, left, right, bi_dir , dir_err, dir_eof
}Directions_T;

typedef struct Positi{
	unsigned short KM;
	unsigned short M;
}Position_T;
void Direct_Init(void);
void Position_Parameter_Access(void);
void Direct_GPIO_Init(void);
void Direct_Light_Control(Directions_T dir);
void Direct_Process(Directions_T dir);
void *Dir_Func(int *arg);

#endif
