#include <iostream>

#include "adc/adc.h"

int main(int argc, char** argv)
{
	PiFly::ADC::Init();
	std::cout << "Hey there ;)" << std::endl;
	return 0;
}

