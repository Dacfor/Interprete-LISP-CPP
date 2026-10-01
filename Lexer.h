#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Token.h"
#include "Exception.h"

// Classe funtore per l'analisi lessicale (scansione caratteri -> lista di Token)
class Lexer {
public:
	Lexer() = default;
	~Lexer() = default;

	// Evitiamo copie accidentali dell'oggetto durante la scansione
	Lexer(Lexer const&) = delete;
	Lexer& operator=(Lexer const&) = delete;

	// riceve lo stream del file e avvia la tokenizzazione
	std::vector<Token> operator()(std::ifstream& inputFile) {
		std::vector<Token> inputTokens;
		tokenizeInputFile(inputFile, inputTokens);
		return inputTokens;
	}

private:
	std::string tokenizeConstant(std::ifstream& inputFile, std::stringstream& temp);
	void tokenizeInputFile(std::ifstream& inputFile, std::vector<Token>& inputTokens);
};

#endif
