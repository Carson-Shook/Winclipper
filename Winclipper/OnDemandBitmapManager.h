#pragma once
#include "Bitmap.h"
#include "stdafx.h"

class OnDemandBitmapManager
{
public:
	static std::wstring GetCacheDir();

	static std::string Add(std::shared_ptr<Bitmap> bitmap);
	static std::weak_ptr<Bitmap> Get(std::string guid);
	static void Remove(std::string guid);
	static std::string CreateGuidString();

private:
	OnDemandBitmapManager() {};
	~OnDemandBitmapManager() {};

	static std::wstring cacheDir;
	static std::map<std::string, std::shared_ptr<Bitmap>> bitmapCache;
};

