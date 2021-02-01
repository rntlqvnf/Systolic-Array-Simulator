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
	void advance_outputs_to_accm();
public:
	//setting
	int matrix_size;
	int diag_width;

	//input
	bool write_en;
	bool switch_en;
	bool advance_en;
	int accm_addr_in;

	//output
	int8_t* input_datas;
	bool* switch_weights;
	bool accm_write_en;
	int accm_addr_out;

	//internal
	int8_t** diagonalized_matrix;
	int count;
	int advance_count;
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
		advance_en = false;
		accm_addr_in = 0;

		input_datas = new int8_t[matrix_size];
		std::fill(input_datas, input_datas + matrix_size, 0);
		accm_write_en = false;
		accm_addr_out = 0;

		diagonalized_matrix = new int8_t * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			diagonalized_matrix[i] = new int8_t[diag_width];

		switch_weights = new bool[matrix_size];
		std::fill(switch_weights, switch_weights + matrix_size, false);

		count = 0;
		advance_count = 0;
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

