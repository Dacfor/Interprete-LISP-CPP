#ifndef PRINT_VISITOR_H
#define PRINT_VISITOR_H

#include <iostream>
#include "Visitor.h"
#include "Syntax.h"
#include "Token.h"

// visitatore per la stampa dell'albero sintattico (utile per il debug del parser)
// visita Pre Order stampo prima la root(operatore)  e poi i figli es: (ADD 3 5)
class PrintVisitor : public Visitor {
public:
    PrintVisitor(std::ostream& console = std::cout) : console_{ console } {}

    void visit(Program const& p) override {
        if (p.rootBlock_) {
            p.rootBlock_->accept(*this);
            console_ << std::endl;
        }
    }

    void visit(BlockStmt const& b) override {
        console_ << "(BLOCK ";
        for (Statement* s : b.statements_) {
            s->accept(*this);
            console_ << " ";
        }
        console_ << ")";
    }

    void visit(SetStmt const& s) override {
        console_ << "(SET " << s.varId_ << " ";
        s.expr_->accept(*this);
        console_ << ")";
    }

    void visit(InputStmt const& i) override {
        console_ << "(INPUT " << i.varId_ << ")";
    }

    void visit(PrintStmt const& p) override {
        console_ << "(PRINT ";
        p.expr_->accept(*this);
        console_ << ")";
    }

    void visit(IfStmt const& stmt) override {
        console_ << "(IF ";
        stmt.guard_->accept(*this);
        console_ << " ";
        stmt.thenBlock_->accept(*this);
        console_ << " ";
        stmt.elseBlock_->accept(*this);
        console_ << ")";
    }

    void visit(WhileStmt const& w) override {
        console_ << "(WHILE ";
        w.guard_->accept(*this);
        console_ << " ";
        w.body_->accept(*this);
        console_ << ")";
    }

    void visit(NumberNode const& n) override {
        console_ << n.val_;
    }

    void visit(VariableNode const& v) override {
        console_ << v.id_;
    }

    void visit(BinaryNumOpNode const& o) override { 
        console_ << "(" << Token::id2word[o.opCode_] << " "; //root stampo l'operatore
        o.left_->accept(*this);  //figlio sinistro
        console_ << " ";
        o.right_->accept(*this); //figlio destro
        console_ << ")";
    }

    void visit(BoolConstNode const& b) override {
        console_ << (b.val_ ? "TRUE" : "FALSE");
    }

    void visit(RelOpNode const& r) override {
        console_ << "(" << Token::id2word[r.opCode_] << " ";
        r.left_->accept(*this);
        console_ << " ";
        r.right_->accept(*this);
        console_ << ")";
    }

    void visit(BoolBinaryOpNode const& b) override {
        console_ << "(" << Token::id2word[b.opCode_] << " ";
        b.left_->accept(*this);
        console_ << " ";
        b.right_->accept(*this);
        console_ << ")";
    }

    void visit(BoolNotNode const& n) override {
        console_ << "(NOT ";
        n.expr_->accept(*this);
        console_ << ")";
    }

private:
    std::ostream& console_;
};

#endif
