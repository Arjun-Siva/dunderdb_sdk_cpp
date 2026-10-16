#pragma once
#include <optional>
#include <cstdint>

enum class FieldType {
    NUMBER,
    STRING
};

struct Column {
    std::string name;
    FieldType type;
    std::optional<uint16_t> max_length = std::nullopt;
    bool nullable = true;
};
