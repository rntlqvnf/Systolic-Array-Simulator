#pragma once

#include <stdint.h>

class MAC
{
private:
	int current_weight_index;

	int8_t input_data;
	int8_t weight_buffer[2];
	int32_t input_sum;

	int8_t output_data;
	int8_t output_weight;
	int16_t output_mult;
	int32_t output_sum;

	bool switch_flag;

public:
	MAC() :
		current_weight_index(0),
		input_data(0),
		weight_buffer{},
		input_sum(0),
		output_data(0),
		output_weight(0),
		output_mult(0),
		output_sum(0),
		switch_flag(false)
	{}

	void tick();

	MAC& set_weight(int8_t weight)
	{
		weight_buffer[1 - current_weight_index] = weight;
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
	MAC& set_swtich_flag(bool flag)
	{
		switch_flag = flag;
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
	void switch_weight()
	{
		current_weight_index = 1 - current_weight_index;
	}
};
