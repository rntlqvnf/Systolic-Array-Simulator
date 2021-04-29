#include "Systolic_Setup.h"

void Systolic_Setup::read_vector_from_UB_when_enable()
{
	SS_Inputs input = { matrix_size, ub_addr, acc_addr_in, switch_en, overwrite_en, unfold_en, cdi_en, cdd_en, start, end, value, crop_en};
	if (unfold_en)
	{
		int result_size = matrix_size - wsreg->get_buffer_size() + 1;
		read_vector_counter.count(result_size * result_size, input);
	}
	else
	{
		read_vector_counter.count(matrix_size, input);
	}
}

void Systolic_Setup::read_vector_from_UB(int step, int max_step, SS_Inputs data)
{
	assert(ub != NULL);

	if (data.unfold_en)
	{
		int weight_size = wsreg->get_size();
		int data_size = data.matrix_size;
		int result_size = data_size - weight_size + 1;
		int row = step / result_size;
		int col = step % result_size;
		
		for (int i = 0; i < weight_size; i++)
		{
			for (int j = 0; j < weight_size; j++)
			{
				diagonalized_matrix[i * weight_size + j][i * weight_size + j + step] 
					= ub->mem_block[data.ub_addr + row + i][col + j];
			}
		}
	}
	else
	{
		for (int i = 0; i < data.matrix_size; i++)
		{
			// 1 2 3 4 > >
			// > 1 2 3 4 >
			// > > 1 2 3 4
			int s_row = data.start / data.matrix_size;
			int s_col = data.start % data.matrix_size;
			int e_row = data.end / data.matrix_size;
			int e_col = data.end % data.matrix_size;

			int c_row = i;
			int c_col = step;

			if (data.cdd_en)
			{
				if(c_row >= s_row && c_row <= e_row && c_col >= s_col && c_col <= e_col)
					diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step] - data.value;
				else
					diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step];

			}
			else if (data.cdi_en)
			{
				if (c_row >= s_row && c_row <= e_row && c_col >= s_col && c_col <= e_col)
					diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step] + data.value;
				else
					diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step];
			}
			else if (data.crop_en)
			{
				if (c_row >= s_row && c_row <= e_row && c_col >= s_col && c_col <= e_col)
					diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step];
				else
					diagonalized_matrix[i][i + step] = 0;
			}
			else
			{
				diagonalized_matrix[i][i + step] = ub->mem_block[data.ub_addr + i][step];
			}

			Vec3b& buf2 = copy.at<Vec3b>(i, step);
			buf2[index] = diagonalized_matrix[i][i + step];
		}

		imshow("Image augmented", copy);
		waitKey(30);
	}
}

void Systolic_Setup::reset_matrix_and_wsreg(SS_Inputs datas)
{
	for (int i = 0; i < mmu_size; i++)
	{
		std::fill(diagonalized_matrix[i], diagonalized_matrix[i] + mmu_size * mmu_size, (int8_t)0);
	}
	if (datas.switch_en)
	{
		if (wsreg != NULL)
			wsreg->switch_size();
	}
}

void Systolic_Setup::push_vectors_to_MMU_when_enable()
{
	if (unfold_en)
	{
		assert(wsreg != NULL);
		int new_matrix_size = wsreg->get_size() * wsreg->get_size();

		int result_size = matrix_size - wsreg->get_size() + 1;
		int result_cols = result_size * result_size;

		int unfold_max_step = DIAG_WIDTH_2(mmu_size, result_cols);
		int non_unfold_max_step = DIAG_WIDTH_1(mmu_size);

		int bigger_max_step = unfold_max_step > non_unfold_max_step ? unfold_max_step : non_unfold_max_step;

		SS_Inputs input = { new_matrix_size, ub_addr, acc_addr_in, switch_en, overwrite_en, unfold_en };
		push_vector_counter.count(bigger_max_step + mmu_size + 1, input);
	}
	else
	{
		SS_Inputs input = { matrix_size, ub_addr, acc_addr_in, switch_en, overwrite_en, unfold_en };
		push_vector_counter.count(DIAG_WIDTH_1(mmu_size) + mmu_size + 1, input); 
		//Why DIAG_WIDTH_1(mmu_size) + mmu_size + 1?
		//Cycle for last column to go last mmu column is  DIAG_WIDTH_1(mmu_size) + mmu_size
		//1 cycle for acc write
	}
}

void Systolic_Setup::reset_switch_vector(SS_Inputs datas)
{
	std::fill(switch_weights, switch_weights + mmu_size, false);
}

void Systolic_Setup::reset_ouputs()
{
	acc_write_en = false;
}

void Systolic_Setup::push_data_and_switch_vector_to_MMU(int step, int max_step, SS_Inputs data)
{
	advance_switch_vector(step, max_step, data.matrix_size, data.switch_en);
	push_data_vector_to_MMU(step, max_step, data.matrix_size);
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
		std::fill(switch_weights, switch_weights + mmu_size, false);
	}
}

void Systolic_Setup::push_data_vector_to_MMU(int step, int max_step, int matrix_size)
{
	for (int i = 0; i < mmu_size; i++)
		input_datas[i] = i >= matrix_size ? 0 : diagonalized_matrix[i][step];
}