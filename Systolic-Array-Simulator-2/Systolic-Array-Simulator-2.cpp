
#include <iostream>
#include <fstream> 
#include "MMU.h"
#include "Unified_Buffer.h"
#include "Accumulator.h"
#include "Activation.h"
#include "Decoder.h"

using namespace std;

const int MAT_SIZE = 8;

void open_file_and_verify(string name, ifstream& stream);
void read_csv_and_save_to_mem(ifstream& file, Memory& mem);
void print_mem_block(string name, int8_t** mem);

int main()
{
    ifstream instruction_file;
    ifstream weight_file;
    ifstream data_file;

    open_file_and_verify("Instruction", instruction_file);
    open_file_and_verify("Weight", weight_file);
    open_file_and_verify("Data", data_file);

    Memory hm(MAT_SIZE, 50);
    Memory dram(MAT_SIZE, 50);
    Unified_Buffer ub(MAT_SIZE, 10);
    Systolic_Setup ss(MAT_SIZE);
    Weight_FIFO wf(MAT_SIZE);
    MMU mmu(MAT_SIZE);
    Accumulator acc(MAT_SIZE, 30);
    Activation act(MAT_SIZE);

    ub.hm = &hm;
    wf.dram = &dram;
    ss.ub = &ub;
    mmu.ss = &ss;
    mmu.wf = &wf;
    acc.mmu = &mmu;
    act.acc = &acc;

    cout << "[Weight]" << endl;
    read_csv_and_save_to_mem(weight_file, dram);
    cout << "[Data]" << endl;
    read_csv_and_save_to_mem(data_file, hm);

    int cycle = 0;
    string buf;
    string delimiter(" ");
    Decoder decoder;
    while (instruction_file.peek() != EOF)
    {
        getline(instruction_file, buf);
        decoder.parse(buf, delimiter);

        ub.read_en = decoder.controls["ub.read_en"];
        ub.write_en = decoder.controls["ub.write_en"];
        ss.read_en = decoder.controls["ss.read_en"];
        ss.push_en = decoder.controls["ss.push_en"];
        ss.switch_en = decoder.controls["ss.switch_en"];
        wf.push_en = decoder.controls["wf.push_en"];
        wf.read_en = decoder.controls["wf.read_en"];
        act.act_en = decoder.controls["act.act_en"];

        ub.addr = decoder.values["ub.addr"];
        ub.hm_addr = decoder.values["ub.hm_addr"];
        ub.matrix_size_in = decoder.values["ub.matrix_size_in"];
        ss.ub_addr = decoder.values["ss.ub_addr"];
        ss.acc_addr_in = decoder.values["ss.acc_addr_in"];
        ss.matrix_size_in = decoder.values["ss.matrix_size_in"];
        wf.dram_addr = decoder.values["wf.dram_addr"];
        act.matrix_size_in = decoder.values["act.matrix_size_in"];
        act.acc_addr = decoder.values["act.acc_addr"];
        act.ub_addr = decoder.values["act.ub_addr"];

        if (decoder.controls["halt"]) exit(0);

        //Register update
        ub.read_vector_from_HM_when_enable();
        ub.write_vector_to_HM_when_enable();

        ss.read_vector_from_UB_when_enable();
        ss.push_vectors_to_MMU_when_enable();

        wf.push_weight_vector_to_MMU_when_en();
        wf.read_matrix_from_DRAM_when_en();

        acc.write_results();

        act.do_activation_and_write_to_UB();

        //Combination Logic
        mmu.setup_array();
        mmu.calculate();

        cycle++;
        cout << "[[Cycle " << cycle << "]]" << endl;
        print_mem_block("Unified Buffer", ub.mem_block);
    }
}

void print_mem_block(string name, int8_t** mem_block)
{
    cout << "[" << name << "]" << endl;
    for (int i = 0; i < MAT_SIZE; i++)
    {
        for (int j = 0; j < MAT_SIZE; j++)
        {
            cout << (int)mem_block[i][j] << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

void open_file_and_verify(string name, ifstream& file)
{
    string input_file_name;
    cout << name << " 파일 이름을 입력하세요 : ";
    cin >> input_file_name;

    file.open(input_file_name);

    if (!file.is_open())
    {
        cout << "[ERROR] 없는 파일입니다." << endl;
        exit(-1);
    }
}

void read_csv_and_save_to_mem(ifstream& file, Memory& mem)
{
    string buf;
    for (int i = 0; i < MAT_SIZE; i++)
    {
        for (int j = 0; j < MAT_SIZE; j++)
        {
            if (file.peek() == EOF)
            {
                mem.mem_block[i][j] = 0;
            }
            else
            {
                getline(file, buf, ',');
                mem.mem_block[i][j] = atoi(buf.c_str());
            }
        }
    }

    for (int i = 0; i < MAT_SIZE; i++)
    {
        for (int j = 0; j < MAT_SIZE; j++)
        {
            cout << (int)mem.mem_block[i][j] << ", ";
        }
        cout << endl;
    }
    cout << endl;

    file.close();
}
