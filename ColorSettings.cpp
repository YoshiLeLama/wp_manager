//
// Created by Antoine Roumilhac on 10/01/2022.
//

#include "ColorSettings.h"

void ColorSettings::LoadCurrentColorMode() {
#if WIN32
    const std::wstring subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    const std::wstring value = L"AppsUseLightTheme";
    DWORD nDefValue = 0;
    DWORD result;
    DWORD data;
    DWORD dataSize;
    LONG error = RegGetValue(HKEY_CURRENT_USER, subKey.c_str(), value.c_str(), RRF_RT_REG_DWORD, nullptr, &data, &dataSize);

    if (error == ERROR_SUCCESS)
        isDark = !data;
    else
        isDark = false;
#else
    isDark = false;
#endif
}

bool ColorSettings::IsDark() {
    return isDark;
}

bool ColorSettings::isDark = true;

const wxColor ColorSettings::DARK_ACCENT_0 = wxColor(51, 51, 51);
const wxColor ColorSettings::DARK_ACCENT_1 = wxColor(32, 32, 32);
const wxColor ColorSettings::DARK_ACCENT_2 = wxColor(25, 25, 25);
const wxColor ColorSettings::DARK_ACCENT_3 = wxColor(0, 0, 0);
const wxColor ColorSettings::LIGHT_ACCENT_0 = wxColor(255, 255, 255);
const wxColor ColorSettings::LIGHT_ACCENT_1 = wxColor(255, 255, 255);
const wxColor ColorSettings::LIGHT_ACCENT_2 = wxColor(245, 246, 247);
const wxColor ColorSettings::LIGHT_ACCENT_3 = wxColor(255, 255, 255);

const wxColor *ColorSettings::Accent1() {
    return isDark ? &DARK_ACCENT_1 : &LIGHT_ACCENT_1;
}

const wxColor *ColorSettings::Accent2() {
    return isDark ? &DARK_ACCENT_2 : &LIGHT_ACCENT_2;
}

const wxColor *ColorSettings::Accent3() {
    return isDark ? &DARK_ACCENT_3 : &LIGHT_ACCENT_3;
}

const wxColor *ColorSettings::Accent0() {
    return isDark ? &DARK_ACCENT_0 : &LIGHT_ACCENT_0;
}
