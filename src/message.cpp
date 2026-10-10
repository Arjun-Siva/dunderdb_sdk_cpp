#include "dunderdb/message.h"

namespace dunderdb
{
    Message::Message(const std::initializer_list<Field> fields)
        : fields_(fields) {}

    Message::Message(const std::initializer_list<Field> fields, const std::string& timestamp) :
    fields_(fields) , timestamp_(timestamp) {}

    Message::Message(std::vector<Field> fields) : fields_(std::move(fields)) {}

    Message::Message(std::vector<Field> fields, const std::string& timestamp) : fields_(std::move(fields)),
        timestamp_(timestamp) {}

    void Message::add_field(Field field) {
        fields_.push_back(std::move(field));
    }

    void Message::add_timestamp(const std::string& timestamp) {
        timestamp_ = timestamp;
    }

    const std::vector<Field>& Message::fields() const {
        return fields_;
    }

    const std::string& Message::timestamp() const{
        return timestamp_;
    }

    const Field* Message::get_field(const std::string& name) const {
        for (const auto& field : fields_) {
            if (field.name() == name) {
                return &field;
            }
        }

        return nullptr;
    }
} // namespace dunderdb
