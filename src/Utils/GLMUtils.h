#pragma once
#include <glm/glm.hpp>

#include <climits>
#include <sstream>
#include <string>

static glm::vec2 stringToVec2(const std::string& value) {
    const std::string stripped = value.substr(5, value.size() - 6);
    std::stringstream stream(stripped);
    if (stream.eof())
        return glm::vec2();

    float x;
    stream >> x;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    if (stream.eof())
        return glm::vec2(x);

    float y;
    stream >> y;
    return glm::vec2(x, y);
}
static glm::vec3 stringToVec3(const std::string& value) {
    const std::string stripped = value.substr(5, value.size() - 6);
    std::stringstream stream(stripped);
    if (stream.eof())
        return glm::vec3();

    float x;
    stream >> x;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    if (stream.eof())
        return glm::vec3(x);

    float y, z;
    stream >> y;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    stream >> z;
    return glm::vec3(x, y, z);
}
static glm::vec4 stringToVec4(const std::string& value) {
    const std::string stripped = value.substr(5, value.size() - 6);
    std::stringstream stream(stripped);
    if (stream.eof())
        return glm::vec4();

    float x;
    stream >> x;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    if (stream.eof())
        return glm::vec4(x);

    float y, z, w;
    stream >> y;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    stream >> z;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    stream >> w;
    return glm::vec4(x, y, z, w);
}

static glm::ivec2 stringToIVec2(const std::string& value) {
    const std::string stripped = value.substr(6, value.size() - 7);
    std::stringstream stream(stripped);
    if (stream.eof())
        return glm::ivec2();

    int x;
    stream >> x;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    if (stream.eof())
        return glm::ivec2(x);

    int y;
    stream >> y;
    return glm::ivec2(x, y);
}
static glm::ivec3 stringToIVec3(const std::string& value) {
    const std::string stripped = value.substr(6, value.size() - 7);
    std::stringstream stream(stripped);
    if (stream.eof())
        return glm::ivec3();

    int x;
    stream >> x;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    if (stream.eof())
        return glm::ivec3(x);

    int y, z;
    stream >> y;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    stream >> z;
    return glm::ivec3(x, y, z);
}
static glm::ivec4 stringToIVec4(const std::string& value) {
    const std::string stripped = value.substr(6, value.size() - 7);
    std::stringstream stream(stripped);
    if (stream.eof())
        return glm::ivec4();

    int x;
    stream >> x;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    if (stream.eof())
        return glm::ivec4(x);

    int y, z, w;
    stream >> y;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    stream >> z;
    stream.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    stream >> w;
    return glm::ivec4(x, y, z, w);
}
