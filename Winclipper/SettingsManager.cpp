#include "stdafx.h"
#include "resource.h"
#include "SettingsManager.h"

UserSettings::UserSettings()
{
    maxDisplayClips = 20;
    maxStoredClips = 100;
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

int UserSettings::MaxStoredClips()
{
    return maxStoredClips;
}

void UserSettings::SetMaxStoredClips(int maxStoredClips)
{
    if (maxStoredClips != UserSettings::maxStoredClips)
    {
        UserSettings::maxStoredClips = maxStoredClips;
    }
}

