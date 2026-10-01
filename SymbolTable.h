#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <cstdint>

#include "Exception.h"

// Tabella dei simboli per la memorizzazione delle variabili globali.
// Gestisce interi con segno a 64 bit (int64_t).
//usiamo pattern iterator, solo accesso diretto
class SymbolTable {
public:
    SymbolTable() = default;
    ~SymbolTable() = default;

    // Disabilitiamo copia e assegnamento per garantire l'unicita' della memoria
    SymbolTable(const SymbolTable& other) = delete;
    SymbolTable& operator=(const SymbolTable& other) = delete;

    // Recupera il valore di una variabile. Lancia EvaluationError se non esiste.
    int64_t getValue(std::string const& key) const {
        auto itr = map_.find(key); //Ricerca valore della chiave (anzi che map_[key] che non è const e creerebbe un valore di deafault(0) e una variabile non dichiarata non darebbe errore)
        if (itr == map_.end()) {
            throw EvaluationError{ "Undefined variable " + key };
        }
        return itr->second;
    }


    // Inserisce una nuova variabile o aggiorna il valore se gia' esistente.
    void setValue(std::string const& key, int64_t value) {
        map_[key] = value;
    }

    // Controlla se una variabile e' gia' presente nella tabella
    bool contains(std::string const& key) const {
        return map_.find(key) != map_.end();
    }

private:
    std::unordered_map<std::string, int64_t> map_;
};

#endif

//controllare corr