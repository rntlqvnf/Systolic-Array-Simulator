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
		ss.adv_en = true;
		for (int i = 0; i < 2*matrix_size-1; i++)
		{
			ss.advance();
			EXPECT_EQ(answer[0][i], ss.input_data[0]) << "Advancing 1 failed in " << i;
			EXPECT_EQ(answer[1][i], ss.input_data[1]) << "Advancing 2 failed in " << i;
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
		ss.adv_en = true;
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
				EXPECT_EQ(copy[matrix_size - i - 1][j], wf.input_weights[j]);
			}
		}

		EXPECT_TRUE(wf.weight_queue.empty());
	}
}