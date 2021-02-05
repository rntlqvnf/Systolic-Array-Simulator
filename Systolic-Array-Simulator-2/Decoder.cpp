#include "Decoder.h"

void Decoder::parse(const string& str, string& delimiter)
{
	string::size_type Fpos = str.find_first_not_of(delimiter, 0);
	string::size_type Lpos = str.find_first_of(delimiter, Fpos);
	vector<string> parsed_inst;

	while (string::npos != Fpos || string::npos != Lpos)
	{
		parsed_inst.push_back(str.substr(Fpos, Lpos - Fpos));
		Fpos = str.find_first_not_of(delimiter, Lpos);
		Lpos = str.find_first_of(delimiter, Fpos);
	}

	set_control_value(parsed_inst);
}

void Decoder::set_control_value(vector<string>& parsed_inst)
{

	string opcode = parsed_inst[0];
	reset();
	if (opcode == "RHM")
	{
		//RHM src dst N
		assert(parsed_inst.size() == 3);

		values["ub.read_en"] = true;

		values["ub.hm_addr"] = atoi(parsed_inst[1].c_str());
		values["ub.addr"] = atoi(parsed_inst[2].c_str());
		values["matrix_size"] = atoi(parsed_inst[3].c_str());
	}
	else if (opcode == "WUB")
	{
		//WUB src N
		assert(parsed_inst.size() == 3);

		controls["ub.write_en"] = true;

		values["ub.addr"] = atoi(parsed_inst[1].c_str());
		values["matrix_size"] = atoi(parsed_inst[2].c_str());
	}
	else if (opcode == "RW")
	{
		//RW
		assert(parsed_inst.size() == 1);

		controls["wf.advance_en"] = true;
		controls["mmu.write_en"] = true;
	}
	else if (opcode == "MMC")
	{
		//MMC src dst N
		assert(parsed_inst.size() == 4);

		values["ub.addr"] = atoi(parsed_inst[2].c_str());
		values["matrix_size"] = atoi(parsed_inst[3].c_str());
	}
	else if (opcode == "ACT")
	{
		//ACT src dst N
		assert(parsed_inst.size() == 4);
	}
	else if (opcode == "NOP")
	{
		//NOP
		assert(parsed_inst.size() == 1);
	}
	else if (opcode == "HLT")
	{
		assert(parsed_inst.size() == 1);
		controls["halt"] = true;
	}
	else
	{
		//Incorrect ISA, halts
		controls["halt"] = true;
	}
}


void Decoder::reset() {
	controls["ss.write_en"] = false;
	controls["ss.advance_en"] = false;
	controls["ss.switch_en"] = false;
	controls["wf.advance_en"] = false;
	controls["mmu.write_en"] = false;
	controls["act.act_en"] = false;
	controls["ub.read_en"] = false;
	controls["ub.write_en"] = false;
	controls["halt"] = false;

	values["ub.hm_addr"] = 0;
	values["ub.addr"] = 0;
	values["ss.acc_addr"] = 0;
	values["act.addr"] = 0;
	values["matrix_size"] = 0;
}
