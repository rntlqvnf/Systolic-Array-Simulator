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
	int32_t output_sums[MAT_WIDTH];
	MAC mac_array[MAT_HEIGHT][MAT_WIDTH];

public:
	MMU();
	void tick();
	void switch_weights();

	const int32_t* get_output_sums()
	{
		return output_sums;
	}
	MMU& set_datas(int8_t datas[MAT_HEIGHT])
	{
		for (int i = 0; i < MAT_HEIGHT; i++)
		{
			mac_array[i][0].set_input_data(datas[i]);
		}
		return *this;
	}
	MMU& set_weights(int8_t weights[MAT_WIDTH])
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			mac_array[0][j].set_weight(weights[j]);
		}
		return *this;
	}
};