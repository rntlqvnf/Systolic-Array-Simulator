#pragma once

#include <stdint.h>

class MAC
{
public:
	void tick();
	MAC& set_active_weight(int8_t weight)
	{
		active_weight = weight;
		return *this;
	}
	MAC& set_buffer_weight(int8_t weight)
	{
		buffer_weight = weight;
		return *this;
	}
	MAC& set_input_data(int8_t data)
	{
		input_data = data;
		return *this;
	}
	MAC& set_input_sum(int32_t sum)
	{
		input_sum = sum;
		return *this;
	}
	int8_t get_output_data()
	{
		return output_data;
	}
	int8_t get_output_weight()
	{
		return output_weight;
	}
	int16_t get_output_mult()
	{
		return output_mult;
	}
	int32_t get_output_sum()
	{
		return output_sum;
	}
private:
	void mult_and_sum();

	int8_t input_data;
	int8_t active_weight;
	int8_t buffer_weight;
	int32_t input_sum;

	int8_t output_data;
	int8_t output_weight;
	int16_t output_mult;
	int32_t output_sum;
};

	