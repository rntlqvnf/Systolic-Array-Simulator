
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

int8_t MAC::get_output_data()
{
	return output_data;
}

int8_t MAC::get_output_weight()
{
	return output_sum;
}

int16_t MAC::get_output_mult()
{
	return output_mult;
}

int32_t MAC::get_output_sum()
{
	return output_sum;
}