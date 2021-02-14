#include "Unified_Buffer.h"

void Unified_Buffer::read_vector_from_HM_when_enable()
{
	assert(hm != NULL);

	read_vector_counter.count(matrix_size_in, matrix_size_in, addr, hm_addr);
}

void Unified_Buffer::read_vector_from_HM(int step, int max_step, int matrix_size, int read_addr, int hm_read_addr)
{
	for (int i = 0; i < matrix_size; i++)
	{
		mem_block[read_addr + i][step] = hm->mem_block[hm_read_addr + i][step];
	}
}

void Unified_Buffer::write_vector_to_HM_when_enable()
{
	assert(hm != NULL);

	write_vector_counter.count(matrix_size_in, matrix_size_in, addr, hm_addr);
}

void Unified_Buffer::write_vector_to_HM(int step, int max_step, int matrix_size, int src_addr, int hm_write_addr)
{
	for (int i = 0; i < matrix_size; i++)
	{
		hm->mem_block[hm_write_addr + i][step] = mem_block[src_addr + i][step];
	}
}