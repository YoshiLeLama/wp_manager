//
// Created by Antoine Roumilhac on 23/12/2021.
//

#ifndef WP_MANAGER_IMAGESLIST_H_
#define WP_MANAGER_IMAGESLIST_H_

#include "FileUtils.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/vlbox.h>
#include <filesystem>
#include <thread>
#include <map>
#include <set>
#include <future>

class ImagesList : public wxVListBox {
public:
    explicit ImagesList(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint &pos = wxDefaultPosition,
                        const wxSize &size = wxDefaultSize);
    void UpdateImages();
    void AddImages(const std::vector<std::string> &paths);
    void AddImages(const std::vector<std::wstring> &paths);
    void AddImage(const std::string &path);
    void SetItemCount(size_t count) override;
    size_t GetItemByRowCount() const;

    void SetImageHeight(int height) {
        image_height = height > 0 ? height : DEFAULT_HEIGHT;
        UpdateImages();
    }

    void SetImageWidth(int width) {
        image_width = width > 0 ? width : DEFAULT_WIDTH;
        UpdateImages();
    }

    void DoDeleteSelectedFiles();
    wxSize GetMinSize() const;

protected:
    void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const override;
    wxCoord OnMeasureItem(size_t n) const override;

    void OnSize(wxSizeEvent &evt);
    void OnLeftClick(wxMouseEvent &evt);
    void OnDoubleClick(wxMouseEvent &evt);
    void OnKeyDown(wxKeyEvent &evt);
    void DoDeleteWallpaper(size_t pos);
    std::vector<std::pair<std::string, wxBitmap>> images;
    std::set<int> selectedImages;
    int image_height;
    int image_width;
    int horizontal_margin;
    int vertical_margin;
    static const int DEFAULT_HEIGHT = 100;
    static const int DEFAULT_WIDTH = 178;
    static const int DEFAULT_H_MARGIN = 10;
    static const int DEFAULT_V_MARGIN = 10;

    wxBitmap LoadImg(const std::string &path);
DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ImagesList, wxVListBox)
        EVT_LEFT_DCLICK(ImagesList::OnDoubleClick)
        EVT_LEFT_DOWN(ImagesList::OnLeftClick)
        EVT_KEY_DOWN(ImagesList::OnKeyDown)
        EVT_SIZE(ImagesList::OnSize)
END_EVENT_TABLE()

ImagesList::ImagesList(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
    : wxVListBox(parent, id, pos, size),
      image_height(DEFAULT_HEIGHT), image_width(DEFAULT_WIDTH),
      horizontal_margin(DEFAULT_H_MARGIN), vertical_margin(DEFAULT_V_MARGIN) {
}

void ImagesList::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const {
    for (size_t i = 0; i < GetItemByRowCount(); ++i) {
        auto index = n * GetItemByRowCount() + i;
        if (index < images.size())
            dc.DrawBitmap(images.at(index).second,
                          static_cast<int>(i) * (image_width + horizontal_margin) + horizontal_margin,
                          static_cast<int>(n - GetVisibleBegin()) * (image_height + vertical_margin) + vertical_margin);
    }
}

wxCoord ImagesList::OnMeasureItem(size_t n) const {
    return image_height + vertical_margin;
}

void ImagesList::UpdateImages() {
    std::vector<std::pair<std::string, std::future<wxBitmap>>> results;

    images.clear();

    auto paths = GetFilesInDir("data");
    SetItemCount(paths.size());
    for (const auto &path: paths) {
        results.emplace_back(path, std::async(std::launch::async, &ImagesList::LoadImg, this, path));
    }

    for (auto &res : results) {
        res.second.wait();
        images.emplace_back(res.first, res.second.get());
    }

    SetItemCount(images.size());
    RefreshAll();
}

void ImagesList::OnDoubleClick(wxMouseEvent &evt) {
    size_t rowIndex
        (std::floor(static_cast<double>(evt.GetPosition().y) / (image_height + vertical_margin)
                        + static_cast<double>(GetVisibleBegin())));

    size_t indexInRow(std::floor(static_cast<double>(evt.GetPosition().x) / (image_width + horizontal_margin)));

    if (indexInRow >= GetItemByRowCount())
        return;

    auto index = rowIndex * GetItemByRowCount() + indexInRow;

    if (index < images.size())
        SetWallpaper(std::filesystem::absolute(images.at(index).first));
}

wxBitmap ImagesList::LoadImg(const std::string &path) {
    wxImage img;
    img.LoadFile(path, GetFileExt(path) == ".png" ? wxBITMAP_TYPE_PNG : wxBITMAP_TYPE_JPEG);
    return {img.Scale(image_width, image_height, wxIMAGE_QUALITY_BOX_AVERAGE)};
}

void ImagesList::AddImages(const std::vector<std::string> &paths) {
    std::map<std::string, std::future<wxBitmap>> results;

    for (const auto &path: paths) {
        results.insert({path, std::async(&ImagesList::LoadImg, this, path)});
    }

    for (auto &result: results) {
        result.second.wait();
        images.emplace_back(result.first, result.second.get());
    }

    SetItemCount(images.size());
    RefreshAll();
}

void ImagesList::AddImage(const std::string &path) {
    wxImage img;
    img.LoadFile(path, GetFileExt(path) == ".png" ? wxBITMAP_TYPE_PNG : wxBITMAP_TYPE_JPEG);
    images.emplace_back(path, wxBitmap(img.Scale(image_width, image_height, wxIMAGE_QUALITY_BOX_AVERAGE)));

    SetItemCount(images.size());
    RefreshAll();
}

void ImagesList::AddImages(const std::vector<std::wstring> &paths) {
    std::vector<std::pair<std::string, std::future<wxBitmap>>> results;

    for (const auto &wPath: paths) {
        std::string sPath;
        std::transform(wPath.begin(), wPath.end(), std::back_inserter(sPath), [](wchar_t wc) {
            return static_cast<char>(wc);
        });

        results.emplace_back(sPath, std::async(std::launch::async, &ImagesList::LoadImg, this, sPath));
    }

    for (auto &res : results) {
        images.emplace_back(res.first, res.second.get());
    }

    SetItemCount(images.size());
    RefreshAll();
}

void ImagesList::OnKeyDown(wxKeyEvent &evt) {
    if (evt.GetKeyCode() == wxKeyCode::WXK_DELETE && GetSelection() != wxNOT_FOUND) {
        int resp = wxMessageBox("Do you want to remove this wallpaper ?",
                                "Delete action",
                                wxOK_DEFAULT | wxCANCEL | wxICON_QUESTION);
        if (resp == wxOK) {
            DoDeleteWallpaper(GetSelection());
        }
    }
}

void ImagesList::DoDeleteWallpaper(size_t pos) {
    RemoveFile(images[pos].first);
    images.erase(images.begin() + static_cast<long long>(pos));
    SetItemCount(GetItemCount() - 1);
    RefreshAll();
}

void ImagesList::DoDeleteSelectedFiles() {
    std::vector<size_t> deleteIndexes;

    unsigned long cookie;
    auto index(GetFirstSelected(cookie));
    while (index != wxNOT_FOUND) {
        deleteIndexes.push_back(index);

        index = GetNextSelected(cookie);
    }

    size_t offset(0);
    for (auto i: deleteIndexes) {
        DoDeleteWallpaper(i - offset);
        ++offset;
    }
    if (deleteIndexes.empty())
        wxMessageBox("No file selected");
    else
        wxMessageBox("Deleted " + std::to_string(deleteIndexes.size()) + " files");
}

void ImagesList::SetItemCount(size_t count) {
    wxVListBox::SetItemCount(std::ceil(
        static_cast<double>(images.size()) / static_cast<double>(GetItemByRowCount())));
}

size_t ImagesList::GetItemByRowCount() const {
    auto size(GetSize());

    return (size.x - horizontal_margin) / (image_width + horizontal_margin);
}

void ImagesList::OnLeftClick(wxMouseEvent &evt) {
}

wxSize ImagesList::GetMinSize() const {
    // Taille minimale arbitraire
    return {image_width + 6 * horizontal_margin, image_height + 4 * vertical_margin};
}

void ImagesList::OnSize(wxSizeEvent &evt) {
    SetSize(evt.GetSize());
    SetItemCount(images.size());
    RefreshAll();
}

#endif //WP_MANAGER_IMAGESLIST_H_
