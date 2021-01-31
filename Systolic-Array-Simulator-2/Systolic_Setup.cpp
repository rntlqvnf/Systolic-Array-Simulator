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
	if (adv_en && !advancing)
	{
		advancing = true;
		adv_count = 0;
		std::fill(switch_weights, switch_weights + matrix_size, false);
	}

	if (advancing)
	{
		advance_switchs();
		advance_internal_vector();
		adv_count++;

		if (adv_count == matrix_size)
		{
			advancing = false;
			adv_count = 0;
		}
	}
}

void Systolic_Setup::advance_switchs()
{
	if(adv_count == 0)
		switch_weights[adv_count] = switch_en;
	else
	{
		switch_weights[adv_count] = switch_weights[adv_count - 1];
		switch_weights[adv_count - 1] = 0;
	}
}

void Systolic_Setup::advance_internal_vector()
{
	for (int i = 0; i < matrix_size; i++)
		input_data[i] = diagonalized_matrix[adv_count][i];
}