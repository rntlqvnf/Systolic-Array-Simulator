#include "Unified_Buffer.h"

void Unified_Buffer::read_vector_from_HM_when_enable()
{
	assert(hm != NULL);
	assert(mmu_size >= matrix_size);

	UB_Inputs inputs = { matrix_size ,addr, hm_addr };
	read_vector_counter.count(mmu_size, inputs);
}

void Unified_Buffer::read_vector_from_HM(int step, int max_step, UB_Inputs data)
{
	for (int i = 0; i < data.matrix_size; i++)
	{
		mem_block[data.addr + i][step] = hm->mem_block[data.hm_addr + i][step];
	}
}

void Unified_Buffer::write_vector_to_HM_when_enable()
{
	assert(hm != NULL);
	assert(mmu_size >= matrix_size);

	UB_Inputs inputs = { matrix_size ,addr, hm_addr };
	write_vector_counter.count(matrix_size, inputs);
}

void Unified_Buffer::write_vector_to_HM(int step, int max_step, UB_Inputs data)
{
	for (int i = 0; i < data.matrix_size; i++)
	{
		hm->mem_block[data.hm_addr + i][step] = mem_block[data.addr + i][step];
	}
}