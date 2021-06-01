#pragma once

#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

//Pass line from outer
//Parse and generate control signals

using namespace std;

enum Mode
{
	CROP_WITH_RESIZE,
	FLIP_LEFT_RIGHT,
	TRANSLATION_RIGHT,
	COLOR_DISTORT,
	RND_COLOR_DISTORT,
	RND_CUTOUT
};

class Decoder
{
public:
	map<string, bool> controls;
	map<string, int> values;

	void parse(const string& str, string& delimiter);
	void set_control_value(vector<string>& parsed_inst);

private:
	void reset();
};

