#include <fstream>
#include <queue>
#include <string>
#include "huffman.h"

using namespace std;


int Header::get_table_size()
{
    int table_size = 2 * sizeof(int32_t);
    for (auto elem: code)
        table_size += 2 * sizeof(char) + (elem.second.size() + BITS - 1) / BITS;
    return table_size;
}


BitOfstreamBuffer::BitOfstreamBuffer(ofstream& fout) : fout(fout) { }

void BitOfstreamBuffer::flush()
{
    if (buffer.empty())
        return;

    char output_block = 0;
    for (size_t i = 0; i < buffer.size(); i++)
        output_block = (output_block << 1) | buffer[i];

    output_block <<= (BITS - buffer.size());
    fout.write(&output_block, sizeof(char));
    buffer.clear();
}

void BitOfstreamBuffer::push(const vector<bool>& word)
{
    for (auto bit: word)
    {
        buffer.push_back(bit);
        if (buffer.size() % BITS == 0)
            flush();
    }
}


void check_and_read(ifstream& fin, void* data, size_t n, const string& msg)
{
    if (!fin.read(reinterpret_cast<char*>(data), n))
        throw HuffmanException(msg);
}


ifstream& operator>>(ifstream& fin, Header& header)
{
    string msg = "Error: information table too short";
    check_and_read(fin, &(header.text_length), sizeof(int32_t), msg);
    check_and_read(fin, &(header.number_of_letters), sizeof(int32_t), msg);
    uint8_t len;
    char buffer, symb;
    for (int i = 0; i < header.number_of_letters; i++)
    {
        check_and_read(fin, &symb, sizeof(char), msg);
        check_and_read(fin, &len, sizeof(uint8_t), msg);
        header.code[symb];
        header.code[symb].reserve(len);
        for (int i = 0; i < (len + BITS - 1) / BITS; i++)
        {
            check_and_read(fin, &buffer, sizeof(char), msg);
            for (int pos = BITS - 1; pos >= 0 && header.code[symb].size() < len; pos--)
                header.code[symb].push_back((buffer >> pos) & 1);
        }
    }
    return fin;
}

ofstream& operator<<(ofstream& fout, Header& header)
{
    fout.write(reinterpret_cast<const char*>(&(header.text_length)), sizeof(int32_t));
    fout.write(reinterpret_cast<const char*>(&(header.number_of_letters)), sizeof(int32_t));
    BitOfstreamBuffer buffer(fout);
    for (auto elem: header.code)
    {
        char symb = elem.first;
        uint8_t len = static_cast<uint8_t>(elem.second.size());
        fout.write(&symb, sizeof(char));
        fout.write(reinterpret_cast<char *>(&len), sizeof(uint8_t));
        buffer.push(elem.second);
        buffer.flush();
    }
    return fout;
}


HuffTree::TreeNode::TreeNode(char symbol, int weight): weight(weight), symbol(symbol)
{
    is_symbol = true;
}

HuffTree::TreeNode::TreeNode(shared_ptr<TreeNode> left, shared_ptr<TreeNode> right):
    left(left), right(right)
{
    is_symbol = false;
    weight = left->weight + right->weight;
}

void HuffTree::TreeNode::assign_code(Header& header, bool is_root)
{
    static vector<bool> current_code;
    if (is_symbol)
    {
        if (is_root)
            current_code.push_back(0);
        header.code[symbol] = current_code;
        header.text_length += weight * current_code.size();
        if (is_root)
            current_code.pop_back();
        return;
    }

    current_code.push_back(0);
    left->assign_code(header);
    current_code.back() = 1;
    right->assign_code(header);
    current_code.pop_back();
}


void HuffTree::add_symbol(char symb) { frequency[symb]++; }

void HuffTree::build()
{
    class TreeNodeComparator
    {
    public:
        bool operator()(shared_ptr<TreeNode> a, shared_ptr<TreeNode> b) { return a->weight > b->weight; }
    };

    priority_queue<shared_ptr<TreeNode>, vector<shared_ptr<TreeNode>>, TreeNodeComparator> left_vertices;
    for (auto elem : frequency)
        left_vertices.push(shared_ptr<TreeNode>(new TreeNode(elem.first, elem.second)));
    while(left_vertices.size() > 1)
    {
        shared_ptr<TreeNode> first, second;
        first = left_vertices.top();
        left_vertices.pop();
        second = left_vertices.top();
        left_vertices.pop();
        left_vertices.push(shared_ptr<TreeNode>(new TreeNode(first, second)));
    }
    root = left_vertices.empty() ? shared_ptr<TreeNode>(0) : left_vertices.top();
}

Header HuffTree::make_header()
{
    Header header;
    header.text_length = 0;
    if (root.get())
        root->assign_code(header, true);
    header.number_of_letters = static_cast<int32_t>(header.code.size());
    return header;
}

shared_ptr<HuffTree::TreeNode> HuffTree::get_root() const { return root; }

HuffmanException::HuffmanException(const string& msg) : _msg(msg) {}

string HuffmanException::msg() const { return _msg; }


HuffmanArchiver::HuffmanArchiver(const char* input_file_name, const char* output_file_name):
    input_file_name(input_file_name)
{
    fin = ifstream(input_file_name, ios::binary);
    if (!fin)
        throw HuffmanException("Error: can't open file " + string(input_file_name));
    fout = ofstream(output_file_name, ios::binary);
    if (!fout)
        throw HuffmanException("Error: can't create file " + string(output_file_name));
}

HuffmanArchiver::~HuffmanArchiver()
{
    fin.close();
    fout.close();
}

tuple<int, int, int> HuffmanArchiver::zip()
{
    char symbol;
    int file_size = 0;
    while(fin.read(&symbol, sizeof(char)))
    {
        tree.add_symbol(symbol);
        file_size++;
    }
    tree.build();
    Header header = tree.make_header();
    fout << header;
    fin.close();
    fin.open(input_file_name, ios_base::binary);

    BitOfstreamBuffer buffer(fout);
    while(fin.read(&symbol, sizeof(char)))
        buffer.push(header.code[symbol]);
    buffer.flush();

    return make_tuple(file_size, (header.text_length + BITS - 1) / BITS, header.get_table_size());
}

tuple<int, int, int> HuffmanArchiver::unzip()
{
    Header header;
    fin >> header;
    for (auto elem: header.code)
        symbol_by_code[elem.second] = elem.first;
    vector<bool> buffer;
    char input_block;
    int file_size = 0;
    size_t left_size = static_cast<size_t>(header.text_length);
    while(buffer.size() < left_size && fin.read(&input_block, 1))
    {
        for (int pos = BITS - 1; pos >= 0 && buffer.size() < left_size; pos--)
        {
            buffer.push_back((input_block >> pos) & 1);
            if (symbol_by_code.count(buffer))
            {
                fout.write(&(symbol_by_code[buffer]), sizeof(char));
                left_size -= buffer.size();
                file_size++;
                buffer.clear();
            }
        }
        if (buffer.size() > 255)
            throw HuffmanException("Error: can't decode file");
    }
    if (fin.read(&input_block, sizeof(char)) || !buffer.empty())
        throw HuffmanException("Error: can't decode file");

    return make_tuple((header.text_length + BITS - 1) / BITS, file_size, header.get_table_size());
}