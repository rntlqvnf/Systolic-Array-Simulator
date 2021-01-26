#pragma once

#include <stdint.h>
#include <algorithm>
#include "config.h"
#include "matrix.h"

const int DIAG_WIDTH = MAT_WIDTH * 2;

class Systolic_Setup
{
private:
	int8_t diagonal_mat[MAT_HEIGHT][DIAG_WIDTH];
	int8_t datas_to_in[MAT_HEIGHT];
	int column_index;

public:
	Systolic_Setup() : datas_to_in{}
	{
		memset(diagonal_mat, 0, sizeof(int8_t) * MAT_HEIGHT * DIAG_WIDTH);
		column_index = DIAG_WIDTH - 1;
	}
	int8_t* get_datas_to_in()
	{
		return datas_to_in;
	}
	void tick();
	void set_mat_and_diagonalize(Matrix<int8_t>);
};

