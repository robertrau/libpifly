#include <atomic>
#include <iostream>

#include <signal.h>

#include "adc/adc.h"

std::atomic<bool> interrupted;
void term_handle(int sig)
{
	printf("Signal received\n");
	interrupted.store(true);
}

int main(int argc, char** argv)
{
	signal(SIGINT, &term_handle);
	PiFly::ADC::Init();
	std::cout << "Hey there ;)" << std::endl;
	return 0;
}

