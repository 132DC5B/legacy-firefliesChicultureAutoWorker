#include "answers.h"
#include "../../lib/httpClient/httpClient.hpp"
#include "../../lib/cJSON/cJSON.h"
#include <string>
#include <vector>
#include <iostream>

const std::string BASE_API = "https://fireflies.chiculture.org.hk/api/quiz";
const std::string COOKIE_STUDENT = "cookies_student.txt";
const std::string COOKIE_ADMIN = "cookies_admin.txt";

bool fetchAnswers(const std::string &assignmentId, const std::string &level, QuizInfo &finalInfo)
{
    HttpClient client;
    client.setCookieFile(COOKIE_ADMIN);
    client.addHeader("referer: https://fireflies.chiculture.org.hk/admin/assignments/" + assignmentId);
    std::string adminUrl = BASE_API + "/assignments/" + assignmentId;
    std::string adminResp = client.get(adminUrl);
    finalInfo.assignmentId = assignmentId;
    finalInfo.level = level;
    cJSON *aJson = cJSON_Parse(adminResp.c_str());
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
            for (int j = 0; j < qCount; j++)
            {
                cJSON *q = cJSON_GetArrayItem(questions, j);
                cJSON *qid = cJSON_GetObjectItem(q, "_id");
                cJSON *ansItem = cJSON_GetObjectItem(q, "answer");
                if (!ansItem)
                    ansItem = cJSON_GetObjectItem(q, "correctAnswer");
                if (qid && qid->valuestring && ansItem)
                {
                    QuestionAnswer qa;
                    qa.qId = qid->valuestring;
                    qa.correctAns = ansItem->valueint;
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

bool submitAnswers(const QuizInfo &finalInfo, const std::string &date)
{
    HttpClient client;
    client.setCookieFile(COOKIE_STUDENT);
    client.addHeader("Content-Type: application/json;charset=UTF-8");
    client.addHeader("origin: https://fireflies.chiculture.org.hk");
    client.addHeader("referer: https://fireflies.chiculture.org.hk/app/assignments/" + date);
    cJSON *submitRoot = cJSON_CreateObject();
    cJSON_AddStringToObject(submitRoot, "assignment", finalInfo.assignmentId.c_str());
    cJSON_AddStringToObject(submitRoot, "assignmentId", finalInfo.assignmentId.c_str());
    cJSON_AddStringToObject(submitRoot, "lv", finalInfo.level.c_str());
    cJSON *answersArray = cJSON_CreateArray();
    for (const auto &qa : finalInfo.questions)
    {
        cJSON *ansObj = cJSON_CreateObject();
        cJSON_AddStringToObject(ansObj, "question", qa.qId.c_str());
        cJSON_AddStringToObject(ansObj, "format", "single-select");
        const char *rands_data[] = {"0", "1", "2", "3"};
        cJSON_AddItemToObject(ansObj, "randoms", cJSON_CreateStringArray(rands_data, 4));
        cJSON_AddStringToObject(ansObj, "answered", std::to_string(qa.correctAns).c_str());
        cJSON_AddItemToObject(ansObj, "answeredSeq", cJSON_CreateArray());
        cJSON_AddItemToArray(answersArray, ansObj);
    }
    cJSON_AddItemToObject(submitRoot, "answers", answersArray);
    char *submitPayload = cJSON_PrintUnformatted(submitRoot);
    std::string submitResp = client.post(BASE_API + "/answers", std::string(submitPayload));
    bool ok = (submitResp.find("score") != std::string::npos || submitResp.find("correct") != std::string::npos);
    cJSON_Delete(submitRoot);
    free(submitPayload);
    return ok;
}

void submitExtraRead(const QuizInfo &finalInfo)
{
    HttpClient client;
    client.setCookieFile(COOKIE_STUDENT); // Set student cookie
    // client.addHeader("accept: application/json, text/plain, */*");
    client.addHeader("content-type: application/json;charset=UTF-8");
    // client.addHeader("priority: u=1, i");
    // client.addHeader("sec-ch-ua: \"Not(A:Brand\";v=\"8\", \"Chromium\";v=\"144\", \"Google Chrome\";v=\"144\"");
    // client.addHeader("sec-ch-ua-mobile: ?0");
    // client.addHeader("sec-ch-ua-platform: \"Windows\"");
    cJSON *readJson = cJSON_CreateObject();

    // Log parameters for debugging
    // std::cout << "[Extra Read Params] assignment: " << finalInfo.assignmentId
    //          << ", lv: " << finalInfo.level << std::endl;

    cJSON_AddStringToObject(readJson, "assignment", finalInfo.assignmentId.c_str());
    cJSON_AddStringToObject(readJson, "lv", finalInfo.level.c_str());
    char *readP = cJSON_PrintUnformatted(readJson);
    std::string resp = client.post(BASE_API + "/answers/extra-read", std::string(readP));
    cJSON_Delete(readJson);
    free(readP);
}
