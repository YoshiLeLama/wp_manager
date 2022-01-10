//
// Created by Antoine Roumilhac on 10/01/2022.
//

#ifndef WP_MANAGER_COLORSETTINGS_H_
#define WP_MANAGER_COLORSETTINGS_H_

#include <string>
#include <Windows.h>
#include <wx/colour.h>

class ColorSettings {
public:
    static void LoadCurrentColorMode();
    static bool IsDark();

    static const wxColor *Accent0();
    static const wxColor *Accent1();
    static const wxColor *Accent2();
    static const wxColor *Accent3();

private:
    static const wxColor DARK_ACCENT_0;
    static const wxColor DARK_ACCENT_1;
    static const wxColor DARK_ACCENT_2;
    static const wxColor DARK_ACCENT_3;
    static const wxColor LIGHT_ACCENT_0;
    static const wxColor LIGHT_ACCENT_1;
    static const wxColor LIGHT_ACCENT_2;
    static const wxColor LIGHT_ACCENT_3;

    static bool isDark;
};

#endif //WP_MANAGER_COLORSETTINGS_H_
