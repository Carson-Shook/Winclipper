#pragma once
#include <codecvt>
#include "base64.h"
#include "ISerializable.h"
#define RECORD_SEPARATOR_CHAR "\x1e"
#define UNIT_SEPARATOR_CHAR "\x1f"

class Clip : public ISerializable
{
public:
	Clip() noexcept;
	~Clip();

	unsigned __int16	Version() noexcept override;
	std::string			Serialize() override;
	void				Deserialize(std::string serializationData) override;
	void				DeserializeVersion0(std::string serializationData);

	bool				Equals(Clip other);

	void				AddFormat(DWORD clipboardFormat);
	bool				AnyFormats() noexcept;
	bool				ContainsFormat(DWORD clipboardFormat);

	void				SetUnicodeText(wchar_t * unicodeText);
	void				SetUnicodeText(std::wstring unicodeText);
	const wchar_t *		UnicodeText() noexcept;
	const std::wstring	UnicodeTextWString();
	const std::wstring	GetUnicodeTextPreview(size_t desiredLength);
private:
	std::set<UINT>		clipboardFormats;
	std::wstring		unicodeText;
	std::wstring		previewText;

	std::string			GetBase64FromWString(const std::wstring data);
	std::wstring		GetWStringFromBase64(const std::string base64);
};