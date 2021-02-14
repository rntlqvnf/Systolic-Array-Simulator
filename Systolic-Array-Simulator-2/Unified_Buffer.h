#pragma once

#include <assert.h>
#include <stdint.h>
#include <iostream>
#include "Memory.h"
#include "Counter.h"

class Unified_Buffer
{
private:
	int addr_size;
	int matrix_size;

	//internal
	Counter read_vector_counter;
	Counter write_vector_counter;

	void read_vector_from_HM(int, int, int, int, int);
	void write_vector_to_HM(int, int, int, int, int);

public:
	int8_t** mem_block;

	//input
	int addr;
	int hm_addr;
	bool read_en;
	bool write_en;
	int matrix_size_in;

	//other HW
	Memory *hm;

	Unified_Buffer(int matrix_size, int addr_size)
		:read_vector_counter(&read_en),
		write_vector_counter(&write_en)
	{
		this->addr_size = addr_size; //col size
		this->matrix_size = matrix_size; //row size

		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < addr_size; i++)
			mem_block[i] = new int8_t[matrix_size];

		addr = 0;
		hm_addr = 0;
		read_en = false;
		matrix_size_in = matrix_size;

		hm = NULL;

		read_vector_counter.addHandlers(
			NULL,
			NULL,
			NULL,
			NULL,
			bind(&Unified_Buffer::read_vector_from_HM, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5),
			NULL
		);

		write_vector_counter.addHandlers(
			NULL,
			NULL,
			NULL,
			NULL,
			bind(&Unified_Buffer::write_vector_to_HM, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5),
			NULL
		);
	}

	~Unified_Buffer()
	{
		for (int i = 0; i < addr_size; ++i)
			delete[] mem_block[i];
		delete[] mem_block;
	}

	void read_vector_from_HM_when_enable();
	void write_vector_to_HM_when_enable();
};

