#pragma once
#include <vector>
#include "column.h"

class Schema {
public:
    explicit Schema(const std::string& service_name) : service_name_(service_name) {};

    void add_column(Column column);

    const std::vector<Column> &columns() const noexcept;
    const std::string &service_name() const noexcept;

private:
    std::string service_name_;
    std::vector<Column> columns_;

    static bool validate_column(Column &column);
};
