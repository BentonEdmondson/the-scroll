#include <iostream>
#include <uPDFParser.h>
#include <uPDFParser_common.h>

int main(int argc, char** argv)
{
    uPDFParser::Parser parser;

    if (argc != 2 || std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
    {
        std::cout << "Usage : " << argv[0] << " <file>" << std::endl;
        return 0;
    }

    std::cout << "Parse " << argv[1] << std::endl;
    try
    {
        parser.parse(argv[1]);
    }
    catch(uPDFParser::Exception e)
    {
	std::cout << e.what() << std::endl;
	return -1;
    }

    return 0;
}
