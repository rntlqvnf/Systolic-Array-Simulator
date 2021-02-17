#pragma once

#include <iostream>
#include <stdint.h>
#include <queue>
#include "Memory.h"
#include "Counter.h"
#include "Weight_Size_Reg.h"
#include <assert.h>

using namespace std;

enum class STATE
{
	INITIAL,
	READ_END,
	PUSH_END
};

struct WF_Inputs
{
	int matrix_size;
	int dram_addr;
};

class Weight_FIFO
{
private:
	//setting
	int mmu_size;

	//internal
	Counter<WF_Inputs> push_matrix_counter;
	Counter<WF_Inputs> read_matrix_counter;
	STATE state;

	void push_when_startup();
	void transpose_and_push(int, int, WF_Inputs);
	void read_matrix_when_max_step(int ,int, WF_Inputs);

public:
	//input
	bool read_en;
	bool push_en;
	bool unfold_en;
	int matrix_size;
	int dram_addr;

	//output
	int8_t* input_weights;
	bool write_en;

	//internal
	std::queue<int8_t**> weight_queue;

	//other HW
	Memory* dram;
	Weight_Size_Reg* wsreg;
	
	Weight_FIFO(int _mmu_size)
		: 
		push_matrix_counter(&push_en),
		read_matrix_counter(&read_en), //64 bytes
		state(STATE::INITIAL)
	{
		mmu_size = _mmu_size;

		read_en = false;
		push_en = false;
		unfold_en = false;
		matrix_size = mmu_size;
		dram_addr = 0;

		input_weights = new int8_t[mmu_size];
		write_en = false;
		std::fill(input_weights, input_weights + mmu_size, 0);
		dram = NULL;
		wsreg = NULL;

		push_matrix_counter.addHandlers(
			bind(&Weight_FIFO::pop_and_set_size_ifn_start, this, placeholders::_1),
			bind(&Weight_FIFO::transpose_and_push, this, placeholders::_1, placeholders::_2, placeholders::_3),
			NULL
		);

		read_matrix_counter.addHandlers(
			NULL,
			bind(&Weight_FIFO::read_matrix_when_max_step, this, placeholders::_1, placeholders::_2, placeholders::_3),
			bind(&Weight_FIFO::push_when_startup, this)
		);
	}

	~Weight_FIFO()
	{
		delete[] input_weights;
	}
	
	void push(int8_t** mat);
	void pop_and_set_size_ifn_start(WF_Inputs);
	void read_matrix_from_DRAM_when_en();
	void push_weight_vector_to_MMU_when_en();
};


