#pragma once

#include <functional>
#include <iostream>

using namespace std;

class Counter
{
private:
	bool* enable;

	function<void(void)> on_start;
	function<void(int, int, int, int)> on_count;
	function<void(void)> on_end;

	int max_count;
	int matrix_size;
	int addr;

	int count_step;
	bool counting;

	Counter();
public:
	Counter(bool* enable)
	{
		this->enable = enable;
		this->max_count = 0;
		this->on_start = NULL;
		this->on_count = NULL;
		this->on_end = NULL;

		matrix_size = 0;
		addr = 0;
		count_step = 0;
		counting = false;
	}

	void addHandlers(
		function<void(void)> on_start, 
		function<void(int, int, int, int)> on_count, 
		function<void(void)> on_end)
	{
		this->on_start = on_start;
		this->on_count = on_count;
		this->on_end = on_end;
	}

	void count(int _max_count, int _matrix_size, int _addr)
	{
		if (*enable && !counting)
		{
			max_count = _max_count;
			matrix_size = _matrix_size;
			addr = _addr;

			counting = true;
			count_step = 0;
			if(on_start != NULL)
				on_start();
		}

		if (counting)
		{
			if (on_count != NULL)
			{
				on_count(count_step, max_count, matrix_size, addr);
			}

			count_step++;
			if (count_step == max_count)
			{
				counting = false;
				count_step = 0;
				if(on_end != NULL)
					on_end();
			}
		}
	}
};

