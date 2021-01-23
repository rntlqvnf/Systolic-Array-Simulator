#pragma once

#include <stdint.h>
#include <queue>
#include "config.h"

//4-entry 64KB FIFO queue

const int MAX_ENTRY = 4;

class Weight_FIFO
{
public:
private:
	std::queue<> buffer_queue;
};

