#pragma once

#include <stdint.h>
#include <algorithm>
#include "config.h"
#include "mac.h"

class MMU
{
public:
	MMU();
	void tick();
	const int32_t* get_output_sums()
	{
		return output_sums;
	}

	MMU& set_input_datas(int8_t datas[MAT_HEIGHT])
	{
		std::copy(datas, datas + MAT_HEIGHT, input_datas);
		return *this;
	}
	MMU& set_input_weights(int8_t weights[MAT_WIDTH])
	{
		std::copy(weights, weights + MAT_WIDTH, input_weights);
		return *this;
	}
private:
	void assign_data_weight();
	void run_row_wise_calculate();
	int8_t input_datas[MAT_HEIGHT];
	int8_t input_weights[MAT_WIDTH];
	int32_t output_sums[MAT_WIDTH];
	MAC mac_array[MAT_HEIGHT][MAT_WIDTH];
};