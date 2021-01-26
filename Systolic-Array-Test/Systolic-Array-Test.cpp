#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "../Systolic-Array-Simulator/mac.h"
#include "../Systolic-Array-Simulator/mac.cpp"
#include "../Systolic-Array-Simulator/mmu.h"
#include "../Systolic-Array-Simulator/mmu.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SystolicArrayTest
{
	TEST_CLASS(MAC_TEST)
	{
	public:
		TEST_METHOD(WITHOUT_INPUT_SUM)
		{
			int8_t input_data = 4;
			int8_t input_weight = 4;

			MAC mac;
			mac.set_input_weight(input_weight)
				.set_input_weight_tag(MAT_HEIGHT - 1)
				.set_write_flag(true);

			mac.tick();

			Assert::AreEqual((int8_t)input_weight, mac.get_output_weight());

			mac.set_input_data(input_data)
				.set_swtich_flag(true)
				.set_write_flag(false);

			mac.tick();

			Assert::AreEqual((int8_t)input_data, mac.get_output_data());
			Assert::AreEqual((int16_t)16, mac.get_output_mult());
			Assert::AreEqual((int32_t)16, mac.get_output_sum());
		}
		TEST_METHOD(WITH_INPUT_SUM)
		{
			int8_t input_data = 4;
			int8_t input_weight = 4;
			int32_t input_sum = 16;

			MAC mac;
			mac.set_input_weight(input_weight)
				.set_input_weight_tag(MAT_HEIGHT - 1)
				.set_write_flag(true);

			mac.tick();

			Assert::AreEqual(input_weight, mac.get_output_weight());

			mac.set_input_data(input_data)
				.set_input_sum(input_sum)
				.set_swtich_flag(true)
				.set_write_flag(false);

			mac.tick();

			Assert::AreEqual(input_data, mac.get_output_data());
			Assert::AreEqual((int16_t)16, mac.get_output_mult());
			Assert::AreEqual((int32_t)32, mac.get_output_sum());
		}
		TEST_METHOD(SWITCH)
		{
			int8_t input_data = 4;
			int8_t input_weight = 4;

			MAC mac;
			mac.set_input_data(input_data)
				.set_input_weight(input_weight)
				.set_input_weight_tag(MAT_HEIGHT - 1)
				.set_write_flag(true);

			mac.tick();

			Assert::AreEqual((int16_t)0, mac.get_output_mult());

			mac.set_input_data(input_data)
				.set_write_flag(false)
				.set_swtich_flag(true);

			mac.tick();

			Assert::AreEqual((int16_t)16, mac.get_output_mult());
		}
	};

	TEST_CLASS(MMU_TEST)
	{
	public:
		TEST_METHOD(ONE_MAC)
		{
			int8_t input_datas[MAT_HEIGHT] = {};
			input_datas[0] = 10;

			int8_t input_weights[MAT_WIDTH] = {};
			input_weights[0] = 5;

			/*
			MMU mmu;


			for (int i = 0; i < MAT_HEIGHT-1; i++)
			{
				mmu.tick();
				Assert::AreEqual(0, mmu.get_output_sums()[0]);
			}

			mmu.tick();
			Assert::AreEqual(50, mmu.get_output_sums()[0]);
			*/
		}
	};
}
