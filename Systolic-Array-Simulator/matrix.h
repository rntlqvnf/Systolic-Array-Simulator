#pragma once

template <typename T>
class Matrix
{
private:
	T** matrix;
	int width;
	int height;

public:
	Matrix(T** _matrix, int _width, int _height)
	{
		matrix = _matrix;
		width = _width;
		height = _height;
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