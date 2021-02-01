#include "pch.h"
#include "../Systolic-Array-Simulator-2/MAC.h"
#include "../Systolic-Array-Simulator-2/MAC.cpp"
#include "../Systolic-Array-Simulator-2/Systolic_Setup.h"
#include "../Systolic-Array-Simulator-2/Systolic_Setup.cpp"
#include "../Systolic-Array-Simulator-2/MMU.h"
#include "../Systolic-Array-Simulator-2/MMU.cpp"
#include "../Systolic-Array-Simulator-2/Weight_FIFO.h"
#include "../Systolic-Array-Simulator-2/Weight_FIFO.cpp"

void allocate_array(int8_t** &mat, int matrix_size)
{
	mat = new int8_t * [matrix_size];
	for (int i = 0; i < matrix_size; i++)
		mat[i] = new int8_t[matrix_size];
}

void allocate_array(int8_t** &mat, int matrix_size, int8_t* copy)
{
	mat = new int8_t * [matrix_size];
	for (int i = 0; i < matrix_size; i++)
		mat[i] = new int8_t[matrix_size];

	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0; j < matrix_size; j++)
		{
			mat[i][j] = copy[i * matrix_size + j];
		}
	}
}

namespace UnitTest
{
	TEST(MACTest, DataThroughTest) {
		MAC mac;
		mac.data_in = (int8_t)2;
		mac.weight_in = (int8_t)4;
		mac.weight_tag_in = (int8_t)10;
		mac.write_en_in = true;
		mac.calculate();

		EXPECT_EQ(2, mac.data_out) << "Data should pass through";
		EXPECT_EQ(4, mac.weight_out) << "Weight should pass through";
		EXPECT_EQ(11, mac.weight_tag_out) << "Weight tag should +1";
		EXPECT_TRUE(mac.write_en_out) << "Write flag should propagate";
	}
	TEST(MACTest, WithoutSum) {
		MAC mac;
		mac.data_in = (int8_t) 1;
		mac.weight_buf[0] = (int8_t)2;
		mac.calculate();

		EXPECT_EQ(2, mac.sum_out) << "Sum in should be zero";
	}
	TEST(MACTest, WithSum) {
		MAC mac;
		mac.data_in = (int8_t)1;
		mac.weight_buf[0] = (int8_t)2;
		mac.sum_in = (int32_t)10;
		mac.calculate();

		EXPECT_EQ(12, mac.sum_out) << "Sum in should effect";
	}
	TEST(MACTest, WeightSwitch) {
		MAC mac;
		mac.matrix_size = 16;
		mac.weight_in = (int8_t)10;
		mac.write_en_in = true;
		mac.weight_tag_in = 15;
		mac.calculate();

		EXPECT_EQ(10, mac.weight_buf[1 - mac.current_weight]) << "Weight buf should write when tag matches";
	}
	TEST(MACTest, SwitchAndCalculate) {
		MAC mac;
		mac.matrix_size = 16;
		mac.weight_in = (int8_t)10;
		mac.write_en_in = true;
		mac.weight_tag_in = 15;
		mac.calculate();

		mac.switch_in = true;
		mac.data_in = 10;
		mac.calculate();

		EXPECT_EQ(100, mac.sum_out) << "Calculation should be done by active weight buf";
	}

	TEST(SystolicSetupTest, OneVectorDiagonalizationTest) {
		int matrix_size = 4;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 1);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[0][i] = i + 1;

		ss.write_en = true;
		ss.program();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(ub.mem_block[0][i], ss.diagonalized_matrix[i][i]) << "Diagonalization failed in " << i;
		}
	}

	TEST(SystolicSetupTest, TwoVectorDiagonalizationTest) {
		int matrix_size = 4;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[0][i] = i + 1;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[1][i] = i + 2;

		ss.write_en = true;
		ss.program();
		ss.program();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(ub.mem_block[0][i], ss.diagonalized_matrix[i][i]) << "Diagonalization 1 failed in " << i;
			EXPECT_EQ(ub.mem_block[1][i], ss.diagonalized_matrix[i][i+1]) << "Diagonalization 2 failed in " << i;
		}
	}

	TEST(SystolicSetupTest, AdvanceTest) {
		int matrix_size = 2;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[0][i] = i + 1;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[1][i] = i + 2;

		int8_t answer[2][3] =
		{
			{1,2,0},
			{0,2,3}
		};

		ss.write_en = true;
		ss.program();
		ss.program();

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < 2*matrix_size - 1; j++)
			{
				EXPECT_EQ(answer[i][j], ss.diagonalized_matrix[i][j]) << "Diagonalization 1 failed in " << i;
			}
		}

		ss.write_en = false;
		ss.advance_en = true;
		ss.switch_en = true;
		for (int i = 0; i < 2*matrix_size-1; i++)
		{
			ss.advance();
			EXPECT_EQ(answer[0][i], ss.input_datas[0]) << "Advancing 1 failed in " << i;
			EXPECT_EQ(answer[1][i], ss.input_datas[1]) << "Advancing 2 failed in " << i;
			if (i < matrix_size)
			{
				for (int j = 0; j < matrix_size; j++)
				{
					if(i == j)
						EXPECT_TRUE(ss.switch_weights[j]) << "Switch " << j << " shoud be true";
					else
						EXPECT_FALSE(ss.switch_weights[j]) << "Switch " << j << " shoud be false";
				}
			}
			else
			{
				for (int j = 0; j < matrix_size; j++)
				{
					EXPECT_FALSE(ss.switch_weights[j]) << "Over mat_size, switch " << j << "shoud be false";
				}
			}
		}
	}

	TEST(SystolicSetupTest, AdvanceStopTest) {
		int matrix_size = 2;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[0][i] = i + 1;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[1][i] = i + 2;

		ss.write_en = true;
		ss.program();
		ss.program();

		ss.write_en = false;
		ss.advance_en = true;
		for (int i = 0; i < 2 * matrix_size - 1; i++)
		{
			ss.advance();
		}
		EXPECT_FALSE(ss.advancing);
	}

	TEST(WeightFIFOTest, PushPopTest) {
		int matrix_size = 2;
		Weight_FIFO wf(matrix_size);

		int8_t copy[2][2]= 
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);

		wf.push(mat);

		ASSERT_FALSE(wf.weight_queue.empty());

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(copy[i][j], wf.weight_queue.front()[i][j]) << "FIFO front failed: " << i << "," << j;
			}
		}

		wf.pop();
		EXPECT_TRUE(wf.weight_queue.empty());
	}

	TEST(WeightFIFOTest, AdvanceTest) {
		int matrix_size = 2;
		Weight_FIFO wf(matrix_size);

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);

		wf.push(mat);

		wf.advance_en = true;
		for (int i = 0; i < matrix_size; i++)
		{
			wf.advance();
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(copy[j][matrix_size - i - 1], wf.input_weights[j]);
			}
		}

		EXPECT_TRUE(wf.weight_queue.empty());
	}

	TEST(MMUTest, SetupTest) {
		int matrix_size = 2;
		MMU mmu(matrix_size);
		Unified_Buffer ub(matrix_size, 2);
		Systolic_Setup ss(matrix_size);
		Weight_FIFO wf(matrix_size);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[0][i] = i + 1;
			ub.mem_block[1][i] = i + 2;
		}

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);
		wf.push(mat);

		//cycle 1
		ss.write_en = true;
		ss.program();

		//cycle2
		ss.program();

		//cycle3
		ss.advance_en = true;
		ss.switch_en = true;
		wf.advance_en = true;
		ss.advance(); //input datas and switchs
		wf.advance(); //input weights
		mmu.setup_array();
		
		//input datas
		//input weights
		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(ss.input_datas[i], mmu.mac_array[0][i].data_in) << "Input data failed in " << i;
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[0][i].weight_in) << "Input weight failed in " << i;
		}
	}

	TEST(MMUTest, WeightWriteTest) {
		int matrix_size = 2;
		MMU mmu(matrix_size);
		Unified_Buffer ub(matrix_size, 2);
		Systolic_Setup ss(matrix_size);
		Weight_FIFO wf(matrix_size);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[0][i] = i + 1;
			ub.mem_block[1][i] = i + 2;
		}

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);
		wf.push(mat);

		//cycle 1
		ss.write_en = true;
		ss.program();

		//cycle2
		ss.program();

		//cycle3
		ss.advance_en = true;
		ss.switch_en = true;
		wf.advance_en = true;
		mmu.write_en = true;
		ss.advance(); //input datas and switchs
		wf.advance(); //input weights
		mmu.setup_array();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[0][i].weight_in) << "Input weight failed in " << i;
			EXPECT_TRUE(mmu.mac_array[0][i].write_en_in) << "Write en failed in " << i;
			EXPECT_EQ(0, mmu.mac_array[0][i].weight_tag_in) << "Weight tag failed in " << i;
		}

		mmu.calculate();
		mmu.setup_array();
		mmu.calculate();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[1][i].weight_in) << "Input weight failed in " << i;
			EXPECT_TRUE(mmu.mac_array[0][i].write_en_in) << "Write en failed in " << i;
			EXPECT_EQ(1, mmu.mac_array[0][i].weight_tag_in) << "Weight tag failed in " << i;
			EXPECT_EQ(1, mmu.mac_array[1][i].weight_tag_in) << "Weight tag failed in " << i;
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[1][i].weight_buf[1 - mmu.mac_array[1][i].current_weight]) << "Weight write failed in " << i;
		}
	}

	TEST(MMUTest, OneMacTest) {
		int matrix_size = 1;
		MMU mmu(matrix_size);
		Unified_Buffer ub(matrix_size, 1);
		Systolic_Setup ss(matrix_size);
		Weight_FIFO wf(matrix_size);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		for (int i = 0; i < matrix_size; i++)
		{
			// matrix
			// 1
			ub.mem_block[0][i] = i + 1;
		}

		int8_t copy[1][1] =
		{
			{2},
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);
		wf.push(mat);

		for (int i = 0; i < 3; i++)
		{
			//Control setting
			switch (i)
			{
			case 0:
				//Systolic setup program
				ss.write_en = true;
				ss.advance_en = false;
				ss.switch_en = false;
				wf.advance_en = false;
				mmu.write_en = false;
				break;
			case 1:
				//Weight FIFO advance
				ss.write_en = false;
				ss.advance_en = false;
				ss.switch_en = false;
				wf.advance_en = true;
				mmu.write_en = true;
				break;
			case 2:
				//Systolic Setup advance
				ss.write_en = false;
				ss.advance_en = true;
				ss.switch_en = true;
				wf.advance_en = false;
				mmu.write_en = false;
				break;
			}

			//Register update
			ss.program();
			ss.advance();
			wf.advance();
			mmu.setup_array();

			//Combination Logic
			mmu.calculate();

			switch (i)
			{
			case 0:
			case 1:
			{
				int answer[1] = { 0 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 2:
			{
				int answer[1] = { 2 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			}
		}
	}
	TEST(MMUTest, TwoMacCalTest) {
		int matrix_size = 2;
		MMU mmu(matrix_size);
		Unified_Buffer ub(matrix_size, 2);
		Systolic_Setup ss(matrix_size);
		Weight_FIFO wf(matrix_size);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		for (int i = 0; i < matrix_size; i++)
		{
			// matrix
			// 1 2
			// 2 3 
			ub.mem_block[0][i] = i + 1; 
			ub.mem_block[1][i] = i + 2;
		}

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);
		wf.push(mat);

		for (int i = 0; i < 7; i++)
		{
			//Control setting
			switch (i)
			{
			case 0:
				//Systolic setup program
				ss.write_en = true;
				ss.advance_en = false;
				ss.switch_en = false;
				wf.advance_en = false;
				mmu.write_en = false;
				break;
			case 1:
				//Weight FIFO advance
				ss.write_en = false;
				ss.advance_en = false;
				ss.switch_en = false;
				wf.advance_en = true;
				mmu.write_en = true;
				break;
			case 2:
			case 4:
			case 5:
			case 6:
				//NOP
				ss.write_en = false;
				ss.advance_en = false;
				ss.switch_en = false;
				wf.advance_en = false;
				mmu.write_en = false;
				break;
			case 3:
				//Systolic Setup advance
				ss.write_en = false;
				ss.advance_en = true;
				ss.switch_en = true;
				wf.advance_en = false;
				mmu.write_en = false;
				break;
			}

			//Register update
			ss.program();
			ss.advance();
			wf.advance();
			mmu.setup_array();

			//Combination Logic
			mmu.calculate();

			switch (i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			{
				int answer[2] = { 0,0 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 4:
			{
				int answer[2] = { 5,0 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 5:
			{
				int answer[2] = { 8,13 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 6:
			{
				int answer[2] = { 0,22 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			}
		}

	}
}