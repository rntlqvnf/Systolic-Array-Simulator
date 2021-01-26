#include "pch.h"
#include "CppUnitTest.h"
#include <fstream>
#include <iostream>
#include "../Systolic-Array-Simulator/Matrix.h"
#include "../Systolic-Array-Simulator/mac.h"
#include "../Systolic-Array-Simulator/mac.cpp"
#include "../Systolic-Array-Simulator/mmu.h"
#include "../Systolic-Array-Simulator/mmu.cpp"
#include "../Systolic-Array-Simulator/Systolic_Setup.h"
#include "../Systolic-Array-Simulator/Systolic_Setup.cpp"
#include "../Systolic-Array-Simulator/Weight_FIFO.h"
#include "../Systolic-Array-Simulator/Weight_FIFO.cpp"

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
			int8_t input_datas[MAT_HEIGHT] = {};
			input_datas[MAT_HEIGHT - 1] = 10;

			int8_t input_weights[MAT_WIDTH] = {};
			input_weights[0] = 5;

			bool switch_flags_false[MAT_HEIGHT];
			bool switch_flags_true[MAT_HEIGHT];
			std::fill(switch_flags_false, switch_flags_false + MAT_WIDTH, false);
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
			bool switch_flags_true[MAT_HEIGHT];
			std::fill(switch_flags_false, switch_flags_false + MAT_WIDTH, false);
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

		TEST_METHOD(TWO_MAC)
		{
			int8_t input_datas[MAT_HEIGHT] = { 0 };
			int8_t input_weights[MAT_WIDTH] = { 0 };

			bool switch_flags_false[MAT_HEIGHT];
			bool switch_flags_true[MAT_HEIGHT];
			std::fill(switch_flags_false, switch_flags_false + MAT_WIDTH, false);
			std::fill(switch_flags_true, switch_flags_true + MAT_WIDTH, true);

			MMU mmu;

			mmu.set_input_weights(input_weights)
				.set_switch_flags(switch_flags_false)
				.set_write_flag(true);

			for (int i = 0; i < MAT_HEIGHT - 1; i++)
			{
				mmu.tick();
			}

			input_weights[0] = 5;
			input_weights[1] = 10;
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

			for (int i = 0; i < MAT_HEIGHT - 1; i++)
			{
				mmu.tick();
			}

			Assert::AreEqual(50, mmu.get_output_sums()[0]);

			mmu.tick();

			Assert::AreEqual(100, mmu.get_output_sums()[1]);
		}
	};

	TEST_CLASS(SYSTOLIC_SETUP_TEST)
	{
	public:
		TEST_METHOD(CONVERSION_TEST)
		{
			/* Must convert well (Weight stationary) */

			int8_t arr[3][3] =
			{
				{1,2,3},
				{0,1,0},
				{1,4,2}
			};

			int8_t answer[3][5] =
			{
				{0,0,3,2,1},
				{0,0,1,0,0},
				{2,4,1,0,0}
			};

			Matrix<int8_t> mat(3, 3, *arr);

			Systolic_Setup ss;
			ss.set_mat_and_diagonalize(mat);

			for (int i = 0; i < 5; i++)
			{
				ss.tick();
				int8_t* result = ss.get_datas_to_in();

				for (int j = 0; j < 3; j++)
				{
					wchar_t wt[256];
					swprintf_s(wt, L"%d : %d", i, j);
					Assert::AreEqual(answer[j][4-i], result[j], wt);
				}
			}
		}
	};

	TEST_CLASS(WEIGHT_FIFO_TEST)
	{
	public:
		TEST_METHOD(POP_TEST)
		{
			/* FIFO behavior test */

			int8_t arr[3][3] =
			{
				{1,2,3},
				{0,1,0},
				{1,4,2}
			};


			int8_t arr2[3][3] =
			{
				{5,9,3},
				{1,1,0},
				{1,2,2}
			};

			Matrix<int8_t> mat(3, 3, *arr);
			Matrix<int8_t> mat2(3, 3, *arr2);
			mat = helper::zero_padding_matrix(mat, MAT_HEIGHT, MAT_WIDTH);
			mat2 = helper::zero_padding_matrix(mat2, MAT_HEIGHT, MAT_WIDTH);

			Weight_FIFO wf;
			wf.push(mat);
			wf.push(mat2);
			Matrix<int8_t> result = wf.pop();

			Assert::AreEqual(mat.get_height(), result.get_height());
			Assert::AreEqual(mat.get_width(), result.get_width());
			for (int i = 0; i < mat.get_height(); i++)
			{
				for (int j = 0; j < mat.get_width(); j++)
				{
					Assert::AreEqual(mat.get_matrix()[i][j], result.get_matrix()[i][j]);
				}
			}
		}
	};

	TEST_CLASS(MODULE_TEST)
	{
	public:
		TEST_METHOD(MATRIX_MULT)
		{
			/* 3 x 3 mult full version (without accm. To use accm, need ISA) */

			int8_t weight[3][3] =
			{
				{0,1,1},
				{0,1,0},
				{1,0,1}
			};

			int8_t input[3][3] =
			{
				{4,2,1},
				{3,0,2},
				{0,0,3}
			};

			int8_t answer[3][3] =
			{
				{3,0,5},
				{3,0,2},
				{4,2,4}
			};

			Matrix<int8_t> weight_mat(3, 3, *weight);
			Matrix<int8_t> input_mat(3, 3, *input);
			Matrix<int8_t> answer_mat(3, 3, *answer);
			weight_mat = helper::zero_padding_matrix(weight_mat, MAT_HEIGHT, MAT_WIDTH);
			input_mat = helper::zero_padding_matrix(input_mat, MAT_HEIGHT, MAT_WIDTH);
			answer_mat = helper::zero_padding_matrix(answer_mat, MAT_HEIGHT, MAT_WIDTH);

			bool switch_flags_false[MAT_HEIGHT];
			bool switch_flags_true[MAT_HEIGHT];
			std::fill(switch_flags_false, switch_flags_false + MAT_WIDTH, false);
			std::fill(switch_flags_true, switch_flags_true + MAT_WIDTH, true);

			Weight_FIFO wf;
			Systolic_Setup ss;
			MMU mmu;
			
			wf.push(weight_mat);
			Matrix<int8_t> transposed_weight_mat = helper::transpose_matrix(wf.pop());
			for (int i = 0; i < transposed_weight_mat.get_height(); i++)
			{
				mmu.set_input_weights(transposed_weight_mat.get_matrix()[transposed_weight_mat.get_height() - 1 - i])
					.set_switch_flags(switch_flags_false)
					.set_write_flag(true); 
				mmu.tick();
			}

			//TODO: switch flags integrate behavior (Current, need on -> off redundant task)
			ss.set_mat_and_diagonalize(input_mat);
			mmu.set_input_datas(ss.get_datas_to_in())
				.set_switch_flags(switch_flags_true)
				.set_write_flag(false);

			ss.tick();
			mmu.tick();

			// TODO: Conver to Assert statement
			// Current: See file (passed)

			std::ofstream ofile2("result.txt");
			for (int i = 0; i < DIAG_WIDTH - 1; i++)
			{
				mmu.set_input_datas(ss.get_datas_to_in())
					.set_switch_flags(switch_flags_false)
					.set_write_flag(false);

				ss.tick();
				mmu.tick();

				if (ofile2.is_open())
				{
					for(int j = 0; j < MAT_WIDTH; j++)
						ofile2 << mmu.get_output_sums()[j] << " ";
				}
				ofile2 << std::endl;
			}

			ofile2.close();
		}
	};
}
