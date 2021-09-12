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

#ifndef _UPDFTYPES_HPP_
#define _UPDFTYPES_HPP_

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace uPDFParser
{
    /**
     * @brief Base class for PDF object type
     * From https://resources.infosecinstitute.com/topic/pdf-file-format-basic-structure/
     */
    class DataType
    {
    public:
	enum TYPE {BOOLEAN, INTEGER, REAL, NAME, STRING, HEXASTRING, REFERENCE, ARRAY, DICTIONARY, STREAM, NULLOBJECT};

	DataType(TYPE _type):
	    _type(_type)
	{}

	virtual ~DataType() {}

	/**
	 * @brief Get current data type
	 */
	TYPE type() { return _type; }

	/**
	 * @brief String representation for serialization
	 */
	virtual std::string str() = 0;

	/**
	 * @brief Clone current object
	 */
	virtual DataType* clone() = 0;
	
    protected:
	TYPE _type;
    };
    
    class Boolean : public DataType
    {
    public:
	Boolean(bool value):
	    DataType(DataType::TYPE::BOOLEAN), _value(value)
	{}

	virtual DataType* clone() {return new Boolean(_value);}
	bool value() {return _value;}
	virtual std::string str() { return (_value)?" true":" false";}
	
    private:
	bool _value;
    };

    class Integer : public DataType
    {
    public:
	Integer(int value, bool _signed=false):
	    DataType(DataType::TYPE::INTEGER), _value(value), _signed(_signed)
	{}

	virtual DataType* clone() {return new Integer(_value, _signed);}
	int value() {return _value;}
	virtual std::string str();

    private:
	int _value;
	bool _signed;
    };
    
    class Real : public DataType
    {
    public:
	Real(float value, bool _signed=false):
	    DataType(DataType::TYPE::REAL), _value(value), _signed(_signed)
	{}

	virtual DataType* clone() {return new Real(_value, _signed);}
	float value() {return _value;}
	virtual std::string str();
	
    private:
	float _value;
	bool _signed;
    };

    class Name : public DataType
    {
    public:
	Name(const std::string&);

	virtual DataType* clone() {return new Name(_value);}
	std::string value() {
	    const char* name = _value.c_str();
	    return std::string(&name[1]);
	}
	virtual std::string str() { return _value;}
	
    private:
	std::string _value;
    };

    class String : public DataType
    {
    public:
	String(const std::string&);

	virtual DataType* clone() {return new String(_value);}
	std::string value() {return _value;}

	// Escape '(' and ')' characters
	virtual std::string str() {
	    char prev = '\0';
	    std::string res("(");

	    for(unsigned int i=0; i<_value.size(); i++)
	    {
		if ((_value[i] == '(' || _value[i] == ')') &&
		    prev != '\\')
		    res += '\\';
		res += _value[i];
		prev = _value[i];
	    }

	    res += ")";
	    return res;
	}

    private:
	std::string _value;
    };

    class HexaString : public DataType
    {
    public:
	HexaString(const std::string&);

	virtual DataType* clone() {return new HexaString(_value);}
	std::string value() {return _value;}
	virtual std::string str() { return std::string("<") + _value + std::string(">");}

    private:
	std::string _value;
    };

    class Reference : public DataType
    {
    public:
	Reference(int objectId, int generationNumber):
	    DataType(DataType::TYPE::REFERENCE), objectId(objectId), generationNumber(generationNumber)
	{}
	
	virtual DataType* clone() {return new Reference(objectId, generationNumber);}
	int value() {return objectId;}
	virtual std::string str() {
	    std::stringstream res;
	    res << " " << objectId << " " << generationNumber << " R";
	    return res.str();
	}

    private:
	int objectId, generationNumber;
    };

    class Array : public DataType
    {
    public:
	Array():
	    DataType(DataType::TYPE::ARRAY)
	{}

	void addData(DataType* data) {_value.push_back(data);}
	
	virtual DataType* clone() {
	    Array* res = new Array();
	    std::vector<DataType*>::iterator it;
	    for(it=_value.begin(); it!=_value.end(); it++)
		res->addData((*it)->clone());
	    return res;
	}
	std::vector<DataType*>& value() {return _value;}
	virtual std::string str();

    private:
	std::vector<DataType*> _value;
    };

    class Dictionary : public DataType
    {
    public:
	Dictionary():
	    DataType(DataType::TYPE::DICTIONARY)
	{}

	void addData(const std::string&, DataType*);

	virtual DataType* clone() {
	    Dictionary* res = new Dictionary();
	    std::map<std::string, DataType*>::iterator it;
	    for(it=_value.begin(); it!=_value.end(); it++)
	    {
		res->addData(it->first, it->second->clone());
	    }
	    return res;
	}
	std::map<std::string, DataType*>& value() {return _value;}
	virtual std::string str();

    private:
	std::map<std::string, DataType*> _value;
    };

    class Stream : public DataType
    {
    public:
	Stream(int startOffset, int endOffset):
	    DataType(DataType::TYPE::STREAM), startOffset(startOffset),
	    endOffset(endOffset)
	{}
	virtual DataType* clone() {return new Stream(startOffset, endOffset);}
	virtual std::string str() { return "stream\nendstream\n";}

    private:
	int startOffset, endOffset;
    };

    class Null : public DataType
    {
    public:
	Null():
	    DataType(DataType::TYPE::NULLOBJECT)
	{}

	virtual DataType* clone() {return new Null();}
	bool value() {return 0;}
	virtual std::string str() { return "null";}
	
    private:
    };
}

#endif
