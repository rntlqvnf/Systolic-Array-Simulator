#pragma once

#include "Accumulator.h"

struct Act_Inputs
{
	int matrix_size;
	int acc_addr;
	int ub_addr;
};

class Activation
{
private:
	Counter<Act_Inputs> act_vector_counter; 
	int mmu_size;

public:
	//input
	int matrix_size;
	int acc_addr;
	int ub_addr;
	bool act_en;

	//internal
	int32_t* vec;

	//other HW
	Accumulator* acc;

	Activation(int _mmu_size)
		:
		act_vector_counter(&act_en)
	{
		mmu_size = _mmu_size;

		matrix_size = mmu_size;
		acc_addr = 0;
		ub_addr = 0;
		act_en = false;

		vec = new int32_t[mmu_size];

		acc = NULL;

		act_vector_counter.addHandlers(
			NULL,
			bind(&Activation::read_activate_write, this, placeholders::_1, placeholders::_2, placeholders::_3),
			NULL
		);
	}

	void do_activation_and_write_to_UB()
	{
		Act_Inputs inputs = { matrix_size, acc_addr, ub_addr };
		act_vector_counter.count(matrix_size, inputs);
	}

private:
	void read_activate_write(int step, int max_step, Act_Inputs data)
	{
		read_vector_from_UB(step, max_step, data.matrix_size, data.acc_addr);
		activate(data.matrix_size);
		write_to_UB(step, max_step, data.matrix_size, data.ub_addr);
	}
	void read_vector_from_UB(int step, int max_step, int matrix_size, int acc_addr)
	{
		assert(acc != NULL);

		// (addr / width)
		//   0  1  2 ...
		// 0 11 -  -
		// 1 12 21 -
		// 2 13 22 31
		// 3 -  23 32
		// 4 -  -  33

		for (int i = 0; i < matrix_size; i++)
		{
			vec[i] = acc->mem_block[acc_addr + step + i][i];
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
			ub->mem_block[ub_addr + i][step] = vec[i];
		}
	}
};

