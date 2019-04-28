#include "stdafx.h"
#include "Bitmap.h"


Bitmap::Bitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits)
{
	pDibBitmapInfoHeader = pBmiHeader;
	rgbQuadCollection = quads;
	pDibBitmapBits = bits;
}

Bitmap::Bitmap() noexcept
{
}

Bitmap::~Bitmap()
{
	if (hBitmap != nullptr)
	{
		DeleteObject(hBitmap);
	}
}

/* ISerializable member function implementations */

std::string Bitmap::Serialize()
{
	std::string data;

	const DWORD bmSize = Size();
	BITMAPFILEHEADER header;
	header.bfType = 0x4d42;
	header.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) + pDibBitmapInfoHeader->biSize + (rgbQuadCollection.size() * sizeof(RGBQUAD)));
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfSize = header.bfOffBits + bmSize;

	// reinterpret_cast should be fine in these situations
	// since we are reinterpreting the values as binary data.
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
		try
		{
			BITMAPFILEHEADER header;
			std::shared_ptr<BITMAPINFOHEADER> bmih = std::make_shared<BITMAPINFOHEADER>();
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

			for (unsigned int i = 0; i < colorBytes; i++)
			{
				RGBQUAD quad = { 0, 0, 0, 0 };
				stream.readsome(reinterpret_cast<char *>(&quad), sizeof(RGBQUAD));

				quads.push_back(quad);
			}

			const size_t bmSize = header.bfSize - header.bfOffBits;

			std::shared_ptr<BYTE> bits(new BYTE[bmSize], array_deleter<BYTE>());

			stream.readsome(reinterpret_cast<char *>(bits.get()), bmSize);

			pDibBitmapInfoHeader = bmih;
			rgbQuadCollection = quads;
			pDibBitmapBits = bits;

			GetHbitmap();
		}
		catch (const std::exception&)
		{
			throw std::exception("Image data is corrupt and could not be loaded.");
		}
	}
	else
	{
		throw std::exception("Image data is empty or missing.");
	}
}

/* End ISerializable implementation */

const std::shared_ptr<BITMAPINFOHEADER> Bitmap::DibBitmapInfoHeader() noexcept
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

const std::shared_ptr<BYTE> Bitmap::DibBitmapBits() noexcept
{
	return pDibBitmapBits;
}

// Creates an HBITMAP for the bitmap.
// It is the responibility of the caller
// to call DeleteObject on the HBITMAP.
HBITMAP Bitmap::GetHbitmap()
{
	if (hBitmap == nullptr)
	{
		HDC screen = GetDC(nullptr);
		BITMAPINFO * bmi = reinterpret_cast<BITMAPINFO *>(new BYTE[pDibBitmapInfoHeader->biSize + rgbQuadCollection.size() * sizeof(RGBQUAD)]);

		bmi->bmiHeader = *pDibBitmapInfoHeader;
		for (size_t i = 0; i < rgbQuadCollection.size(); i++)
		{
			bmi->bmiColors[i] = rgbQuadCollection[i];
		}

		hBitmap = CreateDIBitmap(
			screen,
			pDibBitmapInfoHeader.get(),
			CBM_INIT,
			pDibBitmapBits.get(),
			bmi,
			DIB_RGB_COLORS
		);

		delete[] reinterpret_cast<BYTE *>(bmi);
		ReleaseDC(nullptr, screen);
	}
	return hBitmap;
}

// Returns the size of the image in bytes.
DWORD Bitmap::Size() noexcept
{
	DWORD retVal = 0;
	if (pDibBitmapInfoHeader != nullptr)
	{
		retVal = pDibBitmapInfoHeader->biSizeImage
			? pDibBitmapInfoHeader->biSizeImage
			: ((((pDibBitmapInfoHeader->biWidth * pDibBitmapInfoHeader->biBitCount) + 31) & ~31) >> 3) * pDibBitmapInfoHeader->biHeight;
	}

	return retVal;
}
