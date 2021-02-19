#pragma once

#include "Accumulator.h"

struct Act_Inputs
{
	int matrix_size;
	int kernel_size;
	int acc_addr;
	int ub_addr;
	bool fold_en;
	bool pool_en;
};

class Activation
{
private:
	Counter<Act_Inputs> act_vector_counter; 
	int mmu_size;

public:
	//input
	int matrix_size;
	int kernel_size;
	int acc_addr;
	int ub_addr;
	bool act_en;
	bool fold_en;
	bool pool_en;

	//internal
	int32_t** mat;

	//other HW
	Accumulator* acc;

	Activation(int _mmu_size)
		:
		act_vector_counter(&act_en)
	{
		mmu_size = _mmu_size;

		matrix_size = mmu_size;
		kernel_size = mmu_size;
		acc_addr = 0;
		ub_addr = 0;
		act_en = false;
		fold_en = false;
		pool_en = false;

		mat = new int32_t * [mmu_size];
		for (int i = 0; i < mmu_size; i++)
			mat[i] = new int32_t[mmu_size];

		acc = NULL;

		act_vector_counter.addHandlers(
			NULL,
			bind(&Activation::read_activate_write, this, placeholders::_1, placeholders::_2, placeholders::_3),
			NULL
		);
	}

	void do_activation_and_write_to_UB()
	{
		Act_Inputs inputs = { matrix_size, kernel_size, acc_addr, ub_addr, fold_en, pool_en };
		act_vector_counter.count(matrix_size, inputs);
	}

private:
	void read_activate_write(int step, int max_step, Act_Inputs data)
	{
		read_vector_from_ACC(step, max_step, data.matrix_size, data.acc_addr, data.fold_en);
		activate(step, data.matrix_size);
		write_to_UB(step, max_step, data.matrix_size, data.kernel_size, data.ub_addr, data.pool_en);
	}
	void read_vector_from_ACC(int step, int max_step, int matrix_size, int acc_addr, bool fold_en)
	{
		assert(acc != NULL);

		if (fold_en)
		{
			// (addr / width)
			//   0  1  2 ...
			// 0 11 -  -
			// 1 12 -  -
			// 2 21 -  -
			// 3 22  -  -
			// 4 -  -  -

			for (int i = 0; i < matrix_size; i++)
			{
				mat[i][step] = acc->mem_block[acc_addr + step + i * matrix_size][0];
			}
		}
		else
		{
			// (addr / width)
			//   0  1  2 ...
			// 0 11 -  -
			// 1 12 21 -
			// 2 13 22 31
			// 3 -  23 32
			// 4 -  -  33

			for (int i = 0; i < matrix_size; i++)
			{
				mat[i][step] = acc->mem_block[acc_addr + step + i][i];
			}
		}
	}
	void activate(int step, int matrix_size)
	{
		for (int i = 0; i < matrix_size; i++)
		{
			//ReLU
			mat[i][step] = mat[i][step] < 0 ? 0 : mat[i][step];
		}
	}

	void write_to_UB(int step, int max_step, int matrix_size, int kernel_size, int ub_addr, bool pool_en)
	{
		Unified_Buffer* ub = acc->mmu->ss->ub;

		if (step + 1 == max_step)
		{
			if (pool_en)
			{
				for (int i = 0; i < matrix_size / kernel_size; i++)
				{
					for (int j = 0; j < matrix_size / kernel_size; j++)
					{
						cout << "VALUE : " << get_avg(kernel_size, i * kernel_size, j * kernel_size) << endl;
						ub->mem_block[ub_addr + i][j] = get_avg(kernel_size, i * kernel_size, j * kernel_size);
					}
				}
			}
			else
			{
				for (int i = 0; i < matrix_size; i++)
				{
					for (int j = 0; j < matrix_size; j++)
					{
						ub->mem_block[ub_addr + i][j] = mat[i][j];
					}
				}
			}
		}
	}

	int32_t get_avg(int kernel_size, int row, int col)
	{
		int32_t sum = 0;
		for (int i = 0; i < kernel_size; i++)
		{
			for (int j = 0; j < kernel_size; j++)
			{
				sum += mat[row + i][col + j];
			}
		}
		return sum / (kernel_size * kernel_size);
	}
};

