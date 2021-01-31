#pragma once
#include <stdint.h>
#include <algorithm>
#include "config.h"

#include "Unified_Buffer.h"

class Systolic_Setup
{
private:
	void reset_internal_matrix();
	void program_input_vector();
	void advance_switchs();
	void advance_internal_vector();

public:
	//setting
	int matrix_size;
	int diag_width;

	//input
	bool write_en;
	bool switch_en;
	bool adv_en;

	//output
	int8_t* input_data;

	//internal
	int8_t** diagonalized_matrix;
	bool* switch_weights;
	int count;
	int adv_count;
	bool programming;
	bool advancing;

	//other HW
	Unified_Buffer *ub;

	Systolic_Setup(int _matrix_size)
	{
		matrix_size = _matrix_size;
		diag_width = matrix_size + matrix_size - 1;

		write_en = false;
		switch_en = false;
		adv_en = false;

		input_data = new int8_t[matrix_size];

		diagonalized_matrix = new int8_t * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			diagonalized_matrix[i] = new int8_t[diag_width];

		switch_weights = new bool[matrix_size];

		count = 0;
		adv_count = 0;
		programming = false;
		advancing = false;

		ub = NULL;
	}

	~Systolic_Setup()
	{
		for (int i = 0; i < matrix_size; ++i)
			delete[] diagonalized_matrix[i];
		delete[] diagonalized_matrix;
	}

	void program();
	void advance();
};

