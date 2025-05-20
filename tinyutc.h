/**
 * @file tinyutc.h
 * @brief A simple library for converting between Unix timestamps and UTC time.
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
 *
 */

#ifndef _TINYUTC_H
#define _TINYUTC_H

#include <stdint.h>
#include <stdbool.h>

#ifndef tinyutc_time_t
typedef uint32_t tinyutc_time_t;
#endif

typedef int err_t;

#define _TINYUTC_UNIX_EPOCH_YEAR (1970UL)

#define _TINYUTC_DAYS_PER_YEAR (365UL)
#define _TINYUTC_DAYS_PER_LEAP_YEAR (366UL)

#define _TINYUTC_SECS_PER_MIN (60UL)
#define _TINYUTC_MIN_PER_HOUR (60UL)
#define _TINYUTC_SECS_PER_HOUR (3600UL)
#define _TINYUTC_HOUR_PER_DAY (24UL)
#define _TINYUTC_SECS_PER_DAY (_TINYUTC_SECS_PER_HOUR * _TINYUTC_HOUR_PER_DAY)
#define _TINYUTC_MONTH_PER_YEAR (12UL)

/**
 * A leap year occurs
 *  - Every 4 years
 *      - Except every 100 years
 *          - Except except every 400 years.
 *
 * This macro returns true :
 * - If the year is divisible by 400
 * - If the year is divisible by 4 and not divisible by 100
 */

#define _TINYUTC_IS_LEAP_YEAR(YEAR) \
    (((YEAR) % 400 == 0) || (((YEAR) % 4 == 0) && ((YEAR) % 100 != 0)))

/**
 * The following macros are used to determine the number of days in a month.
 * This removes the following static declaration that should never appear in a header file:
 *    static const uint8_t days_month[2][12] = {{31, 28, 31, 30, 31, 30,
 *                                              31, 31, 30, 31, 30, 31},
 *                                              {31, 29, 31, 30, 31, 30,
 *                                              31, 31, 30, 31, 30, 31}};
 */

#define _TINYUTC_DAYS_IN_BIG_MONTH 31
#define _TINYUTC_DAYS_IN_SMALL_MONTH 30
#define _TINYUTC_DAYS_IN_FEBRUARY_NON_LEAP 28
#define _TINYUTC_DAYS_IN_FEBRUARY_LEAP 29
// In reverse order, 1 where the month contains 31 days, 0 where it contains 30 days
#define _TINYUTC_DAYS_IN_MONTH_PATTERN 0b101011010101
#define _TINYUTC_GET_DAYS_IN_NON_FEBRUARY(month) \
    ((_TINYUTC_DAYS_IN_MONTH_PATTERN & (1 << (month))) ? _TINYUTC_DAYS_IN_BIG_MONTH : _TINYUTC_DAYS_IN_SMALL_MONTH)

#define _TINYUTC_IS_FEBRUARY(month) ((month) == 1)

#define _TINYUTC_GET_DAYS_IN_FEBRUARY(year) \
    (_TINYUTC_IS_LEAP_YEAR(year) ? _TINYUTC_DAYS_IN_FEBRUARY_LEAP : _TINYUTC_DAYS_IN_FEBRUARY_NON_LEAP)

#define _TINYUTC_GET_DAYS_IN_MONTH(month, year) \
    ((_TINYUTC_IS_FEBRUARY(month)) ? _TINYUTC_GET_DAYS_IN_FEBRUARY(year) : _TINYUTC_GET_DAYS_IN_NON_FEBRUARY(month))

#ifdef __cplusplus
extern "C"
{
#endif

    //

    /**
     * @struct TinyUTCTime
     * @brief  Dead-simple structure to hold UTC time.
     *
     * The microseconds field is used to store the microseconds of a second,
     * which is not used in this library, except for the conversion from
     * Iso8601 to TinyUTCTime.
     */
    struct TinyUTCTime
    {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint32_t microseconds;
    };

    /**
     * @brief Converts a Unix timestamp to a UTC time structure.
     *
     * This function takes a Unix timestamp and converts it into a UTC time
     * representation, storing the result in the provided TinyUTCTime structure.
     *
     * @param unix_ts The Unix timestamp to be converted.
     * @param utc_tm Pointer to a TinyUTCTime structure where the converted
     *               UTC time will be stored.
     *
     * @return A uint8_t value indicating success or failure of the conversion.
     *         Typically, 0 indicates success, while non-zero values indicate
     *         an error.
     */
    static inline err_t tinyutc_unix_to_utc(struct TinyUTCTime *utc_tm, tinyutc_time_t unix_ts)
    {
        uint16_t year;
        uint8_t month, days_in_month;
        unsigned long days;

        if (unix_ts < 0)
        {
            return -1;
        }

        // Compute seconds
        utc_tm->second = unix_ts % _TINYUTC_SECS_PER_MIN;

        // Morph seconds to minutes, then compute minutes
        unix_ts /= _TINYUTC_SECS_PER_MIN;
        utc_tm->minute = unix_ts % _TINYUTC_MIN_PER_HOUR;

        // Morph minutes to hours, then compute hours
        unix_ts /= _TINYUTC_MIN_PER_HOUR;
        utc_tm->hour = unix_ts % _TINYUTC_HOUR_PER_DAY;

        // Morph hours to days
        unix_ts /= _TINYUTC_HOUR_PER_DAY;

        // To find the year, we have to "manually" count days
        // until reaching the desired year.

        year = _TINYUTC_UNIX_EPOCH_YEAR;
        days = 0;

        while ((days += (_TINYUTC_IS_LEAP_YEAR(year) ? _TINYUTC_DAYS_PER_LEAP_YEAR : _TINYUTC_DAYS_PER_YEAR)) <= unix_ts)
        {
            year++;
        }
        // Remove the days of the current year, that should not be counted
        days -= _TINYUTC_IS_LEAP_YEAR(year) ? _TINYUTC_DAYS_PER_LEAP_YEAR : _TINYUTC_DAYS_PER_YEAR;

        // Remove the days of all past years, leaving us with only the day of the current year
        unix_ts -= days;

        days = 0;
        days_in_month = 0;
        for (month = 0; month < _TINYUTC_MONTH_PER_YEAR; month++)
        {
            // Compute the number of days in this month
            days_in_month = _TINYUTC_GET_DAYS_IN_MONTH(month, year);

            if (unix_ts < days_in_month)
            {
                // We are in this month, so we can break the loop
                break;
            }

            unix_ts -= days_in_month;
        }

        utc_tm->year = year;
        // Offset month and day to use human-indexing
        utc_tm->month = month + 1;
        utc_tm->day = unix_ts + 1;

        return 0;
    }

    /**
     * @brief Converts a TinyUTCTime structure to a Unix timestamp.
     *
     * This function takes a pointer to a TinyUTCTime structure representing
     * a date and time in UTC and converts it to the corresponding Unix timestamp.
     *
     * @param utc_tm Pointer to a TinyUTCTime structure containing the UTC time.
     * @return A uint8_t value indicating success or failure of the conversion.
     *         Typically, 0 indicates success, while non-zero values indicate
     *         an error.
     */
    static inline err_t tinyutc_utc_to_unix(const struct TinyUTCTime *utc_tm, tinyutc_time_t *unix_ts)
    {
        uint8_t days_in_month;
        int i;

        if (utc_tm->year < _TINYUTC_UNIX_EPOCH_YEAR)
        {
            return -1;
        }

        // Offset year is the number of years since the Unix epoch (1970).

        // Start by couting the number of seconds in the year,
        // assuming non-leap year (365 days).
        *unix_ts = (utc_tm->year - _TINYUTC_UNIX_EPOCH_YEAR) * (_TINYUTC_SECS_PER_DAY * _TINYUTC_DAYS_PER_YEAR);

        // For each leap year, add an extra day (86400 seconds).
        for (i = _TINYUTC_UNIX_EPOCH_YEAR; i < utc_tm->year; i++)
        {
            if (_TINYUTC_IS_LEAP_YEAR(i))
            {
                *unix_ts += _TINYUTC_SECS_PER_DAY;
            }
        }

        // For each month, add the number of days in that month.
        for (i = 1; i < utc_tm->month; i++)
        {
            days_in_month = _TINYUTC_GET_DAYS_IN_MONTH(i - 1, utc_tm->year);
            *unix_ts += _TINYUTC_SECS_PER_DAY * days_in_month;
        }

        // The remaining is trivial:
        // days, hours, minutes and seconds.
        *unix_ts += (utc_tm->day - 1) * _TINYUTC_SECS_PER_DAY;
        *unix_ts += utc_tm->hour * _TINYUTC_SECS_PER_HOUR;
        *unix_ts += utc_tm->minute * _TINYUTC_SECS_PER_MIN;
        *unix_ts += utc_tm->second;

        return 0;
    }

    /**
     * @brief Calculates the day of the week for a given date.
     *
     * This function determines the day of the week for a given date
     * represented by a `TinyUTCTime` structure. It uses the Keith method
     * for day-of-week calculation.
     *
     * @param utc_tm Pointer to a `TinyUTCTime` structure containing the date
     *               for which the day of the week is to be calculated.
     *
     * @return The day of the week as an integer (0-6):
     *         - 0: Monday
     *         - 1: Tuesday
     *         - 2: Wednesday
     *         - 3: Thursday
     *         - 4: Friday
     *         - 5: Saturday
     *         - 6: Sunday
     *
     *         If the year is before the Unix epoch year (_TINYUTC_UNIX_EPOCH_YEAR),
     *         the function returns -1.
     *
     * @see https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Methods_in_computer_code
     */
    static inline err_t tinyutc_get_week_day(const struct TinyUTCTime *utc_tm, bool monday_first)
    {

        uint8_t w_day;

        uint8_t d = utc_tm->day;
        uint8_t m = utc_tm->month;
        uint16_t y = utc_tm->year;

        if (utc_tm->year < _TINYUTC_UNIX_EPOCH_YEAR)
        {
            return -1;
        }
        // https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Methods_in_computer_code

#ifdef TINYUTC_USE_KEITH_METHOD
        // Implementation of Keith method, verbatim from wikipedia
        w_day = (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7;

#else
    // Implementation of Sakamoto's method, verbatim from wikipedia
    static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if (m < 3)
    {
        y -= 1;
    }
    w_day = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;

#endif
        if (!monday_first)
        {
            w_day = (w_day - 1) % 7;
        }
        return w_day;
    }

#ifdef __cplusplus
}
#endif

#endif // _TINYUTC_H
