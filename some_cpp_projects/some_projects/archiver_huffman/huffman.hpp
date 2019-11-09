#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <queue>
#include <algorithm>

enum class file_state {FOR_ENCODE, FOR_DECODE};

namespace util_funcs
{
    bool           getKthBit(unsigned char c, uint8_t k);
    char*          uintToBytes(uint32_t& x);
    uint32_t       bytesToUint(unsigned char* bytes);
    void           writeUint(std::ofstream& outF, uint32_t x);
    uint32_t       readUint(std::ifstream& inF);
}


class ParsedCommand
{
public:
    ParsedCommand(int argc, char** argv);
    const std::string& getInFileName()        const;
    const std::string& getOutFilename()       const;
    file_state         getFileState()         const;
    bool               usedCodesAreRequired() const;

private:
    std::string inFilename_;
    std::string outFilename_;
    file_state  state_;
    bool        usedCodesRequired_;
};


class binaryTree
{
public:
    class node
    {
    public:
        explicit node(unsigned char ch='d', node* leftChild=nullptr, node* rightChild=nullptr);
        unsigned char getChar()       const;
        node*         getLeftChild()  const;
        node*         getRightChild() const;
        void          setChar(unsigned char c);
        void          setLeftChild(node* n);
        void          setRightChild(node* n);

    private:
        unsigned char ch_;
        node*         leftChild_;
        node*         rightChild_;
    };

    node* getTopPtr() const;
    void  setTopPtr(node* n);
    explicit binaryTree(node* topPtr=nullptr);
    ~binaryTree();

private:
    node* topPtr_;
    void destroyTree(node* curr);
};


class charSetFreq
{
public:
    charSetFreq(unsigned char ch, uint32_t frequency, binaryTree::node* treeAddress= nullptr);
    charSetFreq(std::string s, uint32_t frequency, binaryTree::node* treeAddress   = nullptr);
    bool operator < (const charSetFreq &other)  const;
    binaryTree::node*  getTreeAddress()         const;
    uint32_t           getFrequency()           const;
    const std::string& getCharSet()             const;

private:
    std::string       charSet_;
    uint32_t          frequency_;
    binaryTree::node* treeAddress_;
};


class huffman
{
public:
    class fileEncoder
    {
    public:
        fileEncoder();
        unsigned char readNextByte(huffman& obj);
        bool          isValid() const;

    private:
        std::string remainder_;
        bool        valid_;
    };


    class fileDecoder
    {
    public:
        fileDecoder();
        unsigned char            readNextChar(huffman& archiver);
        static binaryTree::node* walk(binaryTree::node* curr_node, bool bit);

    private:
        uint8_t start_bit_;
        char    curr_char_;
    };


    huffman(std::string inFilename, std::string outFilename, file_state state);
    void Encode();
    void Decode();
    uint32_t getEncodedSize()    const;
    uint32_t getNonComprSize()   const;
    uint32_t getAdditionalSize() const;
    void     ShowUsedCodes()     const;
    ~huffman() = default;

private:
    std::priority_queue<charSetFreq, std::vector<charSetFreq>> getFrequencies(file_state state);
    void getDecodeTreeAndCodesMap(std::priority_queue<charSetFreq, std::vector<charSetFreq>>);
    void dumpFreqTable(std::vector<charSetFreq>& base, uint32_t dumpedTableSize);

    std::unordered_map<unsigned char,std::string> codesMap_;
    binaryTree                                    decodeTree_;

    std::ifstream                                 inF_;
    std::ofstream                                 outF_;

    uint32_t                                      encodedSize_;
    uint32_t                                      nonComprSize_;
    uint32_t                                      additionalSize_;

};
