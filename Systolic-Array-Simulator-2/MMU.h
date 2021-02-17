#pragma once

#include "MAC.h"
#include "Systolic_Setup.h"
#include "Weight_FIFO.h"
#include <iostream>

class MMU
{
private:
	//setting
	int mmu_size;

	void program_input_data();
	void program_input_switch();
	void program_input_write_things();
	void program_output_to_input();

public:
	//output
	int32_t* last_row_sum;

	//internal
	MAC** mac_array;
	int progressing;
	int prog_step;

	//other HW
	Systolic_Setup* ss;
	Weight_FIFO* wf;

	MMU(int _mmu_size)
	{
		mmu_size = _mmu_size;

		last_row_sum = new int32_t[mmu_size];

		mac_array = new MAC * [mmu_size];
		for (int i = 0; i < mmu_size; i++)
			mac_array[i] = new MAC[mmu_size];

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				mac_array[i][j] = MAC(mmu_size);
			}
		}
		progressing = false;
		prog_step = 0;

		ss = NULL;
		wf = NULL;
	}

	~MMU()
	{
		for (int i = 0; i < mmu_size; ++i)
			delete[] mac_array[i];
		delete[] mac_array;
	}

	void setup_array();
	void calculate();
};

