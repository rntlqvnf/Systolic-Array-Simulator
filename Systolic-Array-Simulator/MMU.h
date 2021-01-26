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
	void assign_output_sums();
	void assign_inputs();

	bool switch_flags[MAT_HEIGHT];
	bool write_flag;
	int progstep;
	int8_t input_datas[MAT_HEIGHT];
	int8_t input_weights[MAT_WIDTH];
	int32_t output_sums[MAT_WIDTH];
	MAC mac_array[MAT_HEIGHT][MAT_WIDTH];

public:
	MMU();

	void tick();

	const int32_t* get_output_sums()
	{
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
	MMU& set_input_datas(bool datas[MAT_HEIGHT])
	{
		std::copy(datas, datas + MAT_HEIGHT, input_datas);
		return *this;
	}
	MMU& set_input_weights(bool weights[MAT_HEIGHT])
	{
		std::copy(weights, weights + MAT_WIDTH, input_weights);
		return *this;
	}
};