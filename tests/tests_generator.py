"""
@file tests_generator.py
@brief Python tests generator for unix - utc conversion
@author Ulysse Moreau
@date 2025-05-20
@version 2.0
@license WTFPL (Do What The F*ck You Want To Public License)
This program is free software. It comes without any warranty, to
the extent permitted by applicable law. You can redistribute it
and/or modify it under the terms of the Do What The Fuck You Want
To Public License, Version 2, as published by Sam Hocevar. See
http://www.wtfpl.net/ for more details.
"""

import datetime
import random


def format(ts):
    dt = datetime.datetime.fromtimestamp(ts, tz=datetime.timezone.utc)
    return (
        f"{{ {dt.year}, {dt.month}, {dt.day}, {dt.hour}, {dt.minute}, {dt.second}, 0 }}"
    )


print("""
struct Test{
    uint32_t ts;
    const struct TinyUTCTime utc_tm;
};
""")

print("struct Test test_cases[] = {")

for i in range(200):
    ts = random.randint(0, 0xFFFFFFFF)
    c = format(ts)
    print(f"\t{{{ts}, {c} }},")

print("};")
