/**
 * @file tinyutc.h
 * @brief A simple library for converting between Unix timestamps and UTC time.
 * @author Ulysse Moreau
 * @date 2025-04-28
 * @version 1.0
 */

#ifndef _TINYUTC_H
#define _TINYUTC_H

#include <stdint.h>

#ifndef tinyutc_time_t
#define tinyutc_time_t uint32_t
#endif

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
#define LEAP_YEAR(YEAR) \
    (((YEAR) % 400 == 0) || (((YEAR) % 4 == 0) && ((YEAR) % 100 != 0)))

#ifdef __cplusplus
extern "C"
{
#endif

    // Dead-simple structure to hold UTC time
    struct TinyUTCTime
    {
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
        uint8_t day;
        uint8_t month;
        uint16_t year;
    };

    // The number of days in each month
    // - 0. For a non-leap year (February has 28 days)
    // - 1. For a leap year (February has 29 days)
    static const uint8_t days_month[2][12] = {{31, 28, 31, 30, 31, 30,
                                               31, 31, 30, 31, 30, 31},
                                              {31, 29, 31, 30, 31, 30,
                                               31, 31, 30, 31, 30, 31}};

    /**
     * @brief Converts a Unix timestamp to UTC time.
     *
     * This function takes a Unix timestamp and converts it into a UTC time
     * structure, which contains the corresponding year, month, day, hour,
     * minute, and second in Coordinated Universal Time (UTC).
     *
     * @param unix_ts The Unix timestamp to be converted. This represents the
     *                number of seconds since January 1, 1970 (UTC).
     * @param utc_tm  Pointer to a UTCTime structure where the converted UTC
     *                time will be stored.
     */
    static inline void tinyutc_unix_to_utc(tinyutc_time_t unix_ts, struct TinyUTCTime *utc_tm)
    {
        uint16_t year;
        uint8_t month, days_in_month;
        unsigned long days;

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

        while ((days += (LEAP_YEAR(year) ? _TINYUTC_DAYS_PER_LEAP_YEAR : _TINYUTC_DAYS_PER_YEAR)) <= unix_ts)
        {
            year++;
        }
        // Remove the days of the current year, that should not be counted
        days -= LEAP_YEAR(year) ? _TINYUTC_DAYS_PER_LEAP_YEAR : _TINYUTC_DAYS_PER_YEAR;

        // Remove the days of all past years, leaving us with only the day of the current year
        unix_ts -= days;

        days = 0;
        days_in_month = 0;
        for (month = 0; month < _TINYUTC_MONTH_PER_YEAR; month++)
        {
            // Compute the number of days in this month
            days_in_month = days_month[LEAP_YEAR(year)][month];

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
    }

    /**
     * @brief Converts a UTC time structure to a Unix timestamp.
     *
     * This function takes a pointer to a UTCTime structure and converts
     * the provided date and time into a Unix timestamp, which represents
     * the number of seconds since January 1, 1970 (the Unix epoch).
     *
     * @param utc_tm Pointer to a UTCTime structure containing the UTC date and time.
     *
     * @return The corresponding Unix timestamp as a signed 32-bit integer.
     */
    static inline tinyutc_time_t tinyutc_utc_to_unix(struct TinyUTCTime *utc_tm)
    {
        tinyutc_time_t unix_ts;
        int i;

        // Offset year is the number of years since the Unix epoch (1970).

        // Start by couting the number of seconds in the year,
        // assuming non-leap year (365 days).
        unix_ts = (utc_tm->year - _TINYUTC_UNIX_EPOCH_YEAR) * (_TINYUTC_SECS_PER_DAY * _TINYUTC_DAYS_PER_YEAR);

        // For each leap year, add an extra day (86400 seconds).
        for (i = _TINYUTC_UNIX_EPOCH_YEAR; i < utc_tm->year; i++)
        {
            if (LEAP_YEAR(i))
            {
                unix_ts += _TINYUTC_SECS_PER_DAY;
            }
        }

        // For each month, add the number of days in that month.
        for (i = 1; i < utc_tm->month; i++)
        {
            unix_ts += _TINYUTC_SECS_PER_DAY * days_month[LEAP_YEAR(utc_tm->year)][i - 1];
        }

        // The remaining is trivial:
        // days, hours, minutes and seconds.
        unix_ts += (utc_tm->day - 1) * _TINYUTC_SECS_PER_DAY;
        unix_ts += utc_tm->hour * _TINYUTC_SECS_PER_HOUR;
        unix_ts += utc_tm->minute * _TINYUTC_SECS_PER_MIN;
        unix_ts += utc_tm->second;

        return unix_ts;
    }

#ifdef __cplusplus
}
#endif

#endif // _TINYUTC_H
