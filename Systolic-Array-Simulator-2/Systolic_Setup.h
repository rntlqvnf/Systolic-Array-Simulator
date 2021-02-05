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
	bool read_en;
	bool switch_en;
	bool advance_en;
	int ub_addr;
	int acc_addr_in;

	//output
	int8_t* input_datas;
	bool* switch_weights;
	bool acc_write_en;
	int acc_addr_out;

	//internal
	int8_t** diagonalized_matrix;
	int ub_addr_reg;
	int read_count;
	int advance_count;
	bool reading;
	bool advancing;

	//other HW
	Unified_Buffer *ub;

	Systolic_Setup(int _matrix_size)
	{
		matrix_size = _matrix_size;
		diag_width = matrix_size + matrix_size - 1;

		read_en = false;
		switch_en = false;
		advance_en = false;
		ub_addr = 0;
		acc_addr_in = 0;

		input_datas = new int8_t[matrix_size];
		std::fill(input_datas, input_datas + matrix_size, 0);
		acc_write_en = false;
		acc_addr_out = 0;

		diagonalized_matrix = new int8_t * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			diagonalized_matrix[i] = new int8_t[diag_width];

		switch_weights = new bool[matrix_size];
		std::fill(switch_weights, switch_weights + matrix_size, false);

		ub_addr_reg = 0;
		read_count = 0;
		advance_count = 0;
		reading = false;
		advancing = false;

		ub = NULL;
	}

	~Systolic_Setup()
	{
		for (int i = 0; i < matrix_size; ++i)
			delete[] diagonalized_matrix[i];
		delete[] diagonalized_matrix;
	}

	void read_vector_from_UB();
	void advance_vector_to_MMU();
};

