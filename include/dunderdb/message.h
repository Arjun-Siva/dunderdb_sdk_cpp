#pragma once

#include "field.h"

#include <string>
#include <vector>

namespace dunderdb {

    class Message {
    public:
        Message() = default;

        Message(std::initializer_list<Field> fields);
        Message(std::initializer_list<Field> fields, const std::string& timestamp);
        explicit Message(std::vector<Field> fields);
        explicit Message(std::vector<Field> fields, const std::string& timestamp);

        void add_field(Field field);
        void add_timestamp(const std::string& timestamp);

        [[nodiscard]] const std::vector<Field>& fields() const;
        [[nodiscard]] const std::string& timestamp() const;
        [[nodiscard]] const Field* get_field(const std::string& name) const;

    private:
        std::vector<Field> fields_;
        std::string timestamp_;
    };

} // namespace dunderdb