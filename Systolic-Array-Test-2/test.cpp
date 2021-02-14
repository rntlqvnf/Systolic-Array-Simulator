#include "pch.h"
#include "../Systolic-Array-Simulator-2/MAC.h"
#include "../Systolic-Array-Simulator-2/MAC.cpp"
#include "../Systolic-Array-Simulator-2/Systolic_Setup.h"
#include "../Systolic-Array-Simulator-2/Systolic_Setup.cpp"
#include "../Systolic-Array-Simulator-2/MMU.h"
#include "../Systolic-Array-Simulator-2/MMU.cpp"
#include "../Systolic-Array-Simulator-2/Weight_FIFO.h"
#include "../Systolic-Array-Simulator-2/Weight_FIFO.cpp"
#include "../Systolic-Array-Simulator-2/Accumulator.h"
#include "../Systolic-Array-Simulator-2/Accumulator.cpp"
#include "../Systolic-Array-Simulator-2/Unified_Buffer.h"
#include "../Systolic-Array-Simulator-2/Unified_Buffer.cpp"
#include "../Systolic-Array-Simulator-2/Activation.h"
#include "../Systolic-Array-Simulator-2/Activation.cpp"
#include "../Systolic-Array-Simulator-2/Memory.h"

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
		Unified_Buffer ub(matrix_size, matrix_size);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
			ub.mem_block[i][0] = i + 1;

		ss.read_en = true;
		ss.read_vector_from_UB_when_enable();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(ub.mem_block[i][0], ss.diagonalized_matrix[i][i]) << "Diagonalization failed in " << i;
		}
	}

	TEST(SystolicSetupTest, TwoVectorDiagonalizationTest) {
		int matrix_size = 4;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, matrix_size);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(ub.mem_block[i][0], ss.diagonalized_matrix[i][i]) << "Diagonalization 1 failed in " << i;
			EXPECT_EQ(ub.mem_block[i][1], ss.diagonalized_matrix[i][i+1]) << "Diagonalization 2 failed in " << i;
		}
	}

	TEST(SystolicSetupTest, PushTest) {
		int matrix_size = 4;
		int matrix_size_in = 2;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size_in; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		int8_t answer[2][3] =
		{
			{1,2,0},
			{0,2,3}
		};

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size_in = matrix_size_in;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		for (int i = 0; i < matrix_size_in; i++)
		{
			for (int j = 0; j < 2*matrix_size_in - 1; j++)
			{
				EXPECT_EQ(answer[i][j], ss.diagonalized_matrix[i][j]) << "Diagonalization 1 failed in " << i;
			}
		}

		ss.read_en = false;
		ss.push_en = true;
		ss.switch_en = true;
		for (int i = 0; i < DIAG_WIDTH(matrix_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();
			if (i < DIAG_WIDTH(matrix_size_in))
			{
				EXPECT_EQ(answer[0][i], ss.input_datas[0]) << "Advancing 1 failed in " << i;
				EXPECT_EQ(answer[1][i], ss.input_datas[1]) << "Advancing 2 failed in " << i;
			}
			else
			{
				EXPECT_EQ(0, ss.input_datas[0]) << "Advancing 1 failed in " << i;
				EXPECT_EQ(0, ss.input_datas[1]) << "Advancing 2 failed in " << i;
			}

			if (i < matrix_size_in)
			{
				for (int j = 0; j < matrix_size; j++)
				{
					if (j < matrix_size_in)
					{
						if (i == j)
							EXPECT_TRUE(ss.switch_weights[j]) << "Switch " << j << " shoud be true";
						else
							EXPECT_FALSE(ss.switch_weights[j]) << "Switch " << j << " shoud be false";
					}
					else
					{
						EXPECT_FALSE(ss.switch_weights[j]) << "Switch " << j << " shoud be false";
					}
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

	TEST(SystolicSetupTest, PushStopTest) {
		int matrix_size = 2;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size_in = matrix_size;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.read_en = false;
		ss.push_en = true;
		for (int i = 0; i < DIAG_WIDTH(matrix_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();
		}

		EXPECT_EQ(0, ss.diagonalized_matrix[0][DIAG_WIDTH(matrix_size) - 1]);
		EXPECT_EQ(3, ss.diagonalized_matrix[1][DIAG_WIDTH(matrix_size) - 1]);

		ss.push_vectors_to_MMU_when_enable();

		EXPECT_EQ(0, ss.diagonalized_matrix[0][DIAG_WIDTH(matrix_size) - 1]);
		EXPECT_EQ(3, ss.diagonalized_matrix[1][DIAG_WIDTH(matrix_size) - 1]);
	}

	TEST(SystolicSetupTest, PushWithMatrixSizeIn) {
		int matrix_size = 4;
		int matrix_size_in = 2;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size_in; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size_in = matrix_size_in;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.acc_addr_in = 1;
		ss.read_en = false;
		ss.push_en = true;
		for (int i = 0; i < DIAG_WIDTH(matrix_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();
			if (i >= DIAG_WIDTH(matrix_size_in))
			{
				EXPECT_EQ(0, ss.input_datas[0]);
				EXPECT_EQ(0, ss.input_datas[1]);
			}
		}
	}

	TEST(SystolicSetupTest, AccOutTest) {
		int matrix_size = 4;
		int matrix_size_in = 2;
		Systolic_Setup ss(matrix_size);
		Unified_Buffer ub(matrix_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size_in; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size_in = matrix_size_in;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.acc_addr_in = 1;
		ss.read_en = false;
		ss.push_en = true;

		for (int i = 0; i < DIAG_WIDTH(matrix_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();

			if (i >= matrix_size)
			{
				if (i == DIAG_WIDTH(matrix_size) - 1)
				{
					EXPECT_FALSE(ss.acc_write_en);
				}
				else
				{
					EXPECT_EQ(1 + (i - matrix_size), ss.acc_addr_out);
					EXPECT_TRUE(ss.acc_write_en);
				}
			}
			else
			{
				EXPECT_EQ(1, ss.acc_addr_out) << "Expect 1 if under matrix_size " << i;
				EXPECT_FALSE(ss.acc_write_en) << "Expect False if under matrix_size " << i;
			}
		}
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

		wf.push_en = true;
		for (int i = 0; i < matrix_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
		}

		wf.push_weight_vector_to_MMU_when_en();
		EXPECT_TRUE(wf.weight_queue.empty());
	}

	TEST(WeightFIFOTest, DramReadTest) {
		int matrix_size = 2;
		Memory dram(matrix_size, 50);
		Weight_FIFO wf(matrix_size);

		wf.dram = &dram;

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		for (int i = 0; i < matrix_size; i++)
		{
			dram.mem_block[0][i] = copy[0][i];
			dram.mem_block[1][i] = copy[1][i];
		}

		wf.read_en = true;
		wf.dram_addr = 0;
		wf.read_matrix_from_DRAM_when_en();

		for (int i = 0; i < matrix_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(copy[j][matrix_size - i - 1], wf.input_weights[j]);
			}
		}
	}

	TEST(WeightFIFOTest, InitReadPushTest) {
		int matrix_size = 2;
		Memory dram(matrix_size, 50);
		Weight_FIFO wf(matrix_size);

		wf.dram = &dram;

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		for (int i = 0; i < matrix_size; i++)
		{
			dram.mem_block[0][i] = copy[0][i];
			dram.mem_block[1][i] = copy[1][i];
		}

		//Initially, whether push_en is true or not, should be pushed
		wf.read_en = true;
		wf.dram_addr = 0;
		wf.read_matrix_from_DRAM_when_en();

		wf.push_en = false;
		for (int i = 0; i < matrix_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(copy[j][matrix_size - i - 1], wf.input_weights[j]);
			}
		}

		//After init, push only when push_en is true
		for (int i = 0; i < matrix_size; i++)
		{
			dram.mem_block[0][i] = copy[0][i] + 5;
			dram.mem_block[1][i] = copy[1][i] + 5;
		}
		wf.read_matrix_from_DRAM_when_en();

		wf.push_en = false;
		for (int i = 0; i < matrix_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_NE(copy[j][matrix_size - i - 1] + 5, wf.input_weights[j]);
			}
		}

		wf.push_en = true;
		for (int i = 0; i < matrix_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(copy[j][matrix_size - i - 1] + 5, wf.input_weights[j]);
			}
		}
	}

	TEST(WeightFIFOTest, DramReadCycleTest) {
		int matrix_size = 10;
		Memory dram(matrix_size, 50);
		Weight_FIFO wf(matrix_size);

		wf.dram = &dram;

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				dram.mem_block[i][j] = i+j;
			}
		}

		wf.read_en = true;
		wf.dram_addr = 0;

		wf.read_matrix_from_DRAM_when_en();
		EXPECT_TRUE(wf.weight_queue.empty());

		wf.read_matrix_from_DRAM_when_en();
		EXPECT_FALSE(wf.weight_queue.empty());
	}

	TEST(UnifiedBufferTest, ReadHMTest)
	{
		int matrix_size = 4;
		int matrix_size_in = 2;
		Memory hm(matrix_size, 10);
		Unified_Buffer ub(matrix_size, 10);

		ub.hm = &hm;

		//push matrix in Host memory
		for (int i = 0; i < matrix_size; i++)
		{
			// 1 2
			// 3 4
			hm.mem_block[0][i] = i + 1;
			hm.mem_block[1][i] = i + 3;
		}

		ub.addr = 1;
		ub.hm_addr = 0;
		ub.read_en = true;
		ub.matrix_size_in = matrix_size_in;

		for (int i = 0; i < matrix_size_in; i++)
		{
			ub.read_vector_from_HM_when_enable();
		}

		for (int i = 0; i < matrix_size_in; i++)
		{
			for (int j = 0; j < matrix_size_in; j++)
			{
				EXPECT_EQ(ub.mem_block[1 + i][j], hm.mem_block[i][j]) << "Unified Buffer Read failed in " << i << ", " << j;
			}
		}

		ub.hm_addr = 3;
		ub.read_en = false;
		hm.mem_block[3][0] = 100;
		ub.read_vector_from_HM_when_enable();

		for (int i = 0; i < matrix_size_in; i++)
		{
			for (int j = 0; j < matrix_size_in; j++)
			{
				EXPECT_EQ(ub.mem_block[1 + i][j], hm.mem_block[i][j]) << "No read when false " << i << ", " << j;
			}
		}
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
		ss.read_en = true;
		ss.read_vector_from_UB_when_enable();

		//cycle2
		ss.read_vector_from_UB_when_enable();

		//cycle3
		ss.push_en = true;
		ss.switch_en = true;
		wf.push_en = true;
		ss.push_vectors_to_MMU_when_enable(); //input datas and switchs
		wf.push_weight_vector_to_MMU_when_en();
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
		ss.read_en = true;
		ss.read_vector_from_UB_when_enable();

		//cycle2
		ss.read_vector_from_UB_when_enable();

		//cycle3
		ss.push_en = true;
		ss.switch_en = true;
		wf.push_en = true;
		ss.push_vectors_to_MMU_when_enable(); //input datas and switchs
		wf.push_weight_vector_to_MMU_when_en(); //input weights
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
				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				wf.push_en = false;
				break;
			case 1:
				//Weight FIFO advance
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				wf.push_en = true;
				break;
			case 2:
				//Systolic Setup advance
				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				wf.push_en = false;
				break;
			}

			//Register update
			ss.read_vector_from_UB_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
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
		int matrix_size = 4;
		int matrix_size_in = 2;
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

		int8_t copy[4][4] =
		{
			{1,2, 0, 0},
			{5,4, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);
		wf.push(mat);

		for (int i = 0; i < 7; i++)
		{
			// 0, 1 ss program
			// 1 wf read
			// 2, 3, 4, 5 wf push
			// 6, 7, 8, 9 mmu cal

			//Control setting
			switch (i)
			{
			case 0:
				//Systolic setup program
				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				ss.matrix_size_in = matrix_size_in;
				ss.ub_addr = 0;

				//wf.push_en = false;
				wf.read_en = false;

				break;
			case 1:
				//Weight FIFO advance
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.matrix_size_in = matrix_size_in;
				ss.ub_addr = 0;

				//wf.push_en = true;
				wf.read_en = false;

				break;
			case 2:
			case 4:
			case 5:
			case 6:
				//NOP
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				break;
			case 3:
				//Systolic Setup advance
				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				//wf.push_en = false;
				break;
			}

			//Register update
			ss.read_vector_from_UB_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			//wf.read_matrix_and_push_when_en();
			mmu.setup_array(); //wf.push en 을 그대로 받아와서 write_en로 사용해야 함.

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

	TEST(AccumTest, ReadAndWriteTest) {
		int matrix_size = 4;
		Accumulator accm(matrix_size, 12);

		int32_t input[4] = { 1,2,3,4 };

		accm.write(input, 0);

		int32_t* output;
		output = new int32_t[4];
		accm.read(output, 0);
		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(input[i], output[i]);
;		}
	}

	TEST(AccumTest, TwoMacAccmResultTest) {
		int matrix_size = 2;
		MMU mmu(matrix_size);
		Unified_Buffer ub(matrix_size, 2);
		Systolic_Setup ss(matrix_size);
		Weight_FIFO wf(matrix_size);
		Accumulator acc(matrix_size, 100);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;
		acc.mmu = &mmu;

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
				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				break;
			case 1:
				//Weight FIFO advance
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = true;
				break;
			case 2:
			case 4:
			case 5:
			case 6:
				//NOP
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				break;
			case 3:
				//Systolic Setup advance
				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				//wf.push_en = false;

				//Setup accm addr
				ss.acc_addr_in = 0;
				break;
			}

			//Register update
			ss.read_vector_from_UB_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
			mmu.setup_array();

			//Combination Logic
			mmu.calculate();

			//Update accm
			acc.write_results();

			switch (i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				break;
			case 4:
			{
				int answer[2] = { 5,0 };
				int dst[2];
				acc.read(dst, 1);
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], dst[j]) << "Accm failed in step " << i << ", " << j;
				break;
			}
			case 5:
			{
				int answer[2] = { 8,13 };
				int dst[2];
				acc.read(dst, 2);
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], dst[j]) << "Accm failed in step " << i << ", " << j;
				break;
			}
			case 6:
			{
				int answer[2] = { 0,22 };
				int dst[2];
				acc.read(dst, 3);
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], dst[j]) << "Accm failed in step " << i << ", " << j;
				break;
			}
			}
		}
	}

	TEST(ModuleTest, SizeTwoModuleTest) {
		int matrix_size = 2;

		Memory hm(matrix_size, 50);
		Memory dram(matrix_size, 50);
		Unified_Buffer ub(matrix_size, 2);
		Systolic_Setup ss(matrix_size);
		MMU mmu(matrix_size);
		Weight_FIFO wf(matrix_size);
		Accumulator acc(matrix_size, 100);
		Activation act(matrix_size);

		ub.hm = &hm;
		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;
		acc.mmu = &mmu;
		act.acc = &acc;

		//push matrix in Host memory
		for (int i = 0; i < matrix_size; i++)
		{
			// 1 2
			// 3 4
			hm.mem_block[0][i] = i + 1;
			hm.mem_block[1][i] = i + 3;
		}

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, matrix_size, copy[0]);
		wf.push(mat);

		int acc_addr = 0;

		for (int i = 0; i <11; i++)
		{
			//Control setting
			switch (i)
			{
			case 0:
				//Unified Buffer read from HM
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				//mmu.write_en = false;
				act.act_en = false;
				ub.read_en = true;

				ub.hm_addr = 0;
				ub.addr = 0;
			case 2:
				//Systolic setup program
				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				//mmu.write_en = false;
				act.act_en = false;
				ub.read_en = false;

				break;
			case 3:
				//Weight FIFO advance
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = true;
				//mmu.write_en = true;
				act.act_en = false;
				ub.read_en = false;

				break;
			case 1:
			case 4:
			case 6:
			case 7:
			case 8:
				//NOP
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				//mmu.write_en = false;
				act.act_en = false;
				ub.read_en = false;

				break;
			case 5:
				//Systolic Setup advance
				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				//wf.push_en = false;
				//mmu.write_en = false;
				act.act_en = false;
				ub.read_en = false;

				//Setup accm addr
				ss.acc_addr_in = acc_addr;
				break;
			case 9:
				//Activation mat write from accm
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				//mmu.write_en = false;
				act.act_en = true;
				ub.read_en = false;

				//Setup accm addr
				//addr + matrix height -1 (no data cycle)

				//act.addr = acc_addr + matrix_size - 1;
				break;
			case 10:
				//Write to host mem
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				//wf.push_en = false;
				//mmu.write_en = false;
				act.act_en = false;
				ub.read_en = false;

				//Setup ub addr
				ub.addr = 0;
				break;
			}
			//Register update
			ub.read_vector_from_HM_when_enable();
			ss.read_vector_from_UB_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
			mmu.setup_array();
			act.do_activation_and_write_to_UB();

			//Combination Logic
			mmu.calculate();

			//Update accm
			acc.write_results();
		}

		//column-wised
		int8_t answer[2][2] =
		{
			{5,13},
			{8,22}
		};

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(answer[i][0], ub.mem_block[i][0]) << "Module failed " << i << ", " << 1;
			EXPECT_EQ(answer[i][1], ub.mem_block[i][1]) << "Module failed " << i << ", " << 2;
		}
	}
}