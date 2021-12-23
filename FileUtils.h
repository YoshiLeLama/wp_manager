//
// Created by Antoine Roumilhac on 21/12/2021.
//

#ifndef WP_MANAGER_FILEUTILS_H_
#define WP_MANAGER_FILEUTILS_H_

#include <fstream>
#include <regex>
#include <filesystem>

#include <ShObjIdl.h>

bool RemoveFile(const std::string& fileRelativePath) {
    auto path(std::filesystem::absolute(fileRelativePath));
    return remove(path);
}

std::wstring GenerateFileID(long long id, int charCount) {
    std::wstring idString(std::to_wstring(id));
    std::wstring zeroBegin(charCount - idString.size(), L'0');
    idString.insert(idString.begin(), zeroBegin.begin(), zeroBegin.end());

    return idString;
}

void SetWallpaper(const std::wstring &path) {
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path.c_str(), SPIF_UPDATEINIFILE);
}

std::vector<std::string> GetFilesInDir(const std::string &path) {
    std::regex idRegex("id$");

    std::vector<std::string> paths;
    for (const auto &file: std::filesystem::directory_iterator(path)) {
        if (!std::regex_search(file.path().string(), idRegex))
            paths.push_back(file.path().string());
    }

    return paths;
}

std::vector<std::wstring> GetFilePaths() {
    std::vector<std::wstring> paths;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr)) {
        IFileOpenDialog *pFileOpen;

        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void **>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            COMDLG_FILTERSPEC rgSpec[] = {
                {L"Images", L"*.jpg;*.jpeg;*.png"}
            };

            hr = pFileOpen->SetFileTypes(1, rgSpec);

            if (SUCCEEDED(hr)) {
                hr = pFileOpen->SetOptions(FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT);

                if (SUCCEEDED(hr)) {
                    hr = pFileOpen->Show(nullptr);

                    if (SUCCEEDED(hr)) {
                        IShellItemArray *pResult;
                        hr = pFileOpen->GetResults(&pResult);

                        if (SUCCEEDED(hr)) {
                            PWSTR pszFilePath = nullptr;
                            DWORD dwNumItems = 0; // number of items in multiple selection
                            std::wstring strSelected; // will hold file paths of selected items

                            hr = pResult->GetCount(&dwNumItems);  // get number of selected items

                            // Loop through IShellItemArray and construct string for display
                            for (DWORD i = 0; i < dwNumItems; i++) {
                                IShellItem *psi = nullptr;

                                hr = pResult->GetItemAt(i, &psi); // get a selected item from the IShellItemArray

                                if (SUCCEEDED(hr)) {
                                    hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                                    if (SUCCEEDED(hr)) {
                                        paths.emplace_back(pszFilePath);
                                        CoTaskMemFree(pszFilePath);
                                    }

                                    psi->Release();
                                }
                            }

                            pResult->Release();
                        }
                    }
                    pFileOpen->Release();
                }
                CoUninitialize();
            }
        }
    }

    return paths;
}

std::wstring GetFileExt(const std::wstring &file) {
    auto back_it(file.end());
    auto beg(file.begin());

    std::wstring ext;
    while (--back_it != beg && *back_it != '.') {
        ext.insert(ext.begin(), *back_it);
    }

    if (back_it == beg) {
        return L"";
    }

    return L'.' + ext;
}

std::string GetFileExt(const std::string &file) {
    auto back_it(file.end());
    auto beg(file.begin());

    std::string ext;
    while (--back_it != beg && *back_it != '.') {
        ext.insert(ext.begin(), *back_it);
    }

    if (back_it == beg) {
        return "";
    }

    return '.' + ext;
}

std::vector<std::wstring> MoveFiles(std::vector<std::wstring> &files) {
    std::vector<std::wstring> newPaths;

    std::ifstream idFile("data/id");

    long long id = 0;
    if (idFile.is_open()) {
        idFile >> id;
    }

    idFile.close();

    for (const auto &path: files) {
        auto ext(GetFileExt(path));

        auto newPath(L"data/" + GenerateFileID(id++, 8) + ext);

        std::filesystem::copy(path, newPath);
        newPaths.push_back(newPath);
    }

    std::ofstream idFileOut("data/id");
    idFileOut << id;
    idFileOut.close();

    return newPaths;
}

#endif //WP_MANAGER_FILEUTILS_H_
