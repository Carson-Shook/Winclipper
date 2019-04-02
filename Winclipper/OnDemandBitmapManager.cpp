#include "stdafx.h"
#include "File.h"
#include "OnDemandBitmapManager.h"


std::wstring OnDemandBitmapManager::GetCacheDir()
{
	if (cacheDir.empty())
	{
		cacheDir = File::JoinPath(File::GetAppDir(), L"BitmapCache\\");
	}
	return cacheDir;
}

std::string OnDemandBitmapManager::Add(std::shared_ptr<Bitmap> bitmap)
{
	std::string guid(CreateGuidString());
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

	std::thread(File::Write, path, bitmap->Serialize()).detach();

	bitmapCache[guid] = bitmap;
	return guid;
}

std::weak_ptr<Bitmap> OnDemandBitmapManager::Get(std::string guid)
{
	std::weak_ptr<Bitmap> retVal;

	if (bitmapCache.find(guid) != bitmapCache.end())
	{
		retVal = bitmapCache[guid];
	}
	else
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

		std::shared_ptr<Bitmap> bitmap(new Bitmap);

		bitmap->Deserialize(File::Read(path));
		retVal = bitmapCache[guid] =  bitmap;
	}
	return retVal;
}

void OnDemandBitmapManager::Remove(std::string guid)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

	std::thread(File::Delete, path.c_str()).detach();

	bitmapCache.erase(guid);
}

std::string OnDemandBitmapManager::CreateGuidString()
{
	GUID guid;
	char guidString[38];
	CoCreateGuid(&guid);
	sprintf_s(guidString,
		38,
		"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
		guid.Data1, 
		guid.Data2, 
		guid.Data3, 
		guid.Data4[0], 
		guid.Data4[1], 
		guid.Data4[2], 
		guid.Data4[3], 
		guid.Data4[4], 
		guid.Data4[5], 
		guid.Data4[6], 
		guid.Data4[7]);

	return std::string(guidString);
}

std::wstring OnDemandBitmapManager::cacheDir;
std::map<std::string, std::shared_ptr<Bitmap>> OnDemandBitmapManager::bitmapCache;
