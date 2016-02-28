#include <rtthread.h>
#include <rthw.h>

#include "components.h"

#include "board.h"
#include "esp/rom.h"
#include "sdk_internal.h"
#include <xtensa_ops.h>

#define BOOT_INFO_SIZE 28
#define PHY_INFO_SIZE 128

extern uint32_t _bss_start;
extern uint32_t _bss_end;

static inline IRAM void zero_bss(void) {
    uint32_t *addr;

    for (addr = &_bss_start; addr < &_bss_end; addr++) {
        *addr = 0;
    }
}

static void IRAM set_spi0_divisor(uint32_t divisor) {
    int cycle_len, half_cycle_len, clkdiv;

    if (divisor < 2) {
        clkdiv = 0;
        SPI(0).CTRL0 |= SPI_CTRL0_CLOCK_EQU_SYS_CLOCK;
        IOMUX.CONF |= IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK;
    } else {
        cycle_len = divisor - 1;
        half_cycle_len = (divisor / 2) - 1;
        clkdiv = VAL2FIELD(SPI_CTRL0_CLOCK_NUM, cycle_len)
               | VAL2FIELD(SPI_CTRL0_CLOCK_HIGH, half_cycle_len)
               | VAL2FIELD(SPI_CTRL0_CLOCK_LOW, cycle_len);
        SPI(0).CTRL0 &= ~SPI_CTRL0_CLOCK_EQU_SYS_CLOCK;
        IOMUX.CONF &= ~IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK;
    }
    SPI(0).CTRL0 = SET_FIELD(SPI(0).CTRL0, SPI_CTRL0_CLOCK, clkdiv);
}

static void dump_flash_sector(uint32_t start_sector, uint32_t length) {
    uint8_t *buf;
    int bufsize, i;

    bufsize = (length + 3) & 0xfffc;
    buf = rt_malloc(bufsize);
    sdk_spi_flash_read(start_sector * sdk_flashchip.sector_size, (uint32_t *)buf
, bufsize);
    for (i = 0; i < length; i++) {
        if ((i & 0xf) == 0) {
            if (i) {
                rt_kprintf("\n");
            }
            rt_kprintf("%04x:", i);
        }
        rt_kprintf(" %02x", buf[i]);
    }
    rt_kprintf("\n");
    rt_free(buf);
}

static void dump_flash_config_sectors(uint32_t start_sector) {

	rt_components_board_init();
	rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);

    rt_kprintf("system param error\n");
    // Note: original SDK code didn't dump PHY info
    rt_kprintf("phy_info:\n");
    dump_flash_sector(start_sector, PHY_INFO_SIZE);
    rt_kprintf("\ng_ic saved 0:\n");
    dump_flash_sector(start_sector + 1, sizeof(struct sdk_g_ic_saved_st));
    rt_kprintf("\ng_ic saved 1:\n");
    dump_flash_sector(start_sector + 2, sizeof(struct sdk_g_ic_saved_st));
    rt_kprintf("\nboot info:\n");
    dump_flash_sector(start_sector + 3, BOOT_INFO_SIZE);
}

static void IRAM rt_cpu_init()
{
    uint32_t buf32[sizeof(struct sdk_g_ic_saved_st) / 4];
    uint8_t *buf8 = (uint8_t *)buf32;
    uint32_t flash_speed_divisor;
    uint32_t flash_sectors;
    uint32_t flash_size;
    int boot_slot;
    uint32_t cksum_magic;
    uint32_t cksum_len;
    uint32_t cksum_value;
    uint32_t ic_flash_addr;

    SPI(0).USER0 |= SPI_USER0_CS_SETUP;
    sdk_SPIRead(0, buf32, 4);

    switch (buf8[3] & 0x0f) {
        case 0xf:  // 80 MHz
            flash_speed_divisor = 1;
            break;
        case 0x0:  // 40 MHz
            flash_speed_divisor = 2;
            break;
        case 0x1:  // 26 MHz
            flash_speed_divisor = 3;
            break;
        case 0x2:  // 20 MHz
            flash_speed_divisor = 4;
            break;
        default:  // Invalid -- Assume 40 MHz
            flash_speed_divisor = 2;
    }
    switch (buf8[3] >> 4) {
        case 0x0:   // 4 Mbit (512 KByte)
            flash_sectors = 128;
            break;
        case 0x1:  // 2 Mbit (256 Kbyte)
            flash_sectors = 64;
            break;
        case 0x2:  // 8 Mbit (1 Mbyte)
            flash_sectors = 256;
            break;
        case 0x3:  // 16 Mbit (2 Mbyte)
            flash_sectors = 512;
            break;
        case 0x4:  // 32 Mbit (4 Mbyte)
            flash_sectors = 1024;
            break;
        default:   // Invalid -- Assume 4 Mbit (512 KByte)
            flash_sectors = 128;
    }

    flash_size = flash_sectors * 4096;
    sdk_flashchip.chip_size = flash_size;
    set_spi0_divisor(flash_speed_divisor);
    sdk_SPIRead(flash_size - 4096, buf32, BOOT_INFO_SIZE);
    boot_slot = buf8[0] ? 1 : 0;
    cksum_magic = buf32[1];
    cksum_len = buf32[3 + boot_slot];
    cksum_value = buf32[5 + boot_slot];
    ic_flash_addr = (flash_sectors - 3 + boot_slot) * sdk_flashchip.sector_size;
    sdk_SPIRead(ic_flash_addr, buf32, sizeof(struct sdk_g_ic_saved_st));
    Cache_Read_Enable(0, 0, 1);

	/* can load irom now*/
    zero_bss();

    if (cksum_magic == 0xffffffff) {
        // No checksum required
	} else if ((cksum_magic == 0x55aa55aa) &&
            (sdk_system_get_checksum(buf8, cksum_len) == cksum_value)) {
        // Checksum OK
    } else {
        // Bad checksum or bad cksum_magic value
        dump_flash_config_sectors(flash_sectors - 4);
    }

    rt_memcpy(&sdk_g_ic.s, buf32, sizeof(struct sdk_g_ic_saved_st));

    // Wait for UARTs to finish sending anything in their queues.
    uart_flush_txfifo(0);
    uart_clear_txfifo(0);

    /* initialzie hardware interrupt */
    rt_hw_interrupt_init();
}


static rt_uint32_t tick_divisor = (CPU_CLK_FREQ / RT_TICK_PER_SECOND);

static void IRAM tick_handler(int irqno, void* param)
{
    rt_uint32_t tick_divisor = (CPU_CLK_FREQ / RT_TICK_PER_SECOND);
    rt_uint32_t ccompare0;

    rt_tick_increase();

    asm volatile ("rsr %0, ccompare0" : "=a" (ccompare0));

    /* count ccompare0 value*/
    ccompare0 = tick_divisor + ccompare0;

    asm volatile ("wsr %0, ccompare0; esync" :: "a" (ccompare0));
}

void rt_tick_init()
{
    rt_uint32_t ccount, ccompare0;
    asm volatile ("rsr %0, ccount" : "=a" (ccount));
    ccompare0 = tick_divisor + ccount;
    asm volatile ("wsr %0, ccompare0; " :: "a" (ccompare0));


    rt_hw_interrupt_install(INUM_TICK, tick_handler, 
        NULL, NULL);

    rt_hw_interrupt_umask(BIT(INUM_TICK));
}


/**
 * This function will initialize esp8266 board
 */
void IRAM rt_hw_board_init(void)
{
    rt_cpu_init();

    rt_tick_init();

    rt_components_board_init();

    rt_console_set_device("uart0");

}


