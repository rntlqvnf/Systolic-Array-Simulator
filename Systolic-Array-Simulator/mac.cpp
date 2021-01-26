
#include "mac.h"

void MAC::tick()
{
	if (switch_flag)
		switch_weight();

	if (write_flag && input_weight_tag == MAT_HEIGHT - 1)
		write_weight();

	output_mult = (int16_t)weight_buffer[current_weight_index] * (int16_t)input_data;
	output_sum = input_sum + (int32_t)output_mult;
	output_weight = input_weight;
	output_weight_tag = input_weight_tag + 1;
	output_data = input_data;
	output_write_flag = write_flag;
}
