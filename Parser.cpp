#include <sstream>
#include "Parser.h"

// program -> stmt block
Program* Parser::parseProgram(std::vector<Token>::const_iterator& itr) {
    Statement* root = parseStmtBlock(itr); // parsa prima: se lancia eccezione, nessun Program viene allocato (zero leak)
    Program* p = new Program{};            // alloca nell'heap solo dopo che il parsing e' riuscito
    p->rootBlock_ = root;                  // assegna il sottoalbero al rootBlock_
    return p;
}

// stmt block -> statement | ( BLOCK statement list )
Statement* Parser::parseStmtBlock(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Overflow in token stream." }; // errore: stream terminato prima del dovuto
    }

    // caso ( BLOCK statement list )
    if (itr->tag == Token::LP) {
        auto nextItr = itr + 1; //guardo avanti senza consumare token per capire quale ramo della grammatica seguire
        if (nextItr != end_ && nextItr->tag == Token::BLOCK) {
            safe_next(itr); // consuma '('
            safe_next(itr); // consuma 'BLOCK'

            if (itr == end_) {
                throw SyntaxError{ "Overflow in token stream." };
            }
            if (itr->tag == Token::RP) {
                throw SyntaxError{ "Empty BLOCK statement" }; // un blocco non puo' essere vuoto
            }

            BlockStmt* block = new BlockStmt{};

            try {
                // statement list -> statement statement list | statement
                while (itr != end_ && itr->tag != Token::RP) {
                    block->statements_.push_back(parseStatement(itr));
                }
            } catch (...) {
                delete block; // il distruttore di BlockStmt pulisce a cascata anche gli statements_ gia' parsati
                throw;        // rilancia l'eccezione al chiamante
            }

            if (itr == end_) {
                delete block;
                throw SyntaxError{ "Overflow in token stream." };
            }
            if (itr->tag != Token::RP) {
                delete block;
                std::stringstream temp;
                temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
                throw SyntaxError{ temp.str() };
            }
            safe_next(itr); // consuma ')'

            return block;
        }
    }

    // caso statement singolo
    return parseStatement(itr);
}

// statement -> variable def | io stmt | cond stmt | loop stmt
Statement* Parser::parseStatement(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Overflow in token stream." };
    }

    // Ogni istruzione in LISP deve iniziare con '('
    if (itr->tag != Token::LP) {
        std::stringstream temp;
        temp << "Missing left parenthesis at token '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }

    auto nextItr = itr + 1;
    if (nextItr == end_) {
        throw SyntaxError{ "Overflow in token stream." };
    }

    // Chiamiamo il parser specifico in base alla keyword seguente
    switch (nextItr->tag) {
        case Token::SET:
            return parseSetStmt(itr);
        case Token::INPUT:
            return parseInputStmt(itr);
        case Token::PRINT:
            return parsePrintStmt(itr);
        case Token::IF:
            return parseIfStmt(itr);
        case Token::WHILE:
            return parseWhileStmt(itr);
        default: {
            std::stringstream temp;
            temp << "Unrecognized operator " << nextItr->word;
            throw SyntaxError{ temp.str() };
        }
    }
}

// variable id -> ID (identificatore alfabetico)
std::string Parser::parseVariableId(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Overflow in token stream." };
    }

    if (itr->tag != Token::ID) { //es test FAIL_WrongIdentifier2.txt abbiamo che Lexer valida DIV ma il Parser verifica che non è un token ID che si aspetta e syntaxerror
        std::stringstream temp;
        temp << "Expected a variable, got '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }

    std::string varName = itr->word;
    safe_next(itr); // consuma l'ID
    return varName;
}

// io stmt -> ( INPUT variable id )
InputStmt* Parser::parseInputStmt(std::vector<Token>::const_iterator& itr) {
    safe_next(itr); // consuma '('
    safe_next(itr); // consuma 'INPUT'

    std::string varName = parseVariableId(itr);

    if (itr == end_) {
        throw SyntaxError{ "Overflow in token stream." };
    }
    if (itr->tag != Token::RP) {
        std::stringstream temp;
        temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }
    safe_next(itr); // consuma ')'

    return new InputStmt{ varName };
}

// io stmt -> ( PRINT num expr )
PrintStmt* Parser::parsePrintStmt(std::vector<Token>::const_iterator& itr) {
    safe_next(itr); // consuma '('
    safe_next(itr); // consuma 'PRINT'

    NumExpr* expr = parseNumExpr(itr);

    if (itr == end_) {
        delete expr;
        throw SyntaxError{ "Overflow in token stream." };
    }
    if (itr->tag != Token::RP) {
        delete expr;
        std::stringstream temp;
        temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }
    safe_next(itr); // consuma ')'

    return new PrintStmt{ expr };
}

// variable def -> ( SET variable id num expr )
SetStmt* Parser::parseSetStmt(std::vector<Token>::const_iterator& itr) {
    safe_next(itr); // consuma '('
    safe_next(itr); // consuma 'SET'

    std::string varName = parseVariableId(itr);
    NumExpr* expr = parseNumExpr(itr);

    if (itr == end_) {
        delete expr;
        throw SyntaxError{ "Overflow in token stream." };
    }
    if (itr->tag != Token::RP) {
        delete expr;
        std::stringstream temp;
        temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }
    safe_next(itr); // consuma ')'

    return new SetStmt{ varName, expr };
}

//usando puntatori grezzi, siamo dovuti ricorrrere a blocchi try catch

//con smart ppointer in caso di exception durante il parsing  verrebbe invocato il distruttore di left liberando la memoria senza bisogno di try cach per pulizia manuale

// cond stmt -> ( IF bool expr stmt block stmt block )
IfStmt* Parser::parseIfStmt(std::vector<Token>::const_iterator& itr) {
    safe_next(itr); // consuma '('
    safe_next(itr); // consuma 'IF'

    BoolExpr* guard = parseBoolExpr(itr);


    //ogni sotto albero viene protetto da un try e catch così se c'è un errore liberiamo passo passo le parti già allocate (stessa cosa nel while)
    Statement* thenBlock = nullptr;
    try {
        thenBlock = parseStmtBlock(itr);
    } catch (...) {
        delete guard;
        throw;
    }

    Statement* elseBlock = nullptr;
    try {
        elseBlock = parseStmtBlock(itr);
    } catch (...) {
        delete guard;
        delete thenBlock;
        throw;
    }

    if (itr == end_) {
        delete guard;
        delete thenBlock;
        delete elseBlock;
        throw SyntaxError{ "Overflow in token stream." }; //gestiamo i syntax error separatamente
    }
    if (itr->tag != Token::RP) {
        delete guard;
        delete thenBlock;
        delete elseBlock;
        std::stringstream temp;
        temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }
    safe_next(itr); // consuma ')'

    return new IfStmt{ guard, thenBlock, elseBlock };
}

// loop stmt -> ( WHILE bool expr stmt block )
WhileStmt* Parser::parseWhileStmt(std::vector<Token>::const_iterator& itr) {
    safe_next(itr); // consuma '('
    safe_next(itr); // consuma 'WHILE'

    BoolExpr* guard = parseBoolExpr(itr);

    Statement* body = nullptr;
    try {
        body = parseStmtBlock(itr);
    } catch (...) {
        delete guard;
        throw;
    }

    if (itr == end_) {
        delete guard;
        delete body;
        throw SyntaxError{ "Overflow in token stream." };
    }
    if (itr->tag != Token::RP) {
        delete guard;
        delete body;
        std::stringstream temp;
        temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
        throw SyntaxError{ temp.str() };
    }
    safe_next(itr); // consuma ')'

    return new WhileStmt{ guard, body };
}
//per verificare la tipologia incontrata
// helper per verificare gli operatori aritmetici
inline bool isArithmeticOp(int tag) {
    return tag == Token::ADD || tag == Token::SUB || tag == Token::MUL || tag == Token::DIV;
}

// helper per verificare gli operatori relazionali (confronto tra numeri)
inline bool isRelOp(int tag) {
    return tag == Token::LT || tag == Token::GT || tag == Token::EQ;
}

// helper per verificare gli operatori booleani binari (tra booleani)
inline bool isBoolBinaryOp(int tag) {
    return tag == Token::AND || tag == Token::OR;
}

// num expr -> CONST | variable id | ( num op num expr num expr )
NumExpr* Parser::parseNumExpr(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Overflow in token stream." };
    }

    // Caso costante numerica
    if (itr->tag == Token::CONST) {
        int64_t val = std::stoll(itr->word); //per converitre stringa in int64_t
        safe_next(itr);
        return new NumberNode{ val };
    }

    // Caso identificatore di variabile
    if (itr->tag == Token::ID) {
        std::string varName = itr->word;
        safe_next(itr);
        return new VariableNode{ varName };
    }

    // Caso operazione aritmetica composta ( OP left right )
    if (itr->tag == Token::LP) {
        safe_next(itr); // consuma '('

        if (itr == end_) {
            throw SyntaxError{ "Overflow in token stream." };
        }
        if (!isArithmeticOp(itr->tag)) {
            std::stringstream temp;
            temp << "Unrecognized operator " << itr->word;
            throw SyntaxError{ temp.str() };
        }

        int opCode = itr->tag;
        safe_next(itr); // consuma l'operatore

        NumExpr* left = parseNumExpr(itr); //call ricorsiva operando sinistro

        NumExpr* right = nullptr;
        try {
            right = parseNumExpr(itr); //call ricorsiva operando destro

        } catch (...) {
            delete left;
            throw;
        }

        if (itr == end_) {
            delete left;
            delete right;
            throw SyntaxError{ "Overflow in token stream." };
        }
        if (itr->tag != Token::RP) {
            delete left;
            delete right;
            std::stringstream temp;
            temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
            throw SyntaxError{ temp.str() };
        }
        safe_next(itr); // consuma ')'

        return new BinaryNumOpNode{ opCode, left, right };
    }

    std::stringstream temp;
    temp << "Cannot parse expression at token " << itr->word;
    throw SyntaxError{ temp.str() };
}

// bool expr -> TRUE | FALSE | ( rel op num expr num expr ) | ( bool op bool expr bool expr ) | ( NOT bool expr )
BoolExpr* Parser::parseBoolExpr(std::vector<Token>::const_iterator& itr) {
    if (itr == end_) {
        throw SyntaxError{ "Overflow in token stream." };
    }

    // Costanti TRUE e FALSE
    if (itr->tag == Token::TRUE_TOK) {
        safe_next(itr);
        return new BoolConstNode{ true };
    }
    if (itr->tag == Token::FALSE_TOK) {
        safe_next(itr);
        return new BoolConstNode{ false };
    }

    // Espressioni composte tra parentesi
    if (itr->tag == Token::LP) {
        safe_next(itr); // consuma '('

        if (itr == end_) {
            throw SyntaxError{ "Overflow in token stream." };
        }

        // Caso NOT: ha un solo operando booleano
        if (itr->tag == Token::NOT) {
            safe_next(itr); // consuma 'NOT'
            BoolExpr* subExpr = parseBoolExpr(itr);

            if (itr == end_) {
                delete subExpr;
                throw SyntaxError{ "Overflow in token stream." };
            }
            if (itr->tag != Token::RP) {
                delete subExpr;
                std::stringstream temp;
                temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
                throw SyntaxError{ temp.str() };
            }
            safe_next(itr); // consuma ')'
            return new BoolNotNode{ subExpr };
        }

        // Caso operatori relazionali (LT, GT, EQ): operandi numerici
        if (isRelOp(itr->tag)) {
            int opCode = itr->tag;
            safe_next(itr); // consuma l'operatore

            NumExpr* left = parseNumExpr(itr);
            NumExpr* right = nullptr;
            try {
                right = parseNumExpr(itr);
            } catch (...) {
                delete left;
                throw;
            }

            if (itr == end_) {
                delete left;
                delete right;
                throw SyntaxError{ "Overflow in token stream." };
            }
            if (itr->tag != Token::RP) {
                delete left;
                delete right;
                std::stringstream temp;
                temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
                throw SyntaxError{ temp.str() };
            }
            safe_next(itr); // consuma ')'
            return new RelOpNode{ opCode, left, right };
        }

        // Caso operatori booleani binari (AND, OR): operandi booleani
        if (isBoolBinaryOp(itr->tag)) {
            int opCode = itr->tag;
            safe_next(itr); // consuma l'operatore

            BoolExpr* left = parseBoolExpr(itr);
            BoolExpr* right = nullptr;
            try {
                right = parseBoolExpr(itr);
            } catch (...) {
                delete left;
                throw;
            }

            if (itr == end_) {
                delete left;
                delete right;
                throw SyntaxError{ "Overflow in token stream." };
            }
            if (itr->tag != Token::RP) {
                delete left;
                delete right;
                std::stringstream temp;
                temp << "Missing right parenthesis at token '" << itr->tag << ";" << itr->word << "'";
                throw SyntaxError{ temp.str() };
            }
            safe_next(itr); // consuma ')'
            return new BoolBinaryOpNode{ opCode, left, right };
        }

        std::stringstream temp;
        temp << "Unrecognized operator " << itr->word;
        throw SyntaxError{ temp.str() };
    }

    std::stringstream temp;
    temp << "Cannot parse expression at token " << itr->word;
    throw SyntaxError{ temp.str() };
}
