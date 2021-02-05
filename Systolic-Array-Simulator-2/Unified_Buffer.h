#pragma once

#include <assert.h>
#include <stdint.h>
#include <iostream>
#include "Host_Mem.h"

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
	int hm_addr;
	bool read_en;
	bool write_en;

	//internal
	bool reading;
	int read_count;
	int read_addr;
	int hm_read_addr;

	//other HW
	Host_Mem *hm;

	Unified_Buffer(int mat_size, int _addr_size)
	{
		addr_size = _addr_size;
		matrix_size = mat_size;

		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < addr_size; i++)
			mem_block[i] = new int8_t[matrix_size];

		addr = 0;
		hm_addr = 0;
		read_en = false;
		write_en = false;

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

	void read_vector_from_hm()
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
				mem_block[read_addr + read_count][i] = hm->mem_block[i][hm_read_addr + read_count];
			}
			read_count++;

			if (read_count == matrix_size)
			{
				reading = false;
				read_count = 0;
			}
		}
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

