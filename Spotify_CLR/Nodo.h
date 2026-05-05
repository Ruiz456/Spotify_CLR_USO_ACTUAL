#pragma once

template <class T>
class Nodo {
private:
    T elem;
    Nodo<T>* sgte;

public:
    // Constructor
    Nodo(T elem = T(), Nodo<T>* sgte = nullptr) {
        this->elem = elem;
        this->sgte = sgte;
    }

    // Getters
    T getElem() { return elem; }
    Nodo<T>* getSgte() { return sgte; }

    // Setters
    void setElem(T e) { elem = e; }
    void setSgte(Nodo<T>* s) { sgte = s; }
};