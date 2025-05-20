/**
 * @file iso8601_parser.h
 * @brief Header file for ISO 8601 date and time parsing functions.
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

#ifndef ISO8601_PARSER_H
#define ISO8601_PARSER_H

#ifdef TINYUTC_DEBUG
#include <stdio.h>
#define LOG_DBG(...) fprintf(stdout, __VA_ARGS__)
#else
#define LOG_DBG(...)
#endif

#include <stdint.h>
#include <stdbool.h>

#include "tinyutc.h"

#ifdef __cplusplus
extern "C"
{
#endif

    enum TinyUTCISO8601ErrorCode
    {
        TINYUTC_ISO8601_OK = 0,
        TINYUTC_ISO8601_INVALID_FORMAT = -1,
        TINYUTC_ISO8601_INVALID_DATE = -2,
        TINYUTC_ISO8601_INVALID_TIME = -3,
        TINYUTC_ISO8601_INVALID_OFFSET = -4,
        TINYUTC_ISO8601_INVALID_MAIN_SEPARATOR = -5,
        TINYUTC_ISO8601_DATE_SEPARATOR_INCONSISTENCY = -6,
        TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR = -7,
        TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG = -8,
        TINYUTC_ISO8601_EMPTY_STRING = -9,
        TINYUTC_ISO8601_EXTRANEOUS_DATE_COMPONENTS = -10,
        TINYUTC_ISO8601_EXTRANEOUS_TIME_COMPONENTS = -11,
        TINYUTC_ISO8601_UTC_OFFSET_WITHOUT_DATE = -12,
        TINYUTC_INTERNAL_ERROR = -13,
    };

    err_t tinyutc_parse_iso8601_datetime(struct TinyUTCTime *utc_tm, const char *iso8601, bool use_strict_separator);
    err_t tinyutc_parse_iso8601_date(struct TinyUTCTime *utc_tm, const char *iso8601_date);
    err_t tinyutc_parse_iso8601_time(struct TinyUTCTime *utc_tm, const char *iso8601_time);

#ifdef __cplusplus
}
#endif

#endif // ISO8601_PARSER_H