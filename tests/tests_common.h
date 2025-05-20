/**
 * @file tests_common.h
 * @brief Common header file for test cases.
 * @author Ulysse Moreau
 * @date 2025-05-20
 * @version 2.0
 * @license WTFPL (Do What The F*ck You Want To Public License)
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 *
 */

#ifndef _TESTS_COMMON_H
#define _TESTS_COMMON_H

#include "../iso8601_parser.h"

struct Iso8601TestCase
{
    const char *description;
    const char *iso8601;
    struct TinyUTCTime expected;
    int expected_code;
};

struct ErrorStatus
{
    int code;
    const char *message;
};

struct ErrorStatus error_status[] = {
    {TINYUTC_ISO8601_OK, "OK"},
    {TINYUTC_ISO8601_INVALID_FORMAT, "TINYUTC_ISO8601_INVALID_FORMAT"},
    {TINYUTC_ISO8601_INVALID_DATE, "TINYUTC_ISO8601_INVALID_DATE"},
    {TINYUTC_ISO8601_INVALID_TIME, "TINYUTC_ISO8601_INVALID_TIME"},
    {TINYUTC_ISO8601_INVALID_OFFSET, "TINYUTC_ISO8601_INVALID_OFFSET"},
    {TINYUTC_ISO8601_INVALID_MAIN_SEPARATOR, "TINYUTC_ISO8601_INVALID_MAIN_SEPARATOR"},
    {TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY, "TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY"},
    {TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR, "TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR"},
    {TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG, "TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG"},
    {TINYUTC_ISO8601_EXTRANEOUS_DATE_COMPONENTS, "TINYUTC_ISO8601_EXTRANEOUS_DATE_COMPONENTS"},
    {TINYUTC_ISO8601_EXTRANEOUS_TIME_COMPONENTS, "TINYUTC_ISO8601_EXTRANEOUS_TIME_COMPONENTS"},
    {TINYUTC_ISO8601_UTC_OFFSET_WITHOUT_DATE, "TINYUTC_ISO8601_UTC_OFFSET_WITHOUT_DATE"},
    {TINYUTC_ISO8601_EMPTY_STRING, "TINYUTC_ISO8601_EMPTY_STRING"},
};

const char *get_err_string(int code)
{
    for (int i = 0; i < sizeof(error_status) / sizeof(struct ErrorStatus); i++)
    {
        if (error_status[i].code == code)
        {
            return error_status[i].message;
        }
    }
    return "Unknown error";
}

bool compare_utc_structs_datetimes(struct TinyUTCTime *a, struct TinyUTCTime *b)
{
    return (a->year == b->year && a->month == b->month && a->day == b->day &&
            a->hour == b->hour && a->minute == b->minute && a->second == b->second && a->microseconds == b->microseconds);
}

bool compare_utc_struct_dates(struct TinyUTCTime *a, struct TinyUTCTime *b)
{
    return (a->year == b->year && a->month == b->month && a->day == b->day);
}

bool compare_utc_struct_times(struct TinyUTCTime *a, struct TinyUTCTime *b)
{
    return (a->hour == b->hour && a->minute == b->minute && a->second == b->second && a->microseconds == b->microseconds);
}

#endif // _TESTS_COMMON_H