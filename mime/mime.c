#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "image.h"

// Define email addresses and SMTP server details
#define FROM    "guy@om.er"
#define TO      "user-11fb328c-a341-4d3c-af1d-6792ea5d4fdf@mailslurp.net"

// Base64 encoded image content (just a short placeholder example)
static const char *image_base64 = IMAGE_TO_SEND;

// Define the MIME payload text
static const char *payload_text[] = {
        "From: " FROM "\r\n",
        "To: " TO "\r\n",
        "Subject: SMTP + MIME Example\r\n",
        "MIME-Version: 1.0\r\n",
        "Content-Type: multipart/related; boundary=\"boundary_string\"\r\n",
        "\r\n",
        "--boundary_string\r\n",
        "Content-Type: text/html; charset=UTF-8\r\n",
        "\r\n",
        "<html>\r\n",
        "<body>\r\n",
        "<h1>Hello, World!</h1>\r\n",
        "<p>This email contains an inline image.</p>\r\n",
        "<img src=\"cid:image1@example.com\" alt=\"Inline Image\">\r\n",
        "</body>\r\n",
        "</html>\r\n",
        "\r\n",
        "--boundary_string\r\n",
        "Content-Type: image/png\r\n",
        "Content-Transfer-Encoding: base64\r\n",
        "Content-ID: <image1@example.com>\r\n",
        "\r\n",
        "", // Placeholder for image data
        "--boundary_string--\r\n",
        NULL
};

// Struct to keep track of the upload status
struct upload_status {
    int lines_read;
};

// Callback function to read the payload data
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *) userp;
    const char *data;

    // If we've exhausted all lines, return 0
    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }

    // Get the next line to send
    data = payload_text[upload_ctx->lines_read];

    // Handle sending image data as a special case
    if (data == NULL && upload_ctx->lines_read == 18) { // 18 is the index where the image starts
        data = image_base64; // Use the base64 image data
    }

    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;
        return len;
    }

    return 0;
}

int main(void) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = {0};

    // Initialize CURL
    curl = curl_easy_init();
    if (curl) {
        // Set SMTP server URL
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://mxslurp.click:2525");

        // Set authentication details
        curl_easy_setopt(curl, CURLOPT_USERNAME, "nWda0ngQWTJzzqxURmyDDwEX8o4StIcD");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "Mg6s1nmZ7S4UUJGtRsuNYaFMOouTs7wT");

        // Enable verbose output for debugging
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Set MAIL FROM address
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

        // Add recipients
        recipients = curl_slist_append(recipients, TO);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Set the read callback function
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Perform the email send operation
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Free recipient list
        curl_slist_free_all(recipients);

        // Cleanup CURL
        curl_easy_cleanup(curl);
    }

    return (int) res;
}
