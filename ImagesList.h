//
// Created by Antoine Roumilhac on 23/12/2021.
//

#ifndef WP_MANAGER_IMAGESLIST_H_
#define WP_MANAGER_IMAGESLIST_H_

#include "FileUtils.h"

#include "ColorSettings.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/vlbox.h>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <map>
#include <set>
#include <future>

namespace fs = std::filesystem;

class WallpaperFrame;
class ImagesList : public wxVListBox {
public:
    static const int DEFAULT_HEIGHT = 100;
    static const int DEFAULT_WIDTH = 178;
    static const int DEFAULT_H_MARGIN = 10;
    static const int DEFAULT_V_MARGIN = 10;

    explicit ImagesList(wxWindow *parent,
                        const std::string &dataDir = "data",
                        wxWindowID id = wxID_ANY,
                        const wxPoint &pos = wxDefaultPosition,
                        const wxSize &size = wxDefaultSize);

    std::map<int, int> LoadImages();

    int AddImages(const std::vector<std::filesystem::path> &paths);
    void AddImage(const fs::path &path);
    void DeleteImage(int index);

    void AddEmptyCategory(const std::string &name);
    void DeleteCategory(int index);
    static std::string GetDefaultCategoryName();
    std::string GetCategoryName(int index);
    void SetCurrentCategory(int index);
    std::string GetCurrentCategoryName() const;
    int GetCurrentCategoryIndex() const;
    int GetCategoryCount(int index) const;
    int GetCurrentCategoryCount() const;
    std::map<int, int> GetCountByCat();
    bool DoesCategoryExists(const std::string &name);

    fs::path GetDataDir();
    void SetDataDir(const fs::path& path);

    void SelectAllCurrent();
    void UnselectAll();
    void UpdateDisplay();

    void UpdateItemCount();
    int GetItemByRowCount() const;

    void SetImageWidth(int width);
    void SetImageHeight(int height);

    int DeleteSelectedFiles();

    wxSize GetMinSize() const override;
    void SetParentSBText(wxStaticText *sbText);

protected:
    void OnKeyDown(wxKeyEvent &evt);
    void OnSize(wxSizeEvent &evt);

    void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const override;
    wxCoord OnMeasureItem(size_t n) const override;

    int LoadFilesInDir(const fs::path &dir);
    wxBitmap LoadImg(const fs::path &path) const;

    void AddImageToCategory(const std::string &catName, const fs::path &path, const wxBitmap &bitmap);

    wxPoint GetImagePos(size_t row, size_t col) const;
    void OnLeftClick(wxMouseEvent &evt);
    void OnDoubleClick(wxMouseEvent &evt);
    bool IsClickValid(const wxPoint &pos) const;
    int GetIndex(const wxPoint &pos) const;

    std::vector<std::string> categories;
    std::map<std::string, std::vector<std::pair<fs::path, wxBitmap>>> imagesByCategory;
    std::set<int> selectedImages;
    int currentCategory;
    std::filesystem::path dataDirectory;

    bool isSelecting{false};

    int image_height{DEFAULT_HEIGHT};
    int image_width{DEFAULT_WIDTH};
    int horizontal_margin{DEFAULT_H_MARGIN};
    int vertical_margin{DEFAULT_V_MARGIN};

    wxBrush selectionBrush;
    wxStaticText *parentSBText;

DECLARE_EVENT_TABLE()
};

#endif //WP_MANAGER_IMAGESLIST_H_
