#pragma once

#include <cstddef>

#define DO_CHECK(EXPR) check(EXPR, __FUNCTION__, __FILE__, __LINE__)

class Test
{
protected:
    static int failed_num;
    static int total_num;
public:
    static void check(bool expr, const char *func, const char *filename, std::size_t line_num);
    static void show_final_result();
    virtual void run_all_tests() = 0;
};
