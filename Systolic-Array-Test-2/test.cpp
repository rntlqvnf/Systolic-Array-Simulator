#include "pch.h"
#include "../Systolic-Array-Simulator-2/MAC.h"
#include "../Systolic-Array-Simulator-2/MAC.cpp"
#include "../Systolic-Array-Simulator-2/Systolic_Setup.h"
#include "../Systolic-Array-Simulator-2/Systolic_Setup.cpp"


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
		ss.program_input_vector();

		for (int i = 0; i < matrix_size; i++)
		{
			EXPECT_EQ(ub.mem_block[0][i], ss.diagonalized_matrix[i][i]) << "Diagonalization failed in " << i;
		}
	}
}