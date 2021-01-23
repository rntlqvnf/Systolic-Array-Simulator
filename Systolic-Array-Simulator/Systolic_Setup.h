#pragma once

#include <stdint.h>
#include <algorithm>
#include "config.h"

const int DIAG_WIDTH = MAT_WIDTH * 2;

class Systolic_Setup
{
public:
	void tick();
	void set_mat_and_diagonalize(int8_t**, int width, int height);

	Systolic_Setup()
	{
		column_index = DIAG_WIDTH - 1;
	}
private:
	int8_t diagonal_mat[MAT_HEIGHT][DIAG_WIDTH];
	int8_t datas_to_in[MAT_HEIGHT];
	int column_index;
};

