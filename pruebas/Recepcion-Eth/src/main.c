#if defined(D_NEXYS_A7)
   #include "bsp_printf.h"
   #include "bsp_mem_map.h"
   #include "bsp_version.h"
#else
   PRE_COMPILED_MSG("no platform was defined")
#endif

#include "psp_api.h"

#define ETH_ACTIVACION_ENV  0x80001800
#define ETH_DATOS_ENV 0x80001804

#define ETH_DATOS_READ 0x80001808

#define READ_ETH(dir) (*(volatile unsigned *)dir)
#define WRITE_ETH(dir, value) { (*(volatile unsigned *)dir) = (value); }

int main ( void )
{
    int i, delay, datos=0;

    delay = 50000000;
    uartInit();
    while (1) { 
        datos = READ_ETH(ETH_DATOS_READ);
        printfNexys("%d", datos);

		for (i=0; i<delay; i++);
    }

    return(0);
}
