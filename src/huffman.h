#pragma once

#include <vector>
#include <memory>
#include <fstream>
#include <map>
#include <string>
#include <tuple>

const int BITS = 8;

class BitOfstreamBuffer
{
public:
    BitOfstreamBuffer(std::ofstream& fout);
    void flush();
    void push(const std::vector<bool>& word);
private:
    std::ofstream& fout;
    std::vector<bool> buffer;
};

struct Header
{
    int get_table_size();
    int32_t number_of_letters;
    int32_t text_length;
    std::map<char, std::vector<bool> > code;
};


class HuffTree
{
public:
    class TreeNode
    {
    public:
        TreeNode(char symbol, int weight);
        TreeNode(std::shared_ptr<TreeNode> left, std::shared_ptr<TreeNode> right);
        void assign_code(Header& header, bool is_root = false);
    public:
        int weight;
        char symbol;
        bool is_symbol;
        std::shared_ptr<TreeNode> left, right;
    };
public:
    void add_symbol(char symb);
    void build();
    Header make_header();
    std::shared_ptr<TreeNode> get_root() const;
private:

    std::map<char, int> frequency;
    std::shared_ptr<TreeNode> root;
};

class HuffmanException: public std::exception
{
public:
    HuffmanException(const std::string& msg);
    std::string msg() const;
private:
    std::string _msg;
};

class HuffmanArchiver
{
public:
    HuffmanArchiver(const char* input_file_name, const char* output_file_name);
    ~HuffmanArchiver();
    std::tuple<int, int, int> unzip();
    std::tuple<int, int, int> zip();
private:
    const char* input_file_name;
    std::ofstream fout;
    std::ifstream fin;
    HuffTree tree;
    std::map<std::vector<bool>, char> symbol_by_code;
};