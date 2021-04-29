
#include <iostream>
#include <fstream> 
#include <sstream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "MMU.h"
#include "Unified_Buffer.h"
#include "Accumulator.h"
#include "Activation.h"
#include "Decoder.h"

using namespace std;
using namespace cv;

const int MAT_SIZE = 75;

void open_file_and_verify(string name, ifstream& stream);
void read_csv_and_save_to_mem(ifstream& file, Memory& mem);
void read_image_and_save_to_mem(Memory& mem);
void parse_by_comma(vector<string>&, stringstream&);
void print_mem_block(string name, int8_t** mem);
void print_mem_block(string name, int32_t** mem_block);

int main()
{
    ifstream instruction_file;
    ifstream weight_file;
    ifstream data_file;

    open_file_and_verify("Instruction", instruction_file);
    open_file_and_verify("Weight", weight_file);
    open_file_and_verify("Data", data_file);

    Memory hm(MAT_SIZE, 5 * MAT_SIZE);
    Memory dram(MAT_SIZE, 5 * MAT_SIZE);
    Weight_Size_Reg wsreg;
    Unified_Buffer ub(MAT_SIZE, 12 * MAT_SIZE);
    Systolic_Setup ss(MAT_SIZE);
    Weight_FIFO wf(MAT_SIZE);
    MMU mmu(MAT_SIZE);
    Accumulator acc(MAT_SIZE, 4096);
    Activation act(MAT_SIZE);

    ub.hm = &hm;
    wf.dram = &dram;
    wf.wsreg = &wsreg;
    ss.ub = &ub;
    ss.wsreg = &wsreg;
    mmu.ss = &ss;
    mmu.wf = &wf;
    acc.mmu = &mmu;
    act.acc = &acc;

    cout << "[Weight]" << endl;
    read_csv_and_save_to_mem(weight_file, dram);
    cout << "[Data]" << endl;
    read_image_and_save_to_mem(hm);

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
        ss.overwrite_en = decoder.controls["ss.overwrite_en"];
        ss.unfold_en = decoder.controls["ss.unfold_en"];
        wf.push_en = decoder.controls["wf.push_en"];
        wf.read_en = decoder.controls["wf.read_en"];
        wf.unfold_en = decoder.controls["wf.unfold_en"];
        act.act_en = decoder.controls["act.act_en"];
        act.fold_en = decoder.controls["act.fold_en"];
        act.pool_en = decoder.controls["act.pool_en"];

        ub.addr = decoder.values["ub.addr"];
        ub.hm_addr = decoder.values["ub.hm_addr"];
        ub.matrix_size = decoder.values["ub.matrix_size"];
        ss.ub_addr = decoder.values["ss.ub_addr"];
        ss.acc_addr_in = decoder.values["ss.acc_addr_in"];
        ss.matrix_size = decoder.values["ss.matrix_size"];
        wf.dram_addr = decoder.values["wf.dram_addr"];
        wf.matrix_size = decoder.values["wf.matrix_size"];
        act.matrix_size = decoder.values["act.matrix_size"];
        act.acc_addr = decoder.values["act.acc_addr"];
        act.ub_addr = decoder.values["act.ub_addr"];

        //임시
        ss.cdi_en = decoder.controls["ss.cdi_en"];
        ss.cdd_en = decoder.controls["ss.cdd_en"];
        ss.crop_en = decoder.controls["ss.crop_en"];
        ss.start = decoder.values["ss.start"];
        ss.end = decoder.values["ss.end"];
        ss.value = decoder.values["ss.value"];

        if (ss.cdd_en || ss.cdi_en || ss.crop_en)
        {
            ss.index++;
        }

        if (decoder.controls["halt"]) {
            cout << endl << "[[Result]]" << endl;
            //print_mem_block("Host Memory", hm.mem_block);

            imshow("Image augmented after " + ss.index, ss.copy);
            waitKey(0);
            exit(0);
        }

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
    }
}

void print_mem_block(string name, int32_t** mem_block)
{
    cout << "[" << name << "]" << endl;
    for (int i = 0; i < MAT_SIZE; i++)
    {
        for (int j = 0; j < MAT_SIZE; j++)
        {
            cout << mem_block[i][j] << ", ";
        }
        cout << endl;
    }
    cout << endl;
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
        if (file.peek() == EOF)
        {
            for (int j = 0; j < MAT_SIZE; j++)
            {
                mem.mem_block[i][j] = 0;
            }
        }
        else
        {
            getline(file, buf);
            stringstream ss(buf);
            vector<string> result;
            parse_by_comma(result, ss);
            for (int j = 0; j < result.size(); j++)
            {
                mem.mem_block[i][j] = atoi(result[j].c_str());
            }
            for (int j = result.size(); j < MAT_SIZE; j++)
            {
                mem.mem_block[i][j] = 0;
            }
        }
    }

    /*
    for (int i = 0; i < MAT_SIZE; i++)
    {
        for (int j = 0; j < MAT_SIZE; j++)
        {
            cout << (int)mem.mem_block[i][j] << ", ";
        }
        cout << endl;
    }
    cout << endl;
    */

    file.close();
}

void read_image_and_save_to_mem(Memory& mem)
{
    Mat image = imread("cat.jpeg");

    Vec3b buf;

    for (int i = 0; i < MAT_SIZE; i++)
    {
        for (int j = 0; j < MAT_SIZE; j++)
        {
            buf = image.at<Vec3b>(i, j);
            mem.mem_block[i][j] = buf[0];
            mem.mem_block[i + MAT_SIZE][j] = buf[1];
            mem.mem_block[i + 2 * MAT_SIZE][j] = buf[2];
        }
    }

    imshow("Original", image);
}

void parse_by_comma(vector<string>& vec, stringstream& ss)
{
    while (ss.good())
    {
        string substr;
        getline(ss, substr, ',');
        vec.push_back(substr);
    }
}