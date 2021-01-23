#include "Weight_FIFO.h"

void Weight_FIFO::push(Matrix<int8_t> mat)
{
	if (buffer_queue.size() == 4)
		buffer_queue.pop();
	buffer_queue.push(mat);
}

Matrix<int8_t> Weight_FIFO::pop()
{
	Matrix<int8_t> result = buffer_queue.front();
	buffer_queue.pop();
	return result;
}