#pragma once
#include <chrono>

namespace dunderdb::util {
    inline std::string time_point_to_iso_string(const std::chrono::system_clock::time_point tp) {
        const auto duration = tp.time_since_epoch();

        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
            duration - seconds.time_since_epoch()
        );

        return std::format(
            "{:%Y-%m-%dT%H:%M:%S}.{:03d}Z",
            seconds,
            millis.count()
        );
    }

    inline std::chrono::system_clock::time_point iso_string_to_time_point (const std::string_view timestamp) {
        int year, month, day, hour, minute, second;
        int milliseconds = 0;

        const int parsed = std::sscanf(
            timestamp.data(),
            "%4d-%2d-%2dT%2d:%2d:%2d",
            &year, &month, &day, &hour, &minute, &second
        );

        if (parsed != 6)
            throw std::invalid_argument("Invalid ISO timestamp");

        // Parse fractional seconds, if present.
        auto dot = timestamp.find('.');
        if (dot != std::string_view::npos) {
            std::string_view fraction = timestamp.substr(dot + 1, 3);

            if (fraction.size() == 1)
                milliseconds = (fraction[0] - '0') * 100;
            else if (fraction.size() == 2)
                milliseconds = (fraction[0] - '0') * 100
                             + (fraction[1] - '0') * 10;
            else if (fraction.size() == 3)
                milliseconds = (fraction[0] - '0') * 100
                             + (fraction[1] - '0') * 10
                             + (fraction[2] - '0');
        }

        const std::chrono::year_month_day date{
            std::chrono::year{year},
            std::chrono::month{static_cast<unsigned>(month)},
            std::chrono::day{static_cast<unsigned>(day)}
        };

        if (!date.ok() || hour > 23 || minute > 59 || second > 59)
            throw std::invalid_argument("Invalid ISO timestamp");

        const auto tp = std::chrono::sys_days{date}
        + std::chrono::hours{hour}
        + std::chrono::minutes{minute}
        + std::chrono::seconds{second}
        + std::chrono::milliseconds{milliseconds};

        return std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp);
    }
}
