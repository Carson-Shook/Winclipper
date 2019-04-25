#pragma once
#include <codecvt>
#include <wincodec.h>
#include "Bitmap.h"
#include "OnDemandBitmapManager.h"
#include "base64.h"
#include "ISerializable.h"

class Clip : public ISerializable
{
public:
	Clip() noexcept;
	~Clip();

	void						MarkForDelete() noexcept;

	std::string					Serialize() override;
	void						Deserialize(std::string serializationData) override;
	void						DeserializeVersion0(std::string serializationData);

	bool						Equals(std::shared_ptr<Clip> other);

	void						AddFormat(DWORD clipboardFormat);
	bool						AnyFormats() noexcept;
	bool						ContainsFormat(DWORD clipboardFormat);

	void						SetDibBitmap(std::shared_ptr<BITMAPINFOHEADER> pBmiHeader, std::vector<RGBQUAD> quads, std::shared_ptr<BYTE> bits);
	bool						BitmapReady() noexcept;
	std::shared_ptr<Bitmap>		EnsureBitmap();
	void						EnsureBitmapAsync();

	const std::shared_ptr<BITMAPINFOHEADER>		DibBitmapInfoHeader();
	const std::vector<RGBQUAD>					RgbQuadCollection();
	const std::shared_ptr<BYTE>					DibBitmapBits();
	long										DibHeight() noexcept;
	long										DibWidth() noexcept;
	DWORD										DibSize();
	HBITMAP										GetHbitmap();
	HBITMAP										GetThumbnail() noexcept;
	void										PurgeThumbnail();

	void						SetUnicodeText(const wchar_t * unicodeText);
	void						SetUnicodeText(std::wstring unicodeText);
	const wchar_t *				UnicodeText() noexcept;
	const std::wstring			UnicodeTextWString();
	const std::wstring			GetUnicodeMenuText(size_t desiredLength);
private:
	std::set<UINT>				clipboardFormats;

	std::string					bitmapGuid;
	unsigned int				bitmapHeight = 0;
	unsigned int				bitmapWidth = 0;
	std::weak_ptr<Bitmap>		pBitmap;
	HBITMAP						thumbnail = nullptr;

	std::wstring				unicodeText;
	std::wstring				menuTextCache;
	size_t						lastKnownMenuTextLength = 0;

	bool						inCriticalSection = false;
	bool						shouldDelete = false;

	std::string					GetBase64FromWString(const std::wstring data);
	std::wstring				GetWStringFromBase64(const std::string base64);

	static void					EnsureBitmapAsyncInternal(Clip * clip);
};