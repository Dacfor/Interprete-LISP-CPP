#include <sstream>
#include <cctype>

#include "Lexer.h"

// Metodo per estrarre le cifre successive di una costante numerica
std::string Lexer::tokenizeConstant(std::ifstream& inputFile, std::stringstream& temp) {
	char ch = inputFile.get();
	while (std::isdigit(static_cast<unsigned char>(ch))) {  //Il cast esplicito a unsigned char garantisce che il valore convertito a intero sia sempre positivo
		temp << ch;
		ch = inputFile.get(); 	//Per accorgerci se la costante numerica è finita dobbiamo leggere il carattere successivo es: ' ' o ')'
	}

	// Rimettiamo nello stream il carattere che non fa parte del numero, così lexer può rileggerlo ed elaborarlo come altro token
	inputFile.unget();
	return temp.str();
}

// Metodo principale per scansionare l'intero file ed estrarre i token
void Lexer::tokenizeInputFile(std::ifstream& inputFile, std::vector<Token>& inputTokens) {
	char ch{ };
	unsigned int rowCount{ 1 }; 
	ch = inputFile.get(); //Legge un singolo carattere dallo stream senza saltare gli spazi

	while (!inputFile.eof()) {
		if (std::isspace(static_cast<unsigned char>(ch))) {
			// Ignoriamo gli spazi ma aggiorniamo il contatore di riga
			if (ch == '\n') rowCount += 1;
		}
		else if (ch == '(') {
			inputTokens.push_back(Token{ Token::LP, Token::id2word[Token::LP] });
		}
		else if (ch == ')') {
			inputTokens.push_back(Token{ Token::RP, Token::id2word[Token::RP] });
		}
		else if (ch == '-') {
			// Gestione costante numerica negativa
			char nextCh = inputFile.peek(); //Controlla il carattere successivo senza estrarlo
			if (std::isdigit(static_cast<unsigned char>(nextCh))) {
				char firstDigit = inputFile.get();
				if (firstDigit == '0') {
					// Vietati numeri tipo -023 (lo zero non può essere seguito da altre cifre)
					char afterZero = inputFile.peek();
					if (std::isdigit(static_cast<unsigned char>(afterZero))) {
						std::stringstream temp; 
						temp << "Invalid numeric constant at line " << rowCount; 
						throw LexicalError{ temp.str() };
					}
					inputTokens.push_back(Token{ Token::CONST, "-0" });
				} else {
					std::stringstream temp;
					temp << ch << firstDigit;
					tokenizeConstant(inputFile, temp); //legge le cifre successive 
					inputTokens.push_back(Token{ Token::CONST, temp.str() });
				}
			} else {
				// Il carattere '-' da solo non è un operatore (la sottrazione è SUB)
				std::stringstream temp;
				temp << "Stray character " << ch << " in input at line " << rowCount;
				throw LexicalError{ temp.str() };
			}
		}
		else if (std::isdigit(static_cast<unsigned char>(ch))) {
			// Costante numerica positiva
			if (ch == '0') {
				// Lo zero singolo è valido, ma numeri tipo 007 o 05 sono vietati
				char nextCh = inputFile.peek(); //controllo che non ci siano altrre cifre dopo  lo 0
				if (std::isdigit(static_cast<unsigned char>(nextCh))) { //Riconosce una cifra numerica
					std::stringstream temp;
					temp << "Invalid numeric constant at line " << rowCount;
					throw LexicalError{ temp.str() };
				}
				inputTokens.push_back(Token{ Token::CONST, "0" });
			} else {
				// Numero positivo che inizia con cifra significativa 1..9
				std::stringstream temp;
				temp << ch;
				tokenizeConstant(inputFile, temp); //Questa funzione ci permette di ciclare le cofre successive finché non incontra un carattere non numerico
				inputTokens.push_back(Token{ Token::CONST, temp.str() }); //Emettiamo un token con stirnga 
			}
		}
		else if (std::isalpha(static_cast<unsigned char>(ch))) {
			// Scansione parola (keyword o identificatore: solo caratteri alfabetici)
			std::stringstream temp;
			temp << ch;
			do {
				ch = inputFile.get();
				if (std::isalpha(static_cast<unsigned char>(ch))) {
					temp << ch;
				}
			} while (std::isalpha(static_cast<unsigned char>(ch)));

			std::string word = temp.str();
			int tag = Token::ID; //Consideriamo la parole come identificatore di var

			// Verifichiamo se la parola è una keyword riservata
			auto it = Token::keywords.find(word);
			if (it != Token::keywords.end()) {
				tag = it->second;
			}
			inputTokens.push_back(Token{ tag, word }); //aggiunge a inputTokens il tag con la stringa associata

			// Rimettiamo nello stream il carattere che non appartiene alla parola es: ' '
			//Altrimenti andrebbe consumato e perso alterando la corretta sequenza dei successivi token.
			inputFile.unget();
		}
		else {
			// Qualsiasi altro carattere è un errore lessicale
			std::stringstream temp;
			temp << "Stray character " << ch << " in input at line " << rowCount;
			throw LexicalError{ temp.str() };
		}

		ch = inputFile.get(); //Next caratter per ciclo principale
	}
}
