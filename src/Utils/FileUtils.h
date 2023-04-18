#pragma once
#include <string>

class FileUtils {
public:
    static bool openFileDialog(std::string& filePath, const std::string& defaultLocation);
};