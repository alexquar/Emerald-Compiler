#pragma once
#include <iostream>
#include <vector>
#include "main.cpp"
class Tokenizer {
    private:
        const std::string m_src;
        int m_index;
        std::optional<char> peak(int count = 1) const {
            if(m_index + count >= m_src.length()) {
                return std::nullopt; // Out of bounds
            }
            return m_src[m_index + count];
        }
        
        char consume(){
            return m_src[m_index++];
        }

    public:
        inline Tokenizer(const std::string& src): m_src(std::move(src)) {}

        std::vector<Token> tokenize() const {
            std::vector<Token> tokens;
            std::string buf;

            for (size_t i = 0; i < m_src.length(); ++i) {
                char c = m_src[i];

                if (std::isalpha(c)) {
                    buf.push_back(c);
                    ++i;
                    while (i < m_src.length() && std::isalnum(m_src[i])) {
                        buf.push_back(m_src[i]);
                        ++i;
                    }
                    --i; // Adjust for the loop increment
                    if (buf == "exit") {
                        tokens.push_back(Token{TokenType::exit, std::nullopt});
                    } else {
                        std::cerr << "Error: Unknown identifier '" << buf << "' at position " << i - buf.length() << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    buf.clear();
                } else if (std::isspace(c)) {
                    continue; // Skip whitespace
                } else if (std::isdigit(c)) {
                    buf.push_back(c);
                    ++i;
                    while (i < m_src.length() && std::isdigit(m_src[i])) {
                        buf.push_back(m_src[i]);
                        ++i;
                    }
                    --i; // Adjust for the loop increment
                    tokens.push_back(Token{TokenType::int_lit, buf});
                    buf.clear();
                } else if (c == ';') {
                    tokens.push_back(Token{TokenType::semi, std::nullopt});
                } else {
                    std::cerr << "Error: Unknown character '" << c << "' at position " << i << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            return tokens;
        }
};