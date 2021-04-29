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
		values["ub.matrix_size"] = atoi(parsed_inst[3].c_str());
	}
	else if (opcode == "WHM")
	{
		//WHM src dst N
		assert(parsed_inst.size() == 4);

		controls["ub.write_en"] = true;

		values["ub.addr"] = atoi(parsed_inst[1].c_str());
		values["ub.hm_addr"] = atoi(parsed_inst[2].c_str());
		values["ub.matrix_size"] = atoi(parsed_inst[3].c_str());
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
		values["wf.matrix_size"] = atoi(parsed_inst[2].c_str());
	}
	else if (opcode.find("MMC") != string::npos)
	{
		//Matrix Multiply / Convolution
		//MMC.{SOU} src dst N
		//Switch, Overwrite, Unfold

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
			controls["ss.unfold_en"] = true;
		}

		//임시(augmentation)
		if (option.find(":") != string::npos)
		{
			string aug_option = option.substr(option.find(":"));
			if (aug_option == ":CDI")
			{
				controls["ss.cdi_en"] = true;
			}
			else if (aug_option == ":CDD")
			{
				controls["ss.cdd_en"] = true;
			}
			else if (aug_option == ":CZ")
			{
				controls["ss.crop_en"] = true;
			}
		}

		values["ss.ub_addr"] = atoi(parsed_inst[1].c_str());
		values["ss.acc_addr_in"] = atoi(parsed_inst[2].c_str());
		values["ss.matrix_size"] = atoi(parsed_inst[3].c_str());

		//임시
		values["ss.start"] = atoi(parsed_inst[4].c_str());
		values["ss.end"] = atoi(parsed_inst[5].c_str());
		values["ss.value"] = atoi(parsed_inst[6].c_str());
	}
	else if (opcode.find("ACT") != string::npos)
	{
		//ACT.{FP} src dst N {M}
		assert(parsed_inst.size() == 4 || parsed_inst.size() == 5);

		controls["act.act_en"] = true;

		string option = opcode.length() > 4 ? opcode.substr(4) : "";
		//F
		if (option.find("F") != string::npos)
		{
			controls["act.fold_en"] = true;
		}
		//P
		if (option.find("P") != string::npos)
		{
			controls["act.pool_en"] = true;
		}

		values["act.acc_addr"] = atoi(parsed_inst[1].c_str());
		values["act.ub_addr"] = atoi(parsed_inst[2].c_str());
		values["act.matrix_size"] = atoi(parsed_inst[3].c_str());
		if(parsed_inst.size() == 5) values["act.kernel_size"] = atoi(parsed_inst[4].c_str());
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
	controls["ss.unfold_en"] = false;
	controls["wf.push_en"] = false;
	controls["wf.read_en"] = false;
	controls["wf.unfold_en"] = false;
	controls["act.act_en"] = false;
	controls["act.fold_en"] = false;
	controls["act.pool_en"] = false;
	controls["halt"] = false;

	//임시
	controls["ss.cdi_en"] = false;
	controls["ss.cdd_en"] = false;
	controls["ss.crop_en"] = false;
	values["ss.start"] = 0;
	values["ss.end"] = 0;
	values["ss.value"] = 0;

	values["ub.addr"] = 0;
	values["ub.hm_addr"] = 0;
	values["ub.matrix_size"] = 0;
	values["ss.ub_addr"] = 0;
	values["ss.acc_addr_in"] = 0;
	values["ss.matrix_size"] = 0;
	values["wf.dram_addr"] = 0;
	values["wf.matrix_size"] = 0;
	values["act.matrix_size"] = 0;
	values["act.acc_addr"] = 0;
	values["act.ub_addr"] = 0;
}
