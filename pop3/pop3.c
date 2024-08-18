#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../common/secrets.h"

// Callback function to write the received data to stdout
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    (void)stream;
    size_t written = fwrite(ptr, size, nmemb, stdout);
    return written;
}

int main(void) {
    CURL *curl;
    CURLcode res = CURLE_OK;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, MAILOSAUR_USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, MAILOSAUR_PASSWORD);
        curl_easy_setopt(curl, CURLOPT_URL, MAILOSAUR_POP3_HOST);

        // Set the callback function to write the data to stdout
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // List all messages
        printf("Listing messages:\n");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST");
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Retrieve the first message
        printf("\nRetrieving message 1:\n");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "RETR 1");
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}
