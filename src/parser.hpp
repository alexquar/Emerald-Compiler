#pragma once 
#include <vector>
#include "token.hpp"
#include <iostream>

struct NodeExpr{
    Token int_lit;
};

struct NodeExit{
    NodeExpr expr;
};

class Parser {
private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;

    inline std::optional<Token> peak(int count = 0) const {
        if (m_index + count >= m_tokens.size()) {
            return std::nullopt;
        }
        return m_tokens[m_index + count];
    }
    inline Token consume() {
        if (m_index >= m_tokens.size()) {
            throw ("No more tokens to consume");
        }
        return m_tokens[m_index++];
    }
public:
    inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)){ }

    inline std::optional<NodeExpr> parse_expr() {
        if (!peak().has_value() && peak().value().type != TokenType::int_lit) {
            return NodeExpr{consume()};
        }
        NodeExpr expr;
        expr.int_lit = consume(); // Consume the integer literal
        return expr;
    }

    inline std::optional<NodeExit> parse(){
        std::optional<NodeExit> node_exit;
        while(peak().has_value()) {
            if (peak().value().type == TokenType::exit) {
                if(auto node_expr = parse_expr()) {
                    
                } else {
                    std::cerr << "Expected expression after 'exit'" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
    }
}
};