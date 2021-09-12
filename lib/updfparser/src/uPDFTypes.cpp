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

#include "uPDFTypes.h"
#include "uPDFParser_common.h"

namespace uPDFParser
{
    Name::Name(const std::string& name):
	DataType(DataType::TYPE::NAME)
    {
	_value = name;
    }

    String::String(const std::string& value):
	DataType(DataType::TYPE::STRING)
    {
	_value = value;
    }

    HexaString::HexaString(const std::string& value):
	DataType(DataType::TYPE::HEXASTRING)
    {
	_value = value;
    }

    std::string Integer::str()
    {
	std::string sign("");
	if (_signed)
	{
	    if (_value >= 0)
		sign = "+";
	    else
		sign = "-";
	}

	return " " + sign + std::to_string(_value);
    }
    
    std::string Real::str()
    {
	std::string sign("");
	if (_signed)
	{
	    if (_value >= 0)
		sign = "+";
	    else
		sign = "-";
	}

	return " " + sign + std::to_string(_value);
    }

    std::string Array::str()
    {
	std::string res("[");
	std::vector<DataType*>::iterator it;

	for(it = _value.begin(); it!=_value.end(); it++)
	{
	    if (res.size() > 1)
		res += " ";
	    res += (*it)->str();
	}
	    
	return res + std::string("]");
    }

    void Dictionary::addData(const std::string& key, DataType* value)
    {
	_value[key] = value;
    }
    
    std::string Dictionary::str()
    {
	std::string res("<<");
	std::map<std::string, DataType*>::iterator it;

	for(it = _value.begin(); it!=_value.end(); it++)
	{
	    res += std::string("/") + it->first;
	    if (it->second)
		res += it->second->str();
	}
	    
	return res + std::string(">>\n"); 
   }
}
