#pragma once

#include <chrono>
#include <string>

namespace dunderdb {

    class QueryRequest {
    public:
        QueryRequest(
            std::string service_name,
            std::chrono::system_clock::time_point start_time,
            std::chrono::system_clock::time_point end_time
        );

        [[nodiscard]] const std::string& service_name() const;
        [[nodiscard]] const std::chrono::system_clock::time_point& start_time() const;
        [[nodiscard]] const std::chrono::system_clock::time_point& end_time() const;

    private:
        static void validate_timestamp(const std::string& timestamp);

        std::string service_name_;
        std::chrono::system_clock::time_point start_time_;
        std::chrono::system_clock::time_point end_time_;
    };

} // namespace dunderdb