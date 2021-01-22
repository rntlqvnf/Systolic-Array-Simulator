#pragma once

#include <stdint.h>

class MAC
{
public:
	void tick();
	MAC& set_active_weight(int8_t);
	MAC& set_buffer_weight(int8_t);
	MAC& set_input_data(int8_t);
	MAC& set_input_sum(int32_t);
private:
	void mult_and_sum();

	int8_t input_data;
	int8_t active_weight;
	int8_t buffer_weight;
	int32_t input_sum;

	int8_t output_weight;
	int16_t output_mult;
	int32_t output_sum;
};

	