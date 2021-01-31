#pragma once

#include <stdint.h>

class MAC
{
public:
	//setting
	int matrix_size;

	//input
	int8_t data_in;
	int8_t weight_in;
	int8_t weight_tag_in;
	int32_t sum_in;
	bool write_en_in;
	bool switch_in;

	//output
	int8_t data_out;
	int8_t weight_out;
	int8_t weight_tag_out;
	int32_t sum_out;
	bool write_en_out;

	//buffer
	int8_t weight_buf[2];
	int current_weight;

	MAC() :
		weight_buf{}
	{
		matrix_size = 0;

		data_in = 0;
		weight_in = 0;
		weight_tag_in = 0;
		sum_in = 0;
		write_en_in = false;
		switch_in = false;

		data_out = 0;
		weight_out = 0;
		weight_tag_out = 0;
		sum_out = 0;
		write_en_out = 0;

		current_weight = 0;
	}

	void calculate();
};

