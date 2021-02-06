#pragma once

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

public:
	//setting
	int matrix_size;

	//input
	bool advance_en;

	//output
	int8_t* input_weights;

	//internal
	std::queue<int8_t**> weight_queue;

	//other HW
	Memory* dram;
	
	Weight_FIFO(int _matrix_size)
		: push_matrix_counter(&advance_en, _matrix_size)
	{
		matrix_size = _matrix_size;
		advance_en = false;
		input_weights = new int8_t[matrix_size];
		std::fill(input_weights, input_weights + matrix_size, 0);
		dram = NULL;

		push_matrix_counter.addHandlers(
			NULL,
			bind(&Weight_FIFO::transpose_and_push, this, placeholders::_1, placeholders::_2),
			bind(&Weight_FIFO::pop, this)
		);
	}

	~Weight_FIFO()
	{
		delete[] input_weights;
	}

	void transpose_and_push(int step, int max_step)
	{
		if (!weight_queue.empty())
		{
			for (int i = 0;i < matrix_size; i++) //Transpose (Weight stationary MMU needs transposed weight matrix)
				input_weights[i] = weight_queue.front()[i][max_step - step - 1];
		}
	}

	void push(int8_t** mat) 
	{
		//Assume mat is matrix_sizee * matrix_size
		weight_queue.push(mat);
	}

	void pop()
	{
		int8_t** mat_to_remove = weight_queue.front();

		weight_queue.pop();

		for (int i = 0; i < matrix_size; ++i)
			delete[] mat_to_remove[i];
		delete[] mat_to_remove;
	}

	void push_to_MMU()
	{
		push_matrix_counter.count();
	}
};

