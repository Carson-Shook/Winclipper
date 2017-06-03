#include "stdafx.h"
#include "resource.h"
#include "SettingsManager.h"

UserSettings::UserSettings()
{
    maxDisplayClips = 20;
    maxSavedClips = 100;
}

UserSettings::~UserSettings()
{
}

int UserSettings::MaxDisplayClips()
{
    return maxDisplayClips;
}

void UserSettings::SetMaxDisplayClips(int maxDisplayClips)
{
    if (maxDisplayClips != UserSettings::maxDisplayClips)
    {
        UserSettings::maxDisplayClips = maxDisplayClips;
    }
}

int UserSettings::MaxSavedClips()
{
    return maxSavedClips;
}

void UserSettings::SetMaxSavedClips(int maxSavedClips)
{
    if (maxSavedClips != UserSettings::maxSavedClips)
    {
        UserSettings::maxSavedClips = maxSavedClips;
    }
}

