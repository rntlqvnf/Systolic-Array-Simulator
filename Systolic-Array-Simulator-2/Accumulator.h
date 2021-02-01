#pragma once

#include <stdint.h>
#include "MMU.h"

class Accumulator
{
public:
	//setting
	int matrix_size;
	int addr_size;

	//internal
	int32_t** mem_block;

	//other HW
	MMU* mmu;

	Accumulator(int _matrix_size, int _addr_size)
	{
		matrix_size = _matrix_size;
		addr_size = _addr_size;

		mem_block = new int32_t * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			mem_block[i] = new int32_t[addr_size];

		mmu = NULL;
	}

	void write_results()
	{
		assert(mmu != NULL);

		bool write_en = mmu->ss->accm_write_en;
		int addr = mmu->ss->accm_addr_out;

		if (write_en)
		{
			write(mmu->last_row_sum, addr);
		}
	}

	void write(int32_t* data,int addr)
	{
		for (int i = 0; i < matrix_size; i++)
		{
			mem_block[i][addr] = data[i];
		}
	}

	void read(int32_t* dst, int addr)
	{
		for (int i = 0; i < matrix_size; i++)
		{
			dst[i] = mem_block[i][addr];
		}
	}

	~Accumulator()
	{
		for (int i = 0; i < matrix_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}
};

