#include "Weight_FIFO.h"

void Weight_FIFO::push(int8_t** mat)
{
	weight_queue.push(mat);
}

void Weight_FIFO::pop()
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

void Weight_FIFO::push_weight_vector_to_MMU_when_en()
{
	push_matrix_counter.count(matrix_size, matrix_size, 0, 0);
}

void Weight_FIFO::read_matrix_from_DRAM_when_en()
{
	int max_count = (matrix_size * matrix_size + 64 - 1) / 64;
	read_matrix_counter.count(max_count, matrix_size, dram_addr, 0);
}

void Weight_FIFO::transpose_and_push(int step, int max_step, int matrix_size, int addr)
{
	if (!weight_queue.empty())
	{
		for (int i = 0;i < matrix_size; i++) //Transpose (Weight stationary MMU needs transposed weight matrix)
			input_weights[i] = weight_queue.front()[i][max_step - step - 1];
	}
}

void Weight_FIFO::read_matrix_when_max_step(int step, int max_step, int matrix_size, int addr)
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