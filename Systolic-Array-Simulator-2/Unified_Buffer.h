#pragma once

#include <assert.h>
#include <stdint.h>
#include <iostream>

//Column-wise buffer
//[0] = 11, 21, 31...
//[1] = 12, 22, 32...

class Unified_Buffer
{
public:
	//setting
	int addr_size;
	int matrix_size;

	int8_t** mem_block;

	//input
	int addr;
	int write_en;

	//other HW

	Unified_Buffer(int mat_size, int _addr_size)
	{
		addr_size = _addr_size;
		matrix_size = mat_size;

		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < addr_size; i++)
			mem_block[i] = new int8_t[matrix_size];

		addr = 0;
		write_en = false;
	}

	~Unified_Buffer()
	{
		for (int i = 0; i < addr_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}

	void write_from_accm(int32_t** arr)
	{
		if (write_en)
		{
			for (int i = 0; i < matrix_size; i++)
			{
				for (int j = 0; j < matrix_size; j++)
				{
					mem_block[addr + i][j] = (int8_t)arr[j][i];
				}
			}
		}
	}
};

