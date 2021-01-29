#pragma once

#include <stdint.h>

class MAC
{
public:
	//input
	int8_t data_in;
	int8_t weight_in;
	int8_t weight_tag_in;
	int32_t sum_in;
	bool write_en_in;

	//output
	int8_t data_out;
	int8_t weight_out;
	int8_t weight_tag_out;
	int32_t sum_out;
	bool write_en_out;

	MAC()
	{

	}
};

