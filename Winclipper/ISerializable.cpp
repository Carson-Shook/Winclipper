#include "stdafx.h"
#include "ISerializable.h"

ISerializable::ISerializable()
{
}

ISerializable::~ISerializable()
{
}

std::string ISerializable::WriteVersion()
{
	std::string versionString = std::to_string(Version());
	while (versionString.length() < MAX_VERSION_LENGTH)
	{
		versionString.insert(0, "0");
	}
	return versionString;
}
