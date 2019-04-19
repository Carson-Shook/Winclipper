#pragma once
#include <sstream>
#include "ISerializable.h"

class Bitmap : public ISerializable
{
public:
	Bitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits);
	Bitmap() noexcept;
	~Bitmap();
	
	std::string					Serialize() override;
	void						Deserialize(std::string serializationData) override;

	const std::shared_ptr<BITMAPINFOHEADER>		DibBitmapInfoHeader() noexcept;
	const std::vector<RGBQUAD>					RgbQuadCollection();
	const std::shared_ptr<BYTE>					DibBitmapBits() noexcept;
	HBITMAP										GetHbitmap();
	size_t										Size() noexcept;

private:
	std::shared_ptr<BITMAPINFOHEADER>			pDibBitmapInfoHeader = nullptr;
	std::vector<RGBQUAD>						rgbQuadCollection;
	std::shared_ptr<BYTE>						pDibBitmapBits = nullptr;
	HBITMAP										hBitmap = nullptr;
};

