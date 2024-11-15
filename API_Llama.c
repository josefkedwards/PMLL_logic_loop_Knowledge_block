#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define CLIENT_ID "Ov23li5l71lKLE8kYTIc"
#define CLIENT_SECRET "7b4626c63b6b939b9b14680b32ab01ec231cf2e2"
#define REPO_OWNER "bearycool11"
#define REPO_NAME "pmll"

// Function to make a GET request to the GitHub API
void make_get_request(const char* url) {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            printf("cURL error: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

// Function to make a POST request to the GitHub API
void make_post_request(const char* url, const char* data) {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            printf("cURL error: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    // Make a GET request to retrieve the repository information
    char* url = "https://api.github.com/repos/bearycool11/pmll";
    make_get_request(url);

    // Make a POST request to create a new issue
    char* data = "{\"title\":\"New issue\",\"body\":\"This is a new issue\"}";
    url = "https://api.github.com/repos/bearycool11/pmll/issues";
    make_post_request(url, data);

    return 0;
}
