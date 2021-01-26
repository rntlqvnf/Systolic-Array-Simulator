#include "mmu.h"

MMU::MMU() : 
	switch_flags{},
	write_flag(false), 
	progstep(0), 
	input_datas{}, 
	input_weights{},
	output_sums{}
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
	assign_inputs();
	assign_output_to_input();
	calculate_row_wise();
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

void MMU::assign_inputs()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		mac_array[i][0].set_input_data(input_datas[i]);

		for(int j = 0; j < MAT_WIDTH; j++)
			mac_array[i][j].set_switch_flag(switch_flags[i]);
	}
	for (int j = 0; j < MAT_WIDTH; j++)
	{
		mac_array[0][j].set_input_weight(input_weights[j]);
		mac_array[0][j].set_write_flag(write_flag);
		mac_array[0][j].set_input_weight_tag(progstep);
	}
	if(write_flag)
		progstep++;

	if (progstep >= MAT_HEIGHT)
		progstep = 0;
}

void MMU::assign_output_to_input()
{
	for (int i = 0; i < MAT_HEIGHT; i++)
	{
		for (int j = 0; j < MAT_WIDTH; j++)
		{
			if (i != MAT_HEIGHT - 1)
			{
				mac_array[i + 1][j].set_input_sum(mac_array[i][j].get_output_sum());
				mac_array[i + 1][j].set_input_weight_tag(mac_array[i][j].get_output_weight_tag());
				mac_array[i + 1][j].set_write_flag(mac_array[i][j].get_output_write_flag());
				mac_array[i + 1][j].set_input_weight(mac_array[i][j].get_output_weight());
			}
			if (j != MAT_WIDTH - 1)
				mac_array[i][j + 1].set_input_data(mac_array[i][j].get_output_data());
		}
	}
}