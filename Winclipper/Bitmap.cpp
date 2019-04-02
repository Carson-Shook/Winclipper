#include "stdafx.h"
#include "Bitmap.h"


Bitmap::Bitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits, bool hasAlpha)
{
	pDibBitmapInfoHeader = pBmiHeader;
	rgbQuadCollection = quads;
	pDibBitmapBits = bits;
	dibBitmapHasAlpha = hasAlpha;
}

Bitmap::Bitmap()
{
}

Bitmap::~Bitmap()
{
}

/* ISerializable member function implementations */

unsigned __int16 Bitmap::Version() noexcept
{
	return 1;
}

std::string Bitmap::Serialize()
{
	size_t bmSize = pDibBitmapInfoHeader->biSizeImage
		? pDibBitmapInfoHeader->biSizeImage
		: ((((pDibBitmapInfoHeader->biWidth * pDibBitmapInfoHeader->biBitCount) + 31) & ~31) >> 3) * pDibBitmapInfoHeader->biHeight;
	BITMAPFILEHEADER header;
	header.bfType = 0x4d42;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + pDibBitmapInfoHeader->biSize + (rgbQuadCollection.size() * sizeof(RGBQUAD));
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfSize = header.bfOffBits + bmSize;

	std::string data;
	
	data.append(reinterpret_cast<const char *>(&header),
		reinterpret_cast<const char *>(&header) + sizeof(BITMAPFILEHEADER));
	data.append(reinterpret_cast<const char *>(pDibBitmapInfoHeader.get()),
		reinterpret_cast<const char *>(pDibBitmapInfoHeader.get()) + sizeof(BITMAPINFOHEADER));

	if (rgbQuadCollection.size() > 0)
	{
		data.append(reinterpret_cast<const char *>(rgbQuadCollection.data()),
				reinterpret_cast<const char *>(rgbQuadCollection.data()) + (rgbQuadCollection.size() * sizeof(RGBQUAD)));
	}

	data.append(reinterpret_cast<const char *>(pDibBitmapBits.get()),
		reinterpret_cast<const char *>(pDibBitmapBits.get()) + bmSize);

	return data;
}

void Bitmap::Deserialize(std::string serializationData)
{
	const size_t dataLength = serializationData.length();
	if (dataLength != 0)
	{
		BITMAPFILEHEADER header;
		char tempHeader[sizeof(BITMAPFILEHEADER)];
		std::shared_ptr<BITMAPINFOHEADER> bmih(new BITMAPINFOHEADER);
		std::vector<RGBQUAD> quads;

		std::istringstream stream(serializationData);

		stream.readsome(reinterpret_cast<char *>(&header), sizeof(BITMAPFILEHEADER));
		stream.readsome(reinterpret_cast<char *>(bmih.get()), sizeof(BITMAPINFOHEADER));

		unsigned int colorBytes = bmih->biClrUsed;
		if (!colorBytes)
		{
			if (bmih->biBitCount != 24)
			{
				colorBytes = 1 << bmih->biBitCount;
			}
		}
		colorBytes = colorBytes * sizeof(RGBQUAD);

		for (auto i = 0; i < colorBytes; i++)
		{
			RGBQUAD quad;
			stream.readsome(reinterpret_cast<char *>(&quad), sizeof(RGBQUAD));

			quads.push_back(quad);
		}

		size_t bmSize = header.bfSize - header.bfOffBits;

		std::shared_ptr<BYTE> bits(new BYTE[bmSize], array_deleter<BYTE>());

		stream.readsome(reinterpret_cast<char *>(bits.get()), bmSize);

		pDibBitmapInfoHeader = bmih;
		rgbQuadCollection = quads;
		pDibBitmapBits = bits;
	}
}

/* End ISerializable implementation */

const std::shared_ptr<BITMAPINFOHEADER> Bitmap::DibBitmapInfoHeader()
{
	return pDibBitmapInfoHeader;
}

const std::vector<RGBQUAD> Bitmap::RgbQuadCollection()
{
	if (rgbQuadCollection.empty())
	{
		return std::vector<RGBQUAD>();
	}
	return rgbQuadCollection;
}

const std::shared_ptr <BYTE> Bitmap::DibBitmapBits()
{
	return pDibBitmapBits;
}

const bool Bitmap::DibHasAlpha()
{
	return dibBitmapHasAlpha;
}