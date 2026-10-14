#include "dunderdb/query_request.h"

#include <chrono>
#include <regex>
#include <stdexcept>
#include <utility>

namespace dunderdb
{
    QueryRequest::QueryRequest(
        std::string service_name,
        const std::chrono::system_clock::time_point start_time,
        const std::chrono::system_clock::time_point end_time
    )
        : service_name_(std::move(service_name)),
          start_time_(start_time),
          end_time_(end_time) {
        // check if start time is earlier than end time
        if (start_time >= end_time) {
            throw std::invalid_argument("start_time must be earlier than end_time");
        }
    }

    const std::string& QueryRequest::service_name() const {
        return service_name_;
    }

    const std::chrono::system_clock::time_point& QueryRequest::start_time() const {
        return start_time_;
    }

    const std::chrono::system_clock::time_point& QueryRequest::end_time() const {
        return end_time_;
    }

    // NOTE: not used anymore
    void QueryRequest::validate_timestamp(const std::string& timestamp) {
        static const std::regex iso_timestamp(
            R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?Z$)"
        );

        if (!std::regex_match(timestamp, iso_timestamp)) {
            throw std::invalid_argument(
                "Invalid timestamp. Expected ISO 8601 UTC format: "
                "YYYY-MM-DDTHH:MM:SS[.fraction]Z"
            );
        }
    }
} // namespace dunderdb
