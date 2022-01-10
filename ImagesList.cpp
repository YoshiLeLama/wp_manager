//
// Created by Antoine Roumilhac on 24/12/2021.
//

#include "ImagesList.h"

BEGIN_EVENT_TABLE(ImagesList, wxVListBox)
        EVT_LEFT_DCLICK(ImagesList::OnDoubleClick)
        EVT_LEFT_DOWN(ImagesList::OnLeftClick)
        EVT_KEY_DOWN(ImagesList::OnKeyDown)
        EVT_SIZE(ImagesList::OnSize)
END_EVENT_TABLE()

ImagesList::ImagesList(wxWindow *parent,
                       const std::string &dataDir,
                       wxWindowID id,
                       const wxPoint &pos,
                       const wxSize &size)
    : wxVListBox(parent, id, pos, size, wxBORDER_NONE),
      dataDirectory(dataDir),
      selectionBrush(*wxRED, wxBRUSHSTYLE_BDIAGONAL_HATCH),
      currentCategory(0),
      parentSBText(nullptr) {

    this->SetBackgroundColour(*ColorSettings::Accent1());
    this->SetForegroundColour(*ColorSettings::Accent1());
}

void ImagesList::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const {

    auto test = wxSystemSettings::GetAppearance().IsDark();

    for (size_t i = 0; i < GetItemByRowCount(); ++i) {
        int index = static_cast<int>(n * GetItemByRowCount() + i);
        if (index < GetCurrentCategoryCount()) {
            dc.DrawBitmap(imagesByCategory.at(categories.at(currentCategory)).at(index).second,
                          GetImagePos(n - GetVisibleBegin(), i));

            if (selectedImages.contains(index)) {
                dc.SetBrush(selectionBrush);
                dc.DrawRectangle(GetImagePos(n - GetVisibleBegin(), i), {image_width, image_height});
            }
        }
    }
}

wxCoord ImagesList::OnMeasureItem(size_t n) const {
    return image_height + vertical_margin;
}

std::map<int, int> ImagesList::LoadImages() {
    imagesByCategory.clear();

    auto subDirs(GetSubdirectories("data"));

    for (const auto &dir: subDirs)
        LoadFilesInDir(dir);

    if (imagesByCategory.empty()) {
        AddEmptyCategory(GetDefaultCategoryName());
    }

    currentCategory = 0;

    UpdateDisplay();

    return GetCountByCat();
}

fs::path RemoveRootDirName(const fs::path &path) {
    auto pathString = path.string();
    auto iter = pathString.find_first_of("\\/");

    if (iter == std::string::npos) return "";
    else return fs::path(pathString.substr(iter + 1, pathString.size()));
}

int ImagesList::LoadFilesInDir(const fs::path &dir) {
    std::vector<std::pair<std::string, std::future<wxBitmap>>> results;
    auto paths = GetFilesInDir(dir);

    for (const auto &path: paths) {
        results.emplace_back(path, std::async(std::launch::async, &ImagesList::LoadImg, this, path));
    }

    auto name = RemoveRootDirName(dir);

    categories.push_back(name.string());
    imagesByCategory.insert({name.string(), {}});

    for (auto &res: results) {
        res.second.wait();
        AddImageToCategory(name.string(), res.first, res.second.get());
    }

    return results.size();
}

inline bool ImagesList::IsClickValid(const wxPoint &pos) const {
    return (pos.x % (image_width + horizontal_margin) > horizontal_margin) &&
        (pos.y % (image_height + vertical_margin) > vertical_margin) &&
        pos.x < GetItemByRowCount() * (image_width + horizontal_margin);
}

void ImagesList::OnDoubleClick(wxMouseEvent &evt) {
    auto click_pos(evt.GetPosition());

    if (isSelecting || !IsClickValid(click_pos))
        return;

    // Since there, we assume that the click has been done on an image

    size_t rowIndex
        (std::floor(static_cast<double>(click_pos.y) / (image_height + vertical_margin)
                        + static_cast<double>(GetVisibleBegin())));

    size_t indexInRow(std::floor(static_cast<double>(click_pos.x) / (image_width + horizontal_margin)));

    if (indexInRow >= GetItemByRowCount())
        return;

    auto index = rowIndex * GetItemByRowCount() + indexInRow;

    if (index < GetCurrentCategoryCount()) {
        SetWallpaper(std::filesystem::absolute(imagesByCategory.at(GetCurrentCategoryName()).at(index).first).string());
    }
}

wxBitmap ImagesList::LoadImg(const fs::path &path) const {
    wxImage img;
    img.LoadFile(path.string(), GetFileExt(path) == ".png" ? wxBITMAP_TYPE_PNG : wxBITMAP_TYPE_JPEG);
    return {img.Scale(image_width, image_height, wxIMAGE_QUALITY_BOX_AVERAGE)};
}

int ImagesList::AddImages(const std::vector<std::filesystem::path> &paths) {
    std::map<std::string, std::future<wxBitmap>> results;

    for (const auto &path: paths) {
        results.insert({path.string(), std::async(&ImagesList::LoadImg, this, path)});
    }

    for (auto &result: results) {
        result.second.wait();
        imagesByCategory.at(GetCurrentCategoryName()).emplace_back(result.first, result.second.get());
    }

    UpdateDisplay();

    return static_cast<int>(results.size());
}

void ImagesList::AddImage(const fs::path &path) {
    imagesByCategory.at(GetCurrentCategoryName()).emplace_back(path,
                                                               LoadImg(path));

    UpdateDisplay();

}

void ImagesList::UpdateDisplay() {
    if (categories.empty())
        throw std::runtime_error("Cannot draw without any category displayed");

    UpdateItemCount();

    RefreshAll();
}

void ImagesList::OnKeyDown(wxKeyEvent &evt) {
    /*if (evt.GetKeyCode() == wxKeyCode::WXK_DELETE) {
        DoDeleteSelectedFiles();
    }*/


}

void ImagesList::DeleteImage(int index) {
    RemoveFile(imagesByCategory.at(GetCurrentCategoryName()).at(index).first);
    imagesByCategory.at(GetCurrentCategoryName()).erase(
        imagesByCategory.at(GetCurrentCategoryName()).begin() + static_cast<long long>(index));

    UpdateItemCount();

    RefreshAll();
}

int ImagesList::DeleteSelectedFiles() {
    int selectedImagesCount = static_cast<int>(selectedImages.size());

    if (selectedImagesCount <= 0) {
        wxMessageBox("No file selected");
        return -1;
    }
    int confirm = wxMessageBox(
        (selectedImagesCount == imagesByCategory.size()) ? "Are you sure you want to remove all wallpapers ?" :
        ("Do you want to remove the " + (selectedImagesCount == 1 ? "selected image ?" :
                                         std::to_string(selectedImagesCount) + " selected images ?")),
        "Deletion confirmation", wxYES | wxNO | wxNO_DEFAULT | wxICON_WARNING);

    if (confirm == wxNO)
        return -1;

    int offset(0);
    for (auto i: selectedImages) {
        DeleteImage(i - offset);
        ++offset;
    }

    if (imagesByCategory.empty()) {
        std::ofstream idFile("data/id");

        idFile << 0;
    }

    selectedImages.clear();

    UpdateItemCount();

    RefreshAll();

    return selectedImagesCount;
}

int ImagesList::GetItemByRowCount() const {
    auto size(GetSize());

    return (size.x - horizontal_margin) / (image_width + horizontal_margin);
}

void ImagesList::OnLeftClick(wxMouseEvent &evt) {
    int index = GetIndex(evt.GetPosition());

    if (wxGetKeyState(WXK_CONTROL)) {
        if (index == -1 || index >= imagesByCategory.at(GetCurrentCategoryName()).size()) {
            return;
        }
        if (!selectedImages.contains(index))
            selectedImages.insert(index);
        else
            selectedImages.erase(index);

        RefreshRow(evt.GetPosition().y / (image_height + vertical_margin) + GetVisibleBegin());
    } else {
        if (index == -1 || index >= imagesByCategory.at(GetCurrentCategoryName()).size()) {
            selectedImages.clear();
            RefreshAll();
            if (parentSBText) parentSBText->SetLabelText("No image selected");
            return;
        }
        selectedImages.clear();
        selectedImages.insert(index);
        RefreshAll();
    }

    if (parentSBText) {
        int selectedCount(static_cast<int>(selectedImages.size()));
        parentSBText->SetLabelText(std::to_string(selectedCount) +
            (selectedCount > 1 ? " images selected." : " image selected."));
    }
}

wxSize ImagesList::GetMinSize() const {
    // Taille minimale arbitraire
    return {image_width + 8 * horizontal_margin, image_height + 4 * vertical_margin};
}

void ImagesList::OnSize(wxSizeEvent &evt) {
    SetSize(evt.GetSize());
    UpdateItemCount();
    RefreshAll();
}

wxPoint ImagesList::GetImagePos(size_t row, size_t col) const {
    return {static_cast<int>(col) * (image_width + horizontal_margin) + horizontal_margin,
            static_cast<int>(row) * (image_height + vertical_margin) + vertical_margin};
}

int ImagesList::GetIndex(const wxPoint &pos) const {
    if (!IsClickValid(pos))
        return -1;

    int rowIndex(static_cast<int>(GetVisibleBegin()) +
        static_cast<int>(std::floor((pos.y - vertical_margin) / (image_height + vertical_margin))));

    int indexInRow(static_cast<int>(std::floor((pos.x - horizontal_margin) / (image_width + horizontal_margin))));

    return rowIndex * static_cast<int>(GetItemByRowCount()) + indexInRow;
}

void ImagesList::SetParentSBText(wxStaticText *sbText) {
    parentSBText = sbText;
}

void ImagesList::SetImageHeight(int height) {
    image_height = height > 0 ? height : DEFAULT_HEIGHT;
    LoadImages();
}

void ImagesList::SetImageWidth(int width) {
    image_width = width > 0 ? width : DEFAULT_WIDTH;
    LoadImages();
}

void ImagesList::SetCurrentCategory(int index) {
    if (currentCategory == index)
        return;

    selectedImages.clear();
    currentCategory = index;

    UpdateDisplay();
}

int ImagesList::GetCategoryCount(int index) const {
    return static_cast<int>(imagesByCategory.at(categories.at(index)).size());
}

int ImagesList::GetCurrentCategoryCount() const {
    return GetCategoryCount(currentCategory);
}

void ImagesList::AddEmptyCategory(const std::string &name) {
    imagesByCategory.insert({name, {}});
    categories.push_back(name);
    currentCategory = static_cast<int>(imagesByCategory.size()) - 1;

    CreateSubdirectory(name, dataDirectory);

    UpdateDisplay();
}

void ImagesList::DeleteCategory(int index) {
    auto categoryName(GetCategoryName(index));
    imagesByCategory.erase(categoryName);
    categories.erase(categories.cbegin() + index);
    currentCategory = 0;

    RemoveSubdirectory(categoryName, dataDirectory);

    if (categories.empty()) {
        AddEmptyCategory(GetDefaultCategoryName());
    }

    UpdateDisplay();
}

void ImagesList::SelectAllCurrent() {
    for (int i(0); i < GetCurrentCategoryCount(); ++i)
        selectedImages.insert(i);

    RefreshAll();
}

void ImagesList::UnselectAll() {
    selectedImages.clear();
    RefreshAll();
}

std::string ImagesList::GetDefaultCategoryName() {
    return "Default";
}

std::string ImagesList::GetCategoryName(int index) {
    if (index >= categories.size())
        throw std::runtime_error("There no category at given index");

    return categories.at(index);
}

int ImagesList::GetCurrentCategoryIndex() const {
    return currentCategory;
}

void ImagesList::UpdateItemCount() {
    wxVListBox::SetItemCount(std::ceil(
        static_cast<double>(imagesByCategory.at(categories.at(currentCategory)).size())
            / static_cast<double>(GetItemByRowCount())));
}

std::string ImagesList::GetCurrentCategoryName() const {
    if (currentCategory >= categories.size())
        throw std::runtime_error("Current category index is invalid");

    return categories.at(currentCategory);
}

void ImagesList::AddImageToCategory(const std::string &catName, const fs::path &path, const wxBitmap &bitmap) {
    imagesByCategory.at(catName).push_back({path, bitmap});
}

fs::path ImagesList::GetDataDir() {
    return dataDirectory;
}

void ImagesList::SetDataDir(const fs::path &path) {
    dataDirectory = path;
}

std::map<int, int> ImagesList::GetCountByCat() {
    std::map<int, int> countByCat;

    for (int i(0); i < categories.size(); ++i) {
        countByCat.insert({i, static_cast<int>(imagesByCategory.at(categories[i]).size())});
    }

    return countByCat;
}

bool ImagesList::DoesCategoryExists(const std::string &name) {
    return std::find(categories.begin(), categories.end(), name) != categories.end();
}
