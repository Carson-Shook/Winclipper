#include "stdafx.h"
#include "Clip.h"

Clip::Clip() noexcept
{
}

Clip::~Clip()
{
	if (!bitmapGuid.empty())
	{
		OnDemandBitmapManager::Remove(bitmapGuid);
	}
}

/* ISerializable member function implementations */

unsigned __int16 Clip::Version() noexcept
{
	return 1;
}

std::string Clip::Serialize() 
{
	std::string data = WriteVersion();
	data.append(RECORD_SEPARATOR_CHAR);
	if (AnyFormats())
	{
		if (ContainsFormat(CF_UNICODETEXT))
		{
			data.append(std::to_string(CF_UNICODETEXT));
			data.append(UNIT_SEPARATOR_CHAR);
			data.append(GetBase64FromWString(UnicodeTextWString()));
			data.append(RECORD_SEPARATOR_CHAR);
		}
		if (ContainsFormat(CF_DIB))
		{
			data.append(std::to_string(CF_DIB));
			data.append(UNIT_SEPARATOR_CHAR);
			data.append(bitmapGuid);
			data.append(RECORD_SEPARATOR_CHAR);
		}
	}
	return data;
}

void Clip::Deserialize(std::string serializationData)
{
	const size_t dataLength = serializationData.length();
	if (dataLength != 0)
	{
		std::vector<std::string> strings;
		std::string record;

		for (const char c : serializationData)
		{
			if (c == RECORD_SEPARATOR_CHAR[0])
			{
				strings.push_back(record);
				record.clear();
			}
			else
			{
				record.push_back(c);
			}
		}

		unsigned __int16 versionNumber = 0;

		if (!strings.empty())
		{
			versionNumber = stoi(strings.at(0));
			strings.erase(strings.begin());
		}

		switch (versionNumber)
		{
		case 1:
			{
				std::map<DWORD, std::string> records;

				for (std::string value : strings)
				{
					try
					{
						records.emplace(
							std::stoul(value.substr(0, value.find(UNIT_SEPARATOR_CHAR))),
							value.substr(value.find(UNIT_SEPARATOR_CHAR) + 1)
						);
					}
					catch (const std::exception e)
					{
						// May eventually throw an error message, but let's try this for now.
						return;
					}
				}

				for (std::pair<DWORD, std::string> pair : records)
				{
					switch (pair.first)
					{
					case CF_UNICODETEXT:
						Clip::AddFormat(pair.first);
						Clip::SetUnicodeText(GetWStringFromBase64(pair.second));
						break;
					case CF_DIB:
						Clip::AddFormat(pair.first);
						Clip::bitmapGuid = pair.second;
						break;
					default:
						break;
					}
				}
			}
			break;
		default:
			DeserializeVersion0(serializationData);
			break;
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
			isEqual = false;
			break;
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

void Clip::SetDibBitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits, bool hasAlpha)
{
	bitmapGuid = OnDemandBitmapManager::Add(std::make_shared<Bitmap>(pBmiHeader, quads, bits, hasAlpha));
}

std::shared_ptr<Bitmap> Clip::EnsureBitmap()
{
	std::shared_ptr<Bitmap> p = pBitmap.lock();
	if (!p)
	{
		pBitmap = OnDemandBitmapManager::Get(bitmapGuid);
		p = pBitmap.lock();
	}
	return p;
}

const std::shared_ptr<BITMAPINFOHEADER> Clip::DibBitmapInfoHeader()
{
	auto p = EnsureBitmap();
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

const std::shared_ptr <BYTE> Clip::DibBitmapBits()
{
	auto p = EnsureBitmap();
	return p->DibBitmapBits();
}

const bool Clip::DibHasAlpha()
{
	auto p = EnsureBitmap();
	return p->DibHasAlpha();
}

void Clip::SetUnicodeText(wchar_t * unicodeText)
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
	return base64_encode(reinterpret_cast<const unsigned char *>(anotherTemp.c_str()), static_cast<unsigned int>(strlen(anotherTemp.c_str()) + 1));
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
