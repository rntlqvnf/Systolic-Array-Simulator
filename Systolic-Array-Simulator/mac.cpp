
#include "MAC.h"

void MAC::tick()
{
	mult_and_sum();
	output_weight = active_weight;
}

void MAC::mult_and_sum()
{
	output_mult = (int16_t)active_weight * (int16_t)input_data;
	output_sum = input_sum + (int32_t)output_mult;
}

MAC& MAC::set_active_weight(int8_t weight)
{
	active_weight = weight;
	return *this;
}

MAC& MAC::set_buffer_weight(int8_t weight)
{
	buffer_weight = weight;
	return *this;
}

MAC& MAC::set_input_data(int8_t data)
{
	input_data = data;
	return *this;
}

MAC& MAC::set_input_sum(int32_t sum)
{
	input_sum = sum;
	return *this;
}