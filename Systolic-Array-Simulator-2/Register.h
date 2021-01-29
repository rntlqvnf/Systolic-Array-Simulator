#pragma once

#include <stdint.h>

template <typename T>
class Register
{
public:
	T next;
	T value;
	bool write_enable;

	void tick()
	{
		if (write_enable)
			value = next;
	}
};

