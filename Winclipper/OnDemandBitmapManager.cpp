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

	if (saveToDisk)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

		std::thread(File::Write, path, bitmap->Serialize()).detach();
	}

	bitmapCache[guid] = bitmap;

	UpdateUsage(guid);
	RunCleanupAsync();

	return guid;
}

std::weak_ptr<Bitmap> OnDemandBitmapManager::Get(std::string guid)
{
	std::weak_ptr<Bitmap> retVal;

	if (bitmapCache.find(guid) != bitmapCache.end())
	{
		retVal = bitmapCache.at(guid);
	}
	else
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

		std::shared_ptr<Bitmap> bitmap = std::make_shared<Bitmap>();

		bitmap->Deserialize(File::Read(path));
		retVal = bitmapCache[guid] = bitmap;

		while (inCriticalSection)
		{
			Sleep(10);
		}
		inCriticalSection = true;
		usageList.push_front(guid);
		inCriticalSection = false;
	}
	return retVal;
}

void OnDemandBitmapManager::Remove(std::string guid)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

	while (inCriticalSection)
	{
		Sleep(10);
	}
	inCriticalSection = true;

	std::thread(File::Delete, path.c_str()).detach();
	bitmapCache.erase(guid);

	if (!usageList.empty())
	{
		auto findValue = std::find(usageList.begin(), usageList.end(), guid);
		if (findValue != usageList.end())
		{
			usageList.erase(findValue);
		}
	}
	inCriticalSection = false;
}

bool OnDemandBitmapManager::Exists(std::string guid)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

	return File::Exists(path.c_str());
}

unsigned long long OnDemandBitmapManager::MaxBytes() noexcept
{
	return maxBytes;
}

void OnDemandBitmapManager::SetMaxBytes(unsigned long long newMaxBytes)
{
	if (newMaxBytes != maxBytes)
	{
		maxBytes = newMaxBytes;
		RunCleanupAsync();
	}
}

void OnDemandBitmapManager::SetSaveToDisk(bool newSaveToDisk)
{
	if (newSaveToDisk != saveToDisk)
	{
		saveToDisk = newSaveToDisk;

		while (inCriticalSection)
		{
			Sleep(10);
		}

		while (!cleanupMutex.try_lock())
		{
			Sleep(1);
		}
		cleanupMutex.unlock();

		if (saveToDisk)
		{
			for (const auto &pair : bitmapCache)
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(pair.first));
				File::Write(path, pair.second->Serialize());
				CreateAndSaveThumbnail(pair.first, pair.second);
				UpdateUsage(pair.first);
			}
		}
	}
}

void OnDemandBitmapManager::DeleteAllFromDisk()
{
	File::DeleteDir(GetCacheDir());
}

HBITMAP OnDemandBitmapManager::CreateAndSaveThumbnail(std::string guid, std::shared_ptr<Bitmap> bitmap)
{
	HBITMAP retVal = nullptr;

	IWICStream * piFileStream = nullptr;
	IWICBitmapEncoder * piEncoder = nullptr;
	IWICMetadataBlockWriter * piBlockWriter = nullptr;
	IWICMetadataBlockReader * piBlockReader = nullptr;

	const UINT height = GetSystemMetrics(SM_CXMENUCHECK);
	const UINT width = GetSystemMetrics(SM_CXMENUCHECK);


	HBITMAP hBitmap = bitmap->GetHbitmap();
	HRESULT hr = hBitmap ? S_OK : E_FAIL;

	IWICBitmap * pBitmap = nullptr;
	IWICBitmapScaler * piScaler = nullptr;

	if (SUCCEEDED(hr))
	{
		hr = GetWICFactory()->CreateBitmapFromHBITMAP(
			hBitmap,
			nullptr,
			bitmap->DibBitmapInfoHeader()->biBitCount == 32 ? WICBitmapUseAlpha : WICBitmapIgnoreAlpha,
			&pBitmap
		);
	}

	if (SUCCEEDED(hr))
	{
		hr = GetWICFactory()->CreateBitmapScaler(&piScaler);
	}

	if (SUCCEEDED(hr))
	{

		hr = piScaler->Initialize(
			pBitmap,
			width,
			height,
			WICBitmapInterpolationModeFant);
	}

	if (SUCCEEDED(hr))
	{
		hr = GetWICFactory()->CreateEncoder(GUID_ContainerFormatBmp, nullptr, &piEncoder);
	}

	if (SUCCEEDED(hr))
	{
		hr = GetWICFactory()->CreateStream(&piFileStream);
	}

	if (SUCCEEDED(hr))
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

		hr = piFileStream->InitializeFromFilename((path + L"-thumb").c_str(), GENERIC_WRITE);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Initialize(piFileStream, WICBitmapEncoderNoCache);
	}

	// We will only ever be working with single frames, so don't worry about others
	if (SUCCEEDED(hr))
	{
		IWICBitmapFrameEncode * piframeEncode = nullptr;

		if (SUCCEEDED(hr))
		{
			hr = piEncoder->CreateNewFrame(&piframeEncode, nullptr);
		}

		if (SUCCEEDED(hr))
		{
			hr = piframeEncode->Initialize(nullptr);
		}

		if (SUCCEEDED(hr))
		{
			hr = piframeEncode->SetSize(width, height);
		}

		if (SUCCEEDED(hr))
		{
			WICPixelFormatGUID formatGuid;
			hr = piScaler->GetPixelFormat(&formatGuid);
			if (SUCCEEDED(hr))
			{
				hr = piframeEncode->SetPixelFormat(&formatGuid);
			}
		}

		GUID destFormat = GUID_ContainerFormatBmp;

		if (SUCCEEDED(hr))
		{
			hr = piEncoder->GetContainerFormat(&destFormat);
		}

		piframeEncode->QueryInterface(&piBlockWriter);

		if (piBlockReader != nullptr && piBlockWriter != nullptr)
		{
			hr = piBlockWriter->InitializeFromBlockReader(piBlockReader);
		}

		if (SUCCEEDED(hr))
		{
			hr = piframeEncode->WriteSource(piScaler, nullptr);
		}

		if (SUCCEEDED(hr))
		{
			hr = piframeEncode->Commit();
		}

		SafeRelease(&piframeEncode);
	}

	if (SUCCEEDED(hr))
	{
		piEncoder->Commit();
	}

	IWICFormatConverter * pConvertedSourceBitmap = nullptr;

	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConvertedSourceBitmap);
	}

	// Just to make sure it's 32 bits
	if (SUCCEEDED(hr))
	{
		hr = pConvertedSourceBitmap->Initialize(
			piScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.f,
			WICBitmapPaletteTypeCustom
		);
	}

	if (SUCCEEDED(hr))
	{
		UINT bufferSize = width * height * 4;
		std::vector<BYTE> buffer;
		buffer.reserve(size_t{ bufferSize });
		pConvertedSourceBitmap->CopyPixels(nullptr, width * 4, bufferSize, buffer.data());

		retVal = CreateBitmap(width, height, 1, 32, buffer.data());
	}

	SafeRelease(&pConvertedSourceBitmap);
	SafeRelease(&piFileStream);
	SafeRelease(&piEncoder);
	SafeRelease(&piBlockWriter);
	SafeRelease(&piBlockReader);
	SafeRelease(&pBitmap);
	SafeRelease(&piScaler);

	return retVal;
}

HBITMAP OnDemandBitmapManager::GetThumbnail(std::string guid)
{
	HBITMAP retVal = nullptr;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));

	path += L"-thumb";
	if (File::Exists(path.c_str()))
	{
		retVal = static_cast<HBITMAP>(LoadImageW(nullptr, path.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	}
	else
	{
		std::shared_ptr<Bitmap> pBitmap = OnDemandBitmapManager::Get(guid).lock();
		if (pBitmap != nullptr)
		{
			retVal = CreateAndSaveThumbnail(guid, pBitmap);
		}
	}

	return retVal;
}

void OnDemandBitmapManager::RemoveThumbnail(std::string guid, bool async)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	auto path = File::JoinPath(GetCacheDir(), converter.from_bytes(guid));
	
	if (async)
	{
		std::thread(File::Delete, (path + L"-thumb").c_str()).detach();
	}
	else
	{
		File::Delete((path + L"-thumb").c_str());
	}
}

std::string OnDemandBitmapManager::CreateGuidString()
{
	GUID guid;
	char guidString[38];
	const HRESULT hr = CoCreateGuid(&guid);
	if (SUCCEEDED(hr) || hr == RPC_S_UUID_LOCAL_ONLY)
	{
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
	else
	{
		throw std::exception("GUID could not be created.");
	}
}

void OnDemandBitmapManager::UpdateUsage(std::string guid)
{
	while (inCriticalSection)
	{
		Sleep(10);
	}

	inCriticalSection = true;

	auto findValue = std::find(usageList.begin(), usageList.end(), guid);

	if (findValue != usageList.begin())
	{
		if (findValue != usageList.end())
		{
			usageList.erase(findValue);
		}
		usageList.push_front(guid);

		inCriticalSection = false;
		RunCleanupAsync();
	}
	else
	{
		inCriticalSection = false;
	}
}

void OnDemandBitmapManager::SafeClose()
{
	safeClosing = true;

	while (inCriticalSection)
	{
		Sleep(10);
	}

	while (!cleanupMutex.try_lock())
	{
		Sleep(1);
	}
	cleanupMutex.unlock();
}

void OnDemandBitmapManager::RunCleanupAsync()
{
	if (saveToDisk)
	{
		if (cleanupWaitCount == 0)
		{
			cleanupWaitCount++;
			std::thread(RunCleanupInternal).detach();
		}
		else
		{
			cleanupWaitCount++;
		}
	}
}

void OnDemandBitmapManager::RunCleanupInternal()
{
	cleanupMutex.lock();
	while (cleanupWaitCount != 0 && !safeClosing)
	{
		// effective 5 second delay, but allows
		// us to exit in 1 second at worst if
		// program is in closing state.
		int i = 5;
		cleanupWaitCount = 1;
		while (i > 0 && !safeClosing)
		{
			Sleep(1000);
			i--;
		}
		cleanupWaitCount--;
	}
	
	while (inCriticalSection && !safeClosing)
	{
		Sleep(10);
	}

	if (!usageList.empty() && !safeClosing)
	{
		inCriticalSection = true;

		unsigned long long currentSize = 0;
		std::map<std::string, std::shared_ptr<Bitmap>>::iterator it;
		for (it = bitmapCache.begin(); it != bitmapCache.end(); it++)
		{
			// We multiply by two because each Bitmap is roughly the size of
			// itself plus the HBITMAP used in other parts of the program.
			currentSize += static_cast<unsigned long long>(it->second->Size()) * 2;
		}

		long references = 0;
		while (currentSize > maxBytes && references < 2)
		{
			auto key = usageList.back();
			references = bitmapCache.at(key).use_count();
			if (references < 2)
			{
				currentSize -= static_cast<unsigned long long>(bitmapCache.at(key)->Size()) * 2;
				bitmapCache.erase(key);
				usageList.pop_back();
			}
		}

		inCriticalSection = false;
	}
	cleanupMutex.unlock();
}

IWICImagingFactory * OnDemandBitmapManager::GetWICFactory()
{
	if (pIWICFactory == nullptr)
	{
		const HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pIWICFactory)
		);
	}

	return pIWICFactory;
}

IWICImagingFactory * OnDemandBitmapManager::pIWICFactory = nullptr;

std::wstring OnDemandBitmapManager::cacheDir;
std::map<std::string, std::shared_ptr<Bitmap>> OnDemandBitmapManager::bitmapCache;
std::deque<std::string> OnDemandBitmapManager::usageList;

unsigned long long OnDemandBitmapManager::maxBytes = 104857600; // 100 MB
unsigned int OnDemandBitmapManager::cleanupWaitCount = 0;
bool OnDemandBitmapManager::saveToDisk = true;
bool OnDemandBitmapManager::inCriticalSection = false;
bool OnDemandBitmapManager::safeClosing = false;
std::mutex OnDemandBitmapManager::cleanupMutex;