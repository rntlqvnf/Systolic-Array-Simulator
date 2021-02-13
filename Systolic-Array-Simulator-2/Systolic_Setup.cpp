#include "Systolic_Setup.h"

void Systolic_Setup::read_vector_from_UB_when_enable()
{
	read_vector_counter.count(matrix_size, matrix_size, ub_addr, 0);
}

void Systolic_Setup::read_vector_from_UB(int step, int max_step, int matrix_size, int addr)
{
	assert(ub != NULL);

	for (int i = 0; i < matrix_size; i++)
	{
		// 1 2 3 4 > >
		// > 1 2 3 4 >
		// > > 1 2 3 4
		diagonalized_matrix[i][i + step] = ub->mem_block[addr + i][step];
	}
}

void Systolic_Setup::reset_internal_matrix(int matrix_size)
{
	for (int i = 0; i < matrix_size; i++)
	{
		std::fill(diagonalized_matrix[i], diagonalized_matrix[i] + DIAG_WIDTH(matrix_size), (int8_t)0);
	}
}

void Systolic_Setup::push_vectors_to_MMU_when_enable()
{
	push_vector_counter.count(DIAG_WIDTH(matrix_size), matrix_size, switch_en, acc_addr_in);
}

void Systolic_Setup::reset_switch_vector(int matrix_size)
{
	std::fill(switch_weights, switch_weights + matrix_size, false);
}

void Systolic_Setup::reset_acc_outs()
{
	acc_write_en = false;
}

void Systolic_Setup::push_data_and_switch_vector_to_MMU(int step, int max_step, int matrix_size, int addr, int acc_addr)
{
	advance_switch_vector(step, max_step, matrix_size, addr, acc_addr);
	push_data_vector_to_MMU(step, max_step, matrix_size, addr, acc_addr);
	acc_addr_out = acc_addr + step;
	acc_write_en = true;
}

void Systolic_Setup::advance_switch_vector(int step, int max_step, int matrix_size, int switch_en, int acc_addr)
{
	if (step == 0)
	{
		switch_weights[step] = switch_en;
	}
	else if (step < matrix_size)
	{
		switch_weights[step] = switch_weights[step - 1];
		switch_weights[step - 1] = false;
	}
	else
	{
		std::fill(switch_weights, switch_weights + matrix_size, false);
	}
}

void Systolic_Setup::push_data_vector_to_MMU(int step, int max_step, int matrix_size, int addr, int acc_addr)
{
	for (int i = 0; i < matrix_size; i++)
		input_datas[i] = (step >= DIAG_WIDTH(matrix_size)) ? 0 : diagonalized_matrix[i][step];
}