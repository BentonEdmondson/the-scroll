/*
  Copyright 2021 Grégory Soutadé

  This file is part of uPDFParser.

  uPDFParser is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  uPDFParser is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with uPDFParser. If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "uPDFParser.h"
#include "uPDFParser_common.h"

namespace uPDFParser
{
    std::string Object::str()
    {
	std::stringstream res;

	res << _objectId << " " << _generationNumber << " obj\n";
	res << _dictionary.str();

	std::vector<DataType*>::iterator it;
	for(it=_data.begin(); it!=_data.end(); it++)
	    res << (*it)->str();

	res << "endobj\n";

	return res.str();
    }

    /**
     * @brief Read data until '\n' or '\r' is found or buffer is full
     */
    static inline int readline(int fd, char* buffer, int size, bool exceptionOnEOF=true)
    {
	int res = 0;
	char c;

	buffer[0] = 0;
	
	for (;size;size--,res++)
	{
	    if (read(fd, &c, 1) != 1)
	    {
		if (exceptionOnEOF)
		    EXCEPTION(TRUNCATED_FILE, "Unexpected end of file");
		return -1;
	    }

	    if (c == '\n' || c == '\r')
	    {
		// Empty line
		if (!res)
		{
		    size++ ;
		    res--;
		    continue;
		}
		else
		    break;
	    }

	    buffer[res] = c;
	}

	if (size)
	    buffer[res] = 0;
	
	return res;
    }

    /**
     * @brief Read data until EOF, '\n' or '\r' is found
     */
    static inline void finishLine(int fd)
    {
	char c;
	
	while (1)
	{
	    if (read(fd, &c, 1) != 1)
		break;

	    if (c == '\n' || c == '\r')
		break;
	}
	// Support \r\n and \n\r
	if (read(fd, &c, 1) == 1)
	{
	    if (c != '\n' && c != '\r')
		lseek(fd, -1, SEEK_CUR);
	}
    }

    /**
     * @brief Find next token to analyze
     */
    std::string Parser::nextToken(bool exceptionOnEOF)
    {
	char c = 0, prev_c;
	std::string res("");
	int i;
	static const char delims[] = " \t<>[]()/";
	static const char whitespace_prev_delims[] = "+-"; // Need whitespace before
	static const char start_delims[] = "<>[]()";
	bool found = false;
	
	while (!found)
	{
	    prev_c = c;
	    if (read(fd, &c, 1) != 1)
	    {
		if (exceptionOnEOF)
		    EXCEPTION(TRUNCATED_FILE, "Unexpected end of file");
		break;
	    }

	    // Comment, skip line
	    if (c == '%')
	    {
		finishLine(fd);
		break;
	    }

	    // White character while empty result, continue
	    if ((c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0') && !res.size())
		continue;

	    // Quit on line return without lseek(fd, -1, SEEK_CUR)
	    if (c == '\n' || c == '\r')
	    {
		if (res.size())
		    break;
		else
		    continue;
	    }

	    if (res.size())
	    {
		// Push character until delimiter is found
		for (i=0; i<(int)sizeof(delims); i++)
		{
		    if (c == delims[i])
		    {
			lseek(fd, -1, SEEK_CUR);
			found = true;
			break;
		    }
		}
		
		// Push character until delimiter is found
		if (!found && prev_c == ' ')
		{
		    for (i=0; i<(int)sizeof(whitespace_prev_delims); i++)
		    {
			if (c == whitespace_prev_delims[i])
			{
			    lseek(fd, -1, SEEK_CUR);
			    found = true;
			    break;
			}
		    }
		}
		
		if (!found)
		    res += c;
	    }
	    else
	    {
		curOffset = lseek(fd, 0, SEEK_CUR)-1;

		// First character, is it a delimiter ?
		for (i=0; i<(int)sizeof(start_delims); i++)
		{
		    if (c == start_delims[i])
		    {
			found = true;
			break;
		    }
		}

		res += c;
	    }
	}

	// Double '>' and '<' to compute dictionary
	if (res == ">" || res == "<")
	{
	    if (read(fd, &c, 1) == 1)
	    {
		if (c == res[0])
		    res += c;
		else
		    lseek(fd, -1, SEEK_CUR);
	    }
	}
	
	return res;
    }

    void Parser::parseStartXref()
    {
	std::string token;
	char buffer[10];

	// std::cout << "Parse startxref" << std::endl;

	token = nextToken();
	readline(fd, buffer, sizeof(buffer), false);
	if (strncmp(buffer, "%%EOF", 5))
	    EXCEPTION(INVALID_TRAILER, "Invalid trailer at offset " << curOffset);
    }
    
    void Parser::parseTrailer()
    {
	std::string token;

	// std::cout << "Parse trailer" << std::endl;

	token = nextToken();

	if (token != "<<")
	    EXCEPTION(INVALID_TRAILER, "Invalid trailer at offset " << curOffset);

	parseDictionary(&trailer, trailer.dictionary().value());

	token = nextToken();
	if (token != "startxref")
	    EXCEPTION(INVALID_TRAILER, "Invalid trailer at offset " << curOffset);

	parseStartXref();
    }
    
    void Parser::parseXref()
    {
	std::string token;

	// std::cout << "Parse xref" << std::endl;
	xrefOffset = curOffset;

	while (1)
	{
	    token = nextToken();

	    if (token == "trailer")
	    {
		parseTrailer();
		break;
	    }
	}
    }

    static DataType* tokenToNumber(std::string& token, char sign='\0')
    {
	int i;
	float fvalue;
	int ivalue;
	
	for(i=0; i<(int)token.size(); i++)
	{
	    if (token[i] == '.')
	    {
		if (i==0) token = std::string("0") + token;
		fvalue = std::stof(token);
		if (sign == '-')
		    fvalue = -fvalue;
		return new Real(fvalue, (sign!='\0'));
	    }
	}

	ivalue = std::stoi(token);
	if (sign == '-')
	    ivalue = -ivalue;
	
	return new Integer(ivalue, (sign!='\0'));
    }
    
    DataType* Parser::parseSignedNumber(std::string& token)
    {
	char sign = token[0];
	token = std::string(&((token.c_str())[1]));
	return tokenToNumber(token, sign);
    }
    
    DataType* Parser::parseNumber(std::string& token)
    {
	return tokenToNumber(token);
    }

    DataType* Parser::parseNumberOrReference(std::string& token)
    {
	DataType* res = tokenToNumber(token);

	if (res->type() == DataType::TYPE::REAL)
	    return res;
	
	off_t offset = lseek(fd, 0, SEEK_CUR);
	std::string token2 = nextToken();
	std::string token3 = nextToken();

	DataType* generationNumber = 0;
	try
	{
	    generationNumber = tokenToNumber(token2);
	}
	catch (std::invalid_argument& e)
	{
	    lseek(fd, offset, SEEK_SET);
	    return res;
	}
	
	if ((generationNumber->type() != DataType::TYPE::INTEGER) ||
	    token3.size() != 1 || token3[0] != 'R')
	{
	    delete generationNumber;
	    lseek(fd, offset, SEEK_SET);
	    return res;
	}

	DataType* res2 = new Reference(((Integer*)res)->value(),
				       ((Integer*)generationNumber)->value());
	delete res;
	return res2;
    }
    
    DataType* Parser::parseType(std::string& token, Object* object, std::map<std::string, DataType*>& dict)
    {
	DataType* value = 0;
	Dictionary* _value = 0;

	if (token == "<<")
	{
	    _value = new Dictionary();
	    value = _value;
	    parseDictionary(object, _value->value());
	}
	else if (token == "[")
	    value = parseArray(object);
	else if (token == "(")
	    value = parseString();
	else if (token == "<")
	    value = parseHexaString();
	else if (token == "stream")
	    value = parseStream(object);
	else if (token[0] >= '1' && token[0] <= '9')
	    value = parseNumberOrReference(token);
	else if (token[0] == '/')
	    value = parseName(token);
	else if (token[0] == '+' || token[0] == '-')
	    value = parseSignedNumber(token);
	else if (token[0] == '0' || token[0] == '.')
	    value = parseNumber(token);
	else if (token == "true")
	    return new Boolean(true);
	else if (token == "false")
	    return new Boolean(false);
	else if (token == "null")
	    return new Null();
	else
	    EXCEPTION(INVALID_TOKEN, "Invalid token " << token << " at offset " << curOffset);

	return value;
    }

    Array* Parser::parseArray(Object* object)
    {
	std::string token;
	DataType* value;

	Array* res = new Array();
	
	while (1)
	{
	    token = nextToken();

	    if (token == "]")
		break;

	    value = parseType(token, object, object->dictionary().value());
	    //std::cout << "Add " << value->str() << std::endl;
	    res->addData(value);
	}

	return res;
    }
    
    String* Parser::parseString()
    {
	std::string res("");
	char c;
	bool escaped = false;
	
	while (1)
	{
	    if (read(fd, &c, 1) != 1)
		break;

	    if (c == ')' && !escaped)
		break;

	    escaped = (c == '\\');

	    res += c;
	}

	return new String(res);
    }
    
    HexaString* Parser::parseHexaString()
    {
	std::string res("");
	char c;
	
	while (1)
	{
	    if (read(fd, &c, 1) != 1)
		break;

	    if (c == '>')
		break;

	    res += c;
	}

	if ((res.size() % 2))
	    EXCEPTION(INVALID_HEXASTRING, "Invalid hexa String at offset " << curOffset);
	    
	return new HexaString(res);
    }

    Stream* Parser::parseStream(Object* object)
    {
	off_t startOffset, endOffset;
	std::string token;
	
	// std::cout << "parseStream" << std::endl;
	
	startOffset = lseek(fd, 0, SEEK_CUR);

	if (!object->hasKey("Length"))
	    EXCEPTION(INVALID_STREAM, "No Length property at offset " << curOffset);

	DataType* Length = (*object)["Length"];
	if (Length->type() != DataType::INTEGER)
	{
	    if (Length->type() != DataType::REFERENCE)
		EXCEPTION(INVALID_STREAM, "Invalid Length property at offset " << curOffset);

	    // Don't want to parse xref table...
	    while (1)
	    {
		char buffer[4*1024];
		int ret;
		endOffset = lseek(fd, 0, SEEK_CUR);
		ret = readline(fd, buffer, sizeof(buffer));
		if (!strncmp(buffer, "endstream", 9))
		{
		    lseek(fd, -(ret-9), SEEK_CUR);
		    break;
		}
	    }
	    return new Stream(startOffset, endOffset);
	}
	
	Integer* length = (Integer*)Length;
	endOffset = startOffset + length->value();
	lseek(fd, endOffset, SEEK_SET);
	token = nextToken();

	if (token != "endstream")
	    EXCEPTION(INVALID_STREAM, "endstream not found at offset " << endOffset);

	// std::cout << "end parseStream" << std::endl;

	return new Stream(startOffset, endOffset);
    }
    
    Name* Parser::parseName(std::string& name)
    {
	if (!name.size() || name[0] != '/')
	    EXCEPTION(INVALID_NAME, "Invalid Name at offset " << curOffset);

	//std::cout << "Name " << name << std::endl;
	return new Name(name);
    }
   
    void Parser::parseDictionary(Object* object, std::map<std::string, DataType*>& dict)
    {
	std::string token;
	Name* key;
	DataType* value;

	while (1)
	{
	    token = nextToken();
	    if (token == ">>")
		break;

	    key = parseName(token);

	    token = nextToken();
	    if (token == ">>")
	    {
		dict[key->value()] = 0;
		break;
	    }

	    value = parseType(token, object, dict);
	    dict[key->value()] = value;
	}
    }
    
    void Parser::parseObject(std::string& token)
    {
	off_t offset;
	int objectId, generationNumber;
	Object* object;

	offset = curOffset;
	try
	{
	    objectId = std::stoi(token);
	    token = nextToken();
	    generationNumber = std::stoi(token);
	}
	catch(std::invalid_argument& e)
	{
	    EXCEPTION(INVALID_OBJECT, "Invalid object at offset " << curOffset);
	}

	token = nextToken();

	if (token != "obj")
	    EXCEPTION(INVALID_OBJECT, "Invalid object at offset " << curOffset);

	// std::cout << "New obj " << objectId << " " << generationNumber << std::endl;
	
	object = new Object(objectId, generationNumber, offset);
	_objects.push_back(object);

	while (1)
	{
	    token = nextToken();

	    if (token == "endobj")
		break;

	    if (token == "<<")
		parseDictionary(object, object->dictionary().value());
	    else if (token[0] >= '1' && token[0] <= '9')
	    {
		DataType* _offset = tokenToNumber(token);
		if (_offset->type() != DataType::TYPE::INTEGER)
		    EXCEPTION(INVALID_OBJECT, "Invalid object at offset " << curOffset);
		object->setIndirectOffset(((Integer*)_offset)->value());
	    }
	    else
		parseType(token, object, object->dictionary().value());
	}
    }

    void Parser::parse(const std::string& filename)
    {
	char buf[16];
	std::string token;
	bool secondLine = true;
	
	if (fd)
	    close(fd);

	fd = open(filename.c_str(), O_RDONLY);
	
	if (fd <= 0)
	    EXCEPTION(UNABLE_TO_OPEN_FILE, "Unable to open " << filename << " (%m)");

	// Check %PDF at startup
	readline(fd, buf, 4, false);
	if (strncmp(buf, "%PDF", 4))
	    EXCEPTION(INVALID_HEADER, "Invalid PDF header");
	finishLine(fd);

	curOffset = lseek(fd, 0, SEEK_CUR);

	// // Check %%EOF at then end
	// lseek(fd, -5, SEEK_END);
	// readline(fd, buf, 5);
	// if (strncmp(buf, "%%EOF", 5))
	//     EXCEPTION(INVALID_FOOTER, "Invalid PDF footer");

	lseek(fd, curOffset, SEEK_SET);

	while (1)
	{
	    token = nextToken(false);

	    if (!token.size())
		break;

	    if (token == "xref")
		parseXref();
	    else if (token[0] >= '1' && token[0] <= '9')
		parseObject(token);
	    // Can have startxref without trailer (not end of document)
	    else if (token == "startxref")
		parseStartXref();
	    else
	    {
		// The second line may be not commented and invalid (for UTF8 stuff)
		if (!secondLine)
		{
		    EXCEPTION(INVALID_LINE, "Invalid Line at offset " << curOffset);
		}
		else
		    finishLine(fd);
	    }
	    // If for optimization
	    if (secondLine) secondLine = false;
	}
	
	close(fd);
    }

    void Parser::writeUpdate(const std::string& filename)
    {
	int newFd = open(filename.c_str(), O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);

	if (newFd <= 0)
	    EXCEPTION(UNABLE_TO_OPEN_FILE, "Unable to open " << filename << " (%m)");

	::write(newFd, "\r", 1);

	std::stringstream xref;
	int nbNewObjects = 0;

	xref << std::setfill('0');
	xref << "xref\n";
	
	std::vector<Object*>::iterator it;
	for(it=_objects.begin(); it!=_objects.end(); it++)
	{
	    if (!(*it)->isNew())
		continue;
	    nbNewObjects ++;
	    std::string objStr = (*it)->str();
	    curOffset = lseek(newFd, 0, SEEK_CUR);
	    ::write(newFd, objStr.c_str(), objStr.size());
	    xref << std::setw(0) << (*it)->objectId() << " 1\n";
	    xref << std::setw(10) << curOffset << " " << std::setw(5) << (*it)->generationNumber() << " n\r\n"; // Here \r seems important 
	}

	if (!nbNewObjects)
	{
	    close(newFd);
	    return;
	}

	off_t newXrefOffset = lseek(newFd, 0, SEEK_CUR);

	std::string xrefStr = xref.str();
	::write(newFd, xrefStr.c_str(), xrefStr.size());

	if (trailer.hasKey("Prev"))
	    delete trailer["Prev"];
	
	trailer["Prev"] = new Integer((int)xrefOffset);

	std::string trailerStr = trailer.dictionary().str();
	::write(newFd, "trailer\n", 8);
	::write(newFd, trailerStr.c_str(), trailerStr.size());

	std::stringstream startxref;
	startxref << "startxref\n" << newXrefOffset << "\n%%EOF";
	
	std::string startxrefStr = startxref.str();
	::write(newFd, startxrefStr.c_str(), startxrefStr.size());
	
	close(newFd);
    }
    
    void Parser::write(const std::string& filename, bool update)
    {
	if (update)
	    return writeUpdate(filename);
	else
	    EXCEPTION(NOT_IMPLEMENTED, "Full write not implemented");
    }

}
