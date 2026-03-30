#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include "../../lib/httpClient/httpClient.hpp"
#include "../../lib/cJSON/cJSON.h"

const std::string TARGET_URL = "https://fireflies.chiculture.org.hk/api/core/auth";

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Get the directory of the executable
std::string getExeDir() {
    char path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath(path);
    size_t pos = fullPath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return fullPath.substr(0, pos + 1); // include trailing slash
    }
    return "";
}

// Print usage instructions
void print_usage(const char* progName) {
    std::cout << "Usage: " << progName << " [-a | -s] -em <email> -pw <password>" << std::endl;
    std::cout << " -a Generate admin cookie (cookies_admin.txt)" << std::endl;
    std::cout << " -s Generate student cookie (cookies_student.txt, default)" << std::endl;
    std::cout << " -em <email> Specify email (account) via command line" << std::endl;
    std::cout << " -pw <pass> Specify password via command line" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string cookieFile = "cookies_student.txt";
    bool isAdmin = false;
    std::string user_email, user_passwd;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-a") {
            cookieFile = "cookies_admin.txt";
            isAdmin = true;
        } else if (arg == "-s") {
            cookieFile = "cookies_student.txt";
        } else if (arg == "-em" && i + 1 < argc) {
            user_email = argv[++i];
        } else if (arg == "-pw" && i + 1 < argc) {
            user_passwd = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cout << "[Error] Unknown or incomplete argument: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    // If not both provided, print error and usage
    if (user_email.empty() || user_passwd.empty()) {
        std::cout << "[Error] Both -em <email> and -pw <password> must be provided." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    // Get executable directory and prepend to cookie file name
    std::string exeDir = getExeDir();
    std::string cookieFilePath = exeDir + cookieFile;

    // Delete old cookie file if exists
    DeleteFileA(cookieFilePath.c_str());

    std::cout << "[getAccessID] Logging in with account: " << user_email << std::endl;

    HttpClient client;
    client.setCookieFile(cookieFilePath);

    std::cout << "[getAccessID] Logging in..." << std::endl;

    client.addHeader("Accept-Encoding: identity");
    client.addHeader("accept: application/json, text/plain, */*");
    client.addHeader("content-type: application/json;charset=UTF-8");
    client.addHeader("origin: https://fireflies.chiculture.org.hk");
    client.addHeader("referer: https://fireflies.chiculture.org.hk/");
    client.addHeader("sec-ch-ua: \"Google Chrome\";v=\"143\", \"Chromium\";v=\"143\", \"Not A(Brand\";v=\"24\"");
    client.addHeader("sec-ch-ua-mobile: ?0");
    client.addHeader("sec-ch-ua-platform: \"Windows\"");
    client.addHeader("priority: u=1, i");

    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "email", user_email.c_str());
    cJSON_AddStringToObject(root, "password", user_passwd.c_str());
    cJSON_AddNumberToObject(root, "web", 1);
    cJSON_AddBoolToObject(root, "persist", cJSON_True);

    char* jsonPayload = cJSON_PrintUnformatted(root);
    std::string response = client.post(TARGET_URL, std::string(jsonPayload));
    cJSON_Delete(root);
    free(jsonPayload);

    std::string accessId = client.getCookie("access.id");

    if (!accessId.empty()) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "[Success] Login successful!" << std::endl;
        std::cout << "[Access ID] " << accessId.substr(0, 20) << "..." << std::endl;
        std::cout << "[Cookie] Saved to " << cookieFilePath << std::endl;
        std::cout << "========================================" << std::endl;
        return 0;
    } else {
        std::cout << "[Failed] Request completed, but access.id cookie not found." << std::endl;
        std::cout << "Response length: " << response.length() << std::endl;
        return 0xff;
    }
}