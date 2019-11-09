#include <iostream>
#include "huffman.hpp"
#include <cstdlib>

int main(int argc, char ** argv)
{
    try
    {
        ParsedCommand commands(argc, argv);
        huffman archiver(commands.getInFileName(), commands.getOutFilename(), commands.getFileState());

        if (commands.getFileState() == file_state::FOR_ENCODE)
        {
            archiver.Encode();
            std::cout << archiver.getNonComprSize()   << std::endl;
            std::cout << archiver.getEncodedSize()    << std::endl;
            std::cout << archiver.getAdditionalSize() << std::endl;
        }
        else
        {
            archiver.Decode();
            std::cout << archiver.getEncodedSize()    << std::endl;
            std::cout << archiver.getNonComprSize()    << std::endl;
            std::cout << archiver.getAdditionalSize() << std::endl;
        }

        if (commands.usedCodesAreRequired())
        {
            archiver.ShowUsedCodes();
        }
    }
    catch(std::invalid_argument const& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(std::runtime_error const& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}





