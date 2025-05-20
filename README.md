# TinyUTC

C time conversion functions for embedded programming.

This is a single-header header library, exposing the following function:

- `tinyutc_unix_to_utc`: From an UNIX timestamp to a UTC time structure.
- `tinyutc_utc_to_unix`: From an UTC time structure to an UNIX timestamp.
- `tinyutc_get_week_day`: Get the week day from a UTC time structure.

Aditionnaly, an iso datetime parser can be found, with the following function exposed

- `tinyutc_parse_iso8601_datetime`: Parse an ISO8601 datetime string to a UTC time structure.
- `tinyutc_parse_iso8601_date`: Parse an ISO8601 date string to a UTC time structure.
- `tinyutc_parse_iso8601_time`: Parse an ISO8601 time string to a UTC time structure.

The datetime range supported is **after 01/01/1970 00:00:00 UTC**.

## Example codes

### Converting timestamps

```c
#include "tinyutc.h"

int main()
{
    struct TinyUTCTime current_time = {
        .year = 2024,
        .month = 02,
        .day = 29,
        .hour = 17,
        .minute = 05,
        .second = 03,
    };

    uint32_t current_timestamp = 0;
    // The tinyutc_time_t is described in the `# Types` section
    tinyutc_utc_to_unix(&current_time, &current_timestamp);

    // I want an alarm 10 days and 7 seconds in the future
    uint32_t alarm_timestamp = current_timestamp + 10 * _TINYUTC_SECS_PER_DAY + 7;

    struct TinyUTCTime alarm_time = {0};

    tinyutc_unix_to_utc(&alarm_time, alarm_timestamp);

    // Some microcontrollers needs the week day to exhaustively set it up.
    uint8_t week_day = tinyutc_get_week_day(&current_time, 0);

    // Then, you can set up your RTC struct accordingly, and set up the correct alarm.
}
```

### Parsing ISO8601 strings


```c
#include "tinyutc.h"
#include "iso8601_parser.h"

int main(){
    struct TinyUTCTime result = {0};

    // The third argument corresponds to the parser strictness.
    // Strictness means that a date and a time MUST be separated by the caracter 'T'.
    tinyutc_parse_iso8601_datetime(&result, "1972-12-31T17:05:03-0100", false);
    tinyutc_parse_iso8601_date(&result, "2013W217");
    tinyutc_parse_iso8601_time(&result, "T01:23:45.111111Z");
}
```

## Types

As for the timestamp type, TinyUTC uses a custom type `tinyutc_time_t`, defined
by default as a `uint32_t`. You can override it in your application, for instance:

```c
#define tinyutc_time_t uint64_t
#include "tinyutc.h"
```

## Configs

Available configs are

```
TINYUTC_USE_KEITH_METHOD
```

The method for the week day calculation is Sakamoto's method by default. To use
Keith method, use :

```c
#define TINYUTC_USE_KEITH_METHOD
```

The Keith method is valid between only 1905 and 2099, but saves you 12 bytes on the stack.

## About UTC and UNIX timestamp

### What is a UNIX timestamp ?

The UNIX timestamp is the number of seconds ellapsed since 1970. However,
the "seconds" in a UNIX timestamp is defined as "something that there is
$86400$ of in a day. This would be a good measure of time if there were 
$24 \times 60 \times 60$ seconds in a day, but sadly, it is not the case.

### What us UTC ?

UTC (Coordinated Universal Time) is based from TAI (International Atomic Time),
where a second is a second (a precise amount of time measured by atomic clock),
and crucially always the same length (on earth. Don't get started on [TCB](https://en.wikipedia.org/wiki/Barycentric_Coordinate_Time).)

However, due to small variation in earth's rotation, the TAI will desynchronise
over time with the earth rotation. Thus, once in a while, a "leap second" will 
be added or removed to UTC to keep it synchronized with earth position. This
will usually be done on the last day of June or December.

### So ?

What this means for the type of conversion provided in this library is that
- In a day with a leap second, the result will be accurate only to the second.
- When computing time deltas, and going over a day with a leap second, the
  time delta will be off by around a second cumulative with every day
  you go over such a day.

In most applications, this is fine tho.

# Genesis

When developping embedded application, the time library present in `posix` family
are generally not available. Typically, `localtime` is rarely present and `mktime`
is virtually never present. And **that is a good thing**.

Indeed, mktime's resposability is to break apart a UNIX timestamp in meaningful
date & time data, according to the timezone environement. In an embedded environement,
there is no such thing as a timezone environement.

> Note that the UTC-equivalent `timegm` is _occasionnaly_ present, and serves the same purpose.

However, in some rare cases, converting back-and forth an
UNIX timestamp and specifically **UTC** date & time
can be necessary. Many RTC modules only speak in date & time structures,
but it isn't trivial to compute time deltas in such a form.

In the very specific case of wanting to set an alarm in a specific delay (as 3h, 12m and 5s) and
needing to set the registers of an RTC module to set said alarm, we both need the date & time 
representation and the unix timestamp to make easy time offset calculations.

I do not see other use cases for this lib.

# Dependencies

`stdint` & `stdbool`. Please. At least.

I run my own internal functions for parsing strings, as I only really need `strlen`,
which is trivial.

# Tests

Extensive. Check the `tests` folder.

# FAQ

## When will you add support for timestamps before 1970 ?

It is not planned. However, if you provide me with a resonnable
use case for this feature (remember, you are on an embedded chip,
most likely on bare-metal, and do not have access to any time environment),
then I'll gladly add this feature to this library.

## When will you add support for timezones ?

**_Never_**. This is not the purpose of this library.

Moreover, willing to do timezone support on an embedded platform 
_necessarily means_ that you are willing to display dynamic timezone
infos to the user. If you are willing to do that, but you are on
an embedded platform that does not natively support time, you 
absolutely need to go back to the drawing board.

If you need such a feature, there is no way you have done a correct
architecture without _at least_ using an embedded linux to manage
this kind of hurdles.

## Aren't you kind of already parsing time zones in iso8601 strings ?

**No**. Those are not _timezones_, those are _UTC offsets_. They indicate
by how much the specified date-time differ from UTC. Time zones are a political
division of groups of people, where depending on the location on the
earth, the time of the year (aka daylight saving time), and mabye stuff
that I am not aware of, a decision is made to set the local time
with some offset from UTC. This final decision, only valid for some
period of time that I don't wanna hear about, is eventually conveyed by
a UTC offset, that this library _do_ parse, and transforms to UTC.

## Why can't you incorporate leap seconds into the computations ?

I can't predict the future.

## How can I get the week day from a timestamp ?

Convert your timestamp to a `TinyUTCTime` struct first. I could
add the function, but i'll just do exactly that inside, so I
find it cleaner not to expose this function, and let you code it
explicitely if you really need it.

## WTF is that weird license ?

[WTFPL](https://www.wtfpl.net/), more precisely.

### Can I use it in a commercial application ?

Of course.

### Can I strip your name and copy-paste the code in my commercial project ?

Be my guest. I do not care.
