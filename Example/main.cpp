#include <iostream>
#include <chrono>
#include <random>
#include "raduls.h"

bool is_sorted(unsigned __int64* input, uint64_t n_recs);
void sorting(uint64_t n_recs, uint32_t key_size);

int main()
{
	uint64_t n_recs_min = 100000000;
	uint64_t n_recs_max = 100000002; //rozmiar DS
	uint32_t key_size = 8;

	for (uint64_t n_recs = n_recs_min; n_recs < n_recs_max; n_recs++)
	{
		sorting(n_recs, key_size);
	}
}

bool is_sorted(unsigned __int64* input, uint64_t n_recs)
{
	for (uint64_t i = 0; i < n_recs-1; ++i)
	{
		if (input[i] > input[i + 1]) 
		{
			//std::cerr << input[i] << ":" << input[i+1];
			return false;
		}
	}
	return true;
}

void sorting(uint64_t n_recs, uint32_t key_size)
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
	auto n_threads = std::thread::hardware_concurrency(); //12

	raduls::CleanTmpArray(reinterpret_cast<uint8_t*>(tmp), n_recs, sizeof(uint64_t), n_threads);
	//start pomiaru czasu
	auto start = std::chrono::high_resolution_clock::now();
	raduls::RadixSortMSD(reinterpret_cast<uint8_t*>(input), reinterpret_cast<uint8_t*>(tmp), n_recs, sizeof(uint64_t), key_size, n_threads);
	//koniec pomiaru czasu oraz jego wyœwietlenie
	std::cerr << "\nTime: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() << "\n";
	//posortowana tablica
	auto result = key_size % 2 ? tmp : input;
	//for (uint64_t i = 0; i < n_recs; ++i)
	//	std::cerr << result[i] << " ";

	//sprawdzenie czy tablica jest posortowana
	std::cerr << "Czy tablica jest posortowana: " << is_sorted(result, n_recs) << " n_recs: " << n_recs;
	delete[] _raw_input; delete[] _raw_tmp;
}