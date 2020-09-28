#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LEN 256
#define ERROR_CODE -1
#define SUCCESS_CODE 0
/* text to be printed */
#define TEXT "aaa"

void exitWithFailure(const char *msg, int errcode){
    errno = errcode;
    fprintf(stderr, "%.256s:%.256s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

void handle(void *data){
    printf("thread`s got cancelled\n");
}

void *routine(void *data){
    /*
       These functions manipulate the calling thread's stack of thread-cancel‐
       lation  clean-up  handlers.   A  clean-up handler is a function that is
       automatically executed when a thread is canceled (or in  various  other
       circumstances  described  below); it might, for example, unlock a mutex
       so that it becomes available to other threads in the process.

       The pthread_cleanup_push() function pushes routine onto the top of  the
       stack  of clean-up handlers.  When routine is later invoked, it will be
       given arg as its argument.

       The pthread_cleanup_pop() function removes the routine at  the  top  of
       the  stack  of clean-up handlers, and optionally executes it if execute
       is nonzero.

       A cancellation clean-up handler is popped from the stack  and  executed
       in the following circumstances:

       1. When  a thread is canceled, all of the stacked clean-up handlers are
          popped and executed in the reverse of the order in which  they  were
          pushed onto the stack.

       2. When  a  thread  terminates by calling pthread_exit(3), all clean-up
          handlers are executed as described in the preceding point.   (Clean-
          up  handlers are not called if the thread terminates by performing a
          return from the thread start function.)

       3. When a thread calls pthread_cleanup_pop()  with  a  nonzero  execute
          argument, the top-most clean-up handler is popped and executed.

    */
    pthread_cleanup_push(handle, NULL);
    
    while (1){
        /* 
            explicit cancellation point
            (since Solaris printf doesn`t have
            implicit one)
        */
        pthread_testcancel();
        printf("%.256s\n", TEXT);
    }

    /*
        POSIX.1  permits pthread_cleanup_push() and pthread_cleanup_pop() to be
       implemented as macros that expand  to  text  containing  '{'  and  '}',
       respectively.   For  this  reason, the caller must ensure that calls to
       these functions are paired within the same function, and  at  the  same
       lexical  nesting  level.  (In other words, a clean-up handler is estab‐
       lished only during the execution of a specified section of code.)
    */

    /*
        When a thread calls pthread_cleanup_pop() with a nonzero execute argument,
        the top-most clean-up handler is popped and executed. (made here just in case)
    */
    pthread_cleanup_pop(1);
    return SUCCESS_CODE;
}

int main(int argc, char **argv){
    pthread_t pid;
    int err = pthread_create(&pid, NULL, routine, NULL);
    if (err == ERROR_CODE)
        exitWithFailure("main", errno);

    sleep(2);
    err = pthread_cancel(pid);
    if (err == ERROR_CODE)
        exitWithFailure("main", errno);

    /*
        calling pthread_exit not to terminate
        thread before cancellation
    */
    pthread_exit((void*)SUCCESS_CODE);
}
