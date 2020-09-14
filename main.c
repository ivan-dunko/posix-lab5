#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LEN 256
#define ERROR_CODE -1
#define SUCCESS_CODE 0

void exitWithFailure(const char *msg, int errcode){
    errno = errcode;
    fprintf(stderr, "%.256s:%.256s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

void handle(void *data){
    printf("thread`s got cancelled\n");
}

void *routine(void *data){
    if (data == NULL)
        exitWithFailure("routine", EINVAL);

    pthread_cleanup_push(handle, NULL);
    char *str = (char*)(data);
    
    while (1)
        printf("%.256s\n", str);

    pthread_cleanup_pop(1);

    return SUCCESS_CODE;
}

int main(int argc, char **argv){
    int arg_ind = argc > 1;
    
    pthread_t pid;
    int err = pthread_create(&pid, NULL, routine, (void*)(argv[arg_ind]));
    if (err == ERROR_CODE)
        exitWithFailure("main", errno);

    sleep(2);
    err = pthread_cancel(pid);
    if (err == ERROR_CODE)
        exitWithFailure("main", errno);

    err = pthread_join(pid, NULL);
    if (err != SUCCESS_CODE)
        exitWithFailure("main", err);

    return 0;
}
