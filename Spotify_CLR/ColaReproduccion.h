#pragma once
#include <iostream>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class ColaReproduccion {
private:
    Lista<Cancion> cola;

public:
    // Encolar (agregar al final)
    void encolar(Cancion c) {
        cola.agregarFinal(c);
    }

    // Desencolar (eliminar el primero)
    void desencolar() {
        cola.eliminarInicial();
    }

    // Ver canción actual
    Cancion frente() {
        return cola.obtenerPos(0);
    }

    // Ver si está vacía
    bool esVacia() {
        return cola.esVacia();
    }

    // Mostrar cola
    void mostrarCola() {
        cout << "Cola de reproduccion:" << endl;

        cola.recorrer([](Cancion c) {
            cout << "- " << c.getTitulo() << endl;
            });
    }
};