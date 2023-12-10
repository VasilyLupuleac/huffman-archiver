#include "huffman_test.h"

void HuffmanTest::test_build_empty()
{
    HuffTree tree;
    tree.build();
    DO_CHECK(!(tree.get_root().get()));
}

void HuffmanTest::test_build_one_symbol()
{
    HuffTree tree;
    for (int i = 0; i < 10; i++)
        tree.add_symbol('c');
    tree.build();

    std::shared_ptr<typename HuffTree::TreeNode> root = tree.get_root();
    DO_CHECK(root->weight == 10);
    DO_CHECK(root->is_symbol);
    DO_CHECK(root->symbol == 'c');
}

void HuffmanTest::test_build_general()
{
    HuffTree tree;
    for (int i = 0; i < 1; i++)
        tree.add_symbol('a');
    for (int i = 0; i < 2; i++)
        tree.add_symbol(' ');
    for (int i = 0; i < 4; i++)
        tree.add_symbol('\n');
    tree.build();

    std::shared_ptr<HuffTree::TreeNode> root = tree.get_root();
    DO_CHECK(root->weight == 7);
    DO_CHECK(!(root->is_symbol));

    DO_CHECK(root->right->weight == 4);
    DO_CHECK(root->right->is_symbol);
    DO_CHECK(root->right->symbol == '\n');

    std::shared_ptr<HuffTree::TreeNode> left = root->left;
    DO_CHECK(left->weight == 3);
    DO_CHECK(!(left->is_symbol));

    DO_CHECK(left->right->weight == 2);
    DO_CHECK(left->right->is_symbol);
    DO_CHECK(left->right->symbol == ' ');

    DO_CHECK(left->left->weight == 1);
    DO_CHECK(left->left->is_symbol);
    DO_CHECK(left->left->symbol == 'a');
}


void HuffmanTest::test_assign_codes_empty()
{
    HuffTree tree;
    tree.build();
    Header header = tree.make_header();
    DO_CHECK(header.code.empty());
    DO_CHECK(header.number_of_letters == 0);
    DO_CHECK(header.text_length == 0);
}

void HuffmanTest::test_assign_codes_one_symbol()
{
    HuffTree tree;
    for (int i = 0; i < 5; i++)
        tree.add_symbol('a');
    tree.build();
    Header header = tree.make_header();
    DO_CHECK(header.code.size() == 1);
    DO_CHECK(header.code['a'].size() == 1);
    DO_CHECK(header.code['a'][0] == 0);
    DO_CHECK(header.number_of_letters == 1);
    DO_CHECK(header.text_length == 5);
}

void HuffmanTest::test_assign_codes_general()
{
    HuffTree tree;
    for (int i = 0; i < 2; i++)
        tree.add_symbol('0');
    for (int i = 0; i < 8; i++)
        tree.add_symbol('a');
    for (int i = 0; i < 9; i++)
        tree.add_symbol('/');
    for (int i = 0; i < 20; i++)
        tree.add_symbol(0);

    tree.build();

    Header header = tree.make_header();
    DO_CHECK(header.code.size() == 4);
    DO_CHECK(header.code[0].size() == 1);
    DO_CHECK(header.code['/'].size() == 2);
    DO_CHECK(header.code['a'].size() == 3);
    DO_CHECK(header.code['0'].size() == 3);

    char used_first_bit = header.code[0][0];
    DO_CHECK(header.code['/'][0] != used_first_bit);
    DO_CHECK(header.code['a'][0] != used_first_bit);
    DO_CHECK(header.code['0'][0] != used_first_bit);

    char used_second_bit = header.code['/'][0];
    DO_CHECK(header.code['a'][1] != used_second_bit);
    DO_CHECK(header.code['0'][1] != used_second_bit);

    DO_CHECK(header.code['a'][2] != header.code['0'][2]);

    DO_CHECK(header.number_of_letters == 4);
    DO_CHECK(header.text_length == 1 * 20 + 2 * 9 + 3 * 8 + 3 * 2);
}


void HuffmanTest::run_all_tests()
{
    test_build_empty();
    test_build_one_symbol();
    test_build_general();

    test_assign_codes_empty();
    test_assign_codes_one_symbol();
    test_assign_codes_general();
}

