#pragma once


#include "parser.hpp"
#include <cassert>
#include <unordered_map>

class Generator {
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm* term) {
        struct TermVisitor {
            Generator* gen;
            void operator()(const NodeExprIntLit* term_int_lit) const {
                gen->m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
            }
            void operator()(const NodeExprIdent* term_ident) const {
                if (gen->m_vars.find(term_ident->ident.value.value()) == gen->m_vars.end()) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc - 1) * 8 << "]\n";
                gen->push(offset.str());
            }
        };
        TermVisitor visitor{this};
        std::visit(visitor, term->var);
    }

    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator* gen;
            void operator()(const NodeTerm* term) const
            {
                gen->gen_term(term);
            }
            void operator()(const BinExpr* bin_expr) const
            {
                // Visit the variant to handle BinExprAdd* and BinExprMultiply*
                struct BinExprVisitor {
                    Generator* gen;
                    void operator()(const BinExprAdd* add) const {
                        gen->gen_expr(add->left);
                        gen->gen_expr(add->right);
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "    add rax, rbx\n";
                        gen->push("rax");
                    }
                    void operator()(const BinExprMultiply* mul) const {
                        gen->gen_expr(mul->left);
                        gen->gen_expr(mul->right);
                        gen->pop("rax");
                        gen->pop("rbx");
                        gen->m_output << "    imul rax, rbx\n";
                        gen->push("rax");
                    }
                };
                BinExprVisitor bin_visitor{gen};
                std::visit(bin_visitor, bin_expr->var);
            }
        };

        ExprVisitor visitor{this};
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt* stmt)
    {
        struct StmtVisitor {
            Generator* gen;
            void operator()(const NodeStmtExit* stmt_exit) const
            {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "    syscall\n";
            }
            void operator()(const NodeStmtMake* stmt_make) const
            {
                if (gen->m_vars.find(stmt_make->ident.value.value()) != gen->m_vars.end()) {
                    std::cerr << "Identifier already used: " << stmt_make->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({ stmt_make->ident.value.value(), Var { gen->m_stack_size } });
                gen->gen_expr(stmt_make->expr);
            }
        };

        StmtVisitor visitor {this };
        std::visit(visitor, stmt->expr);
    }

    [[nodiscard]] std::string gen_prog()
    {
        m_output << "global _start\n_start:\n";

        for (const NodeStmt* stmt : m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";
        return m_output.str();
    }

private:
    void push(const std::string& reg)
    {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg)
    {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }

    struct Var {
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> m_vars {};
};