#pragma once

#include "Accumulator.h"

class Activation
{
private:
	Counter act_vector_counter; 

public:
	//setting
	int matrix_size;

	//input
	int acc_addr;
	int ub_addr;
	bool act_en;

	//internal
	int32_t* vec;

	//other HW
	Accumulator* acc;

	Activation(int _matrix_size)
		:
		act_vector_counter(&act_en)
	{
		matrix_size = _matrix_size;

		acc_addr = 0;
		ub_addr = 0;
		act_en = false;

		vec = new int32_t[matrix_size];

		acc = NULL;

		act_vector_counter.addHandlers(
			NULL,
			NULL,
			NULL,
			NULL,
			bind(&Activation::read_activate_write, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5),
			NULL
		);
	}

	void do_activation_and_write_to_UB()
	{
		act_vector_counter.count(matrix_size, matrix_size, acc_addr, ub_addr);
	}

private:
	void read_activate_write(int step, int max_step, int matrix_size, int acc_addr, int ub_addr)
	{
		read_vector_from_UB(step, max_step, matrix_size, acc_addr); // 1 cycle
		activate(matrix_size);
		write_to_UB(step, max_step, matrix_size, ub_addr); // M cycle
	}
	void read_vector_from_UB(int step, int max_step, int matrix_size, int acc_addr)
	{
		assert(acc != NULL);

		// addr
		// 0  1  2 ...
		// 11 12 ?
		// ?  21 31
		// ?  ?  32
		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				vec[i] = acc->mem_block[i][acc_addr + i + j];
				//mat[i][j] = acc->mem_block[i][acc_addr + i + j];
			}
		}
	}
	void activate(int matrix_size)
	{
		for (int i = 0; i < matrix_size; i++)
		{
			vec[i] = vec[i] < 0 ? 0 : vec[i];
		}
	}

	void write_to_UB(int step, int max_step, int matrix_size, int ub_addr)
	{
		Unified_Buffer* ub = acc->mmu->ss->ub;

		for (int i = 0; i < matrix_size; i++)
		{
			//11 12 13 14 ...
			ub->mem_block[ub_addr + step][i] = vec[i];
		}
	}

};

