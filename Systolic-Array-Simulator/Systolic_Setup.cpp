#include "Systolic_Setup.h"

void Systolic_Setup::tick()
{
	if (column_index < 0) 
	{
		for (int i = 0; i < MAT_HEIGHT; i++)
		{
			datas_to_in[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < MAT_HEIGHT; i++)
		{
			datas_to_in[i] = diagonal_mat[i][column_index];
		}
		column_index--;
	}
}

void Systolic_Setup::set_mat_and_diagonalize(int8_t** mat, int width, int height)
{
	for (int i = 0; i < height; i++)
	{
		std::fill(diagonal_mat[i], diagonal_mat[i] + DIAG_WIDTH, 0);
		for (int j = 0; j < width; j++)
		{
			diagonal_mat[i][DIAG_WIDTH - j - 1] = mat[i][j];
		}
	}
}