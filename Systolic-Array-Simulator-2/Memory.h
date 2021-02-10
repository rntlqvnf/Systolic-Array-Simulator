#pragma once

#include <stdint.h>

class Memory
{
private:
	int row_size;
	int col_size;

public:
	int8_t** mem_block;

	Memory(int row_size, int col_size)
	{
		this->row_size = row_size;
		this->col_size = col_size;
		mem_block = new int8_t * [col_size];
		for (int i = 0; i < col_size; i++)
			mem_block[i] = new int8_t[row_size];
	}

	~Memory()
	{
		for (int i = 0; i < col_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}
};

