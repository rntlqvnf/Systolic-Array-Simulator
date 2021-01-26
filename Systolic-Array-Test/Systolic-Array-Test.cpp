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
		TEST_METHOD(OUTPUT_WEIGHT)
		{
			/* Output weight = input weight (No matter ohter settings) */

			MAC mac;
			mac.set_input_weight((int8_t)4)
				.set_write_flag(true)
				.set_switch_flag(true);

			mac.tick();

			Assert::AreEqual((int8_t)4, mac.get_output_weight());

			mac.set_input_weight((int8_t)1)
				.set_write_flag(false)
				.set_switch_flag(false);

			mac.tick();

			Assert::AreEqual((int8_t)1, mac.get_output_weight());
		}
		TEST_METHOD(WEIGHT_TAG)
		{
			/* Write weight only when tag = MAT_HEIGHT - 1 */

			int8_t input_data = 4;
			int8_t input_weight = 4;

			MAC mac;
			mac.set_input_weight(input_weight)
				.set_input_weight_tag(0)
				.set_write_flag(true);

			mac.tick();

			mac.set_input_data(input_data)
				.set_write_flag(false)
				.set_switch_flag(true);

			mac.tick();

			Assert::AreEqual((int16_t)0, mac.get_output_mult());

			mac.set_input_weight(input_weight)
				.set_input_weight_tag(MAT_HEIGHT - 1)
				.set_write_flag(true);

			mac.tick();

			mac.set_input_data(input_data)
				.set_write_flag(false)
				.set_switch_flag(true);

			mac.tick();

			Assert::AreEqual((int16_t)16, mac.get_output_mult());
		}
		TEST_METHOD(WITHOUT_INPUT_SUM)
		{
			/* MAC should work fine when input sum is none (default 0) */

			int8_t input_data = 4;
			int8_t input_weight = 4;

			MAC mac;
			mac.set_input_weight(input_weight)
				.set_input_weight_tag(MAT_HEIGHT - 1)
				.set_write_flag(true);

			mac.tick();

			Assert::AreEqual((int8_t)input_weight, mac.get_output_weight());

			mac.set_input_data(input_data)
				.set_switch_flag(true)
				.set_write_flag(false);

			mac.tick();

			Assert::AreEqual((int8_t)input_data, mac.get_output_data());
			Assert::AreEqual((int16_t)16, mac.get_output_mult());
			Assert::AreEqual((int32_t)16, mac.get_output_sum());
		}
		TEST_METHOD(WITH_INPUT_SUM)
		{
			/* MAC should work well when input sum is not none */

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
				.set_switch_flag(true)
				.set_write_flag(false);

			mac.tick();

			Assert::AreEqual(input_data, mac.get_output_data());
			Assert::AreEqual((int16_t)16, mac.get_output_mult());
			Assert::AreEqual((int32_t)32, mac.get_output_sum());
		}
		TEST_METHOD(SWITCH)
		{
			/* Double buffering. If switch flag, change buffer. */

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
				.set_switch_flag(true);

			mac.tick();

			Assert::AreEqual((int16_t)16, mac.get_output_mult());
		}
	};

	TEST_CLASS(MMU_TEST)
	{
	public:
		TEST_METHOD(ONE_MAC)
		{
			/* 
			wchar_t wt[256];
			swprintf_s(wt, L"%d", i);
			*/

			int8_t input_datas[MAT_HEIGHT] = {};
			input_datas[MAT_HEIGHT - 1] = 10;

			int8_t input_weights[MAT_WIDTH] = {};
			input_weights[0] = 5;

			bool switch_flags_false[MAT_HEIGHT];
			std::fill(switch_flags_false, switch_flags_false + MAT_WIDTH, false);
			bool switch_flags_true[MAT_HEIGHT];
			std::fill(switch_flags_true, switch_flags_true + MAT_WIDTH, true);

			MMU mmu;

			mmu.set_input_weights(input_weights)
				.set_switch_flags(switch_flags_false)
				.set_write_flag(true);

			for (int i = 0; i < MAT_HEIGHT; i++)
			{
				mmu.tick();
				Assert::AreEqual(0, mmu.get_output_sums()[0]);
			}

			mmu.set_input_datas(input_datas)
				.set_switch_flags(switch_flags_true)
				.set_write_flag(false);

			mmu.tick();

			Assert::AreEqual(50, mmu.get_output_sums()[0], L"Sum");
		}

		TEST_METHOD(ONE_MAC_2)
		{
			int8_t input_datas[MAT_HEIGHT] = {0};
			int8_t input_weights[MAT_WIDTH] = {0};

			bool switch_flags_false[MAT_HEIGHT];
			std::fill(switch_flags_false, switch_flags_false + MAT_WIDTH, false);
			bool switch_flags_true[MAT_HEIGHT];
			std::fill(switch_flags_true, switch_flags_true + MAT_WIDTH, true);

			MMU mmu;

			mmu.set_input_weights(input_weights)
				.set_switch_flags(switch_flags_false)
				.set_write_flag(true);

			for (int i = 0; i < MAT_HEIGHT-1; i++)
			{
				mmu.tick();
				Assert::AreEqual(0, mmu.get_output_sums()[0]);
			}

			input_weights[0] = 5;
			mmu.set_input_weights(input_weights);
			mmu.tick();

			input_datas[0] = 10;
			mmu.set_input_datas(input_datas)
				.set_switch_flags(switch_flags_true)
				.set_write_flag(false);
			mmu.tick();

			input_datas[0] = 0;
			mmu.set_input_datas(input_datas)
				.set_switch_flags(switch_flags_false);

			for (int i = 0; i < MAT_HEIGHT-1; i++)
			{
				mmu.tick();
			}

			Assert::AreEqual(50, mmu.get_output_sums()[0]);
		}
	};
}
