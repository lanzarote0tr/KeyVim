#include "helper.h"

#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // exit
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

void ClearScreen(void) { // VERIFIED
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

#ifdef _WIN32
void Threading(void *f1, void *f2) {
    // Create Threads
    HANDLE thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f1, NULL, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f2, NULL, 0, NULL);

    // Error Handling
    if (thread1 == NULL || thread2 == NULL) {
        fprintf(stderr, "Failed to create threads.\n");
        exit(1);
    }

    // Wait for threads to finish
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    CloseHandle(thread1);
    CloseHandle(thread2);
}
#else
void Threading(void *f1, void *f2) {
    // Create Threads & Error Handling
    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, f1, NULL) != 0 ||
        pthread_create(&thread2, NULL, f2, NULL) != 0) {
        fprintf(stderr, "Failed to create threads.\n");
        exit(1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}
#endif

void Wait(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

/* TEST_START */
#ifdef HEADER_TEST

void _f1() {
    Wait(2000);
    printf("f1 done\n");
    return;
}

void _f2() {
    Wait(3000);
    printf("f2 done\n");
    return;
}

int main(void) {
    Threading((void*)_f1, (void*)_f2);
    return 0;
}

#endif // HEADER_TEST

/* TEST_END */

