#include <unistd.h>

int main(void)
{
	if(fork()==0)
	{
		execl("./leds", "red");
		return 1;
	}
	return 0;
}

