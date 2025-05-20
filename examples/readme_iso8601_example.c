#include "../tinyutc.h"
#include "../iso8601_parser.h"

int main(){
    struct TinyUTCTime result = {0};

    // The third argument corresponds to the parser strictness.
    // Strictness means that a date and a time MUST be separated by the caracter 'T'.
    tinyutc_parse_iso8601_datetime(&result, "1972-12-31T17:05:03-0100", false);
    tinyutc_parse_iso8601_date(&result, "2013W217");
    tinyutc_parse_iso8601_time(&result, "T01:23:45.111111Z");
}