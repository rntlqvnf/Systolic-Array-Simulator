#include "mmu.h"

MMU::MMU() : output_sums{}
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
	assign_output_to_input();
	calculate_row_wise();
	assign_output_sums();
	//TODO: weight propagate
}

void MMU::switch_weights()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			mac_array[i][j].switch_weight();
		}
	}
}

void MMU::calculate_row_wise()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			mac_array[i][j].tick();
		}
	}
}

void MMU::assign_output_to_input()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			if (i != MAT_HEIGHT - 1)
				mac_array[i + 1][j].set_input_sum(mac_array[i][j].get_output_sum());
			if (j != MAT_WIDTH - 1)
				mac_array[i][j + 1].set_input_data(mac_array[i][j].get_output_data());
		}
	}
}

void MMU::assign_output_sums()
{
	for (int i = 0; i < MAT_WIDTH; i++)
	{
		output_sums[i] = mac_array[MAT_HEIGHT - 1][i].get_output_sum();
	}
}