//
// Created by a on 30/07/2024.
//

#include "smtp.h"
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define FROM    "your_email@example.com"
#define TO      "user-11fb328c-a341-4d3c-af1d-6792ea5d4fdf@mailslurp.net"
#define CC      "<cc_email@example.com>"

static const char *payload_text[] = {
        "To: " TO "\r\n",
//        "Cc: " CC "\r\n",
        "From: " FROM " (Your Name)\r\n",
        "Subject: SMTP email example\r\n",
        "\r\n", /* empty line to divide headers from body, see RFC5322 */
        "This is a test email sent using libcurl.\r\n",
        "It shows how to send an email using the SMTP protocol.\r\n",
        NULL
};

struct upload_status {
    int lines_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    const char *data;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }

    data = payload_text[upload_ctx->lines_read];

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
    struct upload_status upload_ctx = { 0 };

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, "nWda0ngQWTJzzqxURmyDDwEX8o4StIcD");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "Mg6s1nmZ7S4UUJGtRsuNYaFMOouTs7wT");
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://mxslurp.click:2525");

//        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_NONE);

//        curl_easy_setopt(curl, CURLOPT_MAIL_AUTH, "HELO");

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

        recipients = curl_slist_append(recipients, TO);
        recipients = curl_slist_append(recipients, CC);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    return (int)res;
}
