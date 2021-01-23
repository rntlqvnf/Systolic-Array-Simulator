#pragma once

#include <stdint.h>
#include <queue>
#include "config.h"
#include "matrix.h"

//4-entry 64KB FIFO queue

const int MAX_ENTRY = 4;

class Weight_FIFO
{
private:
	std::queue<Matrix<int8_t>> buffer_queue;

public:
	void push(Matrix<int8_t> mat);
	Matrix<int8_t> pop();
};

