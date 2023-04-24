#pragma once
#include <filesystem>
#include <string>
#include <vector>

class FileUtils {
public:
    /**
     * @brief Open file choose dialog.
     * @param filePath Output filepath. Left unchanged if closed or on error.
     * @param defaultLocation Location to open the file dialog in, if filePath contains no directory component.
     * @param filters List of {name, extension} containing valid extensions to include in file list.
     * @return true if a new filepath has been chosen, otherwise false on close or error.
     */
    static bool openFileDialog(std::filesystem::path& filepath, const std::filesystem::path& defaultLocation,
                               const std::vector<std::string>& filters);
    /**
     * @brief Open file choose/create dialog.
     * @param filePath Output filepath. Left unchanged if closed or on error.
     * @param defaultLocation Location to open the file dialog in, if filePath contains no directory component.
     * @param filters List of {name, extension} containing valid extensions to include in file list.
     * @return true if a new filepath has been chosen, otherwise false on close or error.
     */
    static bool openSaveDialog(std::filesystem::path& filepath, const std::filesystem::path& defaultLocation,
                               const std::vector<std::string>& filters);
};