#include "MMU.h"

void MMU::calculate()
{
	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0; j < matrix_size; j++)
		{
			mac_array[i][j].calculate();
		}
	}

	for (int i = 0;i < matrix_size; i++)
	{
		last_row_sum[i] = mac_array[matrix_size - 1][i].sum_out;
;	}
	std::cout << "Calculate " << std::endl;
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
	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0;j < matrix_size; j++)
		{
			if (i != matrix_size - 1)
			{
				mac_array[i + 1][j].weight_in = mac_array[i][j].weight_out;
				mac_array[i + 1][j].weight_tag_in = mac_array[i][j].weight_tag_out;
				mac_array[i + 1][j].write_en_in = mac_array[i][j].write_en_out;
				mac_array[i + 1][j].sum_in = mac_array[i][j].sum_out;
			}

			if(j != matrix_size -1)
				mac_array[i][j + 1].data_in = mac_array[i][j].data_out;
		}
	}
}

void MMU::program_input_data()
{
	for (int i = 0; i < matrix_size; i++)
	{
		mac_array[i][0].data_in = ss->input_datas[i];
	}
}

void MMU::program_input_switch()
{
	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0;j < matrix_size; j++)
		{
			mac_array[i][j].switch_in = ss->switch_weights[i];
		}
	}
}

void MMU::program_input_write_things()
{
	for (int i = 0; i < matrix_size; i++)
	{
		if(progressing)
			mac_array[0][i].write_en_in = progressing;
		else
			mac_array[0][i].write_en_in = write_en;

		mac_array[0][i].weight_tag_in = prog_step;
		mac_array[0][i].weight_in = wf->input_weights[i];
	}
	advance_write();
}

void MMU::advance_write()
{
	if (write_en && !progressing)
	{
		progressing = true;
		prog_step = 0;
	}

	if (progressing)
	{
		prog_step++;
		if (prog_step == matrix_size)
		{
			progressing = false;
			prog_step = 0;
		}
	}
}