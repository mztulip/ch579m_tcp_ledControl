#include "CH57x_common.h"
#include <stdio.h>
#include <string.h>
#include "eth_mac.h"
#include "ethernetif.h"
#include "timer0.h"
#include "lwipcomm.h"
#include "lwip/timeouts.h"
#include "tcp.h"

uint32_t arg = 0;

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

// static void udp_received(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
// {
//     LWIP_UNUSED_ARG(arg);
//     if(p == NULL)
//         return;
//     printf("\n\rgot udp");
//     if (p->len == 1)
//     {
//         uint8_t *data = (uint8_t*)p->payload;
//         char c = data[0];
//         if(c == '1')
//         {
//             led_on();
//             printf("\n\rLed enabled.");
//         }
//         else if(c == '0')
//         {
//             led_off();
//             printf("\n\rLed disabled.");
//         }
//         else
//         {
//             printf("\n\rIncorrect state: %c", c);
//         }
//     }
//     else
//     {
//         printf("\n\rIncorrect packet. Should contain char 0 or 1 with length=1 byte");
//     }
    
//     pbuf_free(p);
// }

static err_t tcp_data_received(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    printf("\033[32m Data received.\n\r\033[0m");

    if (p == NULL)
    {
        printf("p==NULL,Closing connection\n\r");
        tcp_close(tpcb);
}
        return ERR_OK;
    }
    else if(err != ERR_OK)
    {
        printf("Received error.\n\r");
        return err;
    }
    printf("Received Data len: %d\n\r", p->tot_len);


    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

static void tcp_connection_error(void *arg, err_t err)
{
  printf("\033[91mtcp connection fatal Error. Maybe memory shortage.\033[0m\n\r");
}

static err_t tcp_connection_poll(void *arg, struct tcp_pcb *tpcb)
{
    printf("\033[33mTCP poll\n\r\033[0m]");
    return ERR_OK;
}

static err_t tcp_connection_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    printf("\033[32m Connection accepted.\n\r\033[0m");
    tcp_arg(newpcb, &arg);
    tcp_recv(newpcb, tcp_data_received);
    tcp_err(newpcb, tcp_connection_error);
    tcp_poll(newpcb, tcp_connection_poll, 10);
    return ERR_OK;
}

// Very helpful link https://lwip.fandom.com/wiki/Raw/TCP
int main()
{ 
	SystemInit();

    PWR_UnitModCfg(ENABLE, (UNIT_SYS_PLL|UNIT_ETH_PHY)); 
    DelayMs(3); 
    SetSysClock(CLK_SOURCE_HSE_32MHz); 

    led_init();
    InitTimer0();
    uart_init();
    printf("\n\rTCP server example with led control.\n\r");
    lwip_comm_init(); 

    u16_t   port = 8001;
    printf("Listening port: %d\nr", port);
    static struct tcp_pcb *tcp_pcb_handle;
    err_t result;
    tcp_pcb_handle = tcp_new();
    if(tcp_pcb_handle == NULL){printf("tcp_new failed\n\r");goto exit;}

    result = tcp_bind(tcp_pcb_handle, IP_ADDR_ANY, port);
    if(result != ERR_OK) 
    {printf("tcp_bind failed\n\r");memp_free(MEMP_TCP_PCB, tcp_pcb_handle);goto exit;}

    tcp_pcb_handle = tcp_listen(tcp_pcb_handle);
    if(tcp_pcb_handle == NULL){printf("tcp_listen failed\n\r");memp_free(MEMP_TCP_PCB, tcp_pcb_handle);goto exit;}
      
    tcp_accept(tcp_pcb_handle, tcp_connection_accept);

    exit:
    while(1)
    {
        lwip_pkt_handle();
        lwip_periodic_handle();
        sys_check_timeouts();	
    }
}

