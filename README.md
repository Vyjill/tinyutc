# TinyUTC

C time conversion functions for embedded programming.

This is a single-header header library, exposing three functions:

- `tinyutc_unix_to_utc`: From an UNIX timestamp to a UTC time structure.
- `tinyutc_utc_to_unix`: From an UTC time structure to an UNIX timestamp.
- `tinyutc_get_week_day`: Get the week day (0 = Sunday) from an UTC time structure.

The datetime range supported is **after 01/01/1970 00:00:00 UTC**.

## Example

```c
#include "../tinyutc.h"

int main(){
    struct TinyUTCTime current_time = {
        .year = 2024,
        .month = 02,
        .day = 29,
        .hour = 17,
        .minute = 05,
        .second = 03,
    };

    // The tinyutc_time_t is described in the `# Types` section
    tinyutc_time_t current_timestamp = tinyutc_utc_to_unix(&current_time);

    // I want an alarm 10 days and 7 seconds in the future
    tinyutc_time_t alarm_timestamp = current_timestamp + 10 * _TINYUTC_SECS_PER_DAY + 7;

    struct TinyUTCTime alarm_time = {0};

    tinyutc_unix_to_utc(alarm_timestamp, &alarm_time);

    // Some microcontrollers needs the week day to exhaustively set it up.
    uint8_t week_day = tinyutc_get_week_day(&current_time);

    // Then, you can set up your RTC struct accordingly, and set up the correct alarm.
}
```

## Types

As for the timestamp type, TinyUTC uses a custom type `tinyutc_time_t`, defined
by default as a `uint32_t`. You can override it in your application, for instance:

```c
#define tinyutc_time_t uint64_t
#include "tinyutc.h"
```

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

When developping embedded application, the time library usually present in `posix`
are generally not available. Typically, `localtime` is rarely present and `mktime`
is virtually never present. And **that is a good thing**.

Indeed, mktime's resposability is to break apart a UNIX timestamp in meaningful
date & time data, according to the timezone environement. In an embedded environement,
there is no such thing as a timezone environement.

However, in some rare cases, converting back-and forth an
UNIX timestamp and specifically **UTC** date & time
can be necessary. Many RTC modules only speak in date & time structures,
but it isn't trivial to compute time deltas in such a form.

In the very specific case of wanting to set an alarm in a specific delay (as 3h, 12m and 5s) and
needing to set the registers of an RTC module to set said alarm, we both need the date & time 
representation and the unix timestamp to make easy time offset calculations.

I do not see other use cases for this lib.

# Dependencies

`stdint`. Please. At least.

# Tests

Somewhat. Check the `examples` folder.

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
