/**
 * @file iso8601_parser.c
 * @brief ISO 8601 date and time parser for TinyUTC library.
 * @author Ulysse Moreau
 * @date 2025-05-02
 * @version 2.0
 * @license WTFPL (Do What The F*ck You Want To Public License)
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#include <stdint.h>
#include <stdbool.h>
#include "tinyutc.h"
#include "iso8601_parser.h"

// As this is the only string function used in this file, I will implement it here.
static size_t __tinyutc_strlen(const char *str)
{
    // BSD implementation of strlen
    const char *s;

    for (s = str; *s; ++s)
        ;
    return (s - str);
}

/**
 * @brief Converts a string representation of an unsigned integer to a uint32_t.
 *
 * This function takes a string and its length as input, and converts the string
 * to an unsigned 32-bit integer. The string is expected to contain only numeric
 * characters, and its length should not exceed the maximum number of digits
 * representable by a uint32_t.
 *
 * @param str Pointer to the null-terminated string containing the numeric characters.
 * @param len The maximum number of characters to parse.
 * @return The converted unsigned 32-bit integer.
 */
static err_t _str_to_uint(const char *str, int len)
{
    uint32_t result = 0;

    if (__tinyutc_strlen(str) < len)
    {
        return -1; // Invalid length
    }

    for (int i = 0; i < len; i++)
    {
        if (str[i] == 0)
        {
            return -1; // Invalid length
        }
        if (str[i] < '0' || str[i] > '9')
        {
            return -1; // Invalid character
        }
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

/**
 * @brief Removes offset from iso8601 date string to stay with a strictly UTC time,
 *        but keep the leap second.
 *
 * @param utc_tm Pointer to a TinyUTCTime structure that will be tidied.
 *
 * @return An integer indicating the success or failure of the operation.
 *         Typically, 0 indicates success, while a non-zero value indicates an error.
 */
static err_t __tidy_utc_struct(struct TinyUTCTime *utc_tm, int utc_offset)
{
    bool has_extra_leap_second = utc_tm->second == 60; // Set the leap second flag

    tinyutc_time_t unix_ts;
    uint8_t error = tinyutc_utc_to_unix(utc_tm, &unix_ts);
    if (error < 0)
    {
        return -1; // Invalid date
    }

    unix_ts -= utc_offset; // Adjust for UTC offset

    if (has_extra_leap_second)
    {
        unix_ts -= 60; // Use previous minute
    }

    error = tinyutc_unix_to_utc(utc_tm, unix_ts);
    if (error < 0)
    {
        return -1; // Invalid date
    }

    if (has_extra_leap_second)
    {
        utc_tm->second = 60; // Set the beloved leap second back
    }

    return 0; // Success
}

static err_t _parse_date_from_weekno(struct TinyUTCTime *utc_tm, uint16_t year, uint8_t weekno, uint8_t dayno)
{
    if (weekno < 1 || weekno > 53)
    {
        return -1; // Invalid week number
    }

    if (dayno < 1 || dayno > 7)
    {
        return -1; // Invalid day of the week
    }

    // By ISO 8601, week 1 is the first week with a Thursday in it.
    // This means that the 4th of January is always in week 1.
    // I did not check that rigorously, but intuitively, it works,
    // plus everyone on the internet says so.

    struct TinyUTCTime january_4th = {
        .year = year,
        .month = 1,
        .day = 4,
        .hour = 0,
        .minute = 0,
        .second = 0,
        .microseconds = 0};

    uint8_t week_day = tinyutc_get_week_day(&january_4th, true); // Get the week day of January 4th

    tinyutc_time_t january_4th_unix;
    uint8_t error = tinyutc_utc_to_unix(&january_4th, &january_4th_unix);
    if (error < 0)
    {
        return -1; // Invalid date
    }

    tinyutc_time_t week1 = january_4th_unix - (week_day - 1) * _TINYUTC_SECS_PER_DAY; // Get the Unix timestamp of the first day of week 1

    uint16_t days_offset = (weekno - 1) * 7 + (dayno - 1); // Calculate the offset in days from week 1

    struct TinyUTCTime new_date = {0};

    tinyutc_unix_to_utc(&new_date, week1 + days_offset * _TINYUTC_SECS_PER_DAY); // Convert the Unix timestamp back to a TinyUTCTime structure

    // Set the UTC time structure with the new date
    utc_tm->year = new_date.year;
    utc_tm->month = new_date.month;
    utc_tm->day = new_date.day;

    return 0;
}

static int _parse_offset(const char *iso8601_offset, int *utc_offset)
{
    int cursor, pattern_len, mult, hour_offset, minute_offset;

    pattern_len = __tinyutc_strlen(iso8601_offset);

    if (*iso8601_offset == 'z' || *iso8601_offset == 'Z')
    {
        return 1; // Cursor position after parsing the offset
    }

    if (pattern_len > 6 || pattern_len < 3 || pattern_len == 4)
    {
        LOG_DBG("File %s, line %d : Invalid offset format (len %d)\n", __FILE__, __LINE__, pattern_len);
        return -1; // Invalid offset format
    }

    if (*iso8601_offset == '+')
    {
        mult = 1;
    }
    else if (*iso8601_offset == '-')
    {
        mult = -1;
    }
    else
    {
        LOG_DBG("File %s, line %d : Invalid offset format offset char begin: %c\n", __FILE__, __LINE__, *iso8601_offset);
        return -1; // Invalid offset format
    }

    cursor = 1;

    // Parse hour
    hour_offset = _str_to_uint(iso8601_offset + cursor, 2);
    if (hour_offset < 0 || hour_offset > 23)
    {
        return -1; // Invalid hour
    }

    cursor += 2;

    if (iso8601_offset[cursor] == ':')
    {
        cursor++; // Skip separator
    }

    minute_offset = _str_to_uint(iso8601_offset + cursor, 2);

    if (minute_offset < 0 || minute_offset > 59)
    {
        return -1; // Invalid minute
    }

    *utc_offset = mult * (hour_offset * 3600 + minute_offset * 60); // Convert to total minutes

    return cursor + 2; // Return the cursor position after parsing the offset
}

static int _parse_microseconds(struct TinyUTCTime *utc_tm, const char *iso8601_time_tail)
{
    int cursor = 0;
    int result = 0;
    // Check for fractionnal seconds
    uint8_t fraction_len = 0;
    uint8_t rest_len = __tinyutc_strlen(iso8601_time_tail);

    // Find offset, if any
    while (fraction_len < rest_len &&
           iso8601_time_tail[fraction_len] != '+' &&
           iso8601_time_tail[fraction_len] != '-' &&
           iso8601_time_tail[fraction_len] != 'Z' &&
           iso8601_time_tail[fraction_len] != 'z' &&
           fraction_len <= 7)
    {
        fraction_len++;
    }
    if (fraction_len == 7)
    {
        return -1; // Invalid fraction length
    }

    result = _str_to_uint(iso8601_time_tail + cursor, fraction_len);

    if (result < 0)
    {
        return -1; // Invalid fraction
    }

    for (int i = fraction_len; i < 6; i++)
    {
        result *= 10; // Shift to the left
    }

    utc_tm->microseconds = result; // Set the microseconds
    cursor += fraction_len;

    return cursor;
}

static int _parse_time_component(const char *iso8601_time_part, uint8_t *component, bool *reached_end)
{
    int end = 0, result = 0;
    int pattern_len = __tinyutc_strlen(iso8601_time_part);
    if (pattern_len < 1)
    {
        LOG_DBG("File %s, line %d : While parsing time component, component too short (%d)\n", __FILE__, __LINE__, pattern_len);
        return -1;
    }

    // Find the end of the component
    for (end = 1; (end < pattern_len && end < 2); end++)
    {
        if (iso8601_time_part[end + 1] == '+' ||
            iso8601_time_part[end + 1] == '-' ||
            iso8601_time_part[end + 1] == 'Z' ||
            iso8601_time_part[end + 1] == 'z' ||
            iso8601_time_part[end + 1] == 0)
        {
            *reached_end = true; // Found the end of the component
            end++;
            break; // Found the end of the component
        }
    }

    // Parse digits
    result = _str_to_uint(iso8601_time_part, end);
    if (result < 0)
    {
        LOG_DBG("File %s, line %d : While parsing time component, unable to parse int. %s [0:%d]\n", __FILE__, __LINE__, iso8601_time_part, end);
        return -1; // Invalid hour
    }

    *component = (uint8_t)result; // Set the component

    return end;
}

static int _parse_time(const char *iso8601_time, struct TinyUTCTime *utc_tm, int *utc_offset)
{
    int cursor = 0, result = 0, pattern_len = 0;
    bool use_separator = false;

    uint8_t hms[3] = {0};

    pattern_len = __tinyutc_strlen(iso8601_time);

    if (pattern_len < 2)
    {
        LOG_DBG("File %s, line %d : While parsing time, pattern length is too short: %d\n", __FILE__, __LINE__, pattern_len);
        return TINYUTC_ISO8601_INVALID_TIME; // Invalid time format
    }

    uint8_t component = 0;
    bool reached_end = false;
    int i;
    for (i = 0; i < 3; i++)
    {
        result = _parse_time_component(iso8601_time + cursor, &component, &reached_end);
        if (result < 0)
        {
            LOG_DBG("File %s, line %d : While parsing time, unable to parse component %d\n", __FILE__, __LINE__, i);
            return TINYUTC_ISO8601_INVALID_TIME; // Invalid time component
        }
        cursor += result;   // Move the cursor
        hms[i] = component; // Store the component

        if (reached_end)
        {
            break; // No more components to parse
        }

        // Check separator inconsistency
        if (i == 0)
        {
            // First encounter dictates separator usage
            if (iso8601_time[cursor] == ':')
            {
                cursor++; // Skip separator
                use_separator = true;
            }
        }
        else if (i < 2)
        {
            if (iso8601_time[cursor] != ':' && use_separator)
            {
                LOG_DBG("File %s, line %d : Inconsistent use of separator after time component %d\n", __FILE__, __LINE__, i);
                return TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR; // Invalid time format
            }
            else if (iso8601_time[cursor] == ':' && !use_separator)
            {
                LOG_DBG("File %s, line %d : Inconsistent use of separator after time component %d\n", __FILE__, __LINE__, i);
                return TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR; // Invalid time format
            }
            else
            {
                if (use_separator)
                {
                    cursor++; // Skip separator
                }
            }
        }
    }

    // Check limits
    if (hms[2] < 0 || hms[2] > 60)
    {
        // We allow leap seconds
        LOG_DBG("File %s, line %d : Seconds %d not in range 0-60\n", __FILE__, __LINE__, hms[2]);
        return TINYUTC_ISO8601_INVALID_TIME; // Invalid minute
    }

    if (hms[1] < 0 || hms[1] > 59)
    {
        LOG_DBG("File %s, line %d : Minutes %d not in range 0-59\n", __FILE__, __LINE__, hms[1]);
        return TINYUTC_ISO8601_INVALID_TIME; // Invalid minute
    }

    if (hms[0] < 0 || hms[0] > 24)
    {
        LOG_DBG("File %s, line %d : Hours %d not in range 0-24\n", __FILE__, __LINE__, hms[0]);
        return TINYUTC_ISO8601_INVALID_TIME; // Invalid hour
    }

    // 24:00:00 is a valid time, but it means midthight on the next day
    if (hms[0] == 24)
    {
        if (hms[1] != 0 || hms[2] != 0)
        {
            LOG_DBG("File %s, line %d : Hours set to 24, but minutes and seonds not set to 0.\n", __FILE__, __LINE__);
            return TINYUTC_ISO8601_INVALID_TIME; // Invalid hour
        }
        // Let as-is, it will be tidied up later
    }

    utc_tm->hour = hms[0];   // Set the hour
    utc_tm->minute = hms[1]; // Set the minute
    utc_tm->second = hms[2]; // Set the second

    utc_tm->microseconds = 0;

    if (i == 3 && !reached_end)
    {
        // We reached seconds, without so there may be microseconds
        // Check for fractionnal seconds
        if (iso8601_time[cursor] == '.' || iso8601_time[cursor] == ',')
        {
            cursor++;
            result = _parse_microseconds(utc_tm, iso8601_time + cursor);
            if (result < 0)
            {
                LOG_DBG("File %s, line %d : Invalid fractionnal time.\n", __FILE__, __LINE__);
                return TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG; // Invalid fraction
            }
            cursor += result; // Move the cursor
        }
        else
        {
            utc_tm->microseconds = 0; // Set the microseconds
        }
    }

    if (cursor == pattern_len)
    {
        return cursor; // No offset present
    }

    int tmp_cursor = _parse_offset(&(iso8601_time[cursor]), utc_offset);
    if (tmp_cursor < 0)
    {
        LOG_DBG("File %s, line %d : Unable to parse time offset.\n", __FILE__, __LINE__);
        return TINYUTC_ISO8601_INVALID_OFFSET; // Invalid offset format
    }

    return cursor + tmp_cursor; // Return the cursor position after parsing the offset
}

static int _parse_common_date(struct TinyUTCTime *utc_tm, const char *iso8601)
{
    int cursor, result, pattern_len;
    bool use_separator = false;

    pattern_len = __tinyutc_strlen(iso8601);

    if (pattern_len < 4)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid date format
    }

    // Parse year
    result = _str_to_uint(iso8601, 4);
    if (result < 0)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid year
    }
    utc_tm->year = (uint16_t)result;

    cursor = 4;

    if (cursor >= pattern_len)
    {
        return 0;
    }

    // Check for separator
    if (iso8601[cursor] == '-')
    {
        use_separator = true;
        cursor++; // Invalid date format
    }

    // Parse month

    result = _str_to_uint(iso8601 + cursor, 2);
    if (result < 1 || result > 12)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid month
    }
    utc_tm->month = (uint8_t)result;
    cursor += 2;

    if (cursor >= pattern_len)
    {
        return 0;
    }

    // Check for separator
    if (iso8601[cursor] != '-' && use_separator)
    {
        return TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY; // Invalid date format
    }
    else if (iso8601[cursor] == '-' && !use_separator)
    {
        return TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY; // Invalid date format
    }
    else
    {
        if (use_separator)
        {
            cursor++; // Skip separator
        }
    }

    // Parse day
    result = _str_to_uint(iso8601 + cursor, 2);
    if (result < 1 || result > 31)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid day
    }
    utc_tm->day = (uint8_t)result;

    return cursor + 2;
}

static int _parse_uncommon_date(struct TinyUTCTime *utc_tm, const char *iso8601)
{
    int cursor, result, pattern_len, dayno, ordinal_day;
    bool use_separator = false;

    pattern_len = __tinyutc_strlen(iso8601);

    if (pattern_len < 4)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid date format
    }

    // Parse year
    result = _str_to_uint(iso8601, 4);
    if (result < 0)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid year
    }
    utc_tm->year = (uint16_t)result;

    cursor = 4;

    // Check for separator
    if (iso8601[cursor] == '-')
    {
        use_separator = true;
        cursor++; // Invalid date format
    }

    if (iso8601[cursor] == 'W')
    {
        cursor++;
        // Parse week number
        result = _str_to_uint(iso8601 + cursor, 2);

        cursor += 2;

        dayno = 1;

        if (pattern_len > cursor)
        {

            if (iso8601[cursor] != '-' && use_separator)
            {
                return TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY; // Invalid date format
            }
            else if (iso8601[cursor] == '-' && !use_separator)
            {
                return TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY; // Invalid date format
            }
            else
            {
                if (use_separator)
                {
                    cursor++; // Skip separator
                }
            }

            dayno = _str_to_uint(iso8601 + cursor, 1);

            cursor++;
        }

        if (_parse_date_from_weekno(utc_tm, utc_tm->year, result, dayno) < 0)
        {
            return TINYUTC_ISO8601_INVALID_DATE; // Invalid week number or day of the week
        }
    }
    else
    {
        ordinal_day = _str_to_uint(iso8601 + cursor, 3);
        if (ordinal_day < 1 ||
            (ordinal_day > 366 && _TINYUTC_IS_LEAP_YEAR(utc_tm->year)) ||
            (ordinal_day > 365 && !_TINYUTC_IS_LEAP_YEAR(utc_tm->year)))
        {
            return TINYUTC_ISO8601_INVALID_DATE; // Invalid ordinal day
        }

        struct TinyUTCTime january_1st = {
            .year = utc_tm->year,
            .month = 1,
            .day = 1,
            .hour = 0,
            .minute = 0,
            .second = 0,
            .microseconds = 0}; // January 1st of the given year

        tinyutc_time_t current_date;
        uint8_t error = tinyutc_utc_to_unix(&january_1st, &current_date);
        if (error < 0)
        {
            return TINYUTC_ISO8601_INVALID_DATE; // Invalid date
        }

        current_date += (ordinal_day - 1) * _TINYUTC_SECS_PER_DAY; // Calculate the Unix timestamp of the given ordinal day

        struct TinyUTCTime new_date = {0};
        tinyutc_unix_to_utc(&new_date, current_date); // Convert the Unix timestamp back to a TinyUTCTime structure

        utc_tm->month = new_date.month; // Set the month
        utc_tm->day = new_date.day;     // Set the day

        cursor += 3;
    }

    return cursor;
}

static int _tinyutc_parse_iso8601_date(struct TinyUTCTime *utc_tm, const char *iso8601_date)
{

    // Try to pase date in common format
    int _cursor = _parse_common_date(utc_tm, iso8601_date);

    if (_cursor >= 0)
    {
        return _cursor; // Date parsed successfully
    }

    // Try to parse date in uncommon format
    _cursor = _parse_uncommon_date(utc_tm, iso8601_date);

    if (_cursor >= 0)
    {
        return _cursor; // Invalid date format
    }

    return TINYUTC_ISO8601_INVALID_DATE;
}

err_t tinyutc_parse_iso8601_date(struct TinyUTCTime *utc_tm, const char *iso8601_date)
{
    int pattern_len, cursor;
    // Check if the input string is NULL or empty
    if (iso8601_date == 0 || *iso8601_date == '\0')
    {
        return TINYUTC_ISO8601_EMPTY_STRING; // Invalid input
    }
    pattern_len = __tinyutc_strlen(iso8601_date);

    cursor = _tinyutc_parse_iso8601_date(utc_tm, iso8601_date);

    if (cursor < 0)
    {
        return cursor; // Invalid date format
    }

    if (cursor != pattern_len)
    {
        return TINYUTC_ISO8601_EXTRANEOUS_DATE_COMPONENTS; // To much data present
    }

    return 0; // Date parsed successfully
}

err_t tinyutc_parse_iso8601_time(struct TinyUTCTime *utc_tm, const char *iso8601_time)
{
    int pattern_len, cursor, utc_offset = 0;

    pattern_len = __tinyutc_strlen(iso8601_time);
    // Check if the input string is NULL or empty
    if (iso8601_time == 0 || *iso8601_time == '\0')
    {
        return TINYUTC_ISO8601_EMPTY_STRING; // Invalid input
    }

    cursor = 0;
    // ISO8601 allow a "T" as starting character
    if (*iso8601_time == 'T')
    {
        cursor++; // Skip separator
    }

    cursor += _parse_time(&(iso8601_time[cursor]), utc_tm, &utc_offset);

    if (cursor < 0)
    {
        return cursor; // Invalid time format
    }

    if (cursor != pattern_len)
    {
        return TINYUTC_ISO8601_EXTRANEOUS_TIME_COMPONENTS; // UTC offset on time only is preposterous
    }

    if (utc_offset != 0)
    {
        return TINYUTC_ISO8601_UTC_OFFSET_WITHOUT_DATE;
    }

    // Tidy utc struct artifically : if 24:00:00, set to 00:00:00,
    // but do not touch date components
    if (utc_tm->hour == 24)
    {
        utc_tm->hour = 0;
    }

    return 0;
}

err_t tinyutc_parse_iso8601_datetime(struct TinyUTCTime *utc_tm, const char *iso8601, bool use_strict_separator)
{
    int pattern_len, cursor;

    pattern_len = __tinyutc_strlen(iso8601);

    // Check if the input string is NULL or empty
    if (iso8601 == 0 || *iso8601 == '\0')
    {
        return TINYUTC_ISO8601_EMPTY_STRING; // Invalid input
    }

    // Parse the date
    cursor = _tinyutc_parse_iso8601_date(utc_tm, iso8601);

    if (cursor < 0)
    {
        return TINYUTC_ISO8601_INVALID_DATE; // Invalid date format
    }

    if (cursor == pattern_len)
    {
        return 0; // No time or offset present
    }

    // Skip separator
    if (use_strict_separator)
    {
        if (iso8601[cursor] != 'T')
        {
            return TINYUTC_ISO8601_INVALID_MAIN_SEPARATOR; // Invalid separator
        }
    }
    cursor++;

    // Parse the time

    int utc_offset = 0;
    int tmp_cursor = _parse_time(&(iso8601[cursor]), utc_tm, &utc_offset);
    if (tmp_cursor < 0)
    {
        return tmp_cursor; // Invalid date format
    };
    cursor += tmp_cursor; // Move the cursor

    if (cursor != pattern_len)
    {
        return TINYUTC_ISO8601_INVALID_FORMAT; // Return the cursor position after parsing the offset
    }

    if (__tidy_utc_struct(utc_tm, utc_offset) < 0) // Tidy up the UTC structure
    {
        return TINYUTC_INTERNAL_ERROR;
    }

    return TINYUTC_ISO8601_OK;
}
