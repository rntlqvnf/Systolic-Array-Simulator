#pragma once
#include <stdint.h>
#include <algorithm>
#include "Unified_Buffer.h"

#define DIAG_WIDTH(x) ((2 * x) - 1)

class Systolic_Setup
{
private:
	int matrix_size;

	Counter push_vector_counter;
	Counter read_vector_counter;

	void reset_internal_matrix(); //on start
	void read_vector_from_UB(int, int, int, int); //on count

	void reset_switch_vector(); //on start
	void push_data_and_switch_vector_to_MMU(int, int, int, int, int); //on count
	void advance_switch_vector(int, int, int, int, int);
	void push_data_vector_to_MMU(int, int, int, int, int);
	void reset_acc_outs(); //on end

public:
	//input
	bool push_en;
	bool read_en;
	bool switch_en;
	int ub_addr;
	int acc_addr_in;
	int matrix_size_in;

	//output
	int8_t* input_datas;
	bool* switch_weights;
	bool acc_write_en;
	int acc_addr_out;

	//internal
	int8_t** diagonalized_matrix;

	//other HW
	Unified_Buffer *ub;

	Systolic_Setup(int _matrix_size)
		:
		push_vector_counter(&push_en),
		read_vector_counter(&read_en)
	{
		matrix_size = _matrix_size;

		push_en = false;
		read_en = false;
		switch_en = false;
		ub_addr = 0;
		acc_addr_in = 0;
		matrix_size_in = matrix_size;

		input_datas = new int8_t[matrix_size];
		std::fill(input_datas, input_datas + matrix_size, 0);
		acc_write_en = false;
		acc_addr_out = 0;

		diagonalized_matrix = new int8_t * [matrix_size];
		for (int i = 0; i < matrix_size; i++)
			diagonalized_matrix[i] = new int8_t[DIAG_WIDTH(matrix_size)];

		switch_weights = new bool[matrix_size];
		std::fill(switch_weights, switch_weights + matrix_size, false);

		ub = NULL;

		read_vector_counter.addHandlers(
			bind(&Systolic_Setup::reset_internal_matrix, this),
			NULL,
			NULL,
			bind(&Systolic_Setup::read_vector_from_UB, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4),
			NULL,
			NULL
		);

		push_vector_counter.addHandlers(
			bind(&Systolic_Setup::reset_switch_vector, this),
			NULL,
			NULL,
			NULL,
			bind(&Systolic_Setup::push_data_and_switch_vector_to_MMU, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5),
			bind(&Systolic_Setup::reset_acc_outs, this)
		);
	}

	~Systolic_Setup()
	{
		for (int i = 0; i < matrix_size; i++)
			delete[] diagonalized_matrix[i];
		delete[] diagonalized_matrix;
	}

	void read_vector_from_UB_when_enable();
	void push_vectors_to_MMU_when_enable();
};

