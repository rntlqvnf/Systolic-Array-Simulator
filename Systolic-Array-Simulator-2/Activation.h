#pragma once

#include "Accumulator.h"

class Activation
{
public:
	//setting
	int matrix_size;

	//input
	int addr;
	bool act_en;

	//internal
	int32_t** mat;

	//other HW
	Accumulator* acc;

	Activation(int _matrix_size)
	{
		matrix_size = _matrix_size;

		addr = 0;
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

	void program_mat_from_accm()
	{
		assert(acc != NULL);

		if (act_en)
		{
			// addr
			// 0  1  2 ...
			// 11 12 ?
			// ?  21 31
			// ?  ?  32
			for (int i = 0; i < matrix_size; i++)
			{
				for (int j = 0; j < matrix_size; j++)
				{
					mat[i][j] = acc->mem_block[i][addr + i + j];
				}
			}
		}
	}
};

