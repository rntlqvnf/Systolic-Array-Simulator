#include "Weight_FIFO.h"

void Weight_FIFO::push(int8_t** mat)
{
	//Assume mat is matrix_sizee * matrix_size
	weight_queue.push(mat);
}

void Weight_FIFO::pop()
{
	int8_t** mat_to_remove = weight_queue.front();

	weight_queue.pop();

	for (int i = 0; i < matrix_size; ++i)
		delete[] mat_to_remove[i];
	delete[] mat_to_remove;
}

void Weight_FIFO::advance()
{
	if (advance_en && !advancing)
	{
		advancing = true;
		advance_step = 0;
	}

	if (advancing)
	{
		for (int i = 0;i < matrix_size; i++) //Transpose (Weight stationary MMU needs transposed weight matrix)
			input_weights[i] = weight_queue.front()[i][matrix_size - advance_step - 1];

		advance_step++;
		if(advance_step == matrix_size)
		{ 
			advance_step = 0;
			advancing = false;
			pop();
		}
	}
}