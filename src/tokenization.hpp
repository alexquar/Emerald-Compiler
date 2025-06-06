#pragma once
#include <iostream>
#include <vector>
#include "token.hpp"
class Tokenizer {
    private:
        const std::string m_src;
        int m_index = 0;
        std::optional<char> peak(int count = 0) const {
            if(m_index + count >= m_src.length()) {
                return std::nullopt; // Out of bounds
            }
            return m_src[m_index + count];
        }
        
        char consume(){
            return m_src[m_index++];
        }

    public:
        inline explicit Tokenizer(const std::string& src): m_src(std::move(src)) {}

        std::vector<Token> tokenize() {
            std::vector<Token> tokens;
            std::string buf;

            while(peak().has_value()){
                if (std::isalpha(peak().value())) {
                    buf.push_back(consume());
                    while (peak().has_value() && std::isalpha(peak().value()) ){
                        std::cout << peak().value() << std::endl;
                        buf.push_back(consume());
                        std::cout << "buf: " << buf << std::endl;
                    }
                    if (buf == "exit") {
                        tokens.push_back(Token{TokenType::exit, std::nullopt});
                        buf.clear();
                        continue;
                    } else {
                        std::cerr << "You messed up!"<< std::endl;
                        std::cerr << "Unrecognized keyword: " << buf << std::endl;

                        exit(EXIT_FAILURE);
                    }
                } else if (std::isspace(peak().value())) {
                    consume(); // Skip whitespace
                    continue; 
                } else if (std::isdigit(peak().value())) {
                    buf.push_back(consume());
                    while (peak().has_value() && std::isdigit(peak().value())) {
                        buf.push_back(consume());
                    }
                    tokens.push_back(Token{TokenType::int_lit, buf});
                    buf.clear();
                    continue;
                } else if (peak().value() == ';') {
                    consume(); // Consume the semicolon
                    tokens.push_back(Token{TokenType::semi, std::nullopt});
                    continue;
                } else {
                    std::cerr << "You messed up!"<< std::endl;
                    std::cerr << "Unrecognized character: " << peak().value() << std::endl;
                    
                    exit(EXIT_FAILURE);
                }
            }
            m_index = 0; 
            return tokens;
        }
};