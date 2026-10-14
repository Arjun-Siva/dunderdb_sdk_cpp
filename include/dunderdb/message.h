#pragma once

#include <chrono>

#include "field.h"

#include <string>
#include <vector>

namespace dunderdb {

    class Message {
    public:
        Message() = default;

        Message(std::initializer_list<Field> fields);
        Message(std::initializer_list<Field> fields, std::chrono::system_clock::time_point timestamp);
        explicit Message(std::vector<Field> fields);
        explicit Message(std::vector<Field> fields, std::chrono::system_clock::time_point timestamp);

        void add_field(Field field);
        void add_timestamp(std::chrono::system_clock::time_point timestamp);

        [[nodiscard]] const std::vector<Field>& fields() const;
        [[nodiscard]] const std::chrono::system_clock::time_point& timestamp() const;
        [[nodiscard]] const Field* get_field(const std::string& name) const;

    private:
        std::vector<Field> fields_;
        std::chrono::system_clock::time_point timestamp_;
    };

} // namespace dunderdb