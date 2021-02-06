#pragma once

#include "Accumulator.h"

class Activation
{
public:
	//setting
	int matrix_size;

	//input
	int acc_addr;
	int ub_addr;
	bool act_en;

	//internal
	int32_t** mat;

	//other HW
	Accumulator* acc;

	Activation(int _matrix_size)
	{
		matrix_size = _matrix_size;

		acc_addr = 0;
		ub_addr = 0;
		act_en = false;

		mat = new int32_t * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			mat[i] = new int32_t[matrix_size];

		acc = NULL;
	}

	~Activation()
	{
		for (int i = 0; i < matrix_size; ++i)
			delete[] mat[i];
		delete[] mat;
	}

	void do_activation_and_write_to_UB()
	{
		if (act_en)
		{
			program_mat_from_accm(); // 1 cycle
			activate();
			write_to_UB(); // M cycle
		}
	}

private:
	void activate()
	{

	}

	void write_to_UB()
	{
		Unified_Buffer* ub = acc->mmu->ss->ub;

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				ub->mem_block[ub_addr + i][j] = (int8_t)mat[i][j];
			}
		}
	}

	void program_mat_from_accm()
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
				mat[i][j] = acc->mem_block[i][acc_addr + i + j];
			}
		}
	}

};

