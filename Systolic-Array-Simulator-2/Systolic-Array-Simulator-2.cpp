
#include <iostream>
#include "MMU.h"
#include "Unified_Buffer.h"
#include "Accumulator.h"
#include "Activation.h"

void allocate_array(int8_t**& mat, int matrix_size, int8_t* copy)
{
	mat = new int8_t * [matrix_size];
	for (int i = 0; i < matrix_size; i++)
		mat[i] = new int8_t[matrix_size];

	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0; j < matrix_size; j++)
		{
			mat[i][j] = copy[i * matrix_size + j];
		}
	}
}

int main()
{
	int matrix_size = 2;

	MMU mmu(matrix_size);
	Unified_Buffer ub(matrix_size, 2);
	Systolic_Setup ss(matrix_size);
	Weight_FIFO wf(matrix_size);
	Accumulator acc(matrix_size, 100);
	Activation act(matrix_size);

	ss.ub = &ub;
	mmu.ss = &ss;
	mmu.wf = &wf;
	acc.mmu = &mmu;
	act.acc = &acc;

	for (int i = 0; i < matrix_size; i++)
	{
		// matrix
		// 1 2
		// 2 3 
		ub.mem_block[0][i] = i + 1;
		ub.mem_block[1][i] = i + 2;
	}

	int8_t copy[2][2] =
	{
		{1,2},
		{5,4}
	};
	int8_t** mat = NULL;
	allocate_array(mat, matrix_size, copy[0]);
	wf.push(mat);

	int acc_addr = 0;

	for (int i = 0; i < 9; i++)
	{

		//Control setting
		switch (i)
		{
		case 0:
			//Systolic setup program
			ss.read_en = true;
			ss.advance_en = false;
			ss.switch_en = false;
			wf.advance_en = false;
			mmu.write_en = false;
			act.act_en = false;
			ub.write_en = false;

			break;
		case 1:
			//Weight FIFO advance
			ss.read_en = false;
			ss.advance_en = false;
			ss.switch_en = false;
			wf.advance_en = true;
			mmu.write_en = true;
			act.act_en = false;
			ub.write_en = false;

			break;
		case 2:
		case 4:
		case 5:
		case 6:
			//NOP
			ss.read_en = false;
			ss.advance_en = false;
			ss.switch_en = false;
			wf.advance_en = false;
			mmu.write_en = false;
			act.act_en = false;
			ub.write_en = false;

			break;
		case 3:
			//Systolic Setup advance
			ss.read_en = false;
			ss.advance_en = true;
			ss.switch_en = true;
			wf.advance_en = false;
			mmu.write_en = false;
			act.act_en = false;
			ub.write_en = false;

			//Setup accm addr
			ss.acc_addr_in = acc_addr;
			break;
		case 7:
			//Activation mat write from accm
			ss.read_en = false;
			ss.advance_en = false;
			ss.switch_en = false;
			wf.advance_en = false;
			mmu.write_en = false;
			act.act_en = true;
			ub.write_en = false;

			//Setup accm addr
			//addr + matrix height -1 (no data cycle)
			act.addr = acc_addr + matrix_size - 1;
			break;
		case 8:
			//Write to ub mem
			ss.read_en = false;
			ss.advance_en = false;
			ss.switch_en = false;
			wf.advance_en = false;
			mmu.write_en = false;
			act.act_en = false;
			ub.write_en = true;

			//Setup ub addr
			ub.addr = 0;
			break;
		}
		//Register update
		ss.read_vector_from_UB();
		ss.advance_vector_to_MMU();
		wf.advance();
		mmu.setup_array();
		act.program_mat_from_accm();

		//Combination Logic
		mmu.calculate();

		//Update accm
		acc.write_results();
		ub.write_from_accm(act.mat);
	}
}
