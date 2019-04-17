#include "stdafx.h"
#include "Bitmap.h"


Bitmap::Bitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits)
{
	pDibBitmapInfoHeader = pBmiHeader;
	rgbQuadCollection = quads;
	pDibBitmapBits = bits;
}

Bitmap::Bitmap()
{
}

Bitmap::~Bitmap()
{
	if (hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}
}

/* ISerializable member function implementations */

unsigned __int16 Bitmap::Version() noexcept
{
	return 1;
}

std::string Bitmap::Serialize()
{
	size_t bmSize = Size();
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
		std::shared_ptr<BITMAPINFOHEADER> bmih(new BITMAPINFOHEADER);
		std::vector<RGBQUAD> quads;

		std::istringstream stream(serializationData);

		stream.readsome(reinterpret_cast<char *>(&header), sizeof(BITMAPFILEHEADER));
		stream.readsome(reinterpret_cast<char *>(bmih.get()), sizeof(BITMAPINFOHEADER));

		unsigned int colorBytes = bmih->biClrUsed;
		if (!colorBytes)
		{
			if (bmih->biBitCount <= 8)
			{
				colorBytes = 1 << bmih->biBitCount;
			}
			else if (bmih->biBitCount != 24 && bmih->biCompression == BI_BITFIELDS)
			{
				colorBytes = 3;
			}
		}

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

		GetHbitmap();
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

const std::shared_ptr<BYTE> Bitmap::DibBitmapBits()
{
	return pDibBitmapBits;
}

// Creates an HBITMAP for the bitmap.
// It is the responibility of the caller
// to call DeleteObject on the HBITMAP.
HBITMAP Bitmap::GetHbitmap()
{
	if (hBitmap == NULL)
	{
		HDC screen = GetDC(NULL);
		BITMAPINFO * bmi = (BITMAPINFO *)new BYTE[pDibBitmapInfoHeader->biSize + rgbQuadCollection.size() * sizeof(RGBQUAD)];

		bmi->bmiHeader = *pDibBitmapInfoHeader;
		for (auto i = 0; i < rgbQuadCollection.size(); i++)
		{
			bmi->bmiColors[i].rgbBlue = rgbQuadCollection[i].rgbBlue;
			bmi->bmiColors[i].rgbGreen = rgbQuadCollection[i].rgbGreen;
			bmi->bmiColors[i].rgbRed = rgbQuadCollection[i].rgbRed;
			bmi->bmiColors[i].rgbReserved = rgbQuadCollection[i].rgbReserved;
		}

		hBitmap = CreateDIBitmap(
			screen,
			pDibBitmapInfoHeader.get(),
			CBM_INIT,
			pDibBitmapBits.get(),
			bmi,
			DIB_RGB_COLORS
		);

		delete[](BYTE *)bmi;
		ReleaseDC(NULL, screen);
	}
	return hBitmap;
}

// Returns the size of the image in bytes.
size_t Bitmap::Size()
{
	size_t retVal = 0;
	if (pDibBitmapInfoHeader != nullptr)
	{
		retVal = pDibBitmapInfoHeader->biSizeImage
			? pDibBitmapInfoHeader->biSizeImage
			: ((((pDibBitmapInfoHeader->biWidth * pDibBitmapInfoHeader->biBitCount) + 31) & ~31) >> 3) * pDibBitmapInfoHeader->biHeight;
	}

	return retVal;
}
