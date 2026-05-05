#pragma once
#include <iostream>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class HistorialReproduccion {
private:
    Lista<Cancion> historial;

public:
    // Agregar canción al historial
    void agregar(Cancion c) {
        historial.agregarFinal(c);
    }

    // Obtener última canción escuchada
    Cancion obtenerUltima() {
        if (historial.esVacia()) return Cancion();

        return historial.obtenerPos(historial.longitud() - 1);
    }

    // Mostrar historial
    void mostrar() {
        cout << "Historial de reproduccion:" << endl;

        historial.recorrer([](Cancion c) {
            cout << "- " << c.getTitulo() << endl;
            });
    }
};