#pragma once

#include <string>
#include <vector>
#include <optional>
#include <iostream>
enum class TokenType { exit, int_lit, semi, open_paren, close_paren, ident, make, eq };

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

class Tokenizer {
public:
    inline explicit Tokenizer(std::string src)
        : m_src(std::move(src))
    {
    }

    inline std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;
        std::string buf;
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "finish") {
                    tokens.push_back(Token{ TokenType::exit, std::nullopt });
                    buf.clear();
                    continue;
                } else if(buf == "make") {
                    tokens.push_back(Token{ TokenType::make, std::nullopt });
                    buf.clear();
                    continue;
                }
                else {
                    tokens.push_back(Token{ TokenType::ident, buf });
                    buf.clear();
                    continue;
                }
            }
            else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back(Token{ TokenType::int_lit, buf });
                buf.clear();
                continue;
            }
            else if (peek().value() == '(') {
                consume();
                tokens.push_back(Token{ TokenType::open_paren, std::nullopt });
                continue;
            }
            else if (peek().value() == ')') {
                consume();
                tokens.push_back(Token{ TokenType::close_paren, std::nullopt });
                continue;
            } else if(peek().value() == '=') {
                consume();
                tokens.push_back(Token{ TokenType::eq, std::nullopt });
                continue;
            }
            else if (peek().value() == ';') {
                consume();
                tokens.push_back(Token{ TokenType::semi, std::nullopt });
                continue;
            }
            else if (std::isspace(peek().value())) {
                consume();
                continue;
            }
            else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(int offset = 0) const
    {
        if (m_index + offset >= m_src.length()) {
            return {};
        }
        else {
            return m_src.at(m_index + offset);
        }
    }

    inline char consume()
    {
        return m_src.at(m_index++);
    }

    const std::string m_src;
    size_t m_index = 0;
};