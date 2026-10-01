#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <sstream>

#include "Token.h"
#include "Syntax.h"
#include "Exception.h"

// analizzatore sintattico a discesa ricorsiva
class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    // disabilitiamo copia e assegnamento per evitare copie accidentali
    Parser(Parser const&) = delete;
    Parser& operator=(Parser const&) = delete;

    // prende i token e costruisce l'albero
    Program* operator()(std::vector<Token> const& tokenStream) {
        if (tokenStream.empty()) {
            return nullptr;
        }

        auto itr = tokenStream.begin(); //itera fino ad arrivare alla fine del flusso di token
        end_ = tokenStream.end();

        Program* p = parseProgram(itr);

        // se dopo aver parsato il programma avanzano ancora token, e' un errore di sintassi
        if (itr != end_) {
            std::stringstream temp;
            temp << "Unexpected trailing tokens after program termination at '" << itr->word << "'";
            delete p; // puliamo l'albero allocato prima di lanciare l'eccezione, sennò la memoria rimarrebbe orfana nel heap
            throw SyntaxError{ temp.str() };
        }

        return p;
    }

private:
    std::vector<Token>::const_iterator end_;

    // invariante: ogni funzione riceve itr posizionato sul primo token della regola (CFG)
    // e lo lascia subito dopo l'ultimo token consumato & itr
    Program* parseProgram(std::vector<Token>::const_iterator& itr);
    Statement* parseStmtBlock(std::vector<Token>::const_iterator& itr);
    Statement* parseStatement(std::vector<Token>::const_iterator& itr);
    SetStmt* parseSetStmt(std::vector<Token>::const_iterator& itr);
    InputStmt* parseInputStmt(std::vector<Token>::const_iterator& itr);
    PrintStmt* parsePrintStmt(std::vector<Token>::const_iterator& itr);
    IfStmt* parseIfStmt(std::vector<Token>::const_iterator& itr);
    WhileStmt* parseWhileStmt(std::vector<Token>::const_iterator& itr);

    NumExpr* parseNumExpr(std::vector<Token>::const_iterator& itr);
    BoolExpr* parseBoolExpr(std::vector<Token>::const_iterator& itr);
    std::string parseVariableId(std::vector<Token>::const_iterator& itr);

    // avanza l'iteratore sequenzialment in sicurezza controllando di non sforare end_
    //verifichiamo che ci sia ancora un token da leggere
    void safe_next(std::vector<Token>::const_iterator& itr) { //avanza sempre lo stesso iteratore grazie a const_iterator& itr
        if (itr != end_) {
            ++itr;
        } else {
            throw SyntaxError{ "Overflow in token stream." }; //errore se finiscono i token prima del previsto
        }
    }

    // Nota: come alternativa per generalizzare ed evitare ripetizioni di controlli
    // nei vari metodi di parsing in Parser.cpp, si potrebbe usare un helper match():
    /*
    void match(std::vector<Token>::const_iterator& itr, int expectedTag, std::string const& errorMsg) {
        if (itr == end_) {
            throw SyntaxError{ "Overflow in token stream." };
        }
        if (itr->tag != expectedTag) {
            std::stringstream temp;
            temp << errorMsg << " at token '" << itr->tag << ";" << itr->word << "'";
            throw SyntaxError{ temp.str() };
        }
        safe_next(itr);
    }
    */
};

#endif

