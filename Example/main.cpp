#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#ifdef _MSC_VER 
#include <ppl.h>
#elif __GNUG__
#include <omp.h>
#include <parallel/algorithm>
#endif

#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include "raduls.h"
#include "data_to_file.h"

bool is_sorted(unsigned __int64* input, uint64_t n_recs);
double sorting(uint64_t n_recs, uint32_t key_size, unsigned int num_threads);

int main()
{
	//unsigned int num_threads = 6;/*std::thread::hardware_concurrency();*/
	unsigned int num_threads_min = 12;
	unsigned int num_threads_max = 12;/*std::thread::hardware_concurrency();*/
	unsigned int num_threads_step = 1;
	unsigned int testite = 1;
	uint64_t n_recs_min = 400;
	uint64_t n_recs_max = 1048576000; //rozmiar DS
	uint64_t n_recs_mult = 2;
	uint64_t n_recs_step = 0;
	uint32_t key_size = 8;

	double time, sumResults;
	for (unsigned int num_threads = num_threads_min; num_threads <= num_threads_max; num_threads+= num_threads_step)
	{
		string test_type = "Raduls_sort;lengthmin=" + to_string(n_recs_min) + ";lengthmax=" + to_string(n_recs_max) + ";lengtstep=" + to_string(n_recs_step) + ";lengtmult=" + to_string(n_recs_mult) + ";num_threads=" + to_string(num_threads) + ";testite=" + to_string(testite);
		string path = "C://Users/Michal/Desktop/Studia/RADULS-master_my/Result_files/part_2/";
		save_data(test_type, "C://Users/Michal/Desktop/Studia/RADULS-master_my/Result_files/Result.txt");
		save_data("seconds  length", "C://Users/Michal/Desktop/Studia/RADULS-master_my/Result_files/Result.txt");

		for (uint64_t n_recs = n_recs_min; n_recs <= n_recs_max; n_recs = n_recs * n_recs_mult + n_recs_step)
		{
			sumResults = 0;
			for (int testi = 0; testi < testite; testi++)
			{
				time = sorting(n_recs, key_size, num_threads);
				sumResults += time;
				save_data(to_string(time), path + test_type + ";n_recs=" + to_string(n_recs) + ".txt");
			}
			save_data(to_string(sumResults / testite) + " " + to_string(n_recs), "C://Users/Michal/Desktop/Studia/RADULS-master_my/Result_files/Result.txt");
			/*std::cerr << "Threads count: " << num_threads << " Length: " << n_recs << std::endl;*/
		}

	}
}

bool is_sorted(unsigned __int64* input, uint64_t n_recs)
{
	for (uint64_t i = 0; i < n_recs - 1; ++i)
	{
		if (input[i] > input[i + 1])
		{
			//std::cerr << input[i] << ":" << input[i+1];
			return false;
		}
	}
	return true;
}

double sorting(uint64_t n_recs, uint32_t key_size, unsigned int num_threads)
{
	std::mt19937_64 gen;
	std::uniform_int_distribution<uint64_t> dis(0, ~0ull);
	auto _raw_input = new uint8_t[n_recs * sizeof(uint64_t) + raduls::ALIGNMENT];
	auto _raw_tmp = new uint8_t[n_recs * sizeof(uint64_t) + raduls::ALIGNMENT];
	auto input = reinterpret_cast<uint64_t*>(_raw_input); //DS
	auto tmp = reinterpret_cast<uint64_t*>(_raw_tmp);
	while (reinterpret_cast<uintptr_t>(input) % raduls::ALIGNMENT) ++input;
	while (reinterpret_cast<uintptr_t>(tmp) % raduls::ALIGNMENT) ++tmp;

	//tworzenie DS do posortowania
	for (uint64_t i = 0; i < n_recs; ++i)
	{
		input[i] = dis(gen) & (~0ull >> ((8 - key_size) * 8));
		//std::cerr << input[i] << " ";
	}
	//maksymalna wspierana iloœæ w¹tków równoleg³ych
	auto n_threads = num_threads; //12 unsigned int

	raduls::CleanTmpArray(reinterpret_cast<uint8_t*>(tmp), n_recs, sizeof(uint64_t), n_threads);
	//start pomiaru czasu
	auto start = std::chrono::high_resolution_clock::now();

	raduls::RadixSortMSD(reinterpret_cast<uint8_t*>(input), reinterpret_cast<uint8_t*>(tmp), n_recs, sizeof(uint64_t), key_size, n_threads);

	//koniec pomiaru czasu oraz jego wyœwietlenie
	double time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count();
	std::cerr << "Time: " << time << "\n";

	//posortowana tablica
	auto result = key_size % 2 ? tmp : input;
	//for (uint64_t i = 0; i < n_recs; ++i)
	//	std::cerr << result[i] << " ";

	//sprawdzenie czy tablica jest posortowana
	/*std::cerr << "Czy tablica jest posortowana: " << is_sorted(result, n_recs) << " n_recs: " << n_recs;*/
	delete[] _raw_input; delete[] _raw_tmp;
	return time;
}