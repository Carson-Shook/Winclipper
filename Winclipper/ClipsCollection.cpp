#include "stdafx.h"
#include "ClipsCollection.h"


ClipsCollection::ClipsCollection() noexcept
{
}


ClipsCollection::~ClipsCollection()
{
	clips.clear();
}

/* ISerializable member function implementations */

std::string ClipsCollection::Serialize()
{
	std::string data;
	for (auto clip : clips)
	{
		try
		{
			data.append(clip->Serialize());
			data.append("\r\n");
		}
		catch (const std::exception&)
		{
			// no op because maybe we just ran into a weird data edge case.
			// we wouldn't want to save that to the clipboard anyway.
		}	
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
			std::shared_ptr<Clip> clip = std::make_shared<Clip>();

			try
			{
				clip->Deserialize(readData);
				if (clip->AnyFormats())
				{
					clips.push_back(clip);
				}
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
		else
		{
			clip->MarkForDelete();
		}
	}
}

void ClipsCollection::RemoveBack()
{
	if (!clips.empty())
	{
		clips.pop_back();
	}
}

void ClipsCollection::RemoveAllOfFormat(DWORD format)
{
	if (format != 0)
	{
		std::deque<std::shared_ptr<Clip>>::iterator it = clips.begin()++;
		while (it != clips.end())
		{
			std::shared_ptr<Clip> clip = *it;
			if (clip->ContainsFormat(format))
			{
				clip->MarkForDelete();
				it = clips.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

std::shared_ptr<Clip> ClipsCollection::RemoveAt(unsigned int index)
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

unsigned int ClipsCollection::Size() noexcept
{
	return static_cast<unsigned int>(clips.size());
}

unsigned int ClipsCollection::MaxSize() noexcept
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
	const size_t size = clips.size();
	for (size_t i = 0; i < size; i++)
	{
		clips.at(i)->MarkForDelete();
	}
	clips.clear();
}

void ClipsCollection::PurgeThumbnails()
{
	const size_t size = clips.size();
	for (size_t i = 0; i < size; i++)
	{
		clips.at(i)->PurgeThumbnail();
	}
}

void ClipsCollection::LoadAllResources()
{
	const size_t size = clips.size();
	for (size_t i = 0; i < size; i++)
	{
		if (clips.at(i)->ContainsFormat(CF_DIB))
		{
			auto bitmap = clips.at(i)->EnsureBitmap();
			clips.at(i)->GetThumbnail();
		}
	}
}
