#pragma once

#include <iostream>
#include <stdint.h>
#include <queue>
#include "Memory.h"
#include "Counter.h"

using namespace std;

class Weight_FIFO
{
private:
	//internal
	Counter push_matrix_counter;
	Counter read_matrix_counter;

	void transpose_and_push(int, int, int, int);
	void read_matrix_when_max_step(int ,int, int, int);

public:
	//setting
	int matrix_size;

	//input
	bool push_en;
	bool read_en;
	int dram_addr;

	//output
	int8_t* input_weights;

	//internal
	std::queue<int8_t**> weight_queue;

	//other HW
	Memory* dram;
	
	Weight_FIFO(int _matrix_size)
		: 
		push_matrix_counter(&push_en),
		read_matrix_counter(&read_en) //64 bytes
	{
		matrix_size = _matrix_size;

		read_en = false;
		push_en = false;
		dram_addr = 0;

		input_weights = new int8_t[matrix_size];
		std::fill(input_weights, input_weights + matrix_size, 0);
		dram = NULL;

		push_matrix_counter.addHandlers(
			NULL,
			NULL,
			NULL,
			bind(&Weight_FIFO::transpose_and_push, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4),
			NULL,
			bind(&Weight_FIFO::pop, this)
		);

		read_matrix_counter.addHandlers(
			NULL,
			NULL,
			NULL,
			bind(&Weight_FIFO::read_matrix_when_max_step, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4),
			NULL,
			NULL
		);
	}

	~Weight_FIFO()
	{
		delete[] input_weights;
	}
	
	void push(int8_t** mat);
	void pop();
	void push_weight_vector_to_MMU_when_en();
	void read_matrix_from_DRAM_when_en();
};


