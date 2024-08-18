#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "../common/secrets.h"

// Function to write the received data to stdout
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
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_USERNAME, MAILSLURP_IMAP_USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, MAILSLURP_IMAP_PASSWORD);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // List message headers
        printf("Listing message headers:\n");
        curl_easy_setopt(curl, CURLOPT_URL, MAILSLURP_IMAP_HOST "/INBOX");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "FETCH 1:* (FLAGS BODY.PEEK[HEADER.FIELDS (FROM TO SUBJECT DATE)])");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Retrieve a specific message (e.g., message 1)
        printf("\nRetrieving message 1:\n");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "FETCH 1 BODY[TEXT]");
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Mark message 1 as read
        printf("\nMarking message 1 as read:\n");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "STORE 1 +FLAGS \\Seen");
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