#ifndef EVALUATION_VISITOR_H
#define EVALUATION_VISITOR_H

#include <iostream>
#include <string>
#include <cstdint>

#include "Visitor.h"
#include "Syntax.h"
#include "SymbolTable.h"
#include "Token.h"
#include "Exception.h"

// Esecutore semantico dell'interprete:
// Visita l'AST eseguendo le istruzioni e calcolando le espressioni.
class EvaluationVisitor : public Visitor {
public:
    EvaluationVisitor(SymbolTable& st, std::ostream& con = std::cout, std::istream& in = std::cin)
        : symbolTable_{ st }, console_{ con }, inStream_{ in } {}

    void visit(Program const& p) override {
        if (p.rootBlock_) {
            p.rootBlock_->accept(*this);
        }
    }

    void visit(BlockStmt const& b) override {
        for (Statement* stmt : b.statements_) {
            stmt->accept(*this);
        }
    }

    void visit(SetStmt const& s) override {
        s.expr_->accept(*this);
        symbolTable_.setValue(s.varId_, numAccumulator_);
    }

    void visit(InputStmt const& i) override {
        int64_t val;
        if (!(inStream_ >> val)) {
            throw EvaluationError{ "Invalid numeric input from console." };
        }
        symbolTable_.setValue(i.varId_, val);
    }

    void visit(PrintStmt const& p) override {
        p.expr_->accept(*this);
        console_ << numAccumulator_ << std::endl;
    }

    void visit(IfStmt const& stmt) override {
        stmt.guard_->accept(*this);
        if (boolAccumulator_) {
            stmt.thenBlock_->accept(*this);
        } else {
            stmt.elseBlock_->accept(*this);
        }
    }

    void visit(WhileStmt const& w) override {
        while (true) {
            w.guard_->accept(*this);
            if (!boolAccumulator_) {
                break;
            }
            w.body_->accept(*this);
        }
    }

    void visit(NumberNode const& n) override {
        numAccumulator_ = n.val_;
    }

    void visit(VariableNode const& v) override {
        numAccumulator_ = symbolTable_.getValue(v.id_);
    }

    void visit(BinaryNumOpNode const& o) override {
        o.left_->accept(*this);
        int64_t leftVal = numAccumulator_; // Salviamo il sinistro sullo stack C++ perchè poi  con o.right_->accept(*this); sovrascrive numAccuulator_;

        o.right_->accept(*this);
        int64_t rightVal = numAccumulator_; // Ora leggiamo il valore destro

        switch (o.opCode_) {
            case Token::ADD: numAccumulator_ = leftVal + rightVal; break;
            case Token::SUB: numAccumulator_ = leftVal - rightVal; break;
            case Token::MUL: numAccumulator_ = leftVal * rightVal; break;
            case Token::DIV:
                if (rightVal == 0) {
                    throw EvaluationError{ "Division by zero." };
                }
                numAccumulator_ = leftVal / rightVal;
                break;
            default:
                throw EvaluationError{ "Unknown arithmetic operator." };
        }
    }

    void visit(BoolConstNode const& b) override {
        boolAccumulator_ = b.val_;
    }

    void visit(RelOpNode const& r) override {
        r.left_->accept(*this);
        int64_t leftVal = numAccumulator_;

        r.right_->accept(*this);
        int64_t rightVal = numAccumulator_;

        switch (r.opCode_) {
            case Token::LT: boolAccumulator_ = (leftVal < rightVal); break;
            case Token::GT: boolAccumulator_ = (leftVal > rightVal); break;
            case Token::EQ: boolAccumulator_ = (leftVal == rightVal); break;
            default:
                throw EvaluationError{ "Unknown relational operator." };
        }
    }

    // Valutazione cortocircuitata (Short-Circuit) per AND e OR
    void visit(BoolBinaryOpNode const& b) override {
        b.left_->accept(*this);
        bool leftRes = boolAccumulator_;

        if (b.opCode_ == Token::AND) {
            if (!leftRes) {
                boolAccumulator_ = false;
                return; // Cortocircuito: il destro non viene valutato
            }
            b.right_->accept(*this);
            bool rightRes = boolAccumulator_;
            boolAccumulator_ = (leftRes && rightRes);
        } else if (b.opCode_ == Token::OR) {
            if (leftRes) {
                boolAccumulator_ = true;
                return; // Cortocircuito: il destro non viene valutato
            }
            b.right_->accept(*this);
            bool rightRes = boolAccumulator_;
            boolAccumulator_ = (leftRes || rightRes);
        } else {
            throw EvaluationError{ "Unknown boolean operator." };
        }
    }

    void visit(BoolNotNode const& n) override {
        n.expr_->accept(*this);
        boolAccumulator_ = !boolAccumulator_;
    }

private:

    SymbolTable& symbolTable_;
    std::ostream& console_;
    std::istream& inStream_;
    //Nel pattern visitor standdard i method sono void visit(), abbiamo bisogno di registri per far arrivare i risultati
    int64_t numAccumulator_{ 0 };   // Registro per espressioni numeriche
    bool boolAccumulator_{ false }; // Registro per espressioni booleane
};

#endif
