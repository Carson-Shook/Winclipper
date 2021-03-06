#include "stdafx.h"
#include "Clip.h"

Clip::Clip() noexcept
{
}

Clip::~Clip()
{
	if (thumbnail != nullptr)
	{
		DeleteObject(thumbnail);
	}
	if (!bitmapGuid.empty() && shouldDelete)
	{
		try
		{
			OnDemandBitmapManager::RemoveThumbnail(bitmapGuid, true);
			OnDemandBitmapManager::Remove(bitmapGuid);
		}
		catch (const std::exception&)
		{
			// we're tearing down anyway.
			// could be bad, but no way to handle.
		}
	}
}

// Call this if any associated resources should
// be deleted when object is deconstructed.
void Clip::MarkForDelete() noexcept
{
	shouldDelete = true;
}

/* ISerializable member function implementations */

std::string Clip::Serialize() 
{
	//std::string data = WriteVersion();
	std::string data;
	if (AnyFormats())
	{
		if (ContainsFormat(CF_UNICODETEXT))
		{
			data.push_back(RECORD_SEPARATOR_CHAR);
			data.append(std::to_string(CF_UNICODETEXT));
			data.push_back(UNIT_SEPARATOR_CHAR);
			data.append(GetBase64FromWString(UnicodeTextWString()));
		}
		if (ContainsFormat(CF_DIB))
		{
			data.push_back(RECORD_SEPARATOR_CHAR);
			data.append(std::to_string(CF_DIB));
			data.push_back(UNIT_SEPARATOR_CHAR);
			data.append(bitmapGuid);
			data.push_back(UNIT_SEPARATOR_CHAR);
			data.append(std::to_string(bitmapHeight));
			data.push_back(UNIT_SEPARATOR_CHAR);
			data.append(std::to_string(bitmapWidth));
		}
	}
	return data;
}

void Clip::Deserialize(std::string serializationData)
{
	const size_t dataLength = serializationData.length();
	if (dataLength != 0)
	{
		std::istringstream stream(serializationData);

		if (stream.peek() == RECORD_SEPARATOR_CHAR)
		{
			stream.ignore(1);
			while (!stream.eof())
			{
				std::string record;
				std::getline(stream, record, RECORD_SEPARATOR_CHAR);

				if (record.length() > 0)
				{
					std::istringstream recordStream(record);
					std::vector<std::string> units;

					while (!recordStream.eof())
					{
						std::string unit;
						std::getline(recordStream, unit, UNIT_SEPARATOR_CHAR);
						units.push_back(unit);
					}

					if (units.size() != 0)
					{
						const DWORD cf = std::stoul(units.at(0));

						switch (cf)
						{
						case CF_UNICODETEXT:
							AddFormat(cf);
							SetUnicodeText(GetWStringFromBase64(units.at(1)));
							break;
						case CF_DIB:
							AddFormat(cf);
							bitmapGuid = units.at(1);
							if (!OnDemandBitmapManager::Exists(bitmapGuid))
							{
								// If the bitmap data is missing, then we want to
								// ensure the clip is not created.
								throw std::exception("Cached bitmap missing.");
							}
							bitmapHeight = std::stol(units.at(2));
							bitmapWidth = std::stol(units.at(3));
							thumbnail = OnDemandBitmapManager::GetThumbnail(bitmapGuid);
							break;
						default:
							break;
						}
					}
				}
			}
		}
		else
		{
			DeserializeVersion0(stream.str());
		}
	}
}

void Clip::DeserializeVersion0(std::string serializationData)
{
	AddFormat(CF_UNICODETEXT);
	SetUnicodeText(GetWStringFromBase64(serializationData));
}

/* End ISerializable implementation */

bool Clip::Equals(std::shared_ptr<Clip> other)
{
	bool isEqual = true;

	for (const UINT format : clipboardFormats)
	{
		if (!other->ContainsFormat(format))
		{
			isEqual = false;
			break;
		}

		if (format == CF_UNICODETEXT && unicodeText.compare(other->UnicodeTextWString()) != 0)
		{
			isEqual = false;
			break;
		}

		if (format == CF_DIB)
		{
			if (other->bitmapHeight != bitmapHeight || other->bitmapWidth != bitmapWidth)
			{
				isEqual = false;
				break;
			}
			auto mySize = DibSize();
			if (other->DibSize() != mySize)
			{
				isEqual = false;
				break;
			}

			if (std::memcmp(other->DibBitmapBits().get(), DibBitmapBits().get(), mySize))
			{
				isEqual = false;
				break;
			}
		}
	}

	return isEqual;
}

void Clip::AddFormat(DWORD clipboardFormat)
{
	clipboardFormats.insert(clipboardFormat);
}

bool Clip::AnyFormats() noexcept
{
	return !clipboardFormats.empty();
}

bool Clip::ContainsFormat(DWORD clipboardFormat)
{
	return clipboardFormats.find(clipboardFormat) != clipboardFormats.end();
}

void Clip::SetDibBitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits)
{
	bitmapWidth = pBmiHeader->biWidth;
	bitmapHeight = pBmiHeader->biHeight;
	auto bitmap = std::make_shared<Bitmap>(pBmiHeader, quads, bits);

	bitmapGuid = OnDemandBitmapManager::Add(bitmap);
	thumbnail = OnDemandBitmapManager::CreateAndSaveThumbnail(bitmapGuid, bitmap);
	pBitmap = OnDemandBitmapManager::Get(bitmapGuid);
}

bool Clip::BitmapReady() noexcept
{
	if (!inCriticalSection)
	{
		std::shared_ptr<Bitmap> p = pBitmap.lock();
		return !p ? false : true;
	}
	return false;
}

std::shared_ptr<Bitmap> Clip::EnsureBitmap()
{
	std::shared_ptr<Bitmap> p = pBitmap.lock();
	if (!p)
	{
		try
		{
			pBitmap = OnDemandBitmapManager::Get(bitmapGuid);
		}
		catch (const std::exception& e)
		{
			MarkForDelete();
			throw e;
		}
		p = pBitmap.lock();
	}
	OnDemandBitmapManager::UpdateUsage(bitmapGuid);
	return p;
}

void Clip::EnsureBitmapAsync()
{
	std::thread(EnsureBitmapAsyncInternal, this).detach();
}

const std::shared_ptr<BITMAPINFOHEADER> Clip::DibBitmapInfoHeader()
{
	const auto p = EnsureBitmap();
	return p->DibBitmapInfoHeader();
}

const std::vector<RGBQUAD> Clip::RgbQuadCollection()
{
	// Copy constructor should be cheap enough here.
	// Worst case scenario is 256 RGBQUADs.
	auto p = EnsureBitmap();
	if (p->RgbQuadCollection().empty())
	{
		return std::vector<RGBQUAD>();
	}
	return p->RgbQuadCollection();
}

const std::shared_ptr<BYTE> Clip::DibBitmapBits()
{
	const auto p = EnsureBitmap();
	return p->DibBitmapBits();
}

long Clip::DibHeight() noexcept
{
	return bitmapHeight;
}

long Clip::DibWidth() noexcept
{
	return bitmapWidth;
}

DWORD Clip::DibSize()
{
	auto p = EnsureBitmap();
	return p->Size();
}

HBITMAP Clip::GetHbitmap()
{
	auto p = EnsureBitmap();
	return p->GetHbitmap();
}

HBITMAP Clip::GetThumbnail() noexcept
{
	if (thumbnail == nullptr)
	{
		thumbnail = OnDemandBitmapManager::GetThumbnail(bitmapGuid);
	}
	return thumbnail;
}

void Clip::PurgeThumbnail()
{
	if (thumbnail != nullptr)
	{
		HBITMAP tempThumb = thumbnail;
		thumbnail = nullptr;
		DeleteObject(tempThumb);
	}
	if (!bitmapGuid.empty())
	{
		try
		{
			OnDemandBitmapManager::RemoveThumbnail(bitmapGuid, false);
		}
		catch (const std::exception&)
		{
			// no thumbnail to remove? Problem solved? /shrug
		}
	}
}

void Clip::SetUnicodeText(const wchar_t * unicodeText)
{
	Clip::unicodeText.assign(unicodeText);
}

void Clip::SetUnicodeText(std::wstring unicodeText)
{
	Clip::unicodeText.assign(unicodeText);
}

const wchar_t * Clip::UnicodeText() noexcept
{
	if (unicodeText.empty())
	{
		return nullptr;
	}
	return unicodeText.c_str();
}

const std::wstring Clip::UnicodeTextWString()
{
	return unicodeText;
}

const std::wstring Clip::GetUnicodeMenuText(size_t desiredLength)
{
	if (desiredLength != lastKnownMenuTextLength)
	{
		lastKnownMenuTextLength = desiredLength;
		std::wstring menuText;

		if (unicodeText.length() > desiredLength)
		{
			menuText.assign(unicodeText, 0, desiredLength - 3);
			menuText.append(L"...\0");
		}
		else
		{
			menuText.assign(unicodeText);
		}

		for (size_t k = 0; k < menuText.length(); k++)
		{
			switch (menuText.at(k))
			{
			case '\t':
			case '\r':
			case '\n':
				menuText.at(k) = ' ';
			}
		}

		menuTextCache.clear();
		menuTextCache.assign(menuText);
	}

	return menuTextCache;
}

std::string Clip::GetBase64FromWString(const std::wstring data)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string anotherTemp = converter.to_bytes(data);
	return base64_encode(reinterpret_cast<const unsigned char *>(anotherTemp.c_str()), anotherTemp.length() + 1);
}

std::wstring Clip::GetWStringFromBase64(const std::string base64)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	
	if (base64.compare("") == 0)
	{
		return std::wstring();
	}

	return converter.from_bytes(base64_decode(base64));
}

void Clip::EnsureBitmapAsyncInternal(Clip * clip)
{
	if (!clip->inCriticalSection)
	{
		clip->inCriticalSection = true;
		try
		{
			clip->pBitmap = OnDemandBitmapManager::Get(clip->bitmapGuid);
		}
		catch (const std::exception&)
		{
			clip->MarkForDelete();
		}
		clip->inCriticalSection = false;
	}
}
