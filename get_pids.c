#include "get_pids.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

int get_pid_from_str(const char* str) {
    long pid;
    char* end;

    if (!str || str[0] == '\0') {
        fprintf(stderr, "%d\n", __LINE__);
        errno = EINVAL;
        return -1;
    }

    pid = strtol(str, &end, 0);
    if (pid < INT_MIN || pid > INT_MAX) {
        fprintf(stderr, "%d\n", __LINE__);
        errno = EINVAL;
        return -1;
    }

    return (int)pid;
}

char* form_path_to_pid(int pid) {
    char pid_str[sizeof("12345")];
    char* retval;
    const size_t pid_strlen = sizeof(pid_str) / sizeof(pid_str[0]);
    size_t outlen;

    if (snprintf(pid_str, pid_strlen, "%d", pid) > pid_strlen) {
        fprintf(stderr, "%d\n", __LINE__);
        errno = EINVAL;
        return NULL;
    }

    outlen = sizeof("/proc/") + strlen(pid_str) + sizeof("/status") + 1;
    retval = malloc(outlen); /* TODO: thread local storage */

    if (!retval)
        return NULL;

    strcpy(retval, "/proc/");
    strcpy(retval + strlen("/proc/"), pid_str);
    strcpy(retval + strlen("/proc/") + strlen(pid_str), "/status");

    return retval;
}

int get_all_pids(all_pids_t* dest, int pid) {
    char line[BUFSIZ];
    FILE* status;
    char* path_to_pid;
    int pids_set;
    int parsed_pid;

    if (!dest) {
        fprintf(stderr, "%d\n", __LINE__);
        errno = EINVAL;
        return -1;
    }

    path_to_pid = form_path_to_pid(pid);
    if (!path_to_pid) {
        return -1;
    }

    status = fopen(path_to_pid, "r"); /* TODO: thread local buffer */
    if (!status) {
        return -1;
    }

    memset(dest, 0, sizeof(*dest));
    pids_set = 0;

    while (fgets(line, BUFSIZ, status) && pids_set < 3) {

        /* TODO: simplify */
        if (strstr(line, "Tgid:") == line) {
            parsed_pid = get_pid_from_str(line + sizeof("Tgid:"));
            if (parsed_pid != -1) {
                dest->tgid = parsed_pid;
                ++pids_set;
            }
        } else if (strstr(line, "Pid:") == line) {
            parsed_pid = get_pid_from_str(line + sizeof("Pid:"));
            if (parsed_pid != -1) {
                dest->pid = parsed_pid;
                ++pids_set;
            }
        } else if (strstr(line, "PPid:") == line) {
            parsed_pid = get_pid_from_str(line + sizeof("PPid:"));
            if (parsed_pid != -1) {
                dest->ppid = parsed_pid;
                ++pids_set;
            }
        }
    }
    /* cleanup */
    fclose(status);
    free(path_to_pid);

    return 0;
}
