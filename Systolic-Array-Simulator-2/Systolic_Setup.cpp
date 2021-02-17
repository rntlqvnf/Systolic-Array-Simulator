#include "Systolic_Setup.h"

void Systolic_Setup::read_vector_from_UB_when_enable()
{
	SS_Inputs input = { matrix_size, ub_addr, acc_addr_in, switch_en, overwrite_en };
	read_vector_counter.count(matrix_size, input);
}

void Systolic_Setup::read_vector_from_UB(int step, int max_step, SS_Inputs data)
{
	assert(ub != NULL);

	for (int i = 0; i < data.matrix_size; i++)
	{
		// 1 2 3 4 > >
		// > 1 2 3 4 >
		// > > 1 2 3 4
		diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step];
	}
}

void Systolic_Setup::reset_matrix_and_wsreg(SS_Inputs datas)
{
	for (int i = 0; i < mmu_size; i++)
	{
		std::fill(diagonalized_matrix[i], diagonalized_matrix[i] + DIAG_WIDTH(mmu_size), (int8_t)0);
	}
	if (datas.switch_en)
	{
		if (wsreg != NULL)
			wsreg->switch_size();
	}
}

void Systolic_Setup::push_vectors_to_MMU_when_enable()
{
	SS_Inputs input = { matrix_size, ub_addr, acc_addr_in, switch_en, overwrite_en };
	push_vector_counter.count(DIAG_WIDTH(mmu_size) + 1, input);
}

void Systolic_Setup::reset_switch_vector(SS_Inputs datas)
{
	std::fill(switch_weights, switch_weights + mmu_size, false);
}

void Systolic_Setup::reset_acc_outs()
{
	acc_write_en = false;
}

void Systolic_Setup::push_data_and_switch_vector_to_MMU(int step, int max_step, SS_Inputs data)
{
	advance_switch_vector(step, max_step, data.matrix_size, data.switch_en);
	push_data_vector_to_MMU(step, max_step, data.matrix_size);
	//Valid acc value outs when over setting matrix_size
	acc_addr_out = step >= this->mmu_size ? data.acc_addr + (step - this->mmu_size) : data.acc_addr;
	acc_write_en = step >= this->mmu_size ? true : false;
	acc_overwrite_en = data.overwrite_en;
}

void Systolic_Setup::advance_switch_vector(int step, int max_step, int matrix_size, bool switch_en)
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

void Systolic_Setup::push_data_vector_to_MMU(int step, int max_step, int matrix_size)
{
	for (int i = 0; i < matrix_size; i++)
		input_datas[i] = (step >= DIAG_WIDTH(matrix_size)) ? 0 : diagonalized_matrix[i][step];
}