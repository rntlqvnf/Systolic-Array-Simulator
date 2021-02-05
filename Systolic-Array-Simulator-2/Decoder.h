#pragma once

#include <string>
#include <vector>
#include <map>
#include <assert.h>

//Pass line from outer
//Parse and generate control signals

using namespace std;

class Decoder
{
public:
	map<string, bool> controls;
	map<string, int> values;

	Decoder()
	{
		reset();
	}

	void parse(const string& str, string& delimiter);
	void set_control_value(vector<string>& parsed_inst);

private:
	void reset();
};
