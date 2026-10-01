#ifndef VISITOR_H
#define VISITOR_H
//Invece di aggiungere un metodo virtual int64_t evaluate() dentro a ogni singolo nodo in Syntax.h usiamo ilpattern Visitor
// serve a disaccoppiare la struttura dei dati dell'albero sintattico (AST) dagli algoritmi che operano su di esso
// Forward declarations
struct Program;
struct BlockStmt;
struct SetStmt;
struct InputStmt;
struct PrintStmt;
struct IfStmt;
struct WhileStmt;

struct NumberNode;
struct VariableNode;
struct BinaryNumOpNode;

struct BoolConstNode;
struct RelOpNode;
struct BoolBinaryOpNode;
struct BoolNotNode;

class Visitor {
public:
    virtual ~Visitor() = default; //garantisce che venga invocato il distruttore della classe derivata corretta

    // Visita dei nodi Statement e Program
    virtual void visit(Program const& p) = 0;
    virtual void visit(BlockStmt const& b) = 0;
    virtual void visit(SetStmt const& s) = 0;
    virtual void visit(InputStmt const& i) = 0;
    virtual void visit(PrintStmt const& p) = 0;
    virtual void visit(IfStmt const& stmt) = 0;
    virtual void visit(WhileStmt const& w) = 0;

    // Visita dei nodi Espressioni Numeriche (NumExpr)
    virtual void visit(NumberNode const& n) = 0;
    virtual void visit(VariableNode const& v) = 0;
    virtual void visit(BinaryNumOpNode const& o) = 0;

    // Visita dei nodi Espressioni Booleane (BoolExpr)
    virtual void visit(BoolConstNode const& b) = 0;
    virtual void visit(RelOpNode const& r) = 0;
    virtual void visit(BoolBinaryOpNode const& b) = 0;
    virtual void visit(BoolNotNode const& n) = 0;
};

#endif
