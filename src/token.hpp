// token.hpp
#pragma once
#include <string>
#include <optional>

enum class TokenType {
    exit, 
    int_lit, 
    semi
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};
