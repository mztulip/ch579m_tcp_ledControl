#include "CH57x_common.h"
#include <stdio.h>
#include <string.h>
#include "eth_mac.h"
#include "ethernetif.h"
#include "timer0.h"
#include "lwipcomm.h"
#include "lwip/timeouts.h"
#include "udp.h"

void uart_init(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

void led_init(void)
{
    GPIOB_ModeCfg( GPIO_Pin_0, GPIO_ModeOut_PP_20mA );
}

void led_on(void)
{
    GPIOB_SetBits( GPIO_Pin_0 ); 
}

void led_off(void)
{
    GPIOB_ResetBits( GPIO_Pin_0 );
}

static void udp_received(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    LWIP_UNUSED_ARG(arg);
    if(p == NULL)
        return;
    printf("\n\rgot udp");
    if (p->len == 1)
    {
        uint8_t *data = (uint8_t*)p->payload;
        char c = data[0];
        if(c == '1')
        {
            led_on();
            printf("\n\rLed enabled.");
        }
        else if(c == '0')
        {
            led_off();
            printf("\n\rLed disabled.");
        }
        else
        {
            printf("\n\rIncorrect state: %c", c);
        }
    }
    else
    {
        printf("\n\rIncorrect packet. Should contain char 0 or 1 with length=1 byte");
    }
    
    pbuf_free(p);
}

int main()
{ 
	SystemInit();

    PWR_UnitModCfg(ENABLE, (UNIT_SYS_PLL|UNIT_ETH_PHY)); 
    DelayMs(3); 
    SetSysClock(CLK_SOURCE_HSE_32MHz); 

    led_init();
    InitTimer0();
    uart_init();
    printf("\n\rUDP server example with led control.\n");
    lwip_comm_init(); 

    u16_t   port = 8001;
    printf("\n\rListening port: %d\n", port);
    struct udp_pcb *pcb;
    err_t result;
    pcb = udp_new();
    result = udp_bind(pcb, IP_ADDR_ANY, port);
    if(result != ERR_OK) {printf("\n\rudp_bind failed");goto exit;}

	udp_recv(pcb , udp_received, NULL);

    exit:
    while(1)
    {
        lwip_pkt_handle();
        lwip_periodic_handle();
        sys_check_timeouts();	
    }
}

