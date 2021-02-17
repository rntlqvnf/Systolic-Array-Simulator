#include "MMU.h"

void MMU::calculate()
{
	for (int i = 0; i < mmu_size; i++)
	{
		for (int j = 0; j < mmu_size; j++)
		{
			mac_array[i][j].calculate();
		}
	}

	for (int i = 0;i < mmu_size; i++)
	{
		last_row_sum[i] = mac_array[mmu_size - 1][i].sum_out;
;	}
}

void MMU::setup_array()
{
	program_input_switch();
	program_input_data();
	program_input_write_things();
	program_output_to_input();
}

void MMU::program_output_to_input()
{
	for (int i = 0; i < mmu_size; i++)
	{
		for (int j = 0;j < mmu_size; j++)
		{
			if (i != mmu_size - 1)
			{
				mac_array[i + 1][j].weight_in = mac_array[i][j].weight_out;
				mac_array[i + 1][j].weight_tag_in = mac_array[i][j].weight_tag_out;
				mac_array[i + 1][j].write_en_in = mac_array[i][j].write_en_out;
				mac_array[i + 1][j].sum_in = mac_array[i][j].sum_out;
			}

			if(j != mmu_size -1)
				mac_array[i][j + 1].data_in = mac_array[i][j].data_out;
		}
	}
}

void MMU::program_input_data()
{
	for (int i = 0; i < mmu_size; i++)
	{
		mac_array[i][0].data_in = ss->input_datas[i];
	}
}

void MMU::program_input_switch()
{
	for (int i = 0; i < mmu_size; i++)
	{
		for (int j = 0;j < mmu_size; j++)
		{
			mac_array[i][j].switch_in = ss->switch_weights[i];
		}
	}
}

void MMU::program_input_write_things()
{
	if (wf->write_en && !progressing)
	{
		wf->write_en = false;
		progressing = true;
		prog_step = 0;
	}
	
	for (int i = 0; i < mmu_size; i++)
	{
		mac_array[0][i].write_en_in = progressing;
		mac_array[0][i].weight_tag_in = prog_step;
		mac_array[0][i].weight_in = wf->input_weights[i];
	}

	if (progressing)
	{
		prog_step++;
		if (prog_step == mmu_size)
		{
			progressing = false;
			prog_step = 0;
		}
	}
}