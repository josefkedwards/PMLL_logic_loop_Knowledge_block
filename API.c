#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CLIENT_ID "Ov23li5l71lKLE8kYTIc"
#define CLIENT_SECRET "7b4626c63b6b939b9b14680b32ab01ec231cf2e2"
#define REPO_OWNER "bearycool11"
#define REPO_NAME "pmll"
#define API_URL "localhost"
#define API_PORT 8080
#define CROSS_TALK_URL "localhost"
#define CROSS_TALK_PORT 8081
#define IO_SOCKET_URL "localhost"
#define IO_SOCKET_PORT 8082
#define LOGIC_LOOP_URL "localhost"
#define LOGIC_LOOP_PORT 8083
#define PML_LOGIC_LOOP_URL "localhost"
#define PML_LOGIC_LOOP_PORT 8084

// Function to create a new socket
int create_socket() {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

// Function to set up the server address
void set_up_server_address(struct sockaddr_in* server_addr, char* url, int port) {
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port);
    inet_pton(AF_INET, url, &server_addr->sin_addr);
}

// Function to connect to the API server
void connect_to_api_server(int sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
}

// Function to connect to the cross-talk server
void connect_to_cross_talk_server(int sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
}

// Function to connect to the IO socket server
void connect_to_io_socket_server(int sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
}

// Function to connect to the logic loop server
void connect_to_logic_loop_server(int sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
}

// Function to connect to the PML logic loop server
void connect_to_pml_logic_loop_server(int sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
}

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
    int api_sockfd, cross_talk_sockfd, io_socket_sockfd, logic_loop_sockfd, pml_logic_loop_sockfd;
    struct sockaddr_in api_server_addr, cross_talk_server_addr, io_socket_server_addr, logic_loop_server_addr, pml_logic_loop_server_addr;

    // Create a new socket for the API server
    api_sockfd = create_socket();

    // Set up the server address for the API server
    set_up_server_address(&api_server_addr, API_URL, API_PORT);

    // Connect to the API server
    connect_to_api_server(api_sockfd, &api_server_addr);

    // Create a new socket for the cross-talk server
    cross_talk_sockfd = create_socket();

    // Set up the server address for the cross-talk server
    set_up_server_address(&cross_talk_server_addr, CROSS_TALK_URL, CROSS_TALK_PORT);

    // Connect to the cross-talk server
    connect_to_cross_talk_server(cross_talk_sockfd, &cross_talk_server_addr);

    // Create a new socket for the IO socket server
    io_socket_sockfd = create_socket();

    // Set up the server address for the IO socket server
    set_up_server_address(&io_socket_server_addr, IO_SOCKET_URL, IO_SOCKET_PORT);

    // Connect to the IO socket server
    connect_to_io_socket_server(io_socket_sockfd, &io_socket_server_addr);

    // Create a new socket for the logic loop server
    logic_loop_sockfd = create_socket();

    // Set up the server address for the logic loop server
    set_up_server_address(&logic_loop_server_addr, LOGIC_LOOP_URL, LOGIC_LOOP_PORT);

    // Connect to the logic loop server
    connect_to_logic_loop_server(logic_loop_sockfd, &logic_loop_server_addr);

    // Create a new socket for the PML logic loop server
    pml_logic_loop_sockfd = create_socket();

    // Set up the server address for the PML logic loop server
    set_up_server_address(&pml_logic_loop_server_addr, PML_LOGIC_LOOP_URL, PML_LOGIC_LOOP_PORT);

    // Connect to the PML logic loop server
    connect_to_pml_logic_loop_server(pml_logic_loop_sockfd, &pml_logic_loop_server_addr);

    // Make a GET request to retrieve the repository information
    char* url = "https://api.github.com/repos/bearycool11/pmll";
    make_get_request(url);

    // Make a POST request to create a new issue
    char* data = "{\"title\":\"New issue\",\"body\":\"This is a new issue\"}";
    url = "https://api.github.com/repos/bearycool11/pmll/issues";
    make_post_request(url, data);

    // Send a request to the API server
    char* request = "GET /api/endpoint HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
    send(api_sockfd, request, strlen(request), 0);

    // Receive a response from the API server
    char response[1024];
    recv(api_sockfd, response, 1024, 0);
    printf("%s", response);

    // Close the sockets
    close(api_sockfd);
    close(cross_talk_sockfd);
    close(io_socket_sockfd);
    close(logic_loop_sockfd);
    close(pml_logic_loop_sockfd);

    return 0;
}
