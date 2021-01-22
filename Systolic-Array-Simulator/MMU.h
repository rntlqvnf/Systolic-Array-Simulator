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
	const int32_t* get_output_sums();

	MMU& set_input_datas(int8_t[MAT_HEIGHT]);
	MMU& set_input_weights(int8_t[MAT_WIDTH]);
private:
	int8_t input_datas[MAT_HEIGHT];
	int8_t input_weights[MAT_WIDTH];
	int32_t output_sums[MAT_WIDTH];
	MAC mac_array[MAT_HEIGHT][MAT_WIDTH];
};