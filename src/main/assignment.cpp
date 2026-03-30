#include "assignment.h"
#include "../../lib//httpClient/httpClient.hpp"
#include "../../lib/cJSON/cJSON.h"
#include <string>
#include <vector>
#include <random>

const std::string BASE_API = "https://fireflies.chiculture.org.hk/api/quiz";
const std::string COOKIE_STUDENT = "cookies_student.txt";
const std::string COOKIE_ADMIN = "cookies_admin.txt";

bool fetchAssignmentId(const std::string &date, std::string &assignmentId, std::string &level)
{
    HttpClient client;
    client.setCookieFile(COOKIE_STUDENT);
    client.addHeader("Accept-Encoding: identity");
    client.addHeader("referer: https://fireflies.chiculture.org.hk/app/assignments/" + date);
    std::string studentUrl = BASE_API + "/assignments/" + date;
    std::string studentResp = client.get(studentUrl);
    cJSON *sJson = cJSON_Parse(studentResp.c_str());
    bool found = false;
    if (sJson)
    {
        cJSON *data = cJSON_GetObjectItem(sJson, "data");
        cJSON *root = data ? data : sJson;
        cJSON *idItem = cJSON_GetObjectItem(root, "id");
        if (!idItem)
            idItem = cJSON_GetObjectItem(root, "_id");
        cJSON *lvItem = cJSON_GetObjectItem(root, "lv");
        if (idItem && idItem->valuestring)
        {
            assignmentId = idItem->valuestring;
            if (lvItem && lvItem->valuestring)
                level = lvItem->valuestring;
            found = true;
        }
        cJSON_Delete(sJson);
    }
    return found;
}

bool fetchQuestions(const std::string &assignmentId, const std::string &level, QuizInfo &finalInfo)
{
    HttpClient client;
    client.setCookieFile(COOKIE_STUDENT);
    client.addHeader("referer: https://fireflies.chiculture.org.hk/app/assignments/" + assignmentId);
    std::string url = BASE_API + "/assignments/" + assignmentId;
    std::string resp = client.get(url);
    finalInfo.assignmentId = assignmentId;
    finalInfo.level = level;
    cJSON *aJson = cJSON_Parse(resp.c_str());
    bool valid = false;
    if (aJson)
    {
        cJSON *data = cJSON_GetObjectItem(aJson, "data");
        cJSON *root = data ? data : aJson;
        cJSON *questions = cJSON_GetObjectItem(root, "questions");
        if (!questions)
        {
            cJSON *article = cJSON_GetObjectItem(root, "article");
            if (article)
                questions = cJSON_GetObjectItem(article, "questions");
        }
        if (questions && cJSON_IsArray(questions))
        {
            int qCount = cJSON_GetArraySize(questions);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 3);
            for (int j = 0; j < qCount; j++)
            {
                cJSON *q = cJSON_GetArrayItem(questions, j);
                cJSON *qid = cJSON_GetObjectItem(q, "_id");
                if (qid && qid->valuestring)
                {
                    QuestionAnswer qa;
                    qa.qId = qid->valuestring;
                    qa.correctAns = dis(gen);
                    finalInfo.questions.push_back(qa);
                }
            }
            if (!finalInfo.questions.empty())
                valid = true;
        }
        cJSON_Delete(aJson);
    }
    finalInfo.valid = valid;
    return valid;
}
