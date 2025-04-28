# TinyUTC

C time conversion functions for embedded programming.

This is a single-header library, composed of two functions:

- `tinyutc_unix_to_utc`: From an UNIX timestamp to a UTC time structure.
- `tinyutc_utc_to_unix`: From an UTC time structure to an UNIX timestamp.

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

## Example

```c
#include "tinyutc.h"

main(){
    struct TinyUTCTime current_time = {
        .year = my_rtc_time_struct.year,
        .month = my_rtc_time_struct.month,
        .day = my_rtc_time_struct.day,
        .hour = my_rtc_time_struct.hour,
        .minute = my_rtc_time_struct.minute,
        .second = my_rtc_time_struct.second,
    };

    // The __tinyutc_time_t is described in the `# Types` section
    __tinyutc_time_t current_timestamp = tinyutc_utc_to_unix(&current_time)

    // I want an alarm 10 days and 7 seconds in the future
    __tinyutc_time_t alarm_timestamp = current_timestamp + 10 * _TINYUTC_SECS_PER_DAY + 7

    struct TinyUTCTime alarm_time = {0};

    tinyutc_unix_to_utc(alarm_timestamp, &alarm_time);

    // Then, you can set up your RTC struct accordingly, and set up the correct alarm.
}

```

## Types

As for the timestamp type, TinyUTC uses a custom type `__tinyutc_time_t`, defined
by default as a `uint32_t`. You can override it in your application, for instance:

```c
#define __tinyutc_time_t uint64_t
#include "tinyutc.h"
```

# Dependencies

`stdint`. Please. At least.

# Tests

Somewhat. Check the `examples` folder.

# FAQ

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

## WTF is that weird license ?

[WTFPL](https://www.wtfpl.net/), more precisely.

### Can I use it in a commercial application ?

Of course.

### Can I strip your name and copy-paste the code in my commercial project ?

Be my guest. I do not care.
