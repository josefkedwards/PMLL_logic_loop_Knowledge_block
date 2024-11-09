#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "json.h"

// Define the JSON object structure
typedef struct {
  char* benchmark_name;
  char* benchmark_version;
  struct {
    char* metric_name;
    double value;
    char* units;
  } results[3];
} BenchmarkResults;

// Define the function to generate the JSON output
void generate_json_output(BenchmarkResults* results) {
  // Create a JSON object
  json_t* json = json_object();

  // Add the benchmark name and version to the JSON object
  json_object_set(json, "benchmark_name", json_string(results->benchmark_name));
  json_object_set(json, "benchmark_version", json_string(results->benchmark_version));

  // Add the results array to the JSON object
  json_t* results_array = json_array();
  for (int i = 0; i < 3; i++) {
    json_t* result = json_object();
    json_object_set(result, "metric_name", json_string(results->results[i].metric_name));
    json_object_set(result, "value", json_real(results->results[i].value));
    json_object_set(result, "units", json_string(results->results[i].units));
    json_array_append(results_array, result);
  }
  json_object_set(json, "results", results_array);

  // Convert the JSON object to a string
  char* json_string = json_dumps(json, 0);

  // Send the JSON string over the socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  send(sock, json_string, strlen(json_string), 0);
  close(sock);

  // Free the JSON object and string
  json_decref(json);
  free(json_string);
}

int main() {
  // Define the benchmark results
  BenchmarkResults results;
  results.benchmark_name = "Arc-AGI";
  results.benchmark_version = "1.0";
  results.results[0].metric_name = "construction_time";
  results.results[0].value = 0.123;
  results.results[0].units = "seconds";
  results.results[1].metric_name = "query_time";
  results.results[1].value = 0.456;
  results.results[1].units = "seconds";
  results.results[2].metric_name = "reasoning_time";
  results.results[2].value = 0.789;
  results.results[2].units = "seconds";

  // Generate the JSON output
  generate_json_output(&results);

  return 0;
}
