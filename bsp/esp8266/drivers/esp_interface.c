#include <rtthread.h>
#include <rthw.h>
#include <string.h>

#include "board.h"

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"

/* declared in libnet80211.a */
int8_t sdk_ieee80211_output_pbuf(struct netif *ifp, struct pbuf* pb);

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;

  for(q = p; q != NULL; q = q->next) {
      sdk_ieee80211_output_pbuf(netif, q);
  }

  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}


err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  // don't touch netif->state here, the field is used internally in the ESP SDK layers
  netif->name[0] = 'e';
  netif->name[1] = 'n';
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* low_level_init components */
  netif->hwaddr_len = 6;
  /* hwaddr seems to be set elsewhere, or (more likely) is set on tx by MAC layer */
  netif->mtu = 1500;
  netif->flags = NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

  return ERR_OK;
}

/* called from ieee80211_deliver_data with new IP frames */
void ethernetif_input(struct netif *netif, struct pbuf *p)
{
    struct eth_hdr *ethhdr = p->payload;
  /* examine packet payloads ethernet header */


    switch(htons(ethhdr->type)) {
	/* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
//  case ETHTYPE_IPV6:
	/* full packet send to tcpip_thread to process */
	if (netif->input(p, netif)!=ERR_OK)
	{
	    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
	    pbuf_free(p);
	    p = NULL;
	}
	break;

    default:
	pbuf_free(p);
	p = NULL;
	break;
    }
}
