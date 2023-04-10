#pragma once
#include <functional>
#include <fstream>

class SerializationUtils {
public:
    /**
     * @brief Write a serialized data point to the stream.
     *
     * @param stream Stream to write to.
     * @param prefix Data prefix, used to distinguish different levels of data on read (i.e. separate graph data from
     * node data etc.).
     * @param id Name of the data point used to identify on read.
     * @param serializedData Actual data in a serialized string format.
     */
    static void writeDataPoint(std::ostream& stream, char prefix, const std::string& id,
                               const std::string& serializedData);
    /**
     * @brief Write a raw mark without the begin/end suffix.
     */
    static void writeRawMark(std::ostream& stream, const std::string& id);
    /**
     * @brief Write a begin mark with the given ID.
     * @brief Will follow the format "<mark-prefix><id>Begin\n"
     */
    static void writeBeginMark(std::ostream& stream, const std::string& id);
    /**
     * @brief Write an end mark with the given ID.
     * @brief Will follow the format "<mark-prefix><id>End\n"
     */
    static void writeEndMark(std::ostream& stream, const std::string& id);

    /**
     * @brief Move stream to the position of the next data point with dataPrefix, or end if none is found.
     *
     * @param stream     Stream to search in.
     * @param end        End position to stop searching at.
     * @param dataPrefix Prefix of the data type being searched for.
     * @param dataID     Set to the name of the data point on success. Ignored on failure.
     *
     * @return false if no data point is found before end, otherwise true.
     */
    static bool seekNextDataPoint(std::istream& stream, std::streampos end, char dataPrefix, std::string& dataID);
    /**
     * @brief Find the next mark matching markID.
     * @brief Stream position will be kept at it's original position by return;
     *
     * @param stream     Stream to search in.
     * @param end        End position to stop searching at.
     * @param markID     Mark ID to search for.
     * @param markPos    Set to the position of the mark on success. Ignored on failure.
     *
     * @return false if no matching mark is found before end, otherwise true.
     */
    static bool findNextRawMark(std::istream& stream, std::streampos end,
                             const std::string& markID, std::streampos& markPos);
    /**
     * @brief Find the next mark open-close pair matching markID.
     * @brief Stream position will be kept at it's original position by return;
     *
     * @param stream     Stream to search in.
     * @param end        End positiont o stop searching at.
     * @param markID     Mark ID to search for.
     * @param beginPos   Set to the position of the begin mark on success. Ignored on failure.
     * @param endPos     Set to the position of the end mark on success. Ignored on failure.
     *
     * @return false if unable to find both a matching entry and exit mark, otherwise true.
     */
    static bool findNextMarkPair(std::istream& stream, std::streampos end,
                                 const std::string& markID,
                                 std::streampos& beginPos, std::streampos& endPos);

    /**
     * @brief Find the eof position of the stream.
     * @brief Stream position will be kept at it's original position by return.
     */
    static std::streampos findEnd(std::istream& stream);
    /**
     * @brief Move the stream to the beginning of the next line (as marked by newline '\n' character).
     * @brief If there is no next line the stream will be moved to the end of the file and the eof bit will be set.
     */
    static void skipToNextLine(std::istream& stream);
};
