/**
 * @file tests_unix_utc.h
 * @brief Test cases for ISO8601 date parsing
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
#include <string.h>

#include "../iso8601_parser.h"
#include "../tinyutc.h"

#include "tests_common.h"

struct Iso8601TestCase test_cases[] = {
    {"Empty string test", "", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_EMPTY_STRING},
    {"Common date, with separator", "2024-02-29", {2024, 02, 29, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Common date, no separator", "20240429", {2024, 04, 29, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with week number, with separator", "2000-W03-7", {2000, 01, 23, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with week number, no separator", "2000W037", {2000, 01, 23, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with ordinal day 23", "2000-023", {2000, 01, 23, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with ordinal day 100", "2000-100", {2000, 04, 9, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date, ordinal day, no separator", "1989365", {1989, 12, 31, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date, week & day, no separator", "2013W217", {2013, 05, 26, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date, extra characters", "2013W217zugluglu", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_EXTRANEOUS_DATE_COMPONENTS},
    {"Common date, extra characters", "2024-02-29tsointsoin", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_EXTRANEOUS_DATE_COMPONENTS},
};

int main()
{
    struct TinyUTCTime utc_tm = {0};
    int sucess_count = 0;

    char spacingdesc[51] = {' '};
    char spacingiso[31] = {' '};

    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++)
    {
        int parse_result = tinyutc_parse_iso8601_date(&utc_tm, test_cases[i].iso8601);
        int is_expected_time = compare_utc_struct_dates(&utc_tm, &test_cases[i].expected);
        if (test_cases[i].expected_code == parse_result && (parse_result != TINYUTC_ISO8601_OK || is_expected_time))
        {
            int desclength = strlen(test_cases[i].description);
            int spacelength = 50 - desclength;
            memset(spacingdesc, ' ', spacelength);
            spacingdesc[spacelength] = '\0';
            spacingdesc[50] = '\0';

            int isolength = strlen(test_cases[i].iso8601);
            spacelength = 30 - isolength;
            memset(spacingiso, ' ', spacelength);
            spacingiso[spacelength] = '\0';
            spacingiso[30] = '\0';

            if (parse_result == TINYUTC_ISO8601_OK)
            {
                printf("\033[38;5;2m\033[1m[SUCCESS]\033[39m\t Test '%s' %s : '%s' %s => %04d/%02d/%02d %02d:%02d:%02d (+%dus) \n", test_cases[i].description, spacingdesc, test_cases[i].iso8601, spacingiso, utc_tm.year, utc_tm.month, utc_tm.day,
                       utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.microseconds);
            }
            else
            {
                printf("\033[38;5;2m\033[1m[SUCCESS]\033[39m\t Test '%s' %s : '%s' %s => %s\n", test_cases[i].description, spacingdesc, test_cases[i].iso8601, spacingiso, get_err_string(parse_result));
            }
            sucess_count++;
        }
        else
        {
            printf("\033[38;5;1m\033[1m[FAILED]\033[39m\t Test '%s': %s : \n\t\t\t - Expected error code %d, got %d\n", test_cases[i].description, test_cases[i].iso8601, test_cases[i].expected_code, parse_result);
            if (!is_expected_time)
            {
                printf("\t\t\t - Expected time %04d-%02d-%02dT%02d:%02d:%02d (+%dus), got %04d-%02d-%02dT%02d:%02d:%02d (+%dus)\n",
                       test_cases[i].expected.year, test_cases[i].expected.month, test_cases[i].expected.day,
                       test_cases[i].expected.hour, test_cases[i].expected.minute, test_cases[i].expected.second, test_cases[i].expected.microseconds,
                       utc_tm.year, utc_tm.month, utc_tm.day,
                       utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.microseconds);
            }
            // break;
        }
    }
    printf("%d/%llu tests passed.\n", sucess_count, sizeof(test_cases) / sizeof(test_cases[0]));
}
