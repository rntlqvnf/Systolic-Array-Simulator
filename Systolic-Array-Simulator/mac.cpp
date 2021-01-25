
#include "mac.h"

void MAC::tick()
{


	if (switch_flag)
		switch_weight();
	output_mult = (int16_t)weight_buffer[current_weight_index] * (int16_t)input_data;
	output_sum = input_sum + (int32_t)output_mult;
	output_weight = weight_buffer[current_weight_index];
	output_data = input_data;
}
