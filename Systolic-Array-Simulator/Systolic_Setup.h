#pragma once

#include <stdint.h>
#include "config.h"

class Systolic_Setup
{
public:
	void tick();
private:
	int8_t datas_to_in[MAT_HEIGHT];
};

