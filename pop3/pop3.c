#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define POP3_URL "pop3://192.168.0.131:1100"
#define USERNAME "user"
#define PASSWORD "pass"

// Function to write the received data to stdout
static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    (void)stream;
    size_t written = fwrite(ptr, size, nmemb, stdout);
    return written;
}

int main(void) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        // Set username and password
        curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);

        // List messages
        printf("Listing messages:\n");
        curl_easy_setopt(curl, CURLOPT_URL, POP3_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Retrieve message 1
        printf("\nRetrieving message 1:\n");
        curl_easy_setopt(curl, CURLOPT_URL, POP3_URL "/1");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "RETR 1");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Delete message 1
        printf("\nDeleting message 1:\n");
        curl_easy_setopt(curl, CURLOPT_URL, POP3_URL "/1");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELE 1");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}
