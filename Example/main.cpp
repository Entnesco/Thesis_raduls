#include <iostream>
#include <chrono>
#include <random>
#include "raduls.h"

int main()
{
	uint64_t n_recs = 100000000;
	uint32_t key_size = 8;
	std::mt19937_64 gen;
	std::uniform_int_distribution<uint64_t> dis(0, ~0ull);
	auto _raw_input = new uint8_t[n_recs * sizeof(uint64_t) + raduls::ALIGNMENT];
	auto _raw_tmp = new uint8_t[n_recs * sizeof(uint64_t) + raduls::ALIGNMENT];
	auto input = reinterpret_cast<uint64_t*>(_raw_input);
	auto tmp = reinterpret_cast<uint64_t*>(_raw_tmp);
	while (reinterpret_cast<uintptr_t>(input) % raduls::ALIGNMENT) ++input;
	while (reinterpret_cast<uintptr_t>(tmp) % raduls::ALIGNMENT) ++tmp;
	for (uint64_t i = 0; i < n_recs; ++i)
	{
		input[i] = dis(gen) & (~0ull >> ((8 - key_size) * 8));
		//std::cerr << input[i] << " ";
	}
	auto n_threads = std::thread::hardware_concurrency();
	raduls::CleanTmpArray(reinterpret_cast<uint8_t*>(tmp), n_recs, sizeof(uint64_t), n_threads);
	auto start = std::chrono::high_resolution_clock::now();
	raduls::RadixSortMSD(reinterpret_cast<uint8_t*>(input), reinterpret_cast<uint8_t*>(tmp), n_recs, sizeof(uint64_t), key_size, n_threads);
	std::cerr << "\nTime: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() << "\n";
	auto result = key_size % 2 ? tmp : input;
	//for (uint64_t i = 0; i < n_recs; ++i)
	//	std::cerr << result[i] << " ";
	delete[] _raw_input; delete[] _raw_tmp;
}