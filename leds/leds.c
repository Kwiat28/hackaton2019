#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define ON 1
#define OFF 0

#define GPIO_RED "/sys/class/gpio/gpio5/value"
#define GPIO_GREEN "/sys/class/gpio/gpio6/value"
#define GPIO_BLUE "/sys/class/gpio/gpio13/value"

int main(int argc, char **argv)
{
	uint8_t red=OFF;
	uint8_t green=OFF;
	uint8_t blue=OFF;

	for(int i=0; i<argc; i++)
	{
		if(!strcmp(argv[i], "red"))
			red=ON;
		if(!strcmp(argv[i], "green"))
			green=ON;
		if(!strcmp(argv[i], "blue"))
			blue=ON;
	}

	FILE *fp;

	if(fp=fopen(GPIO_RED, "w"))
	{
		fprintf(fp, "%d", red);
		fclose(fp);
	}
        if(fp=fopen(GPIO_GREEN, "w"))
	{
                fprintf(fp, "%d", green);
		fclose(fp);
	}
        if(fp=fopen(GPIO_BLUE, "w"))
	{
                fprintf(fp, "%d", blue);
		fclose(fp);
	}

	return 0;
}
