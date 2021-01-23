
#include "MAC.h"

void MAC::tick()
{
	mult_and_sum();
	output_weight = active_weight;
	output_data = input_data;
}

void MAC::mult_and_sum()
{
	output_mult = (int16_t)active_weight * (int16_t)input_data;
	output_sum = input_sum + (int32_t)output_mult;
}