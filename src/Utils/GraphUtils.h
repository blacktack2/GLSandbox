#pragma once
#include <sstream>
#include <string>

template<typename T>
void concatString(std::stringstream& stream, const std::string& delimiter, T val) {
    stream << val;
}

template<typename... Args, typename T>
void concatString(std::stringstream& stream, const std::string& delimiter, T val, Args... args) {
    stream << val << delimiter;
    concatString(stream, delimiter, args...);
}

template<typename... Args>
std::string generateLabel(const std::string& displayText, Args... args) {
    std::stringstream stream;
    stream << displayText << "##";
    concatString(stream, "_", args...);
    return stream.str();
}

template<typename... Args>
std::string generatePopupID(const std::string& popupName, Args... args) {
    std::stringstream stream;
    stream << popupName;
    concatString(stream, "_", args...);
    return stream.str();
}
