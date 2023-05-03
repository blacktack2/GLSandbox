#include "FileUtils.h"

#include <nfd.h>

std::string parseFilterList(const std::vector<std::string>& filters) {
    std::string filterList;
    for (const std::string& filter : filters) {
        if (!filterList.empty())
            filterList.append(";");
        filterList.append(filter);
    }
    return filterList;
}

bool FileUtils::openFileDialog(std::filesystem::path& filepath, const std::filesystem::path& defaultLocation,
                               const std::vector<std::string>& filters) {
    nfdchar_t* outPath = nullptr;
    std::string filterList = parseFilterList(filters);

    nfdresult_t result = NFD_OpenDialog(
        filterList.empty() ? nullptr : filterList.c_str(),
        filepath.empty() ?
            std::filesystem::path(defaultLocation).make_preferred().string().c_str() :
            filepath.parent_path().make_preferred().string().c_str(),
        &outPath
    );

    if (result == NFD_CANCEL || result != NFD_OKAY)
        return false;

    filepath = outPath;
    free(outPath);
    return true;
}

bool FileUtils::openSaveDialog(std::filesystem::path& filepath, const std::filesystem::path& defaultLocation,
                               const std::vector<std::string>& filters) {
    nfdchar_t* outPath = nullptr;
    std::string filterList = parseFilterList(filters);

    nfdresult_t result = NFD_SaveDialog(
        filterList.empty() ? nullptr : filterList.c_str(),
        filepath.empty() ?
            std::filesystem::path(defaultLocation).make_preferred().string().c_str() :
            filepath.parent_path().make_preferred().string().c_str(),
        &outPath
    );

    if (result == NFD_CANCEL || result != NFD_OKAY)
        return false;

    filepath = outPath;
    free(outPath);
    return true;
}
