#include <iostream>
#include <string>
#include "autotest.h"

using namespace std;


int Test::total_num = 0;
int Test::failed_num = 0;

void Test::check(bool expr, const char *func, const char *file, size_t line_num)
{
    if (!expr)
    {
        cout << "Test failed: " << string(func);
        cout << " in " << string(file) << ":" << line_num << endl;
        failed_num++;
    }
    total_num++;
}

void Test::show_final_result() {
    if (failed_num == 0)
        cout << "All tests passed" << endl;
    else
        cerr << "Failed " << failed_num << " of " << total_num << " tests" << endl;
}
