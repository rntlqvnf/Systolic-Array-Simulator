#pragma once

#include "MAC.h"
#include "config.h"
#include "Systolic_Setup.h"

class MMU
{
private:
	void program_input_data();
	void program_input_switch();
	void program_input_write_things();
	void advance_write();
	void program_output_to_input();
public:
	//setting
	int matrix_size;

	//input
	bool write_en;

	//internal
	MAC** mac_array;
	int progressing;
	int prog_step;

	//other HW
	Systolic_Setup* ss;

	MMU(int _matrix_size = MAT_SIZE)
	{
		matrix_size = _matrix_size;

		write_en = false;

		mac_array = new MAC * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			mac_array[i] = new MAC[matrix_size];

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				mac_array[i][j] = MAC(matrix_size);
			}
		}
		progressing = false;
		prog_step = 0;

		ss = NULL;
	}

	void program();
	void calculate();
};

