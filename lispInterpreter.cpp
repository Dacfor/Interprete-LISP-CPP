#include <iostream>
#include <fstream>
#include <vector>

#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "EvaluationVisitor.h"
#include "Exception.h"

int main(int argc, char* argv[])
{
    //  Controllo degli argomenti da riga di comando
    if (argc < 2) {
        std::cerr << "No input file!" << std::endl;
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " <filename> " << std::endl;
        return EXIT_FAILURE;
    }

    //  Apertura del file sorgente da interpretare
    std::ifstream inputFile{ argv[1] };
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    // Puntatore alla radice dell'AST dichiarato fuori dal try per essere
    // visibile e deallocabile anche nei blocchi catch
    
    Program* program = nullptr;

    try {
        //1: Analisi lessicale (estrazione token dal file)
        //qualunque variabile dichiarata dentro il blocco try muore e viene distrutta all'uscita del blocco
        Lexer tokenize;
        std::vector<Token> inputTokens = tokenize(inputFile);
        inputFile.close();

        // Se il file sorgente e' vuoto, terminiamo con FAILURE (conforme a FAIL_EmptyProgram)
        if (inputTokens.empty()){
            return EXIT_FAILURE;
        }

        //2: Analisi sintattica (costruzione dell'albero sintattico)
        Parser parse;
        program = parse(inputTokens);


        //3: Valutazione semantica (esecuzione delle istruzioni)
        SymbolTable symTab;
        EvaluationVisitor eval{ symTab, std::cout, std::cin};
        if (program !=nullptr) {
            program->accept(eval);// Avvia double dispatch a partire dal root , visitor ricorsiva sul AST


        }

        // Deallocazione RAII dell'intero albero a fine esecuzione
        //alternativa smart pointer renderebbe superfluo il delete dentro i catch perchè il pointer verrebbe distrutto in automatico 
        delete program;
        program = nullptr;
       
    }

    catch (LexicalError const& e) {  //
        std::cout << "(ERROR in tokenizer: " << e.what() << " )" << std::endl; //Lexer
        delete program;
        return EXIT_FAILURE;

    }
   
    catch (SyntaxError const& e) {
        std::cout << "(ERROR in parser: " << e.what() << " )" << std::endl;
        delete program;
        return EXIT_FAILURE;

    }
    catch (EvaluationError const& e) {
        std::cout << "(ERROR in evaluator: " << e.what() << " )" << std::endl;
        delete program;
        return EXIT_FAILURE;
    }
    catch (std::exception const& e) { //ci garantisce che il programma non crashi se avviene un errore non previsto
        std::cout << "(ERROR: " << e.what() << " )" << std::endl;
        delete program;
        return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;
}
