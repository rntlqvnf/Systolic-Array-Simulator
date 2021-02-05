#pragma once

#include <stdint.h>

class Host_Mem
{
private:
	int matrix_size;
	int addr_size;

public:
	int8_t** mem_block;

	Host_Mem(int matrix_size, int addr_size)
	{
		this->matrix_size = matrix_size;
		this->addr_size = addr_size;
		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < addr_size; i++)
			mem_block[i] = new int8_t[matrix_size];
	}

	~Host_Mem()
	{
		for (int i = 0; i < addr_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}
};

