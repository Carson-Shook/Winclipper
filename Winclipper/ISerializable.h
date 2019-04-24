#pragma once

#define RECORD_SEPARATOR_CHAR '\x1e'
#define UNIT_SEPARATOR_CHAR '\x1f'

class ISerializable
{
public:
	ISerializable();
	virtual ~ISerializable();

	// Should return a wstring of all writeable
	// values of the derived class.
	virtual std::string Serialize() = 0;

	// Should set each value into its corresponding
	// variable or object.
	virtual void Deserialize(std::string serializationData) = 0;
};

