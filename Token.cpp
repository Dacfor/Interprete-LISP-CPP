#include "Token.h"

// Definizione della mappa delle keyword:
// associa la stringa del testo letto nel file sorgente con il rispettivo tag
const std::unordered_map<std::string, int> Token::keywords = {
	{ "BLOCK", Token::BLOCK },
	{ "SET",   Token::SET },
	{ "INPUT", Token::INPUT },
	{ "PRINT", Token::PRINT },
	{ "IF",    Token::IF },
	{ "WHILE", Token::WHILE },
	{ "ADD",   Token::ADD },
	{ "SUB",   Token::SUB },
	{ "MUL",   Token::MUL },
	{ "DIV",   Token::DIV },
	{ "LT",    Token::LT },
	{ "GT",    Token::GT },
	{ "EQ",    Token::EQ },
	{ "AND",   Token::AND },
	{ "OR",    Token::OR },
	{ "NOT",   Token::NOT },
	{ "TRUE",  Token::TRUE_TOK },
	{ "FALSE", Token::FALSE_TOK }
};


// se e un identificatore o un numero stampo il valore salvato nella word
// altrimenti stampo la parola chiave standard dall'array id2word questo evita duplicati
std::ostream& operator<<(std::ostream& os, const Token& t) {
	if (t.tag == Token::ID || t.tag == Token::CONST) {
		os << t.word;
	} else if (t.tag >= 0 && t.tag <= Token::FALSE_TOK) {
		os << Token::id2word[t.tag];
	} else {
		os << "UNKNOWN_TOKEN";
	}
	return os;
}
