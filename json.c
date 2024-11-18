#include "json.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

cJSON* json_parse(const char* json_str) {
    if (!json_str) {
        fprintf(stderr, "Input JSON string is NULL.\n");
        return NULL;
    }

    cJSON* json_obj = cJSON_Parse(json_str);
    if (!json_obj) {
        fprintf(stderr, "JSON Parsing Error: %s\n", cJSON_GetErrorPtr());
    }
    return json_obj;
}

char* json_stringify(const cJSON* json_obj) {
    if (!json_obj) {
        fprintf(stderr, "JSON Object is NULL. Cannot stringify.\n");
        return NULL;
    }

    char* json_str = cJSON_Print(json_obj);
    if (!json_str) {
        fprintf(stderr, "Error during JSON stringification.\n");
    }
    return json_str;
}

const char* json_get_string(const cJSON* json_obj, const char* key) {
    if (!json_obj || !key) {
        fprintf(stderr, "Invalid parameters to json_get_string.\n");
        return NULL;
    }

    cJSON* item = cJSON_GetObjectItemCaseSensitive(json_obj, key);
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
        return item->valuestring;
    }

    fprintf(stderr, "Key '%s' not found or not a string.\n", key);
    return NULL;
}

int json_set_string(cJSON* json_obj, const char* key, const char* value) {
    if (!json_obj || !key || !value) {
        fprintf(stderr, "Invalid parameters to json_set_string.\n");
        return 0; // Indicate failure
    }

    cJSON* item = cJSON_GetObjectItemCaseSensitive(json_obj, key);
    if (item) {
        if (cJSON_IsString(item)) {
            cJSON_SetValuestring(item, value);
        } else {
            fprintf(stderr, "Key '%s' exists but is not a string. Overwriting.\n", key);
            cJSON_ReplaceItemInObject(json_obj, key, cJSON_CreateString(value));
        }
    } else {
        cJSON_AddStringToObject(json_obj, key, value);
    }

    return 1; // Indicate success
}

void json_delete(cJSON* json_obj) {
    if (!json_obj) {
        fprintf(stderr, "Invalid JSON object for deletion.\n");
        return;
    }
    cJSON_Delete(json_obj);
}