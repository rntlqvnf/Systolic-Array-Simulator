#pragma once

#include <stdint.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "Decoder.h"
#include "Unified_Buffer.h"
#include "Weight_Size_Reg.h"

#define DIAG_WIDTH_1(x) ((2 * x) - 1)
#define DIAG_WIDTH_2(x, y) ((x + y) - 1)

using namespace cv;

struct Aug_Inputs 
{
	Mode mode;
	int val_1;
	int val_2;
	int val_3;
};

struct SS_Inputs
{
	int matrix_size;
	int ub_addr;
	int acc_addr;
	bool switch_en;
	bool overwrite_en;
	bool unfold_en;

	Aug_Inputs aug_inputs;
};

class Systolic_Setup
{
private:
	int mmu_size;

	Counter<SS_Inputs> push_vector_counter;
	Counter<SS_Inputs> read_vector_counter;

	void reset_matrix_and_wsreg(SS_Inputs); //on start
	void read_vector_from_UB(int, int, SS_Inputs); //on count

	void reset_switch_vector(SS_Inputs); //on start
	void push_data_and_switch_vector_to_MMU(int, int, SS_Inputs); //on count
	void advance_switch_vector(int, int, int, bool);
	void push_data_vector_to_MMU(int, int, int, Aug_Inputs);
	void reset_ouputs(); //on end

public:
	//input
	bool push_en;
	bool read_en;
	bool switch_en;
	bool overwrite_en;
	bool unfold_en;
	int ub_addr;
	int acc_addr_in;
	int matrix_size;
	Aug_Inputs aug_inputs;

	//output
	int8_t* input_datas;
	bool* switch_weights;
	bool acc_write_en;
	bool acc_overwrite_en;
	int acc_addr_out;

	//internal
	int8_t** diagonalized_matrix;
	int rnd_cut_x;
	int rnd_cut_y;
	int index = -1;
	Mat img = Mat::zeros(Size(150, 75), CV_8UC3);

	//other HW
	Unified_Buffer *ub;
	Weight_Size_Reg* wsreg;

	Systolic_Setup(int _mmu_size)
		:
		push_vector_counter(&push_en),
		read_vector_counter(&read_en)
	{
		mmu_size = _mmu_size;

		push_en = false;
		read_en = false;
		switch_en = false;
		overwrite_en = false;
		unfold_en = false;
		ub_addr = 0;
		acc_addr_in = 0;
		matrix_size = mmu_size;

		input_datas = new int8_t[mmu_size];
		std::fill(input_datas, input_datas + mmu_size, 0);
		acc_write_en = false;
		acc_addr_out = 0;

		diagonalized_matrix = new int8_t * [mmu_size];
		for (int i = 0; i < mmu_size; i++)
			diagonalized_matrix[i] = new int8_t[mmu_size * mmu_size]; //Max when convolution

		switch_weights = new bool[mmu_size];
		std::fill(switch_weights, switch_weights + mmu_size, false);

		ub = NULL;
		wsreg = NULL;

		read_vector_counter.addHandlers(
			bind(&Systolic_Setup::reset_matrix_and_wsreg, this, placeholders::_1),
			bind(&Systolic_Setup::read_vector_from_UB, this, placeholders::_1, placeholders::_2, placeholders::_3),
			NULL
		);

		push_vector_counter.addHandlers(
			bind(&Systolic_Setup::reset_switch_vector, this, placeholders::_1),
			bind(&Systolic_Setup::push_data_and_switch_vector_to_MMU, this, placeholders::_1, placeholders::_2, placeholders::_3),
			bind(&Systolic_Setup::reset_ouputs, this)
		);
	}

	~Systolic_Setup()
	{
		for (int i = 0; i < mmu_size; i++)
			delete[] diagonalized_matrix[i];
		delete[] diagonalized_matrix;
	}

	void read_vector_from_UB_when_enable();
	void push_vectors_to_MMU_when_enable();
};

