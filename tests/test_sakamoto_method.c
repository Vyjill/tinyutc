/**
 * @file tests_unix_utc.h
 * @brief Test cases for sakamoto method
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

#include <stdio.h>

#include "../tinyutc.h"

struct DayOfWeekTestCase
{
    int year;
    int month;
    int day;
    int expected_day;
};

struct DayOfWeekTestCase test_cases[] = {
    {1999, 5, 13, 4},
    {2000, 1, 4, 2},
    {2000, 2, 18, 5},
    {2004, 7, 21, 3},
};

int main()
{
    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++)
    {
        struct TinyUTCTime utc_tm = {test_cases[i].year, test_cases[i].month, test_cases[i].day, 0, 0, 0, 0};
        uint8_t day_of_week = tinyutc_get_week_day(&utc_tm, true);

        if (day_of_week != test_cases[i].expected_day)
        {
            printf("Test case %d failed: expected %d but got %d\n", i + 1, test_cases[i].expected_day, day_of_week);
        }
        else
        {
            printf("Test case %d passed\n", i + 1);
        }

        day_of_week = tinyutc_get_week_day(&utc_tm, false);

        int expected_day = (test_cases[i].expected_day - 1) % 7; // Adjust for Sunday = 0, Saturday = 6

        if (day_of_week != expected_day)
        {
            printf("Test case %d failed: expected %d but got %d\n", i + 1, expected_day, day_of_week);
        }
        else
        {
            printf("Test case %d passed\n", i + 1);
        }
    }
}