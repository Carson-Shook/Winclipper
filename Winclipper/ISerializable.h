#pragma once

// Max number of characters that we should pad to
// when writing the version number. This is hard-coded
// so that even if the implementation changes, this
// number should not.
#define MAX_VERSION_LENGTH 5

class ISerializable
{
public:
	ISerializable();
	~ISerializable();

	// Portable and reliable way to write the version number.
	std::string WriteVersion();

	// Should return the current version of the
	// serialization format. Use to differentiate
	// between old and new data formats.
	virtual unsigned __int16 Version() = 0;

	// Should return a wstring of all writeable
	// values of the derived class.
	virtual std::string Serialize() = 0;

	// Should set each value into its corresponding
	// variable or object.
	virtual void Deserialize(std::string serializationData) = 0;
};

