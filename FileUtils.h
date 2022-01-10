//
// Created by Antoine Roumilhac on 21/12/2021.
//

#ifndef WP_MANAGER_FILEUTILS_H_
#define WP_MANAGER_FILEUTILS_H_

#include <fstream>
#include <regex>
#include <filesystem>

#include <ShObjIdl.h>
#include <wx/string.h>
#include <wx/arrstr.h>

namespace fs = std::filesystem;

void CreateFolder(const fs::path &name);

bool RemoveFile(const fs::path &path);

std::string GenerateFileID(long long id, int charCount);

void SetWallpaper(const fs::path &path);

std::vector<std::string> GetFilesInDir(const fs::path &path);

std::string GetFileExt(const fs::path &file);

std::vector<fs::path> MoveFiles(const wxArrayString &files, const fs::path &dir, const fs::path &dataDir);

std::vector<std::string> GetSubdirectories(const fs::path &rootDir);

void CreateSubdirectory(const fs::path &name, const fs::path &dir);

void RemoveSubdirectory(const fs::path &name, const fs::path &dir);

#endif //WP_MANAGER_FILEUTILS_H_
