#pragma once

#include <stdint.h>
#include "Config.h"

class MAC
{
private:
	int current_weight_index;

	//in
	int8_t input_data;
	int8_t input_weight;
	int8_t weight_buffer[2];
	int input_weight_taginput_datas
	int32_t input_sum;

	//out
	int8_t output_data;
	int8_t output_weight;
	int output_weight_tag;
	int16_t output_mult;
	int32_t output_sum;
	bool output_write_flag;

	//control
	bool switch_flag;
	bool write_flag;

	void switch_weight()
	{
		current_weight_index = 1 - current_weight_index;
	}
	void write_weight()
	{
		weight_buffer[1 - current_weight_index] = input_weight;
	}
public:
	MAC() :
		current_weight_index(0),
		input_data(0),
		input_weight(0),
		input_weight_tag(0),
		weight_buffer{},
		input_sum(0),
		output_data(0),
		output_weight(0),
		output_mult(0),
		output_sum(0),
		output_weight_tag(0),
		switch_flag(false),
		write_flag(false),
		output_write_flag(false)
	{}

	void tick();

	MAC& set_input_data(int8_t data)
	{
		input_data = data;
		return *this;
	}
	MAC& set_input_weight(int8_t weight)
	{
		input_weight = weight;
		return *this;
	}
	MAC& set_input_sum(int32_t sum)
	{
		input_sum = sum;
		return *this;
	}
	MAC& set_switch_flag(bool flag)
	{
		switch_flag = flag;
		return *this;
	}
	MAC& set_input_weight_tag(int tag)
	{
		input_weight_tag = tag;
		return *this;
	}
	MAC& set_write_flag(bool flag)
	{
		write_flag = flag;
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
	int8_t get_buffer_weight()
	{
		return weight_buffer[1 - current_weight_index];
	}
	int8_t get_current_weight()
	{
		return weight_buffer[current_weight_index];
	}
	int get_output_weight_tag()
	{
		return output_weight_tag;
	}
	bool get_output_write_flag()
	{
		return output_write_flag;
	}
	int16_t get_output_mult()
	{
		return output_mult;
	}
	int32_t get_output_sum()
	{
		return output_sum;
	}
};
