#pragma once

#include "tokenization.hpp"
#include <iostream>
#include <optional>
#include <variant>
#include "arena.hpp"

struct NodeExprIdent{
    Token ident;
};

struct NodeExprIntLit {
    Token int_lit;
};

struct NodeExpr;
struct NodeStmtExit {
    NodeExpr* expr;
};

struct BinExprAdd {
    NodeExpr left;
    NodeExpr right;
};

struct BinExprMultiply {
    NodeExpr* left;
    NodeExpr* right;
};

struct BinExprMultiply {
    NodeExpr* left;
    NodeExpr* right;
};

struct BinExpr {
    std::variant<BinExprAdd, BinExprMultiply> var;
};

struct NodeExpr {
    std::variant<NodeExprIntLit*, NodeExprIdent*, BinExpr*> var;
};

struct NodeStmtMake {
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt{
    std::variant<NodeStmtExit*, NodeStmtMake*> expr;
};
struct NodeProg{
    std::vector<NodeStmt> stmts;
};





class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)),
            m_allocator(1024 * 1024 * 4)
    {
    }

    std::optional<NodeExpr> parse_expr()
    {
        if (peek().has_value() && peek().value().type == TokenType::int_lit) {
            //next code goes here
            return NodeExpr{ NodeExprIntLit{ consume() } };
        }
        else if(peek().has_value() && peek().value().type == TokenType::ident) {
            return NodeExpr{ NodeExprIdent{ consume() } };
        }
        else {
            return {};
        }
    }
    

    std::optional<NodeStmt> parse_stmt()
    {
        while (peek().has_value()) {
            if (peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren) {
                consume();
                consume();
                std::optional<NodeStmtExit> exit_node;
                if (auto node_expr = parse_expr()) {
                    exit_node = NodeStmtExit { node_expr.value() };
                }
                else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value() && peek().value().type == TokenType::close_paren) {
                    consume();
                } else {
                    std::cerr << "Expected `)`" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek().value().type == TokenType::semi) {
                    consume();
                }
                else {
                    std::cerr << "Expected `;" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return NodeStmt { exit_node.value() };
            } else if (peek().has_value() && peek().value().type == TokenType::make && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq) {
                consume();
                NodeStmtMake stmt_let = NodeStmtMake{
                    consume(),
                };
                consume();
                if (auto node_expr = parse_expr()) {
                    stmt_let.expr = node_expr.value();
                } else {
                    std::cerr << "Invalid expression after `make` statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peek().has_value() && peek().value().type == TokenType::semi) {
                    consume();
                } else {
                    std::cerr << "Expected `;` after `make` statement" << std::endl;
                    exit(EXIT_FAILURE);
                } 
                return NodeStmt { stmt_let };
            }
            else{
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        std::cerr << "No valid statement found" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::optional<NodeProg> parse_prog()
    {
        NodeProg prog;
        while (peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            } else {
                return {};
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const
    {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume()
    {
        return m_tokens.at(m_index++);
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};