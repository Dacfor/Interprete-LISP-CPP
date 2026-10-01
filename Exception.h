#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

// Eccezione per errori riscontrati durante l'analisi lessicale (tokenizer)

//errore di caratteri o costanti nel file.
struct LexicalError : std::runtime_error { //eredito da std::runtime_error, fa parte di <stdexcept>, fornisce anche .what() che ritorna mess di errore e ci permette nel main do catturare eccezione con catch
	LexicalError(const char* msg) : std::runtime_error(msg) {}
	LexicalError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

// Eccezione per errori di violazione grammaticale nel parsing (meno parentesi del dovuto, violazione della grammatica, token non voluti) in Parser
struct SyntaxError : std::runtime_error {
	SyntaxError(const char* msg) : std::runtime_error(msg) {}
	SyntaxError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

// Eccezione per errori semantici a runtime dal visitor (variabili non dichiarate, divisione per zero)
struct EvaluationError : std::runtime_error {
	EvaluationError(const char* msg) : std::runtime_error(msg) {}
	EvaluationError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

#endif
