#ifndef DATE_H
#define DATE_H

#include <vector>
#include <string>

std::vector<std::string> loadDatesFromFile(const std::string &filename);
std::vector<std::string> loadDatesFromLines(const std::vector<std::string> &lines);

#endif // DATE_H