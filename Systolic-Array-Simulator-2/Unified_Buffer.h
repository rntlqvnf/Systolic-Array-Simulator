#pragma once

#include <assert.h>
#include <stdint.h>
#include <iostream>
#include "Memory.h"
#include "Counter.h"

class Unified_Buffer
{
private:
	//internal
	Counter read_vector_counter;

public:
	//setting
	int addr_size;
	int matrix_size;

	int8_t** mem_block;

	//input
	int addr;
	int hm_addr;
	bool read_en;

	//other HW
	Memory *hm;

	Unified_Buffer(int mat_size, int _addr_size)
		:read_vector_counter(&read_en)
	{
		addr_size = _addr_size;
		matrix_size = mat_size;

		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < addr_size; i++)
			mem_block[i] = new int8_t[matrix_size];

		addr = 0;
		hm_addr = 0;
		read_en = false;

		hm = NULL;

		read_vector_counter.addHandlers(
			NULL,
			NULL,
			NULL,
			bind(&Unified_Buffer::read_vector_from_HM, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5),
			NULL
		);
	}

	~Unified_Buffer()
	{
		for (int i = 0; i < addr_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}

	void read_vector_when_enable()
	{
		assert(hm != NULL);

		read_vector_counter.count(matrix_size, matrix_size, addr, hm_addr);
	}

	void read_vector_from_HM(int step, int max_step, int matrix_size, int read_addr, int hm_read_addr)
	{
		for (int i = 0; i < matrix_size; i++)
		{
			mem_block[read_addr + step][i] = hm->mem_block[hm_read_addr + step][i];
		}
	}
};

