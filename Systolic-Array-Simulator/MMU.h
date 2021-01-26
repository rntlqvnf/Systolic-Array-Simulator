#pragma once

#include <stdint.h>
#include <algorithm>
#include <iostream>
#include "config.h"
#include "mac.h"

class MMU
{
private:
	void calculate_row_wise();
	void assign_output_to_input();
	void assign_inputs();

	int progstep;
	bool switch_flags[MAT_HEIGHT];
	bool write_flag;
	int8_t input_datas[MAT_HEIGHT];
	int8_t input_weights[MAT_WIDTH];
	int32_t output_sums[MAT_WIDTH];
	MAC mac_array[MAT_HEIGHT][MAT_WIDTH];

public:
	MMU();

	void tick();

	const int32_t* get_output_sums()
	{
		for (int i = 0; i < MAT_WIDTH; i++)
		{
			output_sums[i] = mac_array[MAT_HEIGHT - 1][i].get_output_sum();
		}
		return output_sums;
	}
	MMU& set_switch_flags(bool flags[MAT_HEIGHT])
	{
		std::copy(flags, flags + MAT_HEIGHT, switch_flags);
		return *this;
	}
	MMU& set_write_flag(bool flag)
	{
		write_flag = flag;
		return *this;
	}
	MMU& set_input_datas(int8_t datas[MAT_HEIGHT])
	{
		std::copy(datas, datas + MAT_HEIGHT, input_datas);
		return *this;
	}
	MMU& set_input_weights(int8_t weights[MAT_HEIGHT])
	{
		std::copy(weights, weights + MAT_WIDTH, input_weights);
		return *this;
	}
};