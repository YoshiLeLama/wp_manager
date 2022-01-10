#include "WallpapersFrame.h"
#include "ColorSettings.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/vlbox.h>

#include <iostream>
#include <filesystem>
#include <map>

class WP_ManagerApp : public wxApp {
public:
    bool OnInit() override;
};

enum {
    ID_LIST
};

bool WP_ManagerApp::OnInit() {
    wxInitAllImageHandlers();
    ColorSettings::LoadCurrentColorMode();
    CreateFolder("data");

    auto *frame = new WallpapersFrame(nullptr, wxID_ANY, "Wallpaper Manager", wxPoint(50, 50), wxSize(800, 600));

    frame->LoadWallpapers();

    frame->Show();

    return true;
}

wxIMPLEMENT_APP(WP_ManagerApp);