#pragma once

#include <assert.h>
#include <stdint.h>

class Unified_Buffer
{
public:
	int addr_size;

	int8_t** mem_block;

	Unified_Buffer(int mat_size, int _addr_size)
	{
		addr_size = _addr_size;

		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < mat_size; i++)
			mem_block[i] = new int8_t[mat_size];
	}

	~Unified_Buffer()
	{
		for (int i = 0; i < addr_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}
};

