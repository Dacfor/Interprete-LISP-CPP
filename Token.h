#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
#include <unordered_map>

// Struct perche e un puro contenitore di dati con membri pubblici
//rendendo accesso diretto ai membri
struct Token {

	// Codici numerici interi associati ad ogni token
	// L'uso di static evita di duplicare queste costanti per ogni istanza di Token,
	//cnstexpr garantisce che siano utilizzabili nei blocchi switch-case del Parser
	static constexpr int LP        = 0;  // Parentesi aperta (
	static constexpr int RP        = 1;  // Parentesi chiusa )
	static constexpr int BLOCK     = 2;  // BLOCK
	static constexpr int SET       = 3;  // SET
	static constexpr int INPUT     = 4;  // INPUT
	static constexpr int PRINT     = 5;  // PRINT
	static constexpr int IF        = 6;  // IF
	static constexpr int WHILE     = 7;  // WHILE
	static constexpr int ADD       = 8;  // ADD
	static constexpr int SUB       = 9;  // SUB
	static constexpr int MUL       = 10; // MUL
	static constexpr int DIV       = 11; // DIV
	static constexpr int LT        = 12; // LT
	static constexpr int GT        = 13; // GT
	static constexpr int EQ        = 14; // EQ
	static constexpr int AND       = 15; // AND
	static constexpr int OR        = 16; // OR
	static constexpr int NOT       = 17; // NOT
	static constexpr int TRUE_TOK  = 18; // TRUE
	static constexpr int FALSE_TOK = 19; // FALSE
	static constexpr int ID        = 20; // Identificatore di variabile
	static constexpr int CONST     = 21; // Costante numerica intera

	// Mappatura dei tag a stringhe leggibili per convertire istantaneamente un tag nella sua forma testuale
	static constexpr const char* id2word[]{
		"(", ")", "BLOCK", "SET", "INPUT", "PRINT", "IF", "WHILE",
		"ADD", "SUB", "MUL", "DIV", "LT", "GT", "EQ", "AND", "OR", "NOT",
		"TRUE", "FALSE", "ID", "CONST"
	};

	// Nomi dei tag per il debug
	static constexpr const char* tag2string[]{
		"LP", "RP", "BLOCK", "SET", "INPUT", "PRINT", "IF", "WHILE",
		"ADD", "SUB", "MUL", "DIV", "LT", "GT", "EQ", "AND", "OR", "NOT",
		"TRUE", "FALSE", "ID", "CONST"
	};

	// Tabella hash delle parole chiave riconosciute usato dal Lexer per verificare un token
	static const std::unordered_map<std::string, int> keywords;
	 //costruttori con list
	Token(int t, const char* w) : tag{ t }, word{ w } {}
	Token(int t, std::string w) : tag{ t }, word{ w } {}
	~Token() = default;
	Token(Token const&) = default;
	Token& operator=(Token const&) = default;
	//Dati token
	int tag;
	std::string word;
};

// Overload dell'operatore di output per i token
std::ostream& operator<<(std::ostream& os, const Token& t); //per unique e chaining

#endif
