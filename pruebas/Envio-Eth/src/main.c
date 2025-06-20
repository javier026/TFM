#define ETH_ENV  0x80001800

#define READ_ETH(dir) (*(volatile unsigned *)dir)
#define WRITE_ETH(dir, value) { (*(volatile unsigned *)dir) = (value); }

int main ( void )
{
    int i, delay;

    delay = 50000000;

    while (1) { 
        WRITE_ETH(ETH_ENV, 0x543D3231);
		for (i=0; i<delay; i++);
    }

    return(0);
}
