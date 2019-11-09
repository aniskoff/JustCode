#include "huffman.hpp"
#include <string_view>
#include <iostream>

bool util_funcs::getKthBit(unsigned char c, uint8_t k)
{
    return c & 1 << (7 - k);
}

char* util_funcs::uintToBytes(uint32_t &x)
{
    return reinterpret_cast<char*>(&x);
}

uint32_t util_funcs::bytesToUint(unsigned char* bytes)
{
    return *reinterpret_cast<uint32_t*>(bytes);
}

void util_funcs::writeUint(std::ofstream& outF, uint32_t x)
{
    const char* bytes = uintToBytes(x);
    outF.write(bytes, 4);
}

uint32_t util_funcs::readUint(std::ifstream& inF)
{
    uint32_t value = 0;
    inF.read(reinterpret_cast<char*>(&value), 4);
    return value;

}
ParsedCommand::ParsedCommand(int argc, char** argv)
        :usedCodesRequired_(false)
{
    const std::string incorrectCommands = "Error: Incorrect commands."
                                    "Commands must be: [-v] (-c|-d) input_file output_file";
    if (argc < 3 || std::string_view(argv[1])[0] != '-')
    {
        throw (std::invalid_argument(incorrectCommands));
    }
    uint8_t  next_lexeme = 1;
    if (std::string_view(argv[1]) == "-v")
    {
        usedCodesRequired_ = true;
        ++next_lexeme;
    }
    if (std::string_view(argv[next_lexeme]) == "-c" || std::string_view(argv[next_lexeme]) == "-d")
    {
        state_ = (std::string_view(argv[next_lexeme]) == "-c") ? (file_state::FOR_ENCODE) : (file_state::FOR_DECODE);
        ++next_lexeme;
    }
    else
        throw (std::invalid_argument(incorrectCommands));

    if (argc - next_lexeme != 2 )
        throw (std::invalid_argument(incorrectCommands));
    else {
        inFilename_ = static_cast<std::string>(argv[next_lexeme++]);
        outFilename_ = static_cast<std::string>(argv[next_lexeme]);
        }
}
const std::string& ParsedCommand::getInFileName()  const
{
    return inFilename_;
}

const std::string& ParsedCommand::getOutFilename() const
{
    return outFilename_;
}

file_state ParsedCommand::getFileState()   const
{
    return state_;
}

bool ParsedCommand::usedCodesAreRequired() const
{
    return usedCodesRequired_;
}



binaryTree::binaryTree(node* topPtr)
        :topPtr_(topPtr)
{}



binaryTree::node::node(unsigned char ch, node* leftChild, node* rightChild)
        :ch_(ch)
        ,leftChild_(leftChild)
        ,rightChild_(rightChild)
{}

binaryTree::node* binaryTree::getTopPtr() const
{
    return topPtr_;
}

unsigned char binaryTree::node::getChar() const
{
    return ch_;
}

binaryTree::node* binaryTree::node::getLeftChild()  const
{
    return leftChild_;
}

binaryTree::node* binaryTree::node::getRightChild() const
{
    return rightChild_;
}

void binaryTree::setTopPtr(node* n)
{
    topPtr_ = n;
}

void binaryTree::node::setChar(unsigned char c)
{
    ch_ = c;
}

void binaryTree::node::setLeftChild(node* n)
{
    leftChild_ = n;
}

void binaryTree::node::setRightChild(node* n)
{
    rightChild_ = n;
}

void binaryTree::destroyTree(node* curr)
{
    if(curr != nullptr)
    {
        destroyTree(curr->getLeftChild());
        destroyTree(curr->getRightChild());
        delete curr;
    }
}

binaryTree::~binaryTree()
{
    destroyTree(topPtr_);
}



charSetFreq::charSetFreq(unsigned char ch, uint32_t frequency, binaryTree::node* treeAddress)
        :charSet_(std::string(1, ch))
        ,frequency_(frequency)
        ,treeAddress_(treeAddress)
{}


charSetFreq::charSetFreq(std::string s, uint32_t frequency, binaryTree::node* treeAddress)
        :charSet_(s)
        ,frequency_(frequency)
        ,treeAddress_(treeAddress)
{}

bool charSetFreq::operator<(const charSetFreq &other) const
{
    return std::tie(frequency_, charSet_) > std::tie(other.frequency_, other.charSet_);
}

uint32_t charSetFreq::getFrequency()  const
{
    return frequency_;
}

const std::string& charSetFreq::getCharSet() const
{
    return charSet_;
}

binaryTree::node* charSetFreq::getTreeAddress() const
{
    return treeAddress_;
}



huffman::fileEncoder::fileEncoder()
        :remainder_("")
        ,valid_    (true)
{}

bool huffman::fileEncoder::isValid() const
{
    return valid_;
}

unsigned char huffman::fileEncoder::readNextByte(huffman& archiver)
{
    std::string futByte = remainder_;
    char tmp = 0;
    while(futByte.size() < 8 && archiver.inF_.get(tmp))
    {

        futByte += archiver.codesMap_[static_cast<unsigned char>(tmp)];
    }
    if (futByte.size() < 8)
    {
        if(futByte.empty())
        {
            valid_ = false;
            return 0;
        }
        std::string toAdd(8 - futByte.size(), '0');
        futByte  += toAdd;
        remainder_ = "";
        return static_cast<unsigned char>(stoul(futByte, nullptr, 2));
    }
    remainder_ = futByte.substr(8, futByte.size() - 8);
    return static_cast<unsigned char>(stoul(futByte.substr(0, 8), nullptr, 2));

}



huffman::fileDecoder::fileDecoder()
        :start_bit_(8)
        ,curr_char_('\0')
{}
binaryTree::node* huffman::fileDecoder::walk(binaryTree::node* curr_node, bool bit)
{
    if(bit)
        return curr_node->getRightChild();
    else
        return curr_node->getLeftChild();
}

unsigned char huffman::fileDecoder::readNextChar(huffman& archiver)
{
    using util_funcs::getKthBit;

    binaryTree::node* it = archiver.decodeTree_.getTopPtr();
    do
    {
        for (; start_bit_ < 8; ++start_bit_)
        {
            if(walk(it, getKthBit(static_cast<unsigned char>(curr_char_), start_bit_)))
                it = walk(it, getKthBit(static_cast<unsigned char>(curr_char_), start_bit_));
            else {
                return it->getChar();
            }

        }

        int32_t flag = archiver.inF_.peek();
        if (flag == EOF && start_bit_ == 8)
            return it->getChar();
        start_bit_ = start_bit_ % static_cast<uint8_t>(8);
    } while (archiver.inF_.get(curr_char_));

    throw std::runtime_error("Decoding failed."
                             "Function huffman::fileDecoder::readNextChar(huffman& archiver)"
                             "didn't return any value, it's impossible in correct decoding");
}



std::priority_queue<charSetFreq, std::vector<charSetFreq>> huffman::getFrequencies(file_state state)
{
    std::vector<charSetFreq> base;
    inF_.seekg(0, std::ios::end);
    std::streampos end= static_cast<uint32_t>(inF_.tellg());
    inF_.seekg(0, std::ios::beg);
    std::streampos allSize= static_cast<uint32_t>(end - inF_.tellg());
    if (state == file_state::FOR_ENCODE) {
        nonComprSize_ = static_cast<uint32_t>(allSize);
        std::unordered_map<unsigned char, uint32_t> charsFrequencies;
        char c;
        while (inF_.get(c))
            charsFrequencies[static_cast<unsigned char>(c)]++;

        uint32_t dumpedTableSize = 0;
        for (auto cF : charsFrequencies)
        {
            base.emplace_back(cF.first, cF.second);
            dumpedTableSize += 1 + 4;
        }
        additionalSize_ = 4 + dumpedTableSize;
        dumpFreqTable(base, dumpedTableSize);

    } else {
        uint32_t dumpedTableSize;
        dumpedTableSize = util_funcs::readUint(inF_);
        additionalSize_ = 4 + dumpedTableSize;
        encodedSize_    = static_cast<uint32_t>(allSize) - additionalSize_;
        char ch;
        uint32_t frequency;
        while (dumpedTableSize > 0)
        {
            inF_.get(ch);
            frequency = util_funcs::readUint(inF_);
            dumpedTableSize -= (1 + 4);
            base.emplace_back(static_cast<unsigned char>(ch), frequency);
        }
        for(auto& e: base)
            nonComprSize_ += e.getFrequency();
    }
    std::priority_queue<charSetFreq, std::vector<charSetFreq>> result(base.begin(), base.end());
    return result;
}
void huffman::getDecodeTreeAndCodesMap(std::priority_queue<charSetFreq, std::vector<charSetFreq>> freqQueue)
{
    if (!freqQueue.empty())
    {
        if (freqQueue.size() == 1)
        {
            codesMap_[freqQueue.top().getCharSet()[0]] = "0";
            auto leaf = new binaryTree::node;
            auto root = new binaryTree::node;
            decodeTree_.setTopPtr(root);
            decodeTree_.getTopPtr()->setLeftChild(leaf);
            leaf->setChar(static_cast<unsigned char>(freqQueue.top().getCharSet()[0]));

        }
        else {
            while(freqQueue.size() >= 2)
            {
                auto left   = freqQueue.top();
                freqQueue.pop();
                auto right  = freqQueue.top();
                freqQueue.pop();
                for (auto c: left.getCharSet())
                    codesMap_[c] += "0";
                for (auto c: right.getCharSet())
                    codesMap_[c] += "1";

                auto currLeaf = new binaryTree::node;
                if (left.getTreeAddress() != nullptr)
                    currLeaf->setLeftChild(left.getTreeAddress());
                else {
                    auto lLeaf = new binaryTree::node;
                    lLeaf->setChar(static_cast<unsigned char>(left.getCharSet()[0]));
                    currLeaf->setLeftChild(lLeaf);
                }
                if (right.getTreeAddress() != nullptr)
                    currLeaf->setRightChild(right.getTreeAddress());
                else {
                    auto rLeaf = new binaryTree::node;
                    rLeaf->setChar(static_cast<unsigned char>(right.getCharSet()[0]));
                    currLeaf->setRightChild(rLeaf);
                }
                charSetFreq tmp(left.getCharSet()   + right.getCharSet()
                               ,left.getFrequency() + right.getFrequency()
                               ,currLeaf);
                freqQueue.push(tmp);

            }
            decodeTree_.setTopPtr(freqQueue.top().getTreeAddress());
            freqQueue.pop();

            for (auto &it: codesMap_)
                std::reverse(it.second.begin(), it.second.end());
        }
    }
}

void huffman::dumpFreqTable(std::vector<charSetFreq>& charFrequencies, uint32_t dumpedTableSize)
{
    util_funcs::writeUint(outF_, dumpedTableSize);
    for (auto& elem: charFrequencies)
    {
        outF_.put(static_cast<unsigned char>(elem.getCharSet()[0]));
        util_funcs::writeUint(outF_, elem.getFrequency());
    }
}

huffman::huffman(std::string inFilename, std::string outFilename, file_state state)
        :inF_(inFilename, std::ios_base::in|std::ios_base::binary)
        ,outF_(outFilename, std::ios_base::out)
        ,encodedSize_   (0)
        ,nonComprSize_  (0)
        ,additionalSize_(0)

{
    if (!inF_)
    {
        throw std::invalid_argument("Failed to open input file\n");
    }
    if (!outF_)
    {
        throw std::invalid_argument("Failed to open output file\n");
    }
    getDecodeTreeAndCodesMap(getFrequencies(state));
}

void huffman::Encode()
{
    encodedSize_= 0;
    inF_.clear();
    inF_.seekg(0, std::ios_base::beg);
    fileEncoder fE;
    unsigned char byte = fE.readNextByte(*this);
    while(fE.isValid())
    {
        outF_.put(byte);
        ++encodedSize_;
        byte = fE.readNextByte(*this);
    }
}

void huffman::Decode()
{
    inF_.clear();
    inF_.seekg(additionalSize_);
    fileDecoder fD;
    for (uint32_t i = 0; i < nonComprSize_; ++i)
    {
        unsigned char ch = fD.readNextChar(*this);
        outF_.put(ch);
    }
}

uint32_t huffman::getEncodedSize() const
{
    return encodedSize_;
}

uint32_t huffman::getNonComprSize() const
{
    return nonComprSize_;
}

uint32_t huffman::getAdditionalSize() const
{
    return additionalSize_;
}

void huffman::ShowUsedCodes() const
{
    std::vector<std::pair<std::string, unsigned char>> res;
    for (auto& charAndCode: codesMap_)
        res.emplace_back(charAndCode.second, charAndCode.first);
    std::sort(res.begin(), res.end());
    for (auto& codeAndChar: res)
        std::cout << codeAndChar.first << " " << static_cast<unsigned int>(codeAndChar.second) << std::endl;
}

