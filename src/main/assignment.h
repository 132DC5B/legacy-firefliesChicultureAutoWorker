#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H
#include <string>
#include <vector>
#include "answers.h"

bool fetchAssignmentId(const std::string& date, std::string& assignmentId, std::string& level);
bool fetchQuestions(const std::string& assignmentId, const std::string& level, QuizInfo& finalInfo);

#endif // ASSIGNMENT_H
