#pragma once

#include <functional>
#include <iostream>

using namespace std;

class Counter
{
private:
	bool* enable;

	function<void(void)> on_start;
	function<void(int, int, int)> on_count_without_addr;
	function<void(int, int, int, int)> on_count_with_one_addr;
	function<void(int, int, int, int, int)> on_count_with_two_addr;
	function<void(void)> on_end;

	int max_count;
	int matrix_size;
	int addr;
	int addr2;

	int count_step;
	bool counting;

	Counter();
public:
	Counter(bool* enable)
	{
		this->enable = enable;
		this->max_count = 0;
		this->on_start = NULL;
		this->on_count_without_addr = NULL;
		this->on_count_with_one_addr = NULL;
		this->on_count_with_two_addr = NULL;
		this->on_end = NULL;

		matrix_size = 0;
		addr = 0;
		count_step = 0;
		counting = false;
	}

	void addHandlers(
		function<void(void)> on_start, 
		function<void(int, int, int)> on_count_without_addr,
		function<void(int, int, int, int)> on_count_with_one_addr,
		function<void(int, int, int, int, int)> on_count_with_two_addr,
		function<void(void)> on_end)
	{
		this->on_start = on_start;
		this->on_count_without_addr = on_count_without_addr;
		this->on_count_with_one_addr = on_count_with_one_addr;
		this->on_count_with_two_addr = on_count_with_two_addr;
		this->on_end = on_end;
	}

	void count(int _max_count, int _matrix_size, int _addr, int _addr2)
	{
		if (*enable && !counting)
		{
			max_count = _max_count;
			matrix_size = _matrix_size;
			addr = _addr;
			addr2 = _addr2;

			counting = true;
			count_step = 0;
			if(on_start != NULL)
				on_start();
		}

		if (counting)
		{
			if (on_count_without_addr != NULL)
			{
				on_count_without_addr(count_step, max_count, matrix_size);
			}
			if (on_count_with_one_addr != NULL)
			{
				on_count_with_one_addr(count_step, max_count, matrix_size, addr);
			}
			if (on_count_with_two_addr != NULL)
			{
				on_count_with_two_addr(count_step, max_count, matrix_size, addr, addr2);
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

