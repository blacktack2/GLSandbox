#include "FileUtils.h"

#include <nfd.h>

bool FileUtils::openFileDialog(std::string& filePath, const std::string& location) {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, location.c_str(), &outPath);

    if (result == NFD_CANCEL) {
        return false;
    } else if (result != NFD_OKAY) {
        std::string error = NFD_GetError();
        std::string adsf;
        return false;
    }

    filePath = outPath;
    free(outPath);
    return true;
}
