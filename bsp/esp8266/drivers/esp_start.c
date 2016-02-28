#include <rtthread.h>
#include <rthw.h>
#include <string.h>

#include "board.h"
#include "sdk_internal.h"

rt_uint32_t pending_maclayer_sv, pending_soft_sv;

uint8_t sdk_user_init_flag;

struct sdk_info_st sdk_info;

void IRAM SV_ISR(int irqno, void* param);

/*static void init_networking(uint8_t *phy_info, uint8_t *mac_addr) {*/
    /*if (sdk_register_chipv6_phy(phy_info)) {*/
        /*printf("FATAL: sdk_register_chipv6_phy failed");*/
        /*halt();*/
    /*}*/
    /*uart_set_baud(0, 74906);*/
    /*uart_set_baud(1, 74906);*/
    /*sdk_phy_disable_agc();*/
    /*sdk_ieee80211_phy_init(sdk_g_ic.s.phy_mode);*/
    /*sdk_lmacInit();*/
    /*sdk_wDev_Initialize();*/
    /*sdk_pp_attach();*/
    /*sdk_ieee80211_ifattach(&sdk_g_ic, mac_addr);*/
    /*_xt_isr_mask(1);*/
    /*DPORT.DPORT0 = SET_FIELD(DPORT.DPORT0, DPORT_DPORT0_FIELD0, 1);*/
    /*sdk_pm_attach();*/
    /*sdk_phy_enable_agc();*/
    /*sdk_cnx_attach(&sdk_g_ic);*/
    /*sdk_wDevEnableRx();*/
/*}*/

/*static void init_g_ic(void) {*/
    /*if (sdk_g_ic.s.wifi_mode == 0xff) {*/
        /*sdk_g_ic.s.wifi_mode = 2;*/
    /*}*/
    /*sdk_wifi_softap_set_default_ssid();*/
    /*if (sdk_g_ic.s._unknown30d < 1 || sdk_g_ic.s._unknown30d > 14) {*/
        /*sdk_g_ic.s._unknown30d = 1;*/
    /*}*/
    /*if (sdk_g_ic.s._unknown544 < 100 || sdk_g_ic.s._unknown544 > 60000) {*/
        /*sdk_g_ic.s._unknown544 = 100;*/
    /*}*/
    /*if (sdk_g_ic.s._unknown30e == 1 || sdk_g_ic.s._unknown30e > 4) {*/
        /*sdk_g_ic.s._unknown30e = 0;*/
    /*}*/
    /*bzero(sdk_g_ic.s._unknown2ac, sizeof(sdk_g_ic.s._unknown2ac));*/
    /*if (sdk_g_ic.s._unknown30f > 1) {*/
        /*sdk_g_ic.s._unknown30f = 0;*/
    /*}*/
    /*if (sdk_g_ic.s._unknown310 > 4) {*/
        /*sdk_g_ic.s._unknown310 = 4;*/
    /*}*/
    /*if (sdk_g_ic.s._unknown1e4._unknown1e4 == 0xffffffff) {*/
        /*bzero(&sdk_g_ic.s._unknown1e4, sizeof(sdk_g_ic.s._unknown1e4));*/
        /*bzero(&sdk_g_ic.s._unknown20f, sizeof(sdk_g_ic.s._unknown20f));*/
    /*}*/
    /*sdk_g_ic.s.wifi_led_enable = 0;*/
    /*if (sdk_g_ic.s._unknown281 > 1) {*/
        /*sdk_g_ic.s._unknown281 = 0;*/
    /*}*/
    /*if (sdk_g_ic.s.ap_number > 5) {*/
        /*sdk_g_ic.s.ap_number = 1;*/
    /*}*/
    /*if (sdk_g_ic.s.phy_mode < 1 || sdk_g_ic.s.phy_mode > 3) {*/
       /*sdk_g_ic.s.phy_mode = PHY_MODE_11N;*/
    /*}*/
/*}*/

/*void sdk_wdt_init(void) {*/
    /*WDT.CTRL &= ~WDT_CTRL_ENABLE;*/
    /*DPORT.INT_ENABLE |= DPORT_INT_ENABLE_WDT;*/
    /*WDT.REG1 = 0x0000000b;*/
    /*WDT.REG2 = 0x0000000c;*/
    /*WDT.CTRL |= WDT_CTRL_FLAG3 | WDT_CTRL_FLAG4 | WDT_CTRL_FLAG5;*/
    /*WDT.CTRL = SET_FIELD(WDT.CTRL, WDT_CTRL_FIELD0, 0);*/
    /*WDT.CTRL |= WDT_CTRL_ENABLE;*/
    /*sdk_pp_soft_wdt_init();*/
/*}*/


/*void sdk_user_init_task(void *params) {*/
    /*int phy_ver, pp_ver;*/

    /*sdk_ets_timer_init();*/
    /*printf("\nESP-Open-SDK ver: %s compiled @ %s %s\n", OS_VERSION_STR, __DATE__, __TIME__);*/
    /*phy_ver = RTCMEM_BACKUP[RTCMEM_BACKUP_PHY_VER] >> 16;*/
    /*printf("phy ver: %d, ", phy_ver);*/
    /*pp_ver = RTCMEM_SYSTEM[RTCMEM_SYSTEM_PP_VER];*/
    /*printf("pp ver: %d.%d\n\n", (pp_ver >> 8) & 0xff, pp_ver & 0xff);*/
    /*user_init();*/
    /*sdk_user_init_flag = 1;*/
    /*sdk_wifi_mode_set(sdk_g_ic.s.wifi_mode);*/
    /*if (sdk_g_ic.s.wifi_mode == 1) {*/
        /*sdk_wifi_station_start();*/
        /*netif_set_default(sdk_g_ic.v.station_netif_info->netif);*/
    /*}*/
    /*if (sdk_g_ic.s.wifi_mode == 2) {*/
        /*sdk_wifi_softap_start();*/
        /*netif_set_default(sdk_g_ic.v.softap_netif_info->netif);*/
    /*}*/
    /*if (sdk_g_ic.s.wifi_mode == 3) {*/
        /*sdk_wifi_station_start();*/
        /*sdk_wifi_softap_start();*/
        /*netif_set_default(sdk_g_ic.v.softap_netif_info->netif);*/
    /*}*/
    /*if (sdk_wifi_station_get_auto_connect()) {*/
        /*sdk_wifi_station_connect();*/
    /*}*/
    /*vTaskDelete(NULL);*/
/*}*/

/*void sdk_init()*/
/*{*/
    /*rt_hw_interrupt_install(INUM_SOFT, SV_ISR, */
        /*RT_NULL, RT_NULL);*/
    /*rt_hw_interrupt_umask(BIT(INUM_SOFT));   */

	/*uint8_t *buf;*/
    /*uint8_t *phy_info;*/

    /*sdk_system_rtc_mem_read(0, &sdk_rst_if, sizeof(sdk_rst_if));*/
    /*if (sdk_rst_if.version > 3) {*/
        /*// Bad version number. Probably garbage.*/
        /*bzero(&sdk_rst_if, sizeof(sdk_rst_if));*/
    /*}*/
    /*buf = malloc(sizeof(sdk_rst_if));*/
    /*bzero(buf, sizeof(sdk_rst_if));*/
    /*sdk_system_rtc_mem_write(0, buf, sizeof(sdk_rst_if));*/
    /*free(buf);*/
    /*sdk_sleep_reset_analog_rtcreg_8266();*/
    /*get_otp_mac_address(sdk_info.sta_mac_addr);*/
    /*sdk_wifi_softap_cacl_mac(sdk_info.softap_mac_addr, sdk_info.sta_mac_addr);*/
    /*sdk_info._unknown0 = 0x0104a8c0;*/
    /*sdk_info._unknown1 = 0x00ffffff;*/
    /*sdk_info._unknown2 = 0x0104a8c0;*/
    /*init_g_ic();*/
    /*phy_info = malloc(PHY_INFO_SIZE);*/
    /*sdk_spi_flash_read(sdk_flashchip.chip_size - sdk_flashchip.sector_size * 4, (uint32_t *)phy_info, PHY_INFO_SIZE);*/

    /*// Disable default buffering on stdout*/
    /*setbuf(stdout, NULL);*/
    /*// Wait for UARTs to finish sending anything in their queues.*/
    /*uart_flush_txfifo(0);*/
    /*uart_flush_txfifo(1);*/

    /*if (phy_info[0] != 5) {*/
        /*// Bad version byte.  Discard what we read and use default values*/
        /*// instead.*/
        /*memcpy(phy_info, default_phy_info, PHY_INFO_SIZE);*/
    /*}*/
    /*init_networking(phy_info, sdk_info.sta_mac_addr);*/
    /*free(phy_info);*/
    /*tcpip_init(NULL, NULL);*/
    /*sdk_wdt_init();*/
    /*xTaskCreate(sdk_user_init_task, (signed char *)"uiT", 1024, 0, 14, &sdk_xUserTaskHandle);*/
    /*vTaskStartScheduler();*/

/*}*/

void PendSV(enum SVC_ReqType req)
{
    rt_enter_critical();

    if(req == SVC_Software)
    {
        pending_soft_sv = 1;
    }
    else if(req == SVC_MACLayer)
        pending_maclayer_sv= 1;

    xthal_set_intset(BIT(INUM_SOFT));
    rt_exit_critical();    
}

void IRAM SV_ISR(int irqno, void* param)
{
    if(pending_maclayer_sv)
    {
        pending_maclayer_sv = 0;
    }
    if( pending_soft_sv)
    {
        pending_soft_sv = 0;
    }
}
