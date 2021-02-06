#pragma once

#include <functional>

using namespace std;

class Counter
{
private:
	bool* enable;
	int max_count;
	function<void(void)> on_start;
	function<void(int, int)> on_count;
	function<void(void)> on_end;

	int count_step;
	bool counting;

	Counter();
public:
	Counter(bool* enable, int max_count)
	{
		this->enable = enable;
		this->max_count = max_count;
		this->on_start = NULL;
		this->on_count = NULL;
		this->on_end = NULL;

		count_step = 0;
		counting = false;
	}

	void addHandlers(function<void(void)> on_start, function<void(int, int)> on_count, function<void(void)> on_end)
	{
		this->on_start = on_start;
		this->on_count = on_count;
		this->on_end = on_end;
	}

	void count()
	{
		if (*enable && !counting)
		{
			counting = true;
			count_step = 0;

			if(on_start != NULL)
				on_start();
		}

		if (counting)
		{
			if (on_count != NULL)
				on_count(count_step, max_count);

			count_step++;
			if (count_step == max_count)
			{
				count_step = 0;
				counting = false;
				if(on_end != NULL)
					on_end();
			}
		}
	}
};

