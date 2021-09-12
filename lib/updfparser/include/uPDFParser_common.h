#ifndef _UPDFPARSER_COMMON_HPP_
#define _UPDFPARSER_COMMON_HPP_

#include <sstream>
#include <iomanip>
#include <string.h>

namespace uPDFParser
{
    enum PARSING_ERROR {
	UNABLE_TO_OPEN_FILE = 1,
	TRUNCATED_FILE,
	INVALID_HEADER,
	INVALID_LINE,
	INVALID_FOOTER,
	INVALID_DICTIONARY,
	INVALID_NAME,
	INVALID_BOOLEAN,
	INVALID_NUMBER,
	INVALID_STREAM,
	INVALID_TOKEN,
	INVALID_OBJECT,
	INVALID_TRAILER,
	INVALID_HEXASTRING,
	NOT_IMPLEMENTED
	
    };

    /**
     * @brief Exception class
     */
    class Exception : public std::exception
    {
    public:

	Exception(int code, const char* message, const char* file, int line):
	    code(code), line(line), file(file)
	{
	    std::stringstream msg;
	    msg << "Exception code : 0x" << std::setbase(16) << code << std::endl;
	    msg << "Message        : " << message << std::endl;
	    msg << "File           : " << file << ":" << std::setbase(10) << line << std::endl;
	    fullmessage = strdup(msg.str().c_str());
	}

	Exception(const Exception& other)
	{
	    this->code = other.code;
	    this->line = line;
	    this->file = file;
	    this->fullmessage = strdup(other.fullmessage);
	}

	~Exception()
	{
	    free(fullmessage);
	}

	const char * what () const throw () { return fullmessage; }
	
	int getErrorCode() {return code;}
	
	private:
	int code, line;
	const char* message, *file;
	char* fullmessage;
    };
    
#define EXCEPTION(code, message)					\
    {std::stringstream __msg;__msg << message; throw uPDFParser::Exception(code, __msg.str().c_str(), __FILE__, __LINE__);}
}
#endif
