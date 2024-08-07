#include <stdio.h>
#include <curl/curl.h>

// Callback function to write data to a file
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int main(void) {
    CURL *curl;
    CURLcode res;
    FILE *fp;

    // Initialize a CURL handle
    curl = curl_easy_init();
    if (curl) {
        // Open a file to save the email message
        fp = fopen("email_message.txt", "wb");
        if (!fp) {
            perror("Failed to open file");
            return 1;
        }

        // Set the POP3 server URL, including the message number to retrieve
        curl_easy_setopt(curl, CURLOPT_URL, "pop3://pop3.example.com/1");

        // Set the username and password for authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, "your_username");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "your_password");

        // Set the callback function to write data to the file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        // Enable verbose output for debugging purposes
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Perform the retrieval operation
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Cleanup and close the file
        fclose(fp);
        curl_easy_cleanup(curl);
    }

    return 0;
}
