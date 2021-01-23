#include "MMU.h"

MMU::MMU()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			mac_array[i][j] = MAC();
		}
	}
}

void MMU::tick()
{
	assign_data_weight();
	run_row_wise_calculate();
}

void MMU::run_row_wise_calculate()
{
	for (int i = 0; i < MAT_HEIGHT - 1; i++)
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			mac_array[i][j].tick();
			mac_array[i + 1][j].set_input_sum(mac_array[i][j].get_output_sum());
			if (j != MAT_WIDTH - 1)
				mac_array[i][j + 1].set_input_data(mac_array[i][j].get_output_data());
		}
	}
}

void MMU::assign_data_weight()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		mac_array[i][0].set_input_data(input_datas[i]);
	}
	for (int j = 0; j < MAT_WIDTH; j++)
	{
		mac_array[0][j].set_active_weight(input_weights[j]);
	}
}

const int32_t* MMU::get_output_sums()
{
	return output_sums;
}


MMU& MMU::set_input_datas(int8_t datas[MAT_HEIGHT])
{
	std::copy(datas, datas + MAT_HEIGHT, input_datas);
	return *this;
}
MMU& MMU::set_input_weights(int8_t weights[MAT_WIDTH])
{
	std::copy(weights, weights + MAT_WIDTH, input_weights);
	return *this;
}