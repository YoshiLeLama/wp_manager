//
// Created by Antoine Roumilhac on 24/12/2021.
//

#include "WallpapersFrame.h"

BEGIN_EVENT_TABLE(WallpapersFrame, wxFrame)
        EVT_MENU(ADD_FILES, WallpapersFrame::OnAddFiles)
        EVT_MENU(DELETE_SELECTED, WallpapersFrame::DeleteSelectedFiles)
        EVT_MENU(wxID_EXIT, WallpapersFrame::OnExit)
        EVT_MENU(wxID_ABOUT, WallpapersFrame::OnAbout)
END_EVENT_TABLE()

wxIcon GetAppIcon() {
    wxIcon iconImg("MAIN_ICON");
    if (!iconImg.IsOk())
        return nullptr;
    return iconImg;
}

inline std::string WallpapersFrame::GetCategoryChoiceLabel(const std::string &cat_name, int count) {
    return (cat_name.empty() ? "Not categorized" : cat_name) + " (" + std::to_string(count) + " elements)";
}

void WallpapersFrame::OnAddFiles(wxCommandEvent &event) {
    //auto files(GetFilePaths());

    wxFileDialog openFileDialog(this,
                                "Open wallpapers images...",
                                "",
                                "",
                                "Images (*.png,*.jpg,*.jpeg)|*.png;*.jpg;*.jpeg",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxArrayString files;
    openFileDialog.GetPaths(files);

    auto newFiles =
        MoveFiles(files, imagesList->GetCategoryName(categoryChoice->GetSelection()), imagesList->GetDataDir());

    int count = imagesList->AddImages(newFiles);

    SetStatusText("Added " + std::to_string(count) + (count > 1 ? " files." : " file."));

    categoryChoice->SetString(categoryChoice->GetSelection(), GetCategoryChoiceLabel(
        imagesList->GetCurrentCategoryName(), imagesList->GetCurrentCategoryCount()));
}

void WallpapersFrame::OnExit(wxCommandEvent &event) {
    if (wxMessageBox("Do you want to exit the app ?", "Exit confirmation",
                     wxYES | wxYES_DEFAULT | wxNO | wxICON_WARNING) == wxYES)
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
                                                       imagesList(new ImagesList(this)),
                                                       pMenuBar(new wxMenuBar(3)) {

    SetIcon(GetAppIcon());
    this->SetBackgroundColour(*ColorSettings::Accent2());

    auto *sizer = new wxBoxSizer(wxVERTICAL);

    auto *topBarSizer = new wxBoxSizer(wxHORIZONTAL);

    WallpapersFrame::SetMinSize(imagesList->GetMinSize());
    categoryChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    categoryChoice->SetBackgroundColour(*ColorSettings::Accent1());
    Connect(categoryChoice->GetId(), wxEVT_CHOICE, wxCommandEventHandler(WallpapersFrame::OnChoice));
    categoryChoice->Disable();

    addCatButton = new wxButton(this, id, "Add category", wxDefaultPosition, wxDefaultSize);
    addCatButton->Bind(wxEVT_BUTTON, wxCommandEventHandler(WallpapersFrame::OnAddCategory), this);

    removeCatButton = new wxButton(this, id, "Remove category", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    removeCatButton->Bind(wxEVT_BUTTON, wxCommandEventHandler(WallpapersFrame::OnRemoveCategory), this);

    SetupMenuBar();
    this->SetMenuBar(pMenuBar);
    this->CreateStatusBar();
    this->GetStatusBar()->SetBackgroundColour(*ColorSettings::Accent0());
    this->GetStatusBar()->SetForegroundColour(*wxWHITE);
    pStatusBarText = new wxStaticText( this->GetStatusBar(), wxID_ANY,wxT("Validation failed"), wxPoint(10, 5), wxDefaultSize, 0 );

    topBarSizer->Add(categoryChoice, 2, wxEXPAND, 20);
    topBarSizer->AddSpacer(10);
    topBarSizer->Add(addCatButton);
    topBarSizer->AddSpacer(5);
    topBarSizer->Add(removeCatButton);
    sizer->Add(topBarSizer, 0, wxALL, 10);
    sizer->Add(imagesList, 15, wxEXPAND, 0);

    SetSizer(sizer);

    imagesList->SetParentSBText(pStatusBarText);

    Bind(wxEVT_KEY_DOWN, wxKeyEventHandler(WallpapersFrame::OnKeyDown), this);
    imagesList->Bind(wxEVT_KEY_DOWN, wxKeyEventHandler(WallpapersFrame::OnKeyDown), this);
}

void WallpapersFrame::SetupMenuBar() {

    pFileMenu = new wxMenu();
    pFileMenu->Append(ADD_FILES, _T("&Add Files\tCtrl+O"));
    pFileMenu->AppendSeparator();
    pFileMenu->Append(wxID_EXIT, _T("&Quit\tCtrl+Q"));
    pMenuBar->Insert(0, pFileMenu, "&File");

    pSelectionMenu = new wxMenu();
    pSelectionMenu->Append(DELETE_SELECTED, _T("&Delete selected files\tDel"));
    pMenuBar->Insert(1, pSelectionMenu, "&Select");

    pHelpMenu = new wxMenu();
    pHelpMenu->Append(wxID_ABOUT, _T("&About"));
    pMenuBar->Insert(2, pHelpMenu, "&Help");
}

void WallpapersFrame::LoadWallpapers() {
    auto oldTitle(wxTopLevelWindowMSW::GetTitle());
    SetCursor(*wxHOURGLASS_CURSOR);
    SetTitle("Loading wallpapers...");
    auto countByCat = imagesList->LoadImages();
    SetTitle(oldTitle);

    int sum(0);
    for (const auto &cnt: imagesList->GetCountByCat()) {
        categoryChoice->Append(GetCategoryChoiceLabel(imagesList->GetCategoryName(cnt.first), cnt.second));
        sum += cnt.second;
    }

    categoryChoice->SetSelection(0);
    categoryChoice->Enable(true);

    SetCursor(*wxSTANDARD_CURSOR);
    pStatusBarText->SetLabelText("Loaded " + std::to_string(sum) + (sum > 1 ? " images" : " image."));
}

void WallpapersFrame::DeleteSelectedFiles(wxCommandEvent &event) {
    auto deletedImagesCount(imagesList->DeleteSelectedFiles());

    if (deletedImagesCount > 0) {
        SetStatusText("Deleted " +
            (deletedImagesCount == 1 ? "1 image." : (std::to_string(deletedImagesCount) + " images.")));
    }

    auto choice = categoryChoice->GetSelection();
    categoryChoice->SetString(choice,
                              GetCategoryChoiceLabel(imagesList->GetCategoryName(choice),
                                                     imagesList->GetCurrentCategoryCount()));
}

void WallpapersFrame::OnChoice(wxCommandEvent &event) {
    imagesList->SetCurrentCategory(categoryChoice->GetSelection());

    imagesList->SetFocus();
}

void WallpapersFrame::OnAddCategory(wxCommandEvent &event) {
    wxTextEntryDialog textEntry(this, "Enter the new category name");
    if (textEntry.ShowModal() == wxID_CANCEL)
        return;

    auto name = textEntry.GetValue();
    if (name.empty()) {
        wxMessageBox("Category name can't be empty");
    }

    auto nameString = name.ToStdString();

    if (imagesList->DoesCategoryExists(nameString)) {
        wxMessageBox("Category already exists");
        return;
    }

    imagesList->AddEmptyCategory(nameString);
    categoryChoice->Append(GetCategoryChoiceLabel(nameString, 0));
    categoryChoice->SetSelection(imagesList->GetCurrentCategoryIndex());
}

void WallpapersFrame::OnRemoveCategory(wxCommandEvent &event) {
    int res = wxMessageBox("Are you sure you want to delete this category ?", "Deletion confirmation",
                           wxYES | wxNO | wxNO_DEFAULT);
    if (res == wxNO)
        return;

    imagesList->DeleteCategory(categoryChoice->GetSelection());

    RefreshCategoryChoice();
}

void WallpapersFrame::OnKeyDown(wxKeyEvent &event) {
    if (wxGetKeyState(WXK_CONTROL)) {
        if (event.GetUnicodeKey() == L'A') {
            imagesList->SelectAllCurrent();
        }

        if (event.GetUnicodeKey() == L'D') {
            imagesList->UnselectAll();
        }
    }

    event.Skip();
}

void WallpapersFrame::RefreshCategoryChoice() {
    categoryChoice->Clear();

    auto countByCat = imagesList->GetCountByCat();

    for (const auto& count : countByCat)
        categoryChoice->Append(GetCategoryChoiceLabel(
            imagesList->GetCategoryName(count.first), count.second));

    categoryChoice->SetSelection(0);
}
