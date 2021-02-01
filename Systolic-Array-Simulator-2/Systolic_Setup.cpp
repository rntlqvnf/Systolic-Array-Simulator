#include "Systolic_Setup.h"

void Systolic_Setup::program()
{
	if (write_en && !programming)
	{
		reset_internal_matrix();
		programming = true;
		count = 0;
	}

	if (programming)
	{
		program_input_vector();
		count++;

		if (count == matrix_size)
		{
			programming = false;
			count = 0;
		}
	}
}

void Systolic_Setup::program_input_vector()
{
	assert(ub != NULL);

	for (int i = 0; i < matrix_size; i++)
	{
		diagonalized_matrix[i][i + count] = ub->mem_block[count][i];
	}
}

void Systolic_Setup::reset_internal_matrix()
{
	for (int i = 0; i < matrix_size; i++)
	{
		std::fill(diagonalized_matrix[i], diagonalized_matrix[i] + diag_width, (int8_t)0);
	}
}

void Systolic_Setup::advance()
{
	if (advance_en && !advancing)
	{
		advancing = true;
		advance_count = 0;
		std::fill(switch_weights, switch_weights + matrix_size, false);
	}

	if (advancing)
	{
		advance_switchs();
		advance_internal_vector();
		advance_count++;

		if (advance_count == diag_width)
		{
			advancing = false;
			advance_count = 0;
		}
	}
	else
	{
		//Reset 0 when not advancing
		for (int i = 0; i < matrix_size; i++)
			input_datas[i] = 0;
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
		input_datas[i] = diagonalized_matrix[i][advance_count];
}