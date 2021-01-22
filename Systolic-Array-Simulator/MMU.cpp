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