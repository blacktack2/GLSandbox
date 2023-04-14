#include "FileUtils.h"

#include <nfd.h>

bool FileUtils::openFileDialog(std::string& filePath) {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, filePath.substr(0, filePath.find_last_of('/')).c_str(), &outPath);

    if (result != NFD_OKAY)
        return false;

    filePath = outPath;
    free(outPath);
    return true;
}
