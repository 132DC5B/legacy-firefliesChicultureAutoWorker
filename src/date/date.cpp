#include "date.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <vector>
#include <string>

static std::string trim(const std::string &str)
{
    if (str.empty())
        return "";

    const std::string whitespace = " \t\n\r";

    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first)
        return "";

    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

static std::string removeBOM(std::string line)
{
    if (line.size() >= 3)
    {
        if (static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF)
        {
            return line.substr(3);
        }
    }
    return line;
}

static bool isWeekend(std::time_t t)
{
    std::tm tm;
    localtime_s(&tm, &t);
    // tm_wday: 0 = Sunday, 6 = Saturday
    return (tm.tm_wday == 0 || tm.tm_wday == 6);
}

static std::time_t stringToTime(const std::string &dateStr)
{
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    if (ss.fail())
        return (std::time_t)-1;

    tm.tm_hour = 12;
    tm.tm_min = 0;
    tm.tm_sec = 0;

    tm.tm_isdst = -1;

    return std::mktime(&tm);
}

static std::string timeToString(std::time_t t)
{
    std::tm tm;
    localtime_s(&tm, &t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return std::string(buf);
}

static void processDateLine(const std::string &line, std::vector<std::string> &dates)
{
    std::string cleanLine = removeBOM(line);

    size_t slashPos = cleanLine.find('/');

    if (slashPos != std::string::npos)
    {

        std::string startStr = trim(cleanLine.substr(0, slashPos));
        std::string endStr = trim(cleanLine.substr(slashPos + 1));

        std::time_t currentT = stringToTime(startStr);
        std::time_t endT = stringToTime(endStr);

        if (currentT == (std::time_t)-1 || endT == (std::time_t)-1)
            return;

        if (currentT > endT)
            std::swap(currentT, endT);

        while (currentT <= endT)
        {
            if (!isWeekend(currentT))
            {
                dates.push_back(timeToString(currentT));
            }
            currentT += 86400;
        }
    }
    else
    {
        std::string singleDate = trim(cleanLine);
        std::time_t t = stringToTime(singleDate);
        if (t != (std::time_t)-1)
        {
            if (!isWeekend(t))
            {
                dates.push_back(singleDate);
            }
        }
    }
}

std::vector<std::string> loadDatesFromFile(const std::string &filename)
{
    std::vector<std::string> dates;
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        bool isFirstLine = true;
        while (std::getline(file, line))
        {
            if (isFirstLine)
            {
                line = removeBOM(line);
                isFirstLine = false;
            }

            line = trim(line);

            if (!line.empty())
            {
                processDateLine(line, dates);
            }
        }
        file.close();
    }
    return dates;
}

std::vector<std::string> loadDatesFromLines(const std::vector<std::string> &lines)
{
    std::vector<std::string> dates;
    for (const auto &line : lines)
    {
        std::string trimmed = trim(line);
        if (!trimmed.empty())
        {
            processDateLine(trimmed, dates);
        }
    }
    return dates;
}