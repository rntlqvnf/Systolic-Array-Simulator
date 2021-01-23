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

void Systolic_Setup::set_mat_and_diagonalize(Matrix<int8_t> mat)
{
	for (int i = 0; i < mat.get_height(); i++)
	{
		std::fill(diagonal_mat[i], diagonal_mat[i] + DIAG_WIDTH, 0);
		for (int j = 0; j < mat.get_width(); j++)
		{
			diagonal_mat[i][DIAG_WIDTH - j - 1] = mat.get_matrix()[i][j];
		}
	}
}