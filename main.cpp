#include "WallpapersFrame.h"

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

    auto *frame = new WallpapersFrame(nullptr, ID_LIST, "Wallpaper Manager", wxPoint(50, 50), wxSize(800, 600));

    frame->Show(true);

    frame->UpdateImages();

    return true;
}

wxIMPLEMENT_APP(WP_ManagerApp);