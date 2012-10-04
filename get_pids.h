#pragma once

typedef struct all_pids_s {
    int pid;  /* Process ID */
    int ppid; /* Parent Process ID */
    int tgid; /* Thread ID */;
} all_pids_t;

int get_all_pids(all_pids_t* dest, int pid);
char* form_path_to_pid(int pid);

/* returns pid on success, -1 on error */
int get_pid_from_str(const char* str); /* from invalid strs too (like "12345pid") */

