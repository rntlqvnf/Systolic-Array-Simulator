#include "pch.h"
#include "../Systolic-Array-Simulator-2/MAC.h"
#include "../Systolic-Array-Simulator-2/MAC.cpp"

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
}