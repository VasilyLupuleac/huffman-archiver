#include "huffman.h"
#include <iostream>
#include <cstring>

using namespace std;


int main(int argc, char** argv) {
    if (argc != 6 || strcmp(argv[2], "-f") || strcmp(argv[4], "-o") ||
        (strcmp(argv[1], "-c") && strcmp(argv[1], "-u")))
    {
        cerr << "Usage: ./huffman -c/-u -f input_file -o output_file" << endl;
        return 1;
    }
    tuple<int, int, int> info;
    try
    {
        HuffmanArchiver archiver(argv[3], argv[5]);
        if (!strcmp(argv[1], "-c"))
            info = archiver.zip();
        else
            info = archiver.unzip();
    }
    catch (HuffmanException &ex)
    {
        cerr << ex.msg() << endl;
        return 0;
    }
    cout << get<0>(info) << "\n" << get<1>(info) << "\n" << get<2>(info) << endl;

    return 0;
}
