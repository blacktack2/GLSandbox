#include "SerializationUtils.h"


static constexpr char gMARK_PREFIX = '$';
static constexpr char gMARK_BEGIN_SUFFIX[] = "Begin";
static constexpr char gMARK_END_SUFFIX[] = "End";

void SerializationUtils::writeDataPoint(std::ostream& stream, char prefix, const std::string& id,
                                        const std::string& serializedData) {
    stream << prefix << id << " " << serializedData << "\n";
}

void SerializationUtils::writeRawMark(std::ostream& stream, const std::string& id) {
    stream << gMARK_PREFIX << id << "\n";
}

void SerializationUtils::writeBeginMark(std::ostream& stream, const std::string& id) {
    writeRawMark(stream, id + gMARK_BEGIN_SUFFIX);
}

void SerializationUtils::writeEndMark(std::ostream& stream, const std::string& id) {
    writeRawMark(stream, id + gMARK_END_SUFFIX);
}

bool SerializationUtils::seekNextDataPoint(std::istream& stream, std::streampos end, char dataPrefix, std::string& dataID) {
    while (stream.tellg() < end) {
        char prefix;
        stream >> prefix;
        if (prefix == dataPrefix) {
            stream >> dataID;
            return true;
        }

        skipToNextLine(stream);
    }

    return false;
}

bool SerializationUtils::findNextRawMark(std::istream& stream, std::streampos end,
                                         const std::string& markID, std::streampos& markPos) {
    std::streampos start = stream.tellg();

    std::streampos current;
    while ((current = stream.tellg()) < end && !stream.eof()) {
        char prefix;
        stream >> prefix;
        if (prefix != gMARK_PREFIX) {
            skipToNextLine(stream);
            continue;
        }

        std::string searchMarkID;
        stream >> searchMarkID;
        if (searchMarkID != markID) {
            skipToNextLine(stream);
            continue;
        }

        markPos = current;

        stream.seekg(start);
        return true;
    }

    stream.seekg(start);
    return false;
}

bool SerializationUtils::findNextMarkPair(std::istream& stream, std::streampos end,
                                          const std::string& markID, std::streampos& beginPos, std::streampos& endPos) {
    std::streampos start = stream.tellg();

    std::streampos tempBeginPos;
    if (!findNextRawMark(stream, end, markID + gMARK_BEGIN_SUFFIX, tempBeginPos))
        return false;

    stream.seekg(tempBeginPos);
    std::streampos tempEndPos;
    if (!findNextRawMark(stream, end, markID + gMARK_END_SUFFIX, tempEndPos))
        return false;

    beginPos = tempBeginPos;
    endPos = tempEndPos;

    stream.seekg(start);
    return true;
}

std::streampos SerializationUtils::findEnd(std::istream& stream) {
    std::streampos start = stream.tellg();
    stream.seekg(0, std::ios::end);
    std::streampos end = stream.tellg();
    stream.seekg(start);
    return end;
}

void SerializationUtils::skipToNextLine(std::istream& stream) {
    if (!stream.ignore(std::numeric_limits<std::streamsize>::max(), stream.widen('\n'))) {
        stream.seekg(0, std::ios::end);
        stream.setstate(std::ios::eofbit);
    }
}

std::string SerializationUtils::readLine(std::istream& stream, char delimiter) {
    std::string result;
    stream >> std::ws;
    std::getline(stream, result, delimiter);
    return result;
}
