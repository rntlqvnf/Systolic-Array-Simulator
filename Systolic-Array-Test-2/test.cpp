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
#include "../Systolic-Array-Simulator-2/Decoder.h"
#include "../Systolic-Array-Simulator-2/Decoder.cpp"

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
		MAC mac(3);
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
		MAC mac(3);
		mac.data_in = (int8_t) 1;
		mac.weight_buf[0] = (int8_t)2;
		mac.calculate();

		EXPECT_EQ(2, mac.sum_out) << "Sum in should be zero";
	}
	TEST(MACTest, WithSum) {
		MAC mac(3);
		mac.data_in = (int8_t)1;
		mac.weight_buf[0] = (int8_t)2;
		mac.sum_in = (int32_t)10;
		mac.calculate();

		EXPECT_EQ(12, mac.sum_out) << "Sum in should effect";
	}
	TEST(MACTest, WeightSwitch) {
		MAC mac(3);
		mac.weight_in = (int8_t)10;
		mac.write_en_in = true;
		mac.weight_tag_in = 2;
		mac.calculate();

		EXPECT_EQ(10, mac.weight_buf[1 - mac.current_weight]) << "Weight buf should write when tag matches";
	}
	TEST(MACTest, SwitchAndCalculate) {
		MAC mac(3);
		mac.weight_in = (int8_t)10;
		mac.write_en_in = true;
		mac.weight_tag_in = 2;
		mac.calculate();

		mac.switch_in = true;
		mac.data_in = 10;
		mac.calculate();

		EXPECT_EQ(100, mac.sum_out) << "Calculation should be done by active weight buf";
	}

	TEST(SystolicSetupTest, OneVectorDiagonalizationTest) {
		int mmu_size = 4;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, mmu_size);

		ss.ub = &ub;

		for (int i = 0; i < mmu_size; i++)
			ub.mem_block[i][0] = i + 1;

		ss.read_en = true;
		ss.read_vector_from_UB_when_enable();

		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(ub.mem_block[i][0], ss.diagonalized_matrix[i][i]) << "Diagonalization failed in " << i;
		}
	}

	TEST(SystolicSetupTest, TwoVectorDiagonalizationTest) {
		int mmu_size = 4;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, mmu_size);

		ss.ub = &ub;

		for (int i = 0; i < mmu_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(ub.mem_block[i][0], ss.diagonalized_matrix[i][i]) << "Diagonalization 1 failed in " << i;
			EXPECT_EQ(ub.mem_block[i][1], ss.diagonalized_matrix[i][i+1]) << "Diagonalization 2 failed in " << i;
		}
	}

	TEST(SystolicSetupTest, PushTest) {
		int mmu_size = 4;
		int matrix_size = 2;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
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
		ss.matrix_size = matrix_size;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < 2* matrix_size - 1; j++)
			{
				EXPECT_EQ(answer[i][j], ss.diagonalized_matrix[i][j]) << "Diagonalization 1 failed in " << i;
			}
		}

		ss.read_en = false;
		ss.push_en = true;
		ss.switch_en = true;
		for (int i = 0; i < DIAG_WIDTH_1(mmu_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();
			if (i < DIAG_WIDTH_1(matrix_size))
			{
				EXPECT_EQ(answer[0][i], ss.input_datas[0]) << "Advancing 1 failed in " << i;
				EXPECT_EQ(answer[1][i], ss.input_datas[1]) << "Advancing 2 failed in " << i;
			}
			else
			{
				EXPECT_EQ(0, ss.input_datas[0]) << "Advancing 1 failed in " << i;
				EXPECT_EQ(0, ss.input_datas[1]) << "Advancing 2 failed in " << i;
			}

			if (i < matrix_size)
			{
				for (int j = 0; j < mmu_size; j++)
				{
					if (j < matrix_size)
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
				for (int j = 0; j < mmu_size; j++)
				{
					EXPECT_FALSE(ss.switch_weights[j]) << "Over mat_size, switch " << j << "shoud be false";
				}
			}
		}
	}

	TEST(SystolicSetupTest, PushStopTest) {
		int mmu_size = 2;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < mmu_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size = mmu_size;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.read_en = false;
		ss.push_en = true;
		for (int i = 0; i < DIAG_WIDTH_1(mmu_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();
		}

		EXPECT_EQ(0, ss.diagonalized_matrix[0][DIAG_WIDTH_1(mmu_size) - 1]);
		EXPECT_EQ(3, ss.diagonalized_matrix[1][DIAG_WIDTH_1(mmu_size) - 1]);

		ss.push_vectors_to_MMU_when_enable();

		EXPECT_EQ(0, ss.diagonalized_matrix[0][DIAG_WIDTH_1(mmu_size) - 1]);
		EXPECT_EQ(3, ss.diagonalized_matrix[1][DIAG_WIDTH_1(mmu_size) - 1]);
	}

	TEST(SystolicSetupTest, PushWithMatrixSizeIn) {
		int mmu_size = 4;
		int matrix_size = 2;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size = matrix_size;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.acc_addr_in = 1;
		ss.read_en = false;
		ss.push_en = true;
		for (int i = 0; i < DIAG_WIDTH_1(mmu_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();
			if (i >= DIAG_WIDTH_1(matrix_size))
			{
				EXPECT_EQ(0, ss.input_datas[0]);
				EXPECT_EQ(0, ss.input_datas[1]);
			}
		}
	}

	TEST(SystolicSetupTest, UnfoldTest) {
		int mmu_size = 4;
		int matrix_size = 4;
		Weight_Size_Reg wsreg;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, 10);

		ss.ub = &ub;
		ss.wsreg = &wsreg;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
			ub.mem_block[i][2] = i + 3;
			ub.mem_block[i][3] = i + 4;
		}

		wsreg.set_size(2);

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.unfold_en = true;
		ss.switch_en = true;
		ss.matrix_size = matrix_size;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.acc_addr_in = 1;
		ss.read_en = false;
		ss.push_en = true;
		ss.unfold_en = true;

		int8_t answer[4][12] =
		{
			{1, 2, 3, 2, 3, 4, 3, 4, 5, 0, 0, 0},
			{0, 2, 3, 4, 3, 4, 5, 4, 5, 6, 0, 0},
			{0, 0, 2, 3, 4, 3, 4, 5, 4, 5, 6, 0},
			{0, 0, 0, 3, 4, 5, 4, 5, 6, 5, 6, 7}
		};
		for (int i = 0; i < 12; i++)
		{
			ss.push_vectors_to_MMU_when_enable();
			EXPECT_EQ(answer[0][i], ss.input_datas[0]) << "FAILED IN " << i;
			EXPECT_EQ(answer[1][i], ss.input_datas[1]) << "FAILED IN " << i;
			EXPECT_EQ(answer[2][i], ss.input_datas[2]) << "FAILED IN " << i;
			EXPECT_EQ(answer[3][i], ss.input_datas[3]) << "FAILED IN " << i;
		}
	}

	TEST(SystolicSetupTest, AccOutTest) {
		int mmu_size = 4;
		int matrix_size = 2;
		Systolic_Setup ss(mmu_size);
		Unified_Buffer ub(mmu_size, 2);

		ss.ub = &ub;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.mem_block[i][0] = i + 1;
			ub.mem_block[i][1] = i + 2;
		}

		ss.ub_addr = 0;
		ss.read_en = true;
		ss.matrix_size = matrix_size;
		ss.read_vector_from_UB_when_enable();
		ss.read_vector_from_UB_when_enable();

		ss.acc_addr_in = 1;
		ss.read_en = false;
		ss.push_en = true;

		for (int i = 0; i < DIAG_WIDTH_1(mmu_size); i++)
		{
			ss.push_vectors_to_MMU_when_enable();

			if (i >= mmu_size)
			{
				EXPECT_EQ(1 + (i - mmu_size), ss.acc_addr_out);
				EXPECT_TRUE(ss.acc_write_en);
			}
			else
			{
				EXPECT_EQ(1, ss.acc_addr_out) << "Expect 1 if under matrix_size " << i;
				EXPECT_FALSE(ss.acc_write_en) << "Expect False if under matrix_size " << i;
			}
		}

		ss.push_vectors_to_MMU_when_enable();
		EXPECT_FALSE(ss.acc_write_en);
	}

	TEST(WeightFIFOTest, PushPopTest) {
		int mmu_size = 2;
		Weight_FIFO wf(mmu_size);

		int8_t copy[2][2]= 
		{
			{1,2},
			{5,4}
		};
		int8_t** mat = NULL;
		allocate_array(mat, mmu_size, copy[0]);

		wf.push(mat);

		ASSERT_FALSE(wf.weight_queue.empty());

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				EXPECT_EQ(copy[i][j], wf.weight_queue.front()[i][j]) << "FIFO front failed: " << i << "," << j;
			}
		}

		wf.push_en = true;
		for (int i = 0; i < mmu_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
		}

		wf.push_weight_vector_to_MMU_when_en();
		EXPECT_TRUE(wf.weight_queue.empty());
	}

	TEST(WeightFIFOTest, DramReadTest) {
		int mmu_size = 2;
		Memory dram(mmu_size, 50);
		Weight_FIFO wf(mmu_size);

		wf.dram = &dram;

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		for (int i = 0; i < mmu_size; i++)
		{
			dram.mem_block[0][i] = copy[0][i];
			dram.mem_block[1][i] = copy[1][i];
		}

		wf.read_en = true;
		wf.dram_addr = 0;
		wf.read_matrix_from_DRAM_when_en();

		for (int i = 0; i < mmu_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < mmu_size; j++)
			{
				EXPECT_EQ(copy[j][mmu_size - i - 1], wf.input_weights[j]);
			}
		}
	}

	TEST(WeightFIFOTest, InitReadPushTest) {
		int mmu_size = 2;
		Memory dram(mmu_size, 50);
		Weight_FIFO wf(mmu_size);

		wf.dram = &dram;

		int8_t copy[2][2] =
		{
			{1,2},
			{5,4}
		};
		for (int i = 0; i < mmu_size; i++)
		{
			dram.mem_block[0][i] = copy[0][i];
			dram.mem_block[1][i] = copy[1][i];
		}

		//Initially, whether push_en is true or not, should be pushed
		wf.read_en = true;
		wf.dram_addr = 0;
		wf.read_matrix_from_DRAM_when_en();

		wf.push_en = false;
		for (int i = 0; i < mmu_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < mmu_size; j++)
			{
				EXPECT_EQ(copy[j][mmu_size - i - 1], wf.input_weights[j]);
			}
		}

		//After init, push only when push_en is true
		for (int i = 0; i < mmu_size; i++)
		{
			dram.mem_block[0][i] = copy[0][i] + 5;
			dram.mem_block[1][i] = copy[1][i] + 5;
		}
		wf.read_matrix_from_DRAM_when_en();

		wf.push_en = false;
		for (int i = 0; i < mmu_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < mmu_size; j++)
			{
				EXPECT_NE(copy[j][mmu_size - i - 1] + 5, wf.input_weights[j]);
			}
		}

		wf.push_en = true;
		for (int i = 0; i < mmu_size; i++)
		{
			wf.push_weight_vector_to_MMU_when_en();
			for (int j = 0; j < mmu_size; j++)
			{
				EXPECT_EQ(copy[j][mmu_size - i - 1] + 5, wf.input_weights[j]);
			}
		}
	}

	TEST(WeightFIFOTest, DramReadCycleTest) {
		int mmu_size = 10;
		int matrix_size = 10;
		Memory dram(mmu_size, 50);
		Weight_FIFO wf(mmu_size);

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
		wf.matrix_size = matrix_size;

		wf.read_matrix_from_DRAM_when_en();
		EXPECT_TRUE(wf.weight_queue.empty());

		wf.read_matrix_from_DRAM_when_en();
		EXPECT_FALSE(wf.weight_queue.empty());

		matrix_size = 5;

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				dram.mem_block[i][j] = i + j;
			}
		}

		wf.read_en = true;
		wf.dram_addr = 0;
		wf.matrix_size = matrix_size;

		wf.read_matrix_from_DRAM_when_en();
		EXPECT_FALSE(wf.weight_queue.empty());
	}

	TEST(WeightFIFOTest, UnfoldWeightTest) {
		int mmu_size = 10;
		int matrix_size = 3;
		Memory dram(mmu_size, 50);
		Weight_FIFO wf(mmu_size);

		wf.dram = &dram;

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				dram.mem_block[i][j] = i + j;
			}
		}

		wf.read_en = true;
		wf.unfold_en = true;
		wf.dram_addr = 0;
		wf.matrix_size = matrix_size;

		wf.read_matrix_from_DRAM_when_en();
		ASSERT_FALSE(wf.weight_queue.empty());

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(dram.mem_block[i][j], wf.weight_queue.front()[0][i * matrix_size + j]);
			}
		}
	}

	TEST(UnifiedBufferTest, ReadHMTest)
	{
		int mmu_size = 4;
		int matrix_size = 2;
		Memory hm(mmu_size, 10);
		Unified_Buffer ub(mmu_size, 10);

		ub.hm = &hm;

		//push matrix in Host memory
		for (int i = 0; i < mmu_size; i++)
		{
			// 1 2
			// 3 4
			hm.mem_block[0][i] = i + 1;
			hm.mem_block[1][i] = i + 3;
		}

		ub.addr = 1;
		ub.hm_addr = 0;
		ub.read_en = true;
		ub.matrix_size = matrix_size;

		for (int i = 0; i < matrix_size; i++)
		{
			ub.read_vector_from_HM_when_enable();
		}

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(ub.mem_block[1 + i][j], hm.mem_block[i][j]) << "Unified Buffer Read failed in " << i << ", " << j;
			}
		}

		ub.hm_addr = 3;
		ub.read_en = false;
		hm.mem_block[3][0] = 100;
		ub.read_vector_from_HM_when_enable();

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(ub.mem_block[1 + i][j], hm.mem_block[i][j]) << "No read when false " << i << ", " << j;
			}
		}
	}

	TEST(ActivationTest, FoldTest)
	{
		int mmu_size = 4;
		int matrix_size = 2;
		Memory hm(mmu_size, 50);
		Memory dram(mmu_size, 50);
		Unified_Buffer ub(mmu_size, 4);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);
		MMU mmu(mmu_size);
		Accumulator acc(mmu_size, 50);
		Activation act(mmu_size);

		ub.hm = &hm;
		wf.dram = &dram;
		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;
		acc.mmu = &mmu;
		act.acc = &acc;

		int32_t input[4] = { 1,0,0,0 };
		acc.write(input, 1);
		input[0] = 2;
		acc.write(input, 2);
		input[0] = 3;
		acc.write(input, 3);
		input[0] = 4;
		acc.write(input, 4);

		act.act_en = true;
		act.fold_en = true;
		act.acc_addr = 1;
		act.ub_addr = 0;
		act.matrix_size = matrix_size;

		act.do_activation_and_write_to_UB();
		act.do_activation_and_write_to_UB();

		int8_t answer[2][2] =
		{
			{1,2},
			{3,4}
		};

		for (int i = 0; i < matrix_size; i++)
		{
			for (int j = 0; j < matrix_size; j++)
			{
				EXPECT_EQ(answer[i][j], ub.mem_block[i][j]) << "Fold Failed " << i << ", " << j;
			}
		}
	}

	TEST(MMUTest, SetupTest) {
		int mmu_size = 2;
		MMU mmu(mmu_size);
		Unified_Buffer ub(mmu_size, 2);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		for (int i = 0; i < mmu_size; i++)
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
		allocate_array(mat, mmu_size, copy[0]);
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
		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(ss.input_datas[i], mmu.mac_array[0][i].data_in) << "Input data failed in " << i;
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[0][i].weight_in) << "Input weight failed in " << i;
		}
	}

	TEST(MMUTest, WeightWriteTest) {
		int mmu_size = 2;
		MMU mmu(mmu_size);
		Unified_Buffer ub(mmu_size, 2);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);

		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		for (int i = 0; i < mmu_size; i++)
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
		allocate_array(mat, mmu_size, copy[0]);
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

		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[0][i].weight_in) << "Input weight failed in " << i;
			EXPECT_TRUE(mmu.mac_array[0][i].write_en_in) << "Write en failed in " << i;
			EXPECT_EQ(0, mmu.mac_array[0][i].weight_tag_in) << "Weight tag failed in " << i;
		}

		mmu.calculate();
		mmu.setup_array();
		mmu.calculate();

		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[1][i].weight_in) << "Input weight failed in " << i;
			EXPECT_TRUE(mmu.mac_array[0][i].write_en_in) << "Write en failed in " << i;
			EXPECT_EQ(1, mmu.mac_array[0][i].weight_tag_in) << "Weight tag failed in " << i;
			EXPECT_EQ(1, mmu.mac_array[1][i].weight_tag_in) << "Weight tag failed in " << i;
			EXPECT_EQ(wf.input_weights[i], mmu.mac_array[1][i].weight_buf[1 - mmu.mac_array[1][i].current_weight]) << "Weight write failed in " << i;
		}
	}

	TEST(MMUTest, OneMacTest) {
		int mmu_size = 1;
		Memory dram(mmu_size, 50);
		MMU mmu(mmu_size);
		Unified_Buffer ub(mmu_size, 1);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);

		ss.ub = &ub;
		wf.dram = &dram;
		mmu.ss = &ss;
		mmu.wf = &wf;

		ub.mem_block[0][0] = 1;
		dram.mem_block[0][0] = 2;

		for (int i = 0; i < 4; i++)
		{
			//Control setting
			switch (i)
			{
			case 0:
				//Systolic setup program
				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				wf.read_en = false;
				wf.push_en = false;
				wf.unfold_en = false;
				break;
			case 1:
				//Weight FIFO Read
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				wf.read_en = true;
				wf.push_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				break;
			case 2:
				//NOP (Weight Write)
				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				wf.read_en = false;
				wf.push_en = false;
				wf.unfold_en = false;
				break;
			case 3:
				//Systolic Setup advance
				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				ss.overwrite_en = true;
				wf.read_en = false;
				wf.push_en = false;
				wf.unfold_en = false;
				break;
			}

			//Register update
			ss.push_vectors_to_MMU_when_enable();
			ss.read_vector_from_UB_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
			wf.read_matrix_from_DRAM_when_en();
			mmu.setup_array();

			//Combination Logic
			mmu.calculate();

			switch (i)
			{
			case 0:
			case 1:
			case 2:
			{
				int answer[1] = { 0 };
				for (int j = 0; j < mmu_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 3:
			{
				int answer[1] = { 2 };
				for (int j = 0; j < mmu_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			}
		}
	}
	TEST(MMUTest, TwoMacCalTest) {
		int mmu_size = 4;
		int matrix_size = 2;

		Memory hm(mmu_size, 50);
		Memory dram(mmu_size, 50);
		Unified_Buffer ub(mmu_size, 2);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);
		MMU mmu(mmu_size);

		ub.hm = &hm;
		wf.dram = &dram;
		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;

		int8_t input[4][4] =
		{
			{1,2, 0, 0},
			{2,3, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				hm.mem_block[i][j] = input[i][j];
			}
		}

		int8_t weight[4][4] =
		{
			{1,2, 0, 0},
			{5,4, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				dram.mem_block[i][j] = weight[i][j];
			}
		}

		for (int i = 0; i < 12; i++)
		{
			// 0 1 ub read & ss program
			// 1 wf read
			// 2, 3, 4, 5 wf push (Auto push)
			// 6, 7, 8, 9, 10, 11 mmu cal

			//Control setting
			switch (i)
			{
			case 0: //UB Read & SS Program
				ub.read_en = true;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = matrix_size;

				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			case 1: //WF Read
				ub.read_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = true;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			case 6: //MMU cal
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				ss.overwrite_en = true;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			default: //NOP
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;
			}

			//Register update
			ub.read_vector_from_HM_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			ss.read_vector_from_UB_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
			wf.read_matrix_from_DRAM_when_en();
			mmu.setup_array();

			//Combination Logic
			mmu.calculate();
			
			switch (i)
			{
			case 9:
			{
				int answer[2] = { 5,0 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 10:
			{
				int answer[2] = { 8,13 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			case 11:
			{
				int answer[2] = { 0,22 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			default:
			{
				int answer[2] = { 0,0 };
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], mmu.last_row_sum[j]) << "Cal failed in step " << i << ", " << j;
				break;
			}
			}
		}
	}

	TEST(MMUTest, ConvolutionTest) {
		int mmu_size = 4;
		int matrix_size = 4;

		Memory hm(mmu_size, 50);
		Memory dram(mmu_size, 50);
		Weight_Size_Reg wsreg;
		Unified_Buffer ub(mmu_size, 10);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);
		MMU mmu(mmu_size);

		ub.hm = &hm;
		wf.dram = &dram;
		wf.wsreg = &wsreg;
		ss.ub = &ub;
		ss.wsreg = &wsreg;
		mmu.ss = &ss;
		mmu.wf = &wf;

		int8_t input[4][4] =
		{
			{1, 1, 2, 2},
			{2, 2, 3, 3},
			{3, 3, 4, 4},
			{5, 5, 6, 6}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				hm.mem_block[i][j] = input[i][j];
			}
		}

		int8_t weight[4][4] =
		{
			{0,1, 0, 0},
			{1,2, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				dram.mem_block[i][j] = weight[i][j];
			}
		}

		int32_t answer[3][3] =
		{
			{7,10,11},
			{11,14,15},
			{18,21,22}
		};

		for (int i = 0; i < 18; i++)
		{
			// 0 1 ub read & ss program
			// 1 wf read
			// 2, 3, 4, 5 wf push (Auto push)
			// 6, 7, 8, 9, 10, 11 mmu cal

			//Control setting
			switch (i)
			{
			case 0: //UB Read
				ub.read_en = true;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = matrix_size;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.overwrite_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			case 1: //WF Read
				ub.read_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.overwrite_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = true;
				wf.unfold_en = true;
				wf.dram_addr = 0;
				wf.matrix_size = 2;

				break;

			case 6: //MMU cal
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = true;
				ss.push_en = true;
				ss.switch_en = true;
				ss.overwrite_en = true;
				ss.unfold_en = true;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			default: //NOP
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.overwrite_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;
			}

			//Register update
			ub.read_vector_from_HM_when_enable();
			ss.read_vector_from_UB_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
			wf.read_matrix_from_DRAM_when_en();
			mmu.setup_array();

			//Combination Logic
			mmu.calculate();

			switch (i)
			{
			case 9:
			{
				EXPECT_EQ(7, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			case 10:
			{
				EXPECT_EQ(10, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			case 11:
			{
				EXPECT_EQ(11, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}			
			case 12:
			{
				EXPECT_EQ(11, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			case 13:
			{
				EXPECT_EQ(14, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			case 14:
			{
				EXPECT_EQ(15, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			case 15:
			{
				EXPECT_EQ(18, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			case 16:
			{
				EXPECT_EQ(21, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}			
			case 17:
			{
				EXPECT_EQ(22, mmu.last_row_sum[0]) << "Cal failed in step " << i;
				break;
			}
			}
		}
	}

	TEST(AccumTest, ReadAndWriteTest) {
		int mmu_size = 4;
		Accumulator accm(mmu_size, 12);

		int32_t input[4] = { 1,2,3,4 };

		accm.write(input, 0);

		int32_t* output;
		output = new int32_t[4];
		accm.read(output, 0);
		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(input[i], output[i]);
;		}
	}

	TEST(AccumTest, AccumulateTest) {
		int mmu_size = 4;
		Accumulator accm(mmu_size, 12);

		int32_t input[4] = { 1,2,3,4 };
		int32_t input_2[4] = { 2,3,4,5 };
		int32_t ans[4] = { 3,5,7,9 };

		accm.write(input, 0);
		accm.accm(input_2, 0);

		int32_t* output;
		output = new int32_t[4];
		accm.read(output, 0);
		for (int i = 0; i < mmu_size; i++)
		{
			EXPECT_EQ(ans[i], output[i]);
		}
	}

	TEST(AccumTest, TwoMacAccmResultTest) {
		int mmu_size = 4;
		int matrix_size = 2;

		Memory hm(mmu_size, 50);
		Memory dram(mmu_size, 50);
		Unified_Buffer ub(mmu_size, 2);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);
		MMU mmu(mmu_size);
		Accumulator acc(mmu_size, 50);

		ub.hm = &hm;
		wf.dram = &dram;
		ss.ub = &ub;
		mmu.ss = &ss;
		mmu.wf = &wf;
		acc.mmu = &mmu;

		int8_t input[4][4] =
		{
			{1,2, 0, 0},
			{2,3, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				hm.mem_block[i][j] = input[i][j];
			}
		}

		int8_t weight[4][4] =
		{
			{1,2, 0, 0},
			{5,4, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				dram.mem_block[i][j] = weight[i][j];
			}
		}

		for (int i = 0; i < 13; i++)
		{
			// 0 1 ub read & ss program
			// 1 wf read
			// 2, 3, 4, 5 wf push (Auto push)
			// 6, 7, 8, 9, 10, 11 mmu cal

			//Control setting
			switch (i)
			{
			case 0: //UB Read & SS Program
				ub.read_en = true;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = matrix_size;

				ss.read_en = true;
				ss.push_en = false;
				ss.switch_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			case 1: //WF Read
				ub.read_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = true;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			case 6: //MMU cal
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = true;
				ss.switch_en = true;
				ss.overwrite_en = true;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;

			default: //NOP
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;

				break;
			}

			//Register update
			ub.read_vector_from_HM_when_enable();
			ss.push_vectors_to_MMU_when_enable();
			ss.read_vector_from_UB_when_enable();
			wf.push_weight_vector_to_MMU_when_en();
			wf.read_matrix_from_DRAM_when_en();
			acc.write_results();

			//Combination Logic
			mmu.setup_array();
			mmu.calculate();

			switch (i)
			{
			case 10:
			{
				int answer[2] = { 5,0 };
				int dst[4];
				acc.read(dst, 0);
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], dst[j]) << "Accm failed in step " << i << ", " << j;
				break;
			}
			case 11:
			{
				int answer[2] = { 8,13 };
				int dst[4];
				acc.read(dst, 1);
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], dst[j]) << "Accm failed in step " << i << ", " << j;
				break;
			}
			case 12:
			{
				int answer[2] = { 0,22 };
				int dst[4];
				acc.read(dst, 2);
				for (int j = 0; j < matrix_size; j++)
					EXPECT_EQ(answer[j], dst[j]) << "Accm failed in step " << i << ", " << j;
				break;
			}
			}
		}
	}

	TEST(ModuleTest, SizeTwoModuleTest) {
		int mmu_size = 4;
		int matrix_size = 2;

		Memory hm(mmu_size, 50);
		Memory dram(mmu_size, 50);
		Weight_Size_Reg wsreg;
		Unified_Buffer ub(mmu_size, 4);
		Systolic_Setup ss(mmu_size);
		Weight_FIFO wf(mmu_size);
		MMU mmu(mmu_size);
		Accumulator acc(mmu_size, 50);
		Activation act(mmu_size);

		ub.hm = &hm;
		wf.dram = &dram;
		wf.wsreg = &wsreg;
		ss.ub = &ub;
		ss.wsreg = &wsreg;
		mmu.ss = &ss;
		mmu.wf = &wf;
		acc.mmu = &mmu;
		act.acc = &acc;

		int8_t input[4][4] =
		{
			{1,2, 0, 0},
			{2,3, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				hm.mem_block[i][j] = input[i][j];
			}
		}

		int8_t weight[4][4] =
		{
			{1,2, 0, 0},
			{5,4, 0, 0},
			{0,0, 0, 0},
			{0,0, 0, 0}
		};

		for (int i = 0; i < mmu_size; i++)
		{
			for (int j = 0; j < mmu_size; j++)
			{
				dram.mem_block[i][j] = weight[i][j];
			}
		}

		int8_t answer[2][2] =
		{
			{5,8},
			{13,22}
		};

		for (int i = 0; i < 15; i++)
		{
			// 0 1 ub read
			// 1 wf read
			// 2, 3, 4, 5 wf push (Auto push)
			// 6, 7, 8, 9, 10, 11 12 mmu cal & ss program(~11) and accm write(~12)
			// 13, 14 act
			// 15, 16 write to hm

			//Control setting
			switch (i)
			{
			case 0: //UB Read & SS Program
				ub.read_en = true;
				ub.write_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = matrix_size;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				wf.matrix_size = matrix_size;

				act.act_en = false;
				act.fold_en = false;
				act.matrix_size = 0;
				act.acc_addr = 0;
				act.ub_addr = 0;

				break;

			case 1: //WF Read
				ub.read_en = false;
				ub.write_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = true;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				wf.matrix_size = 2;

				act.act_en = false;
				act.fold_en = false;
				act.matrix_size = 0;
				act.acc_addr = 0;
				act.ub_addr = 0;

				break;

			case 6: //MMU cal
				ub.read_en = false;
				ub.write_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = true;
				ss.push_en = true;
				ss.switch_en = true;
				ss.overwrite_en = true;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = matrix_size;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				wf.matrix_size = 0;

				act.act_en = false;
				act.fold_en = false;
				act.matrix_size = 0;
				act.acc_addr = 0;
				act.ub_addr = 0;

				break;
			case 13: //Act
				ub.read_en = false;
				ub.write_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				wf.matrix_size = 0;

				act.act_en = true;
				act.fold_en = false;
				act.matrix_size = matrix_size;
				act.acc_addr = 0;
				act.ub_addr = 2;

				break;
			case 15: //Write Host Mem
				ub.read_en = false;
				ub.write_en = true;
				ub.addr = 2;
				ub.hm_addr = 0;
				ub.matrix_size = matrix_size;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				wf.matrix_size = 0;

				act.act_en = false;
				act.fold_en = false;
				act.matrix_size = 0;
				act.acc_addr = 0;
				act.ub_addr = 0;

				break;
			default: //NOP
				ub.read_en = false;
				ub.write_en = false;
				ub.addr = 0;
				ub.hm_addr = 0;
				ub.matrix_size = 0;

				ss.read_en = false;
				ss.push_en = false;
				ss.switch_en = false;
				ss.unfold_en = false;
				ss.ub_addr = 0;
				ss.acc_addr_in = 0;
				ss.matrix_size = 0;

				wf.push_en = false;
				wf.read_en = false;
				wf.unfold_en = false;
				wf.dram_addr = 0;
				wf.matrix_size = 0;

				act.act_en = false;
				act.fold_en = false;
				act.matrix_size = 0;
				act.acc_addr = 0;
				act.ub_addr = 0;
				break;
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

			switch (i)
			{
			case 14:
			{
				for (int i = 0; i < matrix_size; i++)
				{
					for (int j = 0; j < matrix_size; j++)
					{
						EXPECT_EQ(answer[i][j], ub.mem_block[i + 2][j]) << "Act failed " << i << ", " << j;
					}
				}
				break;
			}
			case 16:
			{
				for (int i = 0; i < matrix_size; i++)
				{
					for (int j = 0; j < matrix_size; j++)
					{
						EXPECT_EQ(answer[i][j], hm.mem_block[i][j]) << "Host mem write failed " << i << ", " << j;
					}
				}
				break;
			}
			}
		}
	}

	TEST(DecoderTest, ParseTest) {
		Decoder decoder;

		string delimiter(" ");
		string instruction("RHM 0 1 8");
		decoder.parse(instruction, delimiter);

		EXPECT_TRUE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(0, decoder.values["ub.hm_addr"]);
		EXPECT_EQ(1, decoder.values["ub.addr"]);
		EXPECT_EQ(8, decoder.values["ub.matrix_size"]);

		instruction = "WHM 0 1 8";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_TRUE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(0, decoder.values["ub.addr"]);
		EXPECT_EQ(1, decoder.values["ub.hm_addr"]);
		EXPECT_EQ(8, decoder.values["ub.matrix_size"]);

		instruction = "ACT 3 4 8";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_TRUE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(3, decoder.values["act.acc_addr"]);
		EXPECT_EQ(4, decoder.values["act.ub_addr"]);
		EXPECT_EQ(8, decoder.values["act.matrix_size"]);

		instruction = "HLT";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_TRUE(decoder.controls["halt"]);

		instruction = "WRONG 1 3 4";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_TRUE(decoder.controls["halt"]);
	}

	TEST(DecoderTest, MMCParseTest) {
		Decoder decoder;

		string delimiter(" ");
		string instruction = "MMC.S 1 2 8";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_TRUE(decoder.controls["ss.read_en"]);
		EXPECT_TRUE(decoder.controls["ss.push_en"]);
		EXPECT_TRUE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_TRUE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(1, decoder.values["ss.ub_addr"]);
		EXPECT_EQ(2, decoder.values["ss.acc_addr_in"]);
		EXPECT_EQ(8, decoder.values["ss.matrix_size"]);

		instruction = "MMC.O 1 2 8";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_TRUE(decoder.controls["ss.read_en"]);
		EXPECT_TRUE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_TRUE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(1, decoder.values["ss.ub_addr"]);
		EXPECT_EQ(2, decoder.values["ss.acc_addr_in"]);
		EXPECT_EQ(8, decoder.values["ss.matrix_size"]);

		instruction = "MMC.U 1 2 8";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_TRUE(decoder.controls["ss.read_en"]);
		EXPECT_TRUE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_TRUE(decoder.controls["ss.unfold_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(1, decoder.values["ss.ub_addr"]);
		EXPECT_EQ(2, decoder.values["ss.acc_addr_in"]);
		EXPECT_EQ(8, decoder.values["ss.matrix_size"]);

		instruction = "MMC 1 2 8";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_TRUE(decoder.controls["ss.read_en"]);
		EXPECT_TRUE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(1, decoder.values["ss.ub_addr"]);
		EXPECT_EQ(2, decoder.values["ss.acc_addr_in"]);
		EXPECT_EQ(8, decoder.values["ss.matrix_size"]);

		instruction = "MMC.SO 2 3 9";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_TRUE(decoder.controls["ss.read_en"]);
		EXPECT_TRUE(decoder.controls["ss.push_en"]);
		EXPECT_TRUE(decoder.controls["ss.switch_en"]);
		EXPECT_TRUE(decoder.controls["ss.overwrite_en"]);
		EXPECT_TRUE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(2, decoder.values["ss.ub_addr"]);
		EXPECT_EQ(3, decoder.values["ss.acc_addr_in"]);
		EXPECT_EQ(9, decoder.values["ss.matrix_size"]);

		instruction = "MMC.SOU 2 3 9";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_TRUE(decoder.controls["ss.read_en"]);
		EXPECT_TRUE(decoder.controls["ss.push_en"]);
		EXPECT_TRUE(decoder.controls["ss.switch_en"]);
		EXPECT_TRUE(decoder.controls["ss.overwrite_en"]);
		EXPECT_TRUE(decoder.controls["ss.unfold_en"]);
		EXPECT_TRUE(decoder.controls["wf.push_en"]);
		EXPECT_FALSE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(2, decoder.values["ss.ub_addr"]);
		EXPECT_EQ(3, decoder.values["ss.acc_addr_in"]);
		EXPECT_EQ(9, decoder.values["ss.matrix_size"]);
	}

	TEST(DecoderTest, RWParseTest) {
		Decoder decoder;

		string delimiter(" ");
		string instruction = "RW 2 3";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_TRUE(decoder.controls["wf.read_en"]);
		EXPECT_FALSE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(2, decoder.values["wf.dram_addr"]);
		EXPECT_EQ(3, decoder.values["wf.matrix_size"]);

		instruction = "RW.U 2 4";
		decoder.parse(instruction, delimiter);

		EXPECT_FALSE(decoder.controls["ub.read_en"]);
		EXPECT_FALSE(decoder.controls["ub.write_en"]);
		EXPECT_FALSE(decoder.controls["ss.read_en"]);
		EXPECT_FALSE(decoder.controls["ss.push_en"]);
		EXPECT_FALSE(decoder.controls["ss.switch_en"]);
		EXPECT_FALSE(decoder.controls["ss.overwrite_en"]);
		EXPECT_FALSE(decoder.controls["wf.push_en"]);
		EXPECT_TRUE(decoder.controls["wf.read_en"]);
		EXPECT_TRUE(decoder.controls["wf.unfold_en"]);
		EXPECT_FALSE(decoder.controls["act.act_en"]);
		EXPECT_FALSE(decoder.controls["halt"]);

		EXPECT_EQ(2, decoder.values["wf.dram_addr"]);
		EXPECT_EQ(4, decoder.values["wf.matrix_size"]);
	}
}

