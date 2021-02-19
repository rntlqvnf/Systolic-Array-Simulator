#pragma once

class Weight_Size_Reg
{
private:
	int current_index;
public:
	int size[2];

	Weight_Size_Reg()
	{
		current_index = 0;
		size[0] = 0;
		size[1] = 0;
	}

	int get_size()
	{
		return size[current_index];
	}

	int get_buffer_size()
	{
		return size[1 - current_index];
	}

	void set_size(int new_size)
	{
		size[1 - current_index] = new_size;
	}

	void switch_size()
	{
		current_index = 1 - current_index;
	}
};

