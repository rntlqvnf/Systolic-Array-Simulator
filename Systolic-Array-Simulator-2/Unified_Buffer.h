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

	bool reading;
	int read_count;
	int read_addr;
	int hm_read_addr;

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

		reading = false;
		read_count = 0;
		read_addr = 0;
		hm_read_addr = 0;

		hm = NULL;
	}

	~Unified_Buffer()
	{
		for (int i = 0; i < addr_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}

	void read_vector_from_HM()
	{
		assert(hm != NULL);

		if (read_en && !reading)
		{
			reading = true;
			read_count = 0;
			read_addr = addr;
			hm_read_addr = hm_addr;
		}

		if (reading)
		{
			for (int i = 0; i < matrix_size; i++)
			{
				mem_block[read_addr + read_count][i] = hm->mem_block[hm_read_addr + read_count][i];
			}
			read_count++;

			if (read_count == matrix_size)
			{
				reading = false;
				read_count = 0;
			}
		}
	}
};

