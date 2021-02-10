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
			bind(&Weight_FIFO::transpose_and_push, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4),
			NULL,
			bind(&Weight_FIFO::pop, this)
		);

		read_matrix_counter.addHandlers(
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
	
	void push(int8_t** mat)
	{
		weight_queue.push(mat);
	}

	void transpose_and_push(int step, int max_step, int matrix_size, int addr)
	{
		if (!weight_queue.empty())
		{
			for (int i = 0;i < matrix_size; i++) //Transpose (Weight stationary MMU needs transposed weight matrix)
				input_weights[i] = weight_queue.front()[i][max_step - step - 1];
		}
	}

	void read_matrix_when_max_step(int step, int max_step, int matrix_size, int addr)
	{
		if (step == max_step - 1)
		{
			int8_t** mat = new int8_t * [matrix_size];
			for (int i = 0; i < matrix_size; i++)
				mat[i] = new int8_t[matrix_size];

			for (int i = 0; i < matrix_size; i++)
			{
				for (int j = 0; j < matrix_size; j++)
				{
					mat[i][j] = dram->mem_block[addr + i][j];
				}
			}

			weight_queue.push(mat);
		}
	}

	void pop()
	{
		if (!weight_queue.empty())
		{
			int8_t** mat_to_remove = weight_queue.front();

			weight_queue.pop();

			for (int i = 0; i < matrix_size; ++i)
				delete[] mat_to_remove[i];
			delete[] mat_to_remove;
		}
	}

	void push_weight_vector_to_MMU()
	{
		push_matrix_counter.count(matrix_size, matrix_size, 0, 0);
	}

	void read_matrix_from_DRAM()
	{
		int max_count = (matrix_size * matrix_size + 64 - 1) / 64;
		read_matrix_counter.count(max_count, matrix_size, dram_addr, 0);
	}
};


