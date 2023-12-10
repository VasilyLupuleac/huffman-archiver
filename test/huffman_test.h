#pragma once

#include "autotest.h"
#include "huffman.h"

class HuffmanTest : public Test
{
private:
    void test_build_empty();
    void test_build_one_symbol();
    void test_build_general();

    void test_assign_codes_empty();
    void test_assign_codes_one_symbol();
    void test_assign_codes_general();

public:
    void run_all_tests();
};
