#ifndef SYNTAX_H
#define SYNTAX_H

#include <vector>
#include <string>
#include <cstdint>

class Visitor;
 //Nel progetto i nodi dell'AST usano puntatori grezzi e dealloc ricorsiva manuale nei distruttori
 //approccio alternativo era usar gli Smart pointer che ci garantiva la distruzione automatica del sotto albero dopo la morte del padre

// La grammatica Lisp distingue in modo neto le 3 struct: (numExpr, boolExpr, Statement)
// In questo mdo impediamo al Parser di confondere istruzioni con espression etc

// Interfaccia base astratta per tutte le istruzioni (Statement)
struct Statement {
	virtual ~Statement() = default; //Mjettiamo distruttore virtuale obbligatio in ogni struct base perchè il ignorerebbe il distruttore del nodo derivato così che i figli del nodo non verrebero mai deallocati.
	
	virtual void accept(Visitor& visitor) const = 0; //Metodo virtuale puro che permette ilpolimorfismo con il pattern visitor (rende la struct non istanziabile direttamente)
}; //Permette il double dispatchm così che a runtime ogni nodo chiami il proprio metodo visitatore (visitor.visit(*this))

// Interfaccia base astratta per le espressioni numeriche (NumExpr)
struct NumExpr {
	virtual ~NumExpr() = default;
	virtual void accept(Visitor& visitor) const = 0;
};

// Interfaccia base astratta per le espressioni booleane (BoolExpr)
struct BoolExpr {
	virtual ~BoolExpr() = default;
	virtual void accept(Visitor& visitor) const = 0;
};


// Gerarchia Istruzioni (Statement)

// Nodo per (BLOCK statement1 statement2 etc)
struct BlockStmt : Statement {
    BlockStmt() = default;
    ~BlockStmt() override {  //Facciamo delete su tutto il vettore statements_, popagando a cascata su tutti i nodi dell'albero
        for (auto stmt : statements_) {
            delete stmt;
        }
    }
    void accept(Visitor& visitor) const override;
    std::vector<Statement*> statements_;
};

// Nodo per (SET variable_id num_expr)
struct SetStmt : Statement {
    SetStmt(std::string varId, NumExpr* expr)
        : varId_{ std::move(varId) }, expr_{ expr } {} //Usiamo move per evitare copie
    ~SetStmt() override {
        delete expr_;
    }
    void accept(Visitor& visitor) const override;
    std::string varId_;
    NumExpr* expr_;
};

// Nodo per (INPUT variable_id)
struct InputStmt : Statement {
    InputStmt(std::string varId)
        : varId_{ std::move(varId) } {}
    // Nessun delete: varId_ è std::string, si distrugge da sola
    void accept(Visitor& visitor) const override;
    std::string varId_;
};

// Nodo per (PRINT num_expr)
struct PrintStmt : Statement {
    PrintStmt(NumExpr* expr)
        : expr_{ expr } {}
    ~PrintStmt() override {
        delete expr_; // per evitare memory leak
    }
    void accept(Visitor& visitor) const override;
    NumExpr* expr_;
};

// Nodo per (IF bool_expr stmt_block stmt_block)
struct IfStmt : Statement {
    IfStmt(BoolExpr* guard, Statement* thenBlock, Statement* elseBlock) // Tre componenti : guard_ booleana, il ramo then e else di tipo Statement*
        : guard_{ guard }, thenBlock_{ thenBlock }, elseBlock_{ elseBlock } {}
    ~IfStmt() override {
		//Libera i sottoalberi 
        delete guard_;
        delete thenBlock_;
        delete elseBlock_;
    }
    void accept(Visitor& visitor) const override;
    BoolExpr* guard_;
    Statement* thenBlock_;
    Statement* elseBlock_;
};

// Nodo per (WHILE bool_expr stmt_block)
struct WhileStmt : Statement {
    WhileStmt(BoolExpr* guard, Statement* body) // Due componenti : guard_ booleana, il corpo del ciclo di tipo Statement*
        : guard_{ guard }, body_{ body } {}
    ~WhileStmt() override {
        delete guard_;
        delete body_;
    }
    void accept(Visitor& visitor) const override;
    BoolExpr* guard_;
    Statement* body_;
};

// Gerarchia Espressioni Numeriche (NumExpr)


// Nodo per costanti numeriche (es. 42, -5)
struct NumberNode : NumExpr { 
    NumberNode(int64_t val) : val_{ val } {} //int64_t per garantire un intervallo numerico ampio
    // Nessun puntatore interno: nessun delete necessario
    void accept(Visitor& visitor) const override;
    int64_t val_;
	//Sia NumberNode che VariableNode sono leaf, non dobbiamo definire un distruttore custom
};

// Nodo per variabili numeriche (es. x, total)
struct VariableNode : NumExpr {
    VariableNode(std::string id) : id_{ std::move(id) } {}
    // id_ è std::string: si distrugge automaticamente
    void accept(Visitor& visitor) const override;
    std::string id_;
};

// Nodo per operazioni aritmetiche binarie (ADD, SUB, MUL, DIV)
struct BinaryNumOpNode : NumExpr {
    BinaryNumOpNode(int opCode, NumExpr* left, NumExpr* right)
        : opCode_{ opCode }, left_{ left }, right_{ right } {}
    ~BinaryNumOpNode() override {
        delete left_; //Dealloco ricorsivamente i sottoalberi sinistro e destro  
        delete right_;
    }
    void accept(Visitor& visitor) const override;
    int opCode_;
    NumExpr* left_;
    NumExpr* right_;
};



// Nodo per costanti booleane (TRUE, FALSE)
struct BoolConstNode : BoolExpr {
    BoolConstNode(bool val) : val_{ val } {}
    // Tipo primitivo: nessun delete necessario
    void accept(Visitor& visitor) const override;
    bool val_;
};

// Nodo per confronti relazionali tra numeri (LT, GT, EQ)
// Esempio: (LT x 10) confronta due NumExpr producendo un valore booleano
struct RelOpNode : BoolExpr {
    RelOpNode(int opCode, NumExpr* left, NumExpr* right)
        : opCode_{ opCode }, left_{ left }, right_{ right } {}
    ~RelOpNode() override {
        delete left_;
        delete right_;
    }
    void accept(Visitor& visitor) const override;
    int opCode_;
    NumExpr* left_;
    NumExpr* right_;
};

// Nodo per operatori logici tra booleani (AND, OR)
// Esempio: (AND (LT x 10) TRUE)
struct BoolBinaryOpNode : BoolExpr {
    BoolBinaryOpNode(int opCode, BoolExpr* left, BoolExpr* right)
        : opCode_{ opCode }, left_{ left }, right_{ right } {}
    ~BoolBinaryOpNode() override {
        delete left_;
        delete right_;
    }
    void accept(Visitor& visitor) const override;
    int opCode_;
    BoolExpr* left_;
    BoolExpr* right_;
};

// Nodo per la negazione logica (NOT)
// Esempio: (NOT (EQ x 0))
struct BoolNotNode : BoolExpr {
    BoolNotNode(BoolExpr* expr) : expr_{ expr } {}
    ~BoolNotNode() override {
        delete expr_;
    }
    void accept(Visitor& visitor) const override;
    BoolExpr* expr_;
};


// Programma Principale (Root dell'AST) rappresenta l'intero programma LISP parsato

// Contiene il blocco iniziale (rootBlock_), deallocato tramite RAII dal distruttore
struct Program {
    Program() = default;
    ~Program() {
        delete rootBlock_; //distrugo il resto a cascata
    }
    void accept(Visitor& visitor) const;

    Statement* rootBlock_{ nullptr };
};

#endif
