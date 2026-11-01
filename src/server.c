#include "server.h"
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const char *app_version = "0.0.0";

static enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    (void)cls; (void)method; (void)version; (void)upload_data; (void)upload_data_size; (void)con_cls;
    
    struct MHD_Response *response;
    enum MHD_Result ret;
    const char *page = "Not Found";
    int status_code = MHD_HTTP_OK;

    if (strcmp(url, "/health") == 0) {
        page = "OK";
    } else if (strcmp(url, "/ready") == 0) {
        page = "OK";
    } else if (strcmp(url, "/version") == 0) {
        static char version_buf[256];
        char hostname[64];
        gethostname(hostname, sizeof(hostname));
        snprintf(version_buf, sizeof(version_buf), "%s (host: %s)", app_version, hostname);
        page = version_buf;
    } else {
        page = "404 Not Found";
        status_code = MHD_HTTP_NOT_FOUND;
    }

    response = MHD_create_response_from_buffer(strlen(page), (void*)page, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    return ret;
}

int start_server(int port, const char *version) {
    app_version = version;
    struct MHD_Daemon *daemon;
    
    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, port, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start server on port %d\n", port);
        return 1;
    }

    printf("Server running on port %d...\n", port);
    
    while (1) {
        sleep(1);
    }
    
    MHD_stop_daemon(daemon);
    return 0;
}

