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

#include <map>

enum {
    ADD_FILES,
    DELETE_SELECTED,
    SELECT_CAT
};

class WallpapersFrame : public wxFrame {
public:
    WallpapersFrame(wxWindow *parent,
                    wxWindowID id,
                    const wxString &title,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize);

    void SetupMenuBar();
    void LoadWallpapers();
    void RefreshCategoryChoice();

private:
    void OnAddFiles(wxCommandEvent &event);
    void DeleteSelectedFiles(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnChoice(wxCommandEvent &event);
    void OnAddCategory(wxCommandEvent &event);
    void OnRemoveCategory(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);

    ImagesList *imagesList;
    wxMenuBar *pMenuBar;
    wxStaticText *pStatusBarText;
    wxMenu *pFileMenu;
    wxMenu *pSelectionMenu;
    wxMenu *pParametersMenu;
    wxMenu *pHelpMenu;

    wxChoice *categoryChoice;
    wxButton *addCatButton;
    wxButton *removeCatButton;

DECLARE_EVENT_TABLE()
    std::string GetCategoryChoiceLabel(const std::string &cat_name, int count);
};

wxIcon GetAppIcon();

#endif //WP_MANAGER_WALLPAPERSFRAME_H_
