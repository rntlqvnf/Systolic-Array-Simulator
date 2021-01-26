#pragma once

#include <algorithm>

template <typename T>
class Matrix
{
private:
	T** matrix;
	int width;
	int height;

public:
	Matrix(int _height, int _width)
	{
		width = _width;
		height = _height;

		matrix = new T * [height];
		for (int i = 0; i < height; i++)
			matrix[i] = new T[width];
	}
	Matrix(int _height, int _width, T* _matrix)
	{
		width = _width;
		height = _height;

		matrix = new T*[height];
		for (int i = 0; i < height; i++)
			matrix[i] = new T[width];

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
				matrix[i][j] = _matrix[i*_width + j];
		}
	}
	T** get_matrix()
	{
		return matrix;
	}
	int get_width()
	{
		return width;
	}
	int get_height()
	{
		return height;
	}
};


namespace helper
{
	template <typename T>
	static Matrix<T> zero_padding_matrix(Matrix<T> mat, int new_height, int new_width)
	{
		Matrix<T> result(new_height, new_width);

		for (int i = 0; i < new_height; i++)
		{
			std::fill(result.get_matrix()[i], result.get_matrix()[i] + new_width, 0);
		}

		for (int i = 0; i < mat.get_height(); i++)
		{
			for (int j = 0; j < mat.get_width(); j++)
			{
				result.get_matrix()[i][j] = mat.get_matrix()[i][j];
			}
		}
		return result;
	}

	template <typename T>
	static Matrix<T> transpose_matrix(Matrix<T> mat)
	{
		Matrix<T> result(mat.get_width(), mat.get_height());
		for (int i = 0; i < mat.get_height(); i++)
		{
			for (int j = 0; j < mat.get_width(); j++)
			{
				result.get_matrix()[i][j] = mat.get_matrix()[j][i];
			}
		}
		return result;
	}
}