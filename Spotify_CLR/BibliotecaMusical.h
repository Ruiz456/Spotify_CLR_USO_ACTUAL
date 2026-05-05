#pragma once
#include <iostream>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class BibliotecaMusical {
private:
    Lista<Cancion> canciones;

public:
    // Agregar canción
    void agregarCancion(Cancion c) {
        canciones.agregarFinal(c);
    }

    // Obtener lista completa (para filtros, buscador, etc.)
    Lista<Cancion>& getCanciones() {
        return canciones;
    }

    // Mostrar todas las canciones
    void mostrar() {
        cout << "Biblioteca Musical:" << endl;

        canciones.recorrer([](Cancion c) {
            cout << "- " << c.getTitulo()
                << " (" << c.getGenero() << ")" << endl;
            });
    }
};