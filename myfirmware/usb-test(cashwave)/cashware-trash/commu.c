#include "commu.h"
#include "debug.h"
#include "rtthread.h"
//PIN0~PIN7:info
//PIN8:CLK
//PIN9:endflag(high enable)
//#define CLK_H IO1(IOEB,PIN11)
//#define CLK_L IO0(IOEB,PIN11)
//#define END_H IO1(IOEB,PIN13)
//#define END_L IO0(IOEB,PIN13)
//#define DATA_SET(a) do{GPIOE->ODR&=0XFC03;GPIOE->ODR|=((((u16)a)<<2)&0X3fc);}while(0)




u8 commu_send_byte(u8 info);

//#define RW_H    IO1(IOCB,PIN12)//H:R L:W
//#define RW_L    IO0(IOCB,PIN12)
#define CLK_H   IO1(IOBB,PIN9)
#define CLK_L   IO0(IOBB,PIN9)
#define FINISH_FLAG    PBin(8)

#define DATA_W(data)    do{IOBB->ODR&=0XFF00;IOBB->ODR|=(0X00ff&data);}while(0)
#define DATA_R  (IOBB->IDR&=0XFF)
void commu_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure0;
	SPI_InitTypeDef  SPI_InitStructure;

  
    
	SCPE(PERIOB);
	SCPE(PERIOA);
    IOConfig(IOBB,PIN2|PIN6|PIN7,fukongshuru);
	GPIO_PinAFConfig(IOBB, 3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IOBB, 4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IOBB, 5, GPIO_AF_SPI1);

	GPIO_InitStructure0.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure0.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure0.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure0.GPIO_PuPd  = GPIO_PuPd_DOWN;



	GPIO_InitStructure0.GPIO_Pin=PIN3;
	GPIO_Init(GPIOB, &GPIO_InitStructure0);
	GPIO_InitStructure0.GPIO_Pin=PIN4;
	//IOConfig(IOBB,PIN4,fukongshuru);
	GPIO_Init(GPIOB, &GPIO_InitStructure0);
	GPIO_InitStructure0.GPIO_Pin=PIN5;
	GPIO_Init(GPIOB, &GPIO_InitStructure0);
	

	
	IOConfig(IOAB,PIN15,tuiwanshuchu);
	IO1(IOAB,PIN15);

	SCPE(PERSPI1);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	



}
#define FPGA_CS_L IO0(IOAB,PIN15)
#define FPGA_CS_H IO1(IOAB,PIN15)
// void commu_send_byte_half (u8 info);

u8 commu_send_byte(u8 info)
{

//    commu_send_byte_half(info);
//	commu_send_byte_half(info<<4);

	int i,j,k;
	u8 data=0;

   FPGA_CS_L;
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the sFLASH peripheral */
  SPI_I2S_SendData(SPI1, info);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
   FPGA_CS_H;
  /*!< Return the Half Word read from the SPI bus */
  	for(i=0;i<1;i++)
	{
		for(j=0;j<10;j++)
		for(k=0;k<62;k++);
	}
  data=SPI_I2S_ReceiveData(SPI1);
 // DBG("%c-%c",info,data);
  return data;

	
}
// void commu_send_byte_half(u8 info)
//{
//    volatile int cnt=0;
//	u32 i=0;
//    begin:
//	DATA_W(info);
//    CLK_L;
//    while(!FINISH_FLAG)
//	{
//		i++;
//		if(i>3000000)
//		{
//			CLK_H;
//			 for(cnt=0;cnt<100;cnt++);
//			goto begin;
//		}
//			
//	}
//    CLK_H;
//    for(cnt=0;cnt<100;cnt++);
//	
//}
void blue_putchar(u8 ch);
void commu_blue_send(u8* buf,u32 lenth)
{
    u32 i=0;
   // blue_putchar(7<<4);
    for(i=0;i<lenth;i++)
	{
		blue_putchar(buf[i]);
        rt_thread_delay(1);
	}
}
void commu_send(u8* buf,u32 lenth)
{
	u32 i=0;
	//END_L;
//    RW_L;//W
//    CLK_H;
	//commu_send_byte_half(7<<4);
	if(lenth==512)
	{
		commu_send_byte(11<<4);
	}
	else
		commu_send_byte(7<<4);
	for(i=0;i<lenth;i++)
	{
		commu_send_byte(buf[i]);
		//delay_ms2(10);
	}	
}
void commu_recv_byte_half(u8* data);
 void commu_recv_byte(u8* data)
{
	*data=commu_send_byte(0x12);

}

//void commu_recv_byte_half(u8* data)
//{
//   volatile int cnt=0;
//	u32 i=0;
//    begin:
//    CLK_L;
//    while(!FINISH_FLAG)
//	{
//		i++;
//		if(i>30000)
//		{
//			CLK_H;
//			 for(cnt=0;cnt<100;cnt++);
//			goto begin;
//		}
//			
//	}
//	*data=IOBB->IDR&0XFF;
//	//DBG("%d,",IOBB->IDR&0XFF);
//    CLK_H;
//	 while(FINISH_FLAG);
//    for(cnt=0;cnt<100;cnt++);
//}



void commu_recv(u8* buf,u32 lenth)
{
    u32 i=0;

	if(lenth==512)
	{
		commu_send_byte(12<<4);
	}
	else
    commu_send_byte(9<<4);
commu_send_byte(12);
    for(i=0;i<lenth;i++)
    {
        commu_recv_byte(buf+i);
		
    }

}

