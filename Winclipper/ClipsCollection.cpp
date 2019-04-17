#include "stdafx.h"
#include "ClipsCollection.h"


ClipsCollection::ClipsCollection()
{
}


ClipsCollection::~ClipsCollection()
{
	clips.clear();
}

/* ISerializable member function implementations */

unsigned __int16 ClipsCollection::Version() noexcept
{
	return 1;
}

std::string ClipsCollection::Serialize()
{
	std::string data;
	for (auto clip : clips)
	{
		data.append(clip->Serialize());
		data.append("\r\n");
	}
	return data;
}

void ClipsCollection::Deserialize(std::string serializationData)
{
	const size_t dataLength = serializationData.length();
	if (dataLength != 0)
	{
		std::istringstream stream(serializationData);

		std::string readData;

		unsigned int corruptedClips = 0;

		while (std::getline(stream, readData))
		{
			std::shared_ptr<Clip> clip(new Clip);

			try
			{
				clip->Deserialize(readData);
				clips.push_back(clip);
			}
			catch (const std::exception&)
			{
				clip->MarkForDelete();
				corruptedClips++;
			}
		}

		if (corruptedClips > 0)
		{
			std::string message(std::to_string(corruptedClips));
			if (corruptedClips == 1)
			{
				message += " clip was corrupt and was cleaned up.";
			}
			else
			{
				message += " clips were corrupt and were cleaned up.";
			}
			throw std::exception(message.c_str());
		}
	}
}

/* End ISerializable implementation */

void ClipsCollection::AddFront(std::shared_ptr<Clip> clip)
{
	if (clips.empty())
	{
		clips.push_front(clip);
	}
	else
	{
		if (!clip->Equals(clips.front()))
		{
			clips.push_front(clip);
			while (clips.size() > maxSize)
			{
				clips.back()->MarkForDelete();
				RemoveBack();
			}
		}
	}
}

std::shared_ptr<Clip> ClipsCollection::RemoveBack()
{
	std::shared_ptr<Clip> clip(nullptr);

	if (!clips.empty())
	{
		clip = clips.back();
		clips.pop_back();
	}

	return clip;
}

std::shared_ptr<Clip> ClipsCollection::RemoveAt(size_t index)
{
	std::shared_ptr<Clip> clip(nullptr);

	if (!clips.empty() && index < clips.size())
	{
		clip = clips.at(index);
		clips.erase(clips.begin() + index);
	}

	return clip;
}

std::shared_ptr<Clip> ClipsCollection::At(size_t index)
{
	std::shared_ptr<Clip> clip(nullptr);

	if (!clips.empty() && index < clips.size())
	{
		clip = clips.at(index);
	}

	return clip;
}

std::shared_ptr<Clip> ClipsCollection::Front()
{
	return clips.front();
}

unsigned int ClipsCollection::Size()
{
	return clips.size();
}

unsigned int ClipsCollection::MaxSize()
{
	return maxSize;
}

void ClipsCollection::SetMaxSize(unsigned int newMaxSize)
{
	if (newMaxSize != maxSize && newMaxSize > 1)
	{
		maxSize = newMaxSize;

		while (clips.size() > maxSize)
		{
			clips.back()->MarkForDelete();
			RemoveBack();
		}
	}
}

void ClipsCollection::Clear()
{
	size_t size = clips.size();
	for (size_t i = 0; i < size; i++)
	{
		clips.at(i)->MarkForDelete();
	}
	clips.clear();
}
