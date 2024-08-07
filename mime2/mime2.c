#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

// Define email addresses and SMTP server details
#define FROM    "guy@om.er"
#define TO      "user-11fb328c-a341-4d3c-af1d-6792ea5d4fdf@mailslurp.net"


// Callback function to read the file data
size_t read_file_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
    FILE *readhere = (FILE *) userp;
    curl_off_t nread;

    size_t retcode = fread(ptr, size, nmemb, readhere);
    nread = (curl_off_t) retcode;

    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T " bytes from file\n", nread);
    return retcode;
}

int main(void) {
    CURL *curl;
    CURLcode res = CURLE_OK;

    // SMTP server credentials
    const char *smtp_url = "smtp://mxslurp.click:2525"; // Use your SMTP server and port
    const char *username = "nWda0ngQWTJzzqxURmyDDwEX8o4StIcD";      // Use your email address
    const char *password = "Mg6s1nmZ7S4UUJGtRsuNYaFMOouTs7wT";         // Use your email password

    // Email details
    const char *sender = "From: Your Name " FROM;
    const char *recipient = "To: Recipient Name " TO;
    const char *subject = "Subject: Test Email with Image Attachment";
    const char *body_text = "This is a test email with a JPEG image attached.";
    const char *file_path = "/mnt/c/Users/omer1/CLionProjects/email/mime2/image.jpg";

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set SMTP URL
        curl_easy_setopt(curl, CURLOPT_URL, smtp_url);

        // Enable verbose output
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Set username and password for authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);

        // Set the sender email
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

        // Set the recipient email
        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, TO);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Create a multipart MIME structure
        curl_mime *mime;
        curl_mimepart *part;

        mime = curl_mime_init(curl);

        // Add the text part
        part = curl_mime_addpart(mime);
        curl_mime_data(part, body_text, CURL_ZERO_TERMINATED);

        // Add the image part (inline)
        part = curl_mime_addpart(mime);
        curl_mime_type(part, "image/jpeg");
        curl_mime_filedata(part, file_path);
        curl_mime_encoder(part, "base64");
        curl_mime_headers(part, curl_slist_append(NULL, "Content-Disposition: inline; filename=\"image.jpg\""), 0);
        curl_mime_headers(part, curl_slist_append(NULL, "Content-ID: <image1>"), 0);

        // Add the text part with reference to the inline image
        part = curl_mime_addpart(mime);
        curl_mime_data(part, "This is a test email with an inline JPEG image.<br><img src=\"cid:image1\"/>",
                       CURL_ZERO_TERMINATED);
        curl_mime_type(part, "text/html");

        // Attach the MIME structure to the email
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Perform the sending operation
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Cleanup
        curl_mime_free(mime);
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    return (int) res;
}
