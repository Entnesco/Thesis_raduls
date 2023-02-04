#include <fstream>

using namespace std;

void save_data(string data, string filename)
{
    fstream plik;

    plik.open(filename, ios::app);
    if (plik.good() == true)
    {
        plik << data << "\n";
        plik.close();
    }
}

void delete_content(string filename)
{
    fstream plik(filename, std::ios::out | std::ios::trunc);
    plik.close();
}