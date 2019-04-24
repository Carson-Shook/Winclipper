#pragma once
#include "Bitmap.h"
#include <mutex>
#include <wincodec.h>
#include <wincodecsdk.h>
#include "ControlUtilities.h"
#include "stdafx.h"

class OnDemandBitmapManager
{
public:

	static std::wstring				GetCacheDir();
	static std::string				Add(std::shared_ptr<Bitmap> bitmap);
	static std::weak_ptr<Bitmap>	Get(std::string guid);
	static void						Remove(std::string guid);
	static bool						Exists(std::string guid);

	unsigned long long				MaxBytes();
	static void						SetMaxBytes(unsigned long long newMaxBytes);

	static HBITMAP					CreateAndSaveThumbnail(std::string guid, std::shared_ptr<Bitmap> bitmap);
	static HBITMAP					GetThumbnail(std::string guid);
	static void						RemoveThumbnail(std::string guid);

	static std::string				CreateGuidString();

	static void						UpdateUsage(std::string guid);

	static void						SafeClose();

private:
	OnDemandBitmapManager() {};
	~OnDemandBitmapManager() {};

	static void						RunCleanupAsync();
	static void						RunCleanupInternal();

	static IWICImagingFactory *		GetWICFactory();
	static IWICImagingFactory *		pIWICFactory;

	static std::wstring				cacheDir;
	static std::map<std::string, std::shared_ptr<Bitmap>> bitmapCache;
	static std::deque<std::string>	usageList;

	static unsigned long long		maxBytes;
	static unsigned int				cleanupWaitCount;
	static bool						inCriticalSection;
	static bool						safeClosing;
	static std::mutex				cleanupMutex;
};

