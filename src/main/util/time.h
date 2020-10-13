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

typedef uint64_t util_time_counter_t;

void util_time_sleep_ns(uint64_t time_ns);

void util_time_sleep_us(uint64_t time_us);

void util_time_sleep_ms(uint64_t time_ms);

void util_time_sleep_sec(uint64_t time_sec);

void util_time_get_current_time(struct util_time_timestamp* timestamp);

util_time_counter_t util_time_get_counter();

uint64_t util_time_get_elapsed_ns(util_time_counter_t counter_delta);

uint64_t util_time_get_elapsed_us(util_time_counter_t counter_delta);

double util_time_get_elapsed_us_double(util_time_counter_t counter_delta);

uint32_t util_time_get_elapsed_ms(util_time_counter_t counter_delta);

double util_time_get_elapsed_ms_double(util_time_counter_t counter_delta);

#endif //UTIL_TIME_H
