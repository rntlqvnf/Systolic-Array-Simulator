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
		assert(parsed_inst.size() == 4);

		controls["ub.read_en"] = true;

		values["ub.hm_addr"] = atoi(parsed_inst[1].c_str());
		values["ub.addr"] = atoi(parsed_inst[2].c_str());
		values["ub.matrix_size_in"] = atoi(parsed_inst[3].c_str());
	}
	else if (opcode == "WHM")
	{
		//WHM src dst N
		assert(parsed_inst.size() == 4);

		controls["ub.write_en"] = true;

		values["ub.addr"] = atoi(parsed_inst[1].c_str());
		values["ub.hm_addr"] = atoi(parsed_inst[2].c_str());
		values["ub.matrix_size_in"] = atoi(parsed_inst[3].c_str());
	}
	else if (opcode.find("RW") != string::npos)
	{
		//RW.{U} addr N
		//Unfold
		assert(parsed_inst.size() == 3);

		string option = opcode.length() > 3 ? opcode.substr(3) : "";

		//default
		controls["wf.read_en"] = true;
		if (option.find("U") != string::npos)
		{
			controls["wf.unfold_en"] = true;
		}
		
		values["wf.dram_addr"] = atoi(parsed_inst[1].c_str());
		values["wf.dram_addr"] = atoi(parsed_inst[2].c_str());
	}
	else if (opcode.find("MMC") != string::npos)
	{
		//Matrix Multiply / Convolution
		//MMC.{SOU} src dst N
		//Switch, Overwrite, Unfold

		assert(parsed_inst.size() == 4);

		string option = opcode.length() > 4 ? opcode.substr(4) : "";

		//default
		controls["ss.read_en"] = true;
		controls["ss.push_en"] = true;
		//S
		if (option.find("S") != string::npos)
		{
			controls["ss.switch_en"] = true;
			controls["wf.push_en"] = true;
		}
		//O
		if (option.find("O") != string::npos)
		{
			controls["ss.overwrite_en"] = true;
		}
		//C
		if (option.find("U") != string::npos)
		{

		}

		values["ss.ub_addr"] = atoi(parsed_inst[1].c_str());
		values["ss.acc_addr_in"] = atoi(parsed_inst[2].c_str());
		values["ss.matrix_size_in"] = atoi(parsed_inst[3].c_str());
	}
	else if (opcode == "ACT")
	{
		//ACT src dst N
		assert(parsed_inst.size() == 4);

		controls["act.act_en"] = true;

		values["act.acc_addr"] = atoi(parsed_inst[1].c_str());
		values["act.ub_addr"] = atoi(parsed_inst[2].c_str());
		values["act.matrix_size_in"] = atoi(parsed_inst[3].c_str());
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
	controls["ub.read_en"] = false;
	controls["ub.write_en"] = false;
	controls["ss.read_en"] = false;
	controls["ss.push_en"] = false;
	controls["ss.switch_en"] = false;
	controls["ss.overwrite_en"] = false;
	controls["wf.push_en"] = false;
	controls["wf.read_en"] = false;
	controls["wf.unfold_en"] = false;
	controls["act.act_en"] = false;
	controls["halt"] = false;

	values["ub.addr"] = 0;
	values["ub.hm_addr"] = 0;
	values["ub.matrix_size_in"] = 0;
	values["ss.ub_addr"] = 0;
	values["ss.acc_addr_in"] = 0;
	values["ss.matrix_size_in"] = 0;
	values["wf.dram_addr"] = 0;
	values["wf.matrix_size"] = 0;
	values["act.matrix_size_in"] = 0;
	values["act.acc_addr"] = 0;
	values["act.ub_addr"] = 0;
}
