#pragma once

#include <assert.h>
#include <stdint.h>
#include <iostream>
#include "Memory.h"
#include "Counter.h"

struct UB_Inputs
{
	int matrix_size;
	int addr;
	int hm_addr;
};

class Unified_Buffer
{
private:
	int addr_size;
	int mmu_size;

	//internal
	Counter<UB_Inputs> read_vector_counter;
	Counter<UB_Inputs> write_vector_counter;

	void read_vector_from_HM(int, int, UB_Inputs);
	void write_vector_to_HM(int, int, UB_Inputs);

public:
	int8_t** mem_block;

	//input
	int addr;
	int hm_addr;
	bool read_en;
	bool write_en;
	int matrix_size;

	//other HW
	Memory *hm;

	Unified_Buffer(int mmu_size, int addr_size)
		:read_vector_counter(&read_en),
		write_vector_counter(&write_en)
	{
		this->addr_size = addr_size; //col size
		this->mmu_size = mmu_size; //row size

		mem_block = new int8_t * [addr_size];
		for (int i = 0; i < addr_size; i++)
			mem_block[i] = new int8_t[mmu_size];

		addr = 0;
		hm_addr = 0;
		read_en = false;
		matrix_size = mmu_size;

		hm = NULL;

		read_vector_counter.addHandlers(
			NULL,
			bind(&Unified_Buffer::read_vector_from_HM, this, placeholders::_1, placeholders::_2, placeholders::_3),
			NULL
		);

		write_vector_counter.addHandlers(
			NULL,
			bind(&Unified_Buffer::write_vector_to_HM, this, placeholders::_1, placeholders::_2, placeholders::_3),
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

