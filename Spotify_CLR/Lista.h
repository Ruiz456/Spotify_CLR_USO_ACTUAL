//Lista.h
#pragma once
#include <functional>
#include "Nodo.h"

using namespace std;
typedef unsigned int uint;

template <class T>
class Lista {
private:
    Nodo<T>* ini;
    uint lon;
    function<bool(T, T)> comparar;

public:
    // Constructor con lambda
    Lista(function<bool(T, T)> comp = nullptr) {
        ini = nullptr;
        lon = 0;

        if (comp == nullptr) {
            comparar = [](T a, T b) { return false; };
        }
        else {
            comparar = comp;
        }
    }

    // Basicos
    bool esVacia() { return lon == 0; }
    uint longitud() { return lon; }

    // Agregar al final
    void agregarFinal(T elem) {
        Nodo<T>* nuevo = new Nodo<T>(elem);

        if (!ini) {
            ini = nuevo;
        }
        else {
            Nodo<T>* aux = ini;
            while (aux->getSgte())
                aux = aux->getSgte();

            aux->setSgte(nuevo);
        }
        lon++;
    }

    // Eliminar inicial
    void eliminarInicial() {
        if (ini) {
            Nodo<T>* aux = ini;
            ini = ini->getSgte();
            delete aux;
            lon--;
        }
    }

    // Obtener por posicion
    T obtenerPos(uint pos) {
        Nodo<T>* aux = ini;

        for (uint i = 0; i < pos && aux; i++)
            aux = aux->getSgte();

        return aux ? aux->getElem() : T();
    }

    // Buscar elemento
    T buscar(T elem) {
        Nodo<T>* aux = ini;

        while (aux) {
            if (comparar(aux->getElem(), elem))
                return aux->getElem();

            aux = aux->getSgte();
        }
        return T();
    }

    // Recorrer con lambda
    template <typename Func>
    void recorrer(Func f) {
        Nodo<T>* aux = ini;

        while (aux) {
            f(aux->getElem());
            aux = aux->getSgte();
        }
    }

    // Eliminar en posicion
    void eliminarEn(uint pos) {
        if (pos >= lon) return;

        if (pos == 0) {
            eliminarInicial();
            return;
        }

        Nodo<T>* aux = ini;

        for (uint i = 0; i < pos - 1; i++)
            aux = aux->getSgte();

        Nodo<T>* borrar = aux->getSgte();
        aux->setSgte(borrar->getSgte());

        delete borrar;
        lon--;
    }

    // Insertar en posicion
    void insertarEn(T elem, uint pos)
    {
        if (pos == 0)
        {
            ini = new Nodo<T>(elem, ini);
            lon++;
            return;
        }

        if (pos >= lon)
        {
            agregarFinal(elem);
            return;
        }

        Nodo<T>* aux = ini;

        for (uint i = 0; i < pos - 1; i++)
            aux = aux->getSgte();

        Nodo<T>* nuevo = new Nodo<T>(elem, aux->getSgte());
        aux->setSgte(nuevo);

        lon++;
    }

    /*
     * Funcion: modificarPos()
     * Proposito: Reemplazar el elemento en la posicion indicada.
     *            Necesaria para Bubble Sort y Fisher-Yates in-place.
     * Parametros:
     *   - uint pos : posicion (0-based) a modificar
     *   - T    elem: nuevo valor a colocar
     * Retorno: void
     */
    void modificarPos(uint pos, T elem) {
        Nodo<T>* aux = ini;
        for (uint i = 0; i < pos && aux; i++)
            aux = aux->getSgte();
        if (aux) aux->setElem(elem);
    }
};