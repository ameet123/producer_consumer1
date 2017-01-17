//
// Created by achaub001c on 1/16/2017.
//

#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void printTime() {
    char buffer[26];
    long millisec;
    struct tm *tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec / 1000.0); // Round to nearest millisec
    if (millisec >= 1000) { // Allow for rounding up to nearest second
        millisec -= 1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
    printf("%s.%03d\n", buffer, millisec);
}

void printMilliTime() {
    char buffer[14];
    long millisec;
    struct tm *tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    millisec = lrint(tv.tv_usec / 1000.0); // Round to nearest millisec
    if (millisec >= 1000) { // Allow for rounding up to nearest second
        millisec -= 1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 14, "%M:%S", tm_info);
    printf("%s.%03d\n", buffer, millisec);

}

/**
 * pass a char array of len 9
 * @param t
 */
void get_milli_time(char *t) {
    long millisec;
    struct tm *tm_info;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    millisec = lrint(tv.tv_usec / 1000.0); // Round to nearest millisec
    if (millisec >= 1000) { // Allow for rounding up to nearest second
        millisec -= 1000;
        tv.tv_sec++;
    }
    tm_info = localtime(&tv.tv_sec);
    strftime(t, 9, "%M:%S.", tm_info);
    // get milliseconds
    int nDigits = digits_in_int(millisec);
    // null terminated string returned, so we need to add one more char
    char buf[nDigits + 1];

    snprintf(buf, sizeof(buf), "%03d", (int) millisec);
    // concatenate two results
    strcat(t, buf);
}

int digits_in_int(int d) {
    return (int) ((d == 0) ? 1 : floor(log10(abs(d))) + 1);
}
