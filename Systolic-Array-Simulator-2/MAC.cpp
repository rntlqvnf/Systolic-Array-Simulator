#include "MAC.h"

void MAC::calculate()
{
	if (switch_in)
		current_weight = 1 - current_weight;

	if (write_en_in && weight_tag_in == (int8_t)mmu_size - 1)
		weight_buf[1 - current_weight] = weight_in;

	int16_t mult = (int16_t)data_in * (int16_t)weight_buf[current_weight];
	int32_t sum = (int32_t)mult + sum_in;

	data_out = data_in;
	weight_out = weight_in;
	weight_tag_out = weight_tag_in + 1;
	sum_out = sum;
	write_en_out = write_en_in;
}