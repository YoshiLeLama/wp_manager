//
// Created by Antoine Roumilhac on 23/12/2021.
//

#ifndef WP_MANAGER_WALLPAPERSFRAME_H_
#define WP_MANAGER_WALLPAPERSFRAME_H_

#include "ImagesList.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

enum {
    ADD_FILES,
    DELETE_SELECTED
};

class WallpapersFrame : public wxFrame {
public:
    WallpapersFrame(wxWindow *parent,
                    wxWindowID id,
                    const wxString &title,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize);

    void SetupMenuBar();
    void UpdateImages();

private:
    ImagesList *imagesList;
    wxMenuBar *pMenuBar;
    wxMenu *pFileMenu;
    wxMenu *pSelectionMenu;
    wxMenu *pParametersMenu;
    wxMenu *pHelpMenu;

    void OnAddFiles(wxCommandEvent &event);
    void DeleteSelectedFiles(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);

DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(WallpapersFrame, wxFrame)
        EVT_MENU(ADD_FILES, WallpapersFrame::OnAddFiles)
        EVT_MENU(DELETE_SELECTED, WallpapersFrame::DeleteSelectedFiles)
        EVT_MENU(wxID_EXIT, WallpapersFrame::OnExit)
        EVT_MENU(wxID_ABOUT, WallpapersFrame::OnAbout)
END_EVENT_TABLE()

enum PARAMETERS {
    ID_
};

void WallpapersFrame::OnAddFiles(wxCommandEvent &event) {
    auto files(GetFilePaths());
    auto newFiles = MoveFiles(files);
    imagesList->AddImages(newFiles);
}

void WallpapersFrame::OnExit(wxCommandEvent &event) {
    Close(true);
}

void WallpapersFrame::OnAbout(wxCommandEvent &event) {
    wxMessageBox("Little app to manage wallpapers", "About", wxOK | wxICON_INFORMATION);

}

WallpapersFrame::WallpapersFrame(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString &title,
                                 const wxPoint &pos,
                                 const wxSize &size) : wxFrame(parent, id, title, pos, size),
                                                       imagesList(new ImagesList(this)) {
    auto *sizer = new wxBoxSizer(wxHORIZONTAL);

    SetMinSize(imagesList->GetMinSize());
    sizer->Add(imagesList, 1, wxEXPAND, 0);

    SetSizer(sizer);
    SetupMenuBar();
}

void WallpapersFrame::SetupMenuBar() {
    pMenuBar = new wxMenuBar();
    pFileMenu = new wxMenu();
    pFileMenu->Append(ADD_FILES, _T("&Add Files...\tCtrl+O"));
    pFileMenu->AppendSeparator();
    pFileMenu->Append(wxID_EXIT, _T("&Quit"));
    pMenuBar->Append(pFileMenu, _T("&File"));

    pSelectionMenu = new wxMenu();
    pSelectionMenu->Append(DELETE_SELECTED, _T("&Delete files"));
    pMenuBar->Append(pSelectionMenu, _T("&Select"));

    pHelpMenu = new wxMenu();
    pHelpMenu->Append(wxID_ABOUT, _T("&A&bout"));
    pMenuBar->Append(pHelpMenu, _T("&Help"));
    SetMenuBar(pMenuBar);
}

void WallpapersFrame::UpdateImages() {
    auto oldTitle(wxTopLevelWindowMSW::GetTitle());
    SetTitle("Loading wallpapers...");
    imagesList->UpdateImages();
    SetTitle(oldTitle);
}

void WallpapersFrame::DeleteSelectedFiles(wxCommandEvent &event) {
    imagesList->DoDeleteSelectedFiles();
}

#endif //WP_MANAGER_WALLPAPERSFRAME_H_
