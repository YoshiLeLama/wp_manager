//
// Created by Antoine Roumilhac on 24/12/2021.
//

#include "FileUtils.h"

void CreateFolder(const fs::path &name) {
    std::filesystem::create_directory(name);
}

bool RemoveFile(const fs::path &path) {
    auto absolutePath(fs::absolute(path));
    return remove(absolutePath);
}

std::string GenerateFileID(long long id, int charCount) {
    std::string idString(std::to_string(id));
    std::string zeroBegin(charCount - idString.size(), L'0');
    idString.insert(idString.begin(), zeroBegin.begin(), zeroBegin.end());

    return idString;
}

void SetWallpaper(const fs::path &path) {
    SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path.c_str(), SPIF_UPDATEINIFILE);
}

std::vector<std::string> GetFilesInDir(const fs::path &path) {
    std::regex idRegex("id$");

    std::vector<std::string> paths;
    for (const auto &file: std::filesystem::directory_iterator(path)) {
        if (!file.is_directory() && !std::regex_search(file.path().string(), idRegex))
            paths.push_back(file.path().string());
    }

    return paths;
}

std::string GetFileExt(const fs::path &file) {
    auto fileString(file.string());
    auto back_it(fileString.end());
    auto beg(fileString.begin());

    std::string ext;
    while (--back_it != beg && *back_it != '.') {
        ext.insert(ext.begin(), *back_it);
    }

    if (back_it == beg) {
        return "";
    }

    return '.' + ext;
}

std::vector<fs::path> MoveFiles(const wxArrayString &files, const fs::path &dir, const fs::path &dataDir) {
    std::vector<fs::path> newPaths;

    std::ifstream idFile(dataDir / "id");

    long long id = 0;
    if (idFile.is_open()) {
        idFile >> id;
    }

    idFile.close();

    for (const auto &path : files) {
        auto sPath(path.ToStdString());
        auto ext(GetFileExt(sPath));

        auto newPath(dataDir / dir / (GenerateFileID(id++, 8) + ext));

        std::filesystem::copy(sPath, newPath);
        newPaths.push_back(newPath);
    }

    std::ofstream idFileOut(dataDir / "id");
    idFileOut << id;
    idFileOut.close();

    return newPaths;
}

std::vector<std::string> GetSubdirectories(const fs::path &rootDir) {
    std::vector<std::string> subDirs;

    namespace fs = std::filesystem;
    fs::directory_iterator rootDirIter{rootDir};

    for (const auto &elt : rootDirIter) {
        if (elt.is_directory())
            subDirs.push_back(elt.path().string());
    }

    return subDirs;
}

void CreateSubdirectory(const fs::path &name, const fs::path &dir) {
    fs::path newDir(dir);
    newDir /= name;

    fs::create_directory(newDir);
}

void RemoveSubdirectory(const fs::path &name, const fs::path &dir) {
    fs::path dirToDelete(dir);
    dirToDelete /= name;

    fs::remove_all(dirToDelete);
}