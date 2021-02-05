#include "Systolic_Setup.h"

void Systolic_Setup::read_vector_from_UB()
{
	if (read_en && !reading)
	{
		reset_internal_matrix();
		reading = true;
		read_count = 0;
		ub_addr_reg = ub_addr;
	}

	if (reading)
	{
		program_input_vector();
		read_count++;

		if (read_count == matrix_size)
		{
			reading = false;
			read_count = 0;
		}
	}
}

void Systolic_Setup::program_input_vector()
{
	assert(ub != NULL);

	for (int i = 0; i < matrix_size; i++)
	{
		diagonalized_matrix[i][i + read_count] = ub->mem_block[ub_addr_reg = program_count][i];
	}
}

void Systolic_Setup::reset_internal_matrix()
{
	for (int i = 0; i < matrix_size; i++)
	{
		std::fill(diagonalized_matrix[i], diagonalized_matrix[i] + diag_width, (int8_t)0);
	}
}

void Systolic_Setup::advance_vector_to_MMU()
{
	if (advance_en && !advancing)
	{
		advancing = true;
		advance_count = 0;
		std::fill(switch_weights, switch_weights + matrix_size, false);
	}

	advance_outputs_to_accm();

	if (advancing)
	{
		advance_switchs();
		advance_internal_vector();
		advance_count++;

		if (advance_count == diag_width + matrix_size - 1) //last input column got into last weight column
		{
			advancing = false;
			advance_count = 0;
		}
	}
}

void Systolic_Setup::advance_switchs()
{
	if (advance_count == 0)
	{
		switch_weights[advance_count] = switch_en;
	}
	else if (advance_count < matrix_size)
	{
		switch_weights[advance_count] = switch_weights[advance_count - 1];
		switch_weights[advance_count - 1] = false;
	}
	else
	{
		std::fill(switch_weights, switch_weights + matrix_size, false);
	}
}

void Systolic_Setup::advance_internal_vector()
{
	for (int i = 0; i < matrix_size; i++)
		input_datas[i] = (advance_count >= diag_width) ? 0 : diagonalized_matrix[i][advance_count];
}

void Systolic_Setup::advance_outputs_to_accm()
{
	acc_write_en = advancing;
	acc_addr_out = acc_addr_in + advance_count;
}