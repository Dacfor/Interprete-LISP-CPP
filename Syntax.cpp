#include "Syntax.h"
#include "Visitor.h"

// implementazione accept per il visitor (double dispatch)
// Non lo implementiamo inline nel Syntax.h perchè dovremmo includere Visitor.h a a sua volta il Visitor.h deve conoscere i nodi BlockStmt, SetStmt, ecc. questo creerebbe un loop di inclusioni   
void BlockStmt::accept(Visitor& visitor) const { visitor.visit(*this); }
void SetStmt::accept(Visitor& visitor) const { visitor.visit(*this); }
void InputStmt::accept(Visitor& visitor) const { visitor.visit(*this); }
void PrintStmt::accept(Visitor& visitor) const { visitor.visit(*this); }
void IfStmt::accept(Visitor& visitor) const { visitor.visit(*this); }
void WhileStmt::accept(Visitor& visitor) const { visitor.visit(*this); }

void NumberNode::accept(Visitor& visitor) const { visitor.visit(*this); }
void VariableNode::accept(Visitor& visitor) const { visitor.visit(*this); }
void BinaryNumOpNode::accept(Visitor& visitor) const { visitor.visit(*this); }

void BoolConstNode::accept(Visitor& visitor) const { visitor.visit(*this); }
void RelOpNode::accept(Visitor& visitor) const { visitor.visit(*this); }
void BoolBinaryOpNode::accept(Visitor& visitor) const { visitor.visit(*this); }
void BoolNotNode::accept(Visitor& visitor) const { visitor.visit(*this); }

void Program::accept(Visitor& visitor) const { visitor.visit(*this); }
