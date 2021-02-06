
#include <iostream>
#include "MMU.h"
#include "Unified_Buffer.h"
#include "Accumulator.h"
#include "Activation.h"

void allocate_array(int8_t**& mat, int matrix_size, int8_t* copy)
{
	mat = new int8_t * [matrix_size];
	for (int i = 0; i < matrix_size; i++)
		mat[i] = new int8_t[matrix_size];

	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0; j < matrix_size; j++)
		{
			mat[i][j] = copy[i * matrix_size + j];
		}
	}
}

int main()
{
}
