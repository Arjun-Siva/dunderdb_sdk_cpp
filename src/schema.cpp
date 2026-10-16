#include <stdexcept>
#include <dunderdb/schema.h>

void Schema::add_column(Column column) {
    if (this->validate_column(column)) {
        this->columns_.push_back(std::move(column));
    } else {
        throw std::invalid_argument("Invalid column type");
    }
}

const std::vector<Column> &Schema::columns() const noexcept {
    return this->columns_;
}

const std::string &Schema::service_name() const noexcept {
    return this->service_name_;
}

bool Schema::validate_column(Column &column) {
    if (column.name.empty()) {
        return false;
    }

    if (column.type == FieldType::NUMBER && column.max_length.has_value()) {
        return false;
    }

    if (column.type == FieldType::STRING && !column.max_length.has_value()) {
        return false;
    }

    return true;
}
