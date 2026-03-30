#ifndef ANSWERS_H
#define ANSWERS_H
#include <string>
#include <vector>

struct QuestionAnswer
{
    std::string qId;
    int correctAns;
};

struct QuizInfo
{
    std::string assignmentId;
    std::string level;
    std::vector<QuestionAnswer> questions;
    bool valid = false;
};

bool fetchAnswers(const std::string &assignmentId, const std::string &level, QuizInfo &finalInfo);
bool submitAnswers(const QuizInfo &finalInfo, const std::string &date);
void submitExtraRead(const QuizInfo &finalInfo);

#endif // ANSWERS_H
