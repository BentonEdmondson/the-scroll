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

#ifndef _UPDFOBJECT_HPP_
#define _UPDFOBJECT_HPP_

#include "uPDFTypes.h"

namespace uPDFParser
{
    /**
     * @brief PDF Object
     */
    class Object
    {
    public:
	Object():
	    _objectId(0), _generationNumber(0),
	    offset(0), _isNew(false), indirectOffset(0)
	{}

	/**
	 * @brief Object constructor
	 *
	 * @param objectId          Object ID
	 * @param generationNumber  Object generation number
	 * @param offset            Offset of object in current PDF file
	 * @param isNew             false if object has been read from file,
	 *                          true if it has been created or updated
	 * @param indirectOffset    Object is indirect
	 */
	Object(int objectId, int generationNumber, uint64_t offset, bool isNew=false,
	       off_t indirectOffset=0):
	    _objectId(objectId), _generationNumber(generationNumber),
	    offset(offset), _isNew(isNew), indirectOffset(indirectOffset)
	{}

	~Object()
	{
	    std::vector<DataType*>::iterator it;
	    for(it=_data.begin(); it!=_data.end(); it++)
		delete *it;
	}

	Object(const Object& other)
	{
	    _objectId = other._objectId;
	    _generationNumber = other._generationNumber;
	    offset = other.offset;
	    indirectOffset = other.indirectOffset;
	    _isNew = true;

	    std::vector<DataType*>::const_iterator it;
	    for(it=other._data.begin(); it!=other._data.end(); it++)
		_data.push_back((*it)->clone());

	    const std::map<std::string, DataType*> _dict = ((Dictionary)other._dictionary).value();
	    std::map<std::string, DataType*>& _myDict = _dictionary.value();
	    std::map<std::string, DataType*>::const_iterator it2;
	    for(it2=_dict.begin(); it2!=_dict.end(); it2++)
		_myDict[it2->first] = it2->second->clone();
	}

	/**
	 * @brief Clone current object (call copy constructor)
	 */
	Object* clone() { return new Object(*this); }

	/**
	 * @brief Return internal dictionary
	 */
	Dictionary& dictionary() {return _dictionary;}

	/**
	 * @brief Return vector of data contained into object
	 */
	std::vector<DataType*>& data() {return _data;}

	/**
	 * @brief Object string representation
	 */
	std::string str();

	/**
	 * @brief Set object as indirect if offset != 0 or not indirect if offset == 0
	 */
	void setIndirectOffset(off_t offset) {indirectOffset = offset;}

	/**
	 * @brief is object indirect (indirectOffset != 0)
	 */
	bool isIndirect() {return indirectOffset != 0;}

	/**
	 * @brief Get dictionary value
	 */
	DataType*& operator[](const std::string& key) { return _dictionary.value()[key]; }

	/**
	 * @brief Check for key in object's dictionary
	 */
	bool hasKey(const std::string& key) { return _dictionary.value().count(key)?true:false; }

	/**
	 * @brief is object new (or not updated) ?
	 */
	bool isNew() { return _isNew; }

	/**
	 * @brief Mark object as updated
	 */
	void update(void) { _isNew = true; }

	/**
	 * @brief Return object's id
	 */
	int objectId() { return _objectId; }

	/**
	 * @brief Return object's generation number
	 */
	int generationNumber() { return _generationNumber; }
	
    private:
	int _objectId;
	int _generationNumber;
	off_t offset;
	bool _isNew;
	off_t indirectOffset;
	Dictionary _dictionary;
	std::vector<DataType*> _data;
    };
}
#endif
