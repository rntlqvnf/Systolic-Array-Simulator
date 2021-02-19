#include "Weight_FIFO.h"

void Weight_FIFO::push(int8_t** mat)
{
	weight_queue.push(mat);
}

void Weight_FIFO::pop_and_set_size_ifn_start(WF_Inputs data)
{
	write_en = true;
	
	if (wsreg != NULL)
	{
		wsreg->set_size(data.matrix_size);
	}

	if (state == STATE::READ_END)
	{
		state = STATE::PUSH_END;
	}
	else if (!weight_queue.empty())
	{
		int8_t** mat_to_remove = weight_queue.front();

		weight_queue.pop();

		for (int i = 0; i < mmu_size; ++i)
			delete[] mat_to_remove[i];
		delete[] mat_to_remove;
	}
}

void Weight_FIFO::read_matrix_from_DRAM_when_en()
{
	int max_count = (matrix_size * matrix_size + 64 - 1) / 64;
	WF_Inputs inputs = { matrix_size, dram_addr };
	read_matrix_counter.count(max_count, inputs);
}

void Weight_FIFO::push_weight_vector_to_MMU_when_en()
{
	WF_Inputs inputs = { matrix_size, 0 };
	if(state == STATE::READ_END)
		push_matrix_counter.count(true, mmu_size, inputs);
	else
		push_matrix_counter.count(mmu_size, inputs);
}

void Weight_FIFO::transpose_and_push(int step, int max_step, WF_Inputs data)
{
	if (!weight_queue.empty())
	{
		for (int i = 0; i < mmu_size; i++) //Transpose (Weight stationary MMU needs transposed weight matrix)
			input_weights[i] = weight_queue.front()[i][max_step - step - 1];
	}
}

void Weight_FIFO::read_matrix_when_max_step(int step, int max_step, WF_Inputs data)
{
	if (step == max_step - 1)
	{
		assert(mmu_size >= data.matrix_size);
		if (unfold_en) assert(mmu_size >= data.matrix_size * data.matrix_size);

		int8_t** mat = new int8_t * [mmu_size];
		for (int i = 0; i < mmu_size; i++)
			mat[i] = new int8_t[mmu_size];

		for (int i = 0; i < data.matrix_size; i++)
		{
			for (int j = 0; j < data.matrix_size; j++)
			{
				if(unfold_en)
					mat[0][data.matrix_size * i + j] = dram->mem_block[data.dram_addr + i][j];
				else
					mat[i][j] = dram->mem_block[data.dram_addr + i][j];
			}
		}

		weight_queue.push(mat);
	}
}

void Weight_FIFO::push_when_startup()
{
	if (state == STATE::INITIAL)
		state = STATE::READ_END;
}