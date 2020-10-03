#ifndef UTIL_TIME_H
#define UTIL_TIME_H

#include <stdint.h>

struct util_time_timestamp {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t millisec;
};

void util_time_sleep_ns(uint64_t time_ns);

void util_time_sleep_us(uint64_t time_us);

void util_time_sleep_ms(uint64_t time_ms);

void util_time_sleep_sec(uint64_t time_sec);

void util_time_get_current_time(struct util_time_timestamp* timestamp);

#endif //UTIL_TIME_H
