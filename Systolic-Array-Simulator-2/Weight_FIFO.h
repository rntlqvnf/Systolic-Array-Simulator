#pragma once

#include <stdint.h>
#include <queue>

class Weight_FIFO
{
public:
	//setting
	int matrix_size;

	//input
	bool advance_en;

	//output
	int8_t* input_weights;

	//internal
	std::queue<int8_t**> weight_queue;
	bool advancing;
	int advance_step;
	
	Weight_FIFO(int _matrix_size)
	{
		matrix_size = _matrix_size;
		advance_en = false;
		advancing = false;
		advance_step = 0;
		input_weights = new int8_t[matrix_size];
	}

	~Weight_FIFO()
	{
		delete[] input_weights;
	}

	void push(int8_t **);
	void pop();
	void advance();
};

