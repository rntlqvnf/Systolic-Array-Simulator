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
	assign_inputs();
	assign_output_to_input();
	assign_output_sums();
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
	}
	for (int j = 0; j < MAT_WIDTH; j++)
	{
		mac_array[0][j].set_input_weight(input_weights[j]);
		mac_array[0][j].set_swtich_flag(switch_flags[j]);
		if (progstep < MAT_HEIGHT)
		{
			mac_array[0][j].set_write_flag(write_flag);
			mac_array[0][j].set_input_weight_tag(progstep);
		}
		else
		{
			progstep = 0;
			mac_array[0][j].set_write_flag(false);
		}
	}
	progstep++;
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
			}
			if (j != MAT_WIDTH - 1)
				mac_array[i][j + 1].set_input_data(mac_array[i][j].get_output_data());

			mac_array[i][j + 1].set_input_weight(mac_array[i][j].get_output_weight());
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