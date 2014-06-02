#include <rtthread.h>
#include <rtgui/driver.h>
#include "ra8875.h"

/********* control ***********/
#include "board.h"


/* redirect. */
#define printf               rt_kprintf     /* use rt_kprintf. */
//#define printf(...)                       /* none. */

typedef struct
{
  __IO uint16_t LCD_RAM;
	__IO uint16_t LCD_REG;
} LCD_TypeDef;
     
#define Bank1_SRAM1_ADDR  ((uint32_t)0x60000000)
#define Bank1_SRAM2_ADDR  ((uint32_t)0x64000000) 
#define Bank1_SRAM3_ADDR  ((uint32_t)0x68000000)
#define Bank1_SRAM4_ADDR  ((uint32_t)0x6C000000)

#define LCD_BASE           Bank1_SRAM3_ADDR
#define LCD                ((LCD_TypeDef *) LCD_BASE)

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE2 is used as ship select signal */
/* RS <==> A19 */
#define LCD_DATA              LCD->LCD_RAM /* RS = 0 */
#define LCD_CMD               LCD->LCD_REG /* RS = 1 */

//#define Delay1ms(tick)        rt_thread_delay(tick)
void delay(uint32_t delayCnt)
{
	uint32_t i;

	for ( i = 0; i < delayCnt; i++ );
	return;
}

static struct rt_device _lcd_device;

/* LCD_busy --> PD6 --> FSMC_NWAIT */
rt_inline void _wait_bus_ready(void)
{
    while(!(GPIOD->IDR & GPIO_Pin_6)); // 0-busy 1-ready
}

static int ra8875_is_ready(void)
{
    return (GPIOD->IDR & GPIO_Pin_6);
}

rt_inline void _wait_lcd_ready(void)
{
    uint16_t status;
    do
    {
        _wait_bus_ready();
        status = LCD_CMD;
    }
    while(status & (1<<7)); // [7] 0-ready 1- busy
}

rt_inline void LCD_CmdWrite(uint16_t reg)
{
    _wait_bus_ready();
    LCD_CMD = reg;
}

rt_inline void LCD_DataWrite(uint16_t value)
{
    _wait_bus_ready();
    LCD_DATA = value;
}

rt_inline uint16_t LCD_DataRead(void)
{
    _wait_bus_ready();
    return LCD_DATA;
}

static void LCD_write_reg(uint8_t reg, uint8_t value)
{
    LCD_CmdWrite(reg);
    LCD_DataWrite(value);
}

static uint8_t LCD_read_reg(uint8_t reg)
{
    LCD_CmdWrite(reg);
    return LCD_DataRead();
}
void FSMC_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  //FSMC_NORSRAMTimingInitTypeDef  p;
	FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;
  GPIO_InitTypeDef GPIO_InitStructure; 
	
	
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;
		

  
  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                         RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable FSMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
/*-- GPIOs Configuration ------------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
 +                       SRAM pins assignment                                  +
 +-------------------+--------------------+------------------+------------------+
 | PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0 <-> FSMC_A10 |
 | PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1 <-> FSMC_A11 |
 | PD4  <-> FSMC_NOE | PE7  <-> FSMC_D4   | PF2  <-> FSMC_A2 | PG2 <-> FSMC_A12 |
 | PD5  <-> FSMC_NWE | PE8  <-> FSMC_D5   | PF3  <-> FSMC_A3 | PG3 <-> FSMC_A13 |
 | PD8  <-> FSMC_D13 | PE9  <-> FSMC_D6   | PF4  <-> FSMC_A4 | PG4 <-> FSMC_A14 |
 | PD9  <-> FSMC_D14 | PE10 <-> FSMC_D7   | PF5  <-> FSMC_A5 | PG5 <-> FSMC_A15 |
 | PD10 <-> FSMC_D15 | PE11 <-> FSMC_D8   | PF12 <-> FSMC_A6 | PG9 <-> FSMC_NE2 |
 | PD11 <-> FSMC_A16 | PE12 <-> FSMC_D9   | PF13 <-> FSMC_A7 |------------------+
 | PD12 <-> FSMC_A17 | PE13 <-> FSMC_D10  | PF14 <-> FSMC_A8 | PG10 <-> FSMC_NE3
 | PD14 <-> FSMC_D0  | PE14 <-> FSMC_D11  | PF15 <-> FSMC_A9 | PG12 <-> FSMC_NE4
 | PD15 <-> FSMC_D1  | PE15 <-> FSMC_D12  |------------------+
 +-------------------+--------------------+
 PD7 <-> FSMC_NE1
*/
  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  //GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); 
  //GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | //GPIO_Pin_7 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | //GPIO_Pin_11 |
                                /*GPIO_Pin_12 |*/ GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* GPIOE configuration */
  //GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
  //GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = /*GPIO_Pin_0  | GPIO_Pin_1 |*/ GPIO_Pin_7 |
                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_Init(GPIOE, &GPIO_InitStructure);


  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 /* | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5  | 
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15*/;      

  GPIO_Init(GPIOF, &GPIO_InitStructure);


  /* GPIOG configuration */
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
//   GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = /*GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5  |*/GPIO_Pin_10;//  |GPIO_Pin_12;      

  GPIO_Init(GPIOG, &GPIO_InitStructure);

/*-- FSMC Configuration ------------------------------------------------------*/
 // p.FSMC_AddressSetupTime = 5;
 // p.FSMC_AddressHoldTime = 0;
 // p.FSMC_DataSetupTime = 9;
 // p.FSMC_BusTurnAroundDuration = 0;
 // p.FSMC_CLKDivision = 1;
 // p.FSMC_DataLatency = 0;
 // p.FSMC_AccessMode = FSMC_AccessMode_A;
//   p.FSMC_AddressSetupTime = 0x5;
//   p.FSMC_AddressHoldTime = 0x7;
//   p.FSMC_DataSetupTime = 0x7;
//   p.FSMC_BusTurnAroundDuration = 0x00;
//   p.FSMC_CLKDivision = 0x00;
//   p.FSMC_DataLatency = 0x00;
//   p.FSMC_AccessMode = FSMC_AccessMode_B;
//   p.FSMC_AddressSetupTime = 0;
//   p.FSMC_AddressHoldTime = 0;
//   p.FSMC_DataSetupTime = 4;
//   p.FSMC_BusTurnAroundDuration = 1;
//   p.FSMC_CLKDivision = 0;
//   p.FSMC_DataLatency = 0;
//   p.FSMC_AccessMode = FSMC_AccessMode_A;

    /*--------------------- read timings configuration ---------------------*/
    Timing_read.FSMC_AddressSetupTime = 5;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_read.FSMC_AddressHoldTime = 0;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_read.FSMC_DataSetupTime = 9;     /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_read.FSMC_BusTurnAroundDuration = 0;
    Timing_read.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;

    /*--------------------- write timings configuration ---------------------*/
    Timing_write.FSMC_AddressSetupTime = 5;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_write.FSMC_AddressHoldTime = 0;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_write.FSMC_DataSetupTime = 9;     /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_write.FSMC_BusTurnAroundDuration = 0;
    Timing_write.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
    Timing_write.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
    Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;


  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  //FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
 // FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /*!< Enable FSMC Bank1_SRAM3 Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE); 	
}

#define RESOLUTION  1 
void LCD_Init(void)
{
	LCD_write_reg(0x88, 0x0A);
	delay(2000);
	LCD_write_reg(0x89, 0x02);
	delay(2000);
	
	LCD_write_reg(0x04, 0x82);  // PCLK
	delay(2000);

	//Horizontal set
#if RESOLUTION == 1	
	LCD_write_reg(0x14, 0x3B);  // 
	LCD_write_reg(0x15, 0x00);
	LCD_write_reg(0x16, 0x01);
	LCD_write_reg(0x17, 0x00);
	LCD_write_reg(0x18, 0x05);
#else
	LCD_write_reg(0x14, 0x63);   
	LCD_write_reg(0x15, 0x00);
	LCD_write_reg(0x16, 0x01);
	LCD_write_reg(0x17, 0x00);
	LCD_write_reg(0x18, 0x05);	
#endif

#if RESOLUTION == 1	
	//Vertical  set
	LCD_write_reg(0x19, 0x0F);  // 
	LCD_write_reg(0x1A, 0x01);
	LCD_write_reg(0x1B, 0x02);
	LCD_write_reg(0x1C, 0x00);
	LCD_write_reg(0x1D, 0x07);
	LCD_write_reg(0x1E, 0x00);
	LCD_write_reg(0x1F, 0x09);
#else
	LCD_write_reg(0x19, 0xDF);   
	LCD_write_reg(0x1A, 0x01);
	LCD_write_reg(0x1B, 0x02);
	LCD_write_reg(0x1C, 0x00);
	LCD_write_reg(0x1D, 0x07);
	LCD_write_reg(0x1E, 0x00);
	LCD_write_reg(0x1F, 0x09);
#endif

// 	//setting active window X
// 	LCD_DataWrite(0x30, 0x00);  // 
// 	LCD_DataWrite(0x31, 0x00);
// 	LCD_DataWrite(0x34, 0xDF);
// 	LCD_DataWrite(0x35, 0x01);
// 	
// 	//setting active window Y
// 	LCD_DataWrite(0x32, 0x00);  // 
// 	LCD_DataWrite(0x33, 0x00);
// 	LCD_DataWrite(0x36, 0x0F);
// 	LCD_DataWrite(0x37, 0x01);	
	

	LCD_write_reg(0x10, 0x0F);  // SYSR 16-bit interface, 65k colors
}
//--------------------------------------------//

/* LCD fun */
static void _set_draw_start_cursor(uint32_t X, uint32_t Y)
{
    LCD_write_reg(DLHSR1, X>>8);
    LCD_write_reg(DLHSR0, X);
    LCD_write_reg(DLVSR1, Y>>8);
    LCD_write_reg(DLVSR0, Y);
}

static void _set_draw_end_cursor(uint32_t X, uint32_t Y)
{
    LCD_write_reg(DLHER1, X>>8);
    LCD_write_reg(DLHER0, X);
    LCD_write_reg(DLVER1, Y>>8);
    LCD_write_reg(DLVER0, Y);
}

static void _set_draw_point2_cursor(uint32_t X, uint32_t Y)
{
    LCD_write_reg(DTPH1, X>>8);
    LCD_write_reg(DTPH0, X);
    LCD_write_reg(DTPV1, Y>>8);
    LCD_write_reg(DTPV0, Y);
}

static void _set_draw_center_cursor(uint32_t X, uint32_t Y)
{
    LCD_write_reg(DCHR1, X>>8);
    LCD_write_reg(DCHR0, X);
    LCD_write_reg(DCVR1, Y>>8);
    LCD_write_reg(DCVR0, Y);
}

static void _set_draw_radius(uint32_t radius)
{
    LCD_write_reg(DCRR, radius);
}

static void _set_draw_center_ellipse_cursor(uint32_t X, uint32_t Y)
{
    LCD_write_reg(DEHR1, X>>8);
    LCD_write_reg(DEHR0, X);
    LCD_write_reg(DEVR1, Y>>8);
    LCD_write_reg(DEVR0, Y);
}

static void _set_draw_radius_ellipse(uint32_t X, uint32_t Y)
{
    LCD_write_reg(ELL_A1, X >> 8);
    LCD_write_reg(ELL_A0, X);
    LCD_write_reg(ELL_B1, Y >> 8);
    LCD_write_reg(ELL_B0, Y);
}

static void _set_fore_color(uint16_t pixel)
{
    /* REG 565 */
    LCD_write_reg(FGCR2, pixel & 0x1F); /* blue */
    pixel >>= 5;

    LCD_write_reg(FGCR1, pixel & 0x3F); /* green */
    pixel >>= 6;

    LCD_write_reg(FGCR0, pixel & 0x1F); /* red */
}

static void _set_write_cursor(uint32_t X, uint32_t Y)
{
    LCD_CmdWrite(CURH1);
    LCD_DataWrite(X>>8);
    LCD_CmdWrite(CURH0);
    LCD_DataWrite(X);

    LCD_CmdWrite(CURV1);
    LCD_DataWrite(Y>>8);
    LCD_CmdWrite(CURV0);
    LCD_DataWrite(Y);
}

static void _set_read_cursor(uint32_t X, uint32_t Y)
{
    LCD_CmdWrite(RCURH1);
    LCD_DataWrite(X>>8);
    LCD_CmdWrite(RCURH0);
    LCD_DataWrite(X);

    LCD_CmdWrite(RCURV1);
    LCD_DataWrite(Y>>8);
    LCD_CmdWrite(RCURV0);
    LCD_DataWrite(Y);
}

/** \brief LCD brightness PWM setting.
 *
 * \param pwm_duty_cycle int LED pwm duty cycle, 0~100.
 * \return void
 *
 */
static void pwm_setting(int pwm_duty_cycle)
{
    uint32_t value;

    value  = (1 << 7); /* enable PWM. */
    value |= (0 << 6); /* ouput LOW when PWM STOP. */
    value |= (0 << 4); /* selet PWM1 function. */
    value |= 8;        /* 8: PWM clk = SYS_CLK/256. */
    LCD_CmdWrite(0x8A);
    LCD_DataWrite(value);


    value = (pwm_duty_cycle * 256) / 100;
    if(value > 0xFF)
    {
        value = 0xFF;
    }
    LCD_CmdWrite(0x8B);
    LCD_DataWrite(value);
}

#ifdef RTGUI_USING_HW_CURSOR
static const uint8_t cursor_arrow[256] =
{
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa5,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x91,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x41,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa9, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa9, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa9, 0x00, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0x00, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x00, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x00, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xa9, 0x00, 0x00, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x00, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x00, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x15, 0x01,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa9, 0x00, 0x1a, 0x41,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa9, 0x00, 0x6a, 0x91,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0x00, 0x6a, 0xa5,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa4, 0x01, 0xaa, 0xa9,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa0, 0x01, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x06, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x06, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x0a, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0x90, 0x1a, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xa5, 0x6a, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};

static void _set_mouse_image(const uint8_t *mouse_img)
{
    uint32_t i, j;
    LCD_write_reg(GCC0, 0xFF);
    LCD_write_reg(GCC1, 0x00);
    LCD_write_reg(MWCR1,  2 << 2);

    _set_write_cursor(0, 0);
    LCD_CmdWrite(MRWC);
    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 8; j++)
        {
            LCD_DataWrite(*mouse_img++);
        }
    }

    LCD_write_reg(MWCR1, (1 << 7) | (0 << 2));
}

static void _set_mouse_position(uint16_t X, uint16_t Y)
{
    LCD_CmdWrite(GCHP1);
    LCD_DataWrite(X >> 8);
    LCD_CmdWrite(GCHP0);
    LCD_DataWrite(X);

    LCD_CmdWrite(GCVP1);
    LCD_DataWrite(Y >> 8);
    LCD_CmdWrite(GCVP0);
    LCD_DataWrite(Y);
}
#endif /* RTGUI_USING_HW_CURSOR */


static rt_err_t lcd_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t lcd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    rt_err_t result = -RT_EIO;

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info;

        info = (struct rt_device_graphic_info*) args;
        RT_ASSERT(info != RT_NULL);

        info->bits_per_pixel = 16;
        info->pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565P;
        info->framebuffer = RT_NULL;
        info->width = 480;
        info->height = 272;

        result = RT_EOK;
    }
    break;

#ifdef RTGUI_USING_HW_CURSOR
    case RT_DEVICE_CTRL_CURSOR_SET_TYPE:
    {
        rt_uint32_t type = *(rt_uint32_t *)args;

        if(type == RTGUI_CURSOR_ARROW)
        {
            _set_mouse_image(cursor_arrow);
        }

        result = RT_EOK;
    }
    break;

    case RT_DEVICE_CTRL_CURSOR_SET_POSITION:
    {
        rt_uint32_t value;
        rt_uint16_t x, y;

        value = *(rt_uint32_t *)args;
        x = (value >> 16) & 0xFFFF;
        y = value & 0xFFFF;

        _set_mouse_position(x, y);
        result = RT_EOK;
    }
    break;
#endif /* RTGUI_USING_HW_CURSOR */

    case RTGRAPHIC_CTRL_RECT_UPDATE:
        /* nothong to be done */
        break;

    default:
        break;
    }

    return result;
}

static void ra8875_lcd_set_pixel(const char* pixel, int x, int y)
{
    _set_write_cursor(x, y);
    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write
    LCD_DataWrite(*(uint16_t *)pixel);  //write red data
}

static void _set_gpio_od(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* PG12 NE4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void _set_gpio_pp(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* PG12 NE4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void ra8875_lcd_get_pixel(char* pixel, int x, int y)
{
    _set_read_cursor(x, y);
    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    _set_gpio_od();

    *(rt_uint16_t*)pixel = LCD_DataRead(); /* dummy read */
    *(rt_uint16_t*)pixel = LCD_DataRead();

    _set_gpio_pp();
}

static void ra8875_lcd_draw_hline(const char* pixel, int x1, int x2, int y)
{
    {
        LCD_write_reg(0x40, 0x00);

        _set_write_cursor(x1,y);

        LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

        for(; x1<x2; x1++)
        {
            LCD_DataWrite(*(uint16_t *)pixel);  //write red data
        }
    }
}

static void ra8875_lcd_draw_vline(const char* pixel, int x, int y1, int y2)
{
    {
        LCD_write_reg(0x40, 0x00 | 1<<3);

        _set_write_cursor(x,y1);

        LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

        for(; y1<y2; y1++)
        {
            LCD_DataWrite(*(uint16_t *)pixel);  //write red data
        }
    }
}

static void ra8875_lcd_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
    rt_uint16_t *ptr;

    LCD_write_reg(0x40, 0x00);

    _set_write_cursor(x,y);

    ptr = (rt_uint16_t*)pixels;

    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    while(size--)
    {
        LCD_DataWrite(*ptr++);  //write red data
    }
}

static const struct rt_device_graphic_ops ra8875_ops =
{
    ra8875_lcd_set_pixel,
    ra8875_lcd_get_pixel,
    ra8875_lcd_draw_hline,
    ra8875_lcd_draw_vline,
    ra8875_lcd_blit_line
};

/*RA8875 reset Pin PC6*/
/*RA8875 INT PE5*/
static void _lcd_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH |
                           RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /*RA8875 NWAIT Pin PD6*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void LCD_Reset(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	uint32_t i;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOH, &GPIO_InitStructure);	 
	GPIOH->BSRRH = GPIO_Pin_2;	
	for(i = 0; i < 1000000; i++)
	GPIOH->BSRRL = GPIO_Pin_2;	
}

void LCD_DDRAMWrite(uint32_t count)
{

    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    while(count--)
    {
        LCD_DataWrite(50);  //write red data
    }
}
void LCD_Mode8080(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOI, &GPIO_InitStructure);
	GPIOI->BSRRH = GPIO_Pin_8;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);	
	GPIOE->BSRRH = GPIO_Pin_6;			 // 8080
}
void Active_Window(uint16_t XL,uint16_t XR ,uint16_t YT ,uint16_t YB)
{
	LCD_write_reg(0x30, XL);
	LCD_write_reg(0x31, XL >> 8);
	
	LCD_write_reg(0x34, XR);
	LCD_write_reg(0x35, XR >> 8);
	
	LCD_write_reg(0x32, YT);
	LCD_write_reg(0x33, YT >> 8);
	
	LCD_write_reg(0x36, YB);
	LCD_write_reg(0x37, YB >> 8);
}

void Display_ON(void)
{
	LCD_write_reg(0x01, 0x80);
	LCD_write_reg(0x8A, 1<<6);
}



int ra8875_init(void)
{
    _lcd_gpio_init();
		
    LCD_Reset();
    FSMC_Init();
		LCD_Mode8080();
    LCD_Init();
		Active_Window(0, 479, 0, 271);
   //LCD_write_reg(0x01, 0x80); /* Display on */
		LCD_DDRAMWrite(480*272);
		
		Display_ON();
		
    /*PWM set*/
    pwm_setting(60);

    /*set RA8875 GPOX pin to 1 - disp panel on*/
   // LCD_write_reg(0xC7, 0x01);
    /*set lift right*/
    //LCD_write_reg(0x20, 0x08);

    /* register lcd device */
    _lcd_device.type  = RT_Device_Class_Graphic;
    _lcd_device.init  = lcd_init;
    _lcd_device.open  = lcd_open;
    _lcd_device.close = lcd_close;
    _lcd_device.control = lcd_control;
    _lcd_device.read  = RT_NULL;
    _lcd_device.write = RT_NULL;

    _lcd_device.user_data = (void *)&ra8875_ops;

    /* register graphic device driver */
   rt_device_register(&_lcd_device, "lcd",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
	return 0;
}
INIT_BOARD_EXPORT(ra8875_init);
//FINISH_VAR_EXPORT(count,finish_type_int,count variable for finish);
