#pragma once
#include <iostream>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class Favoritos {
private:
    Lista<Cancion> favoritas;

public:
    // Agregar a favoritos
    void agregar(Cancion c) {
        favoritas.agregarFinal(c);
    }

    // Eliminar de favoritos
    void eliminar(string titulo) {
        Lista<Cancion> nueva;

        favoritas.recorrer([&](Cancion c) {
            if (c.getTitulo() != titulo) {
                nueva.agregarFinal(c);
            }
            });

        favoritas = nueva;
    }

    // Mostrar favoritos
    void mostrar() {
        cout << "Canciones favoritas:" << endl;

        favoritas.recorrer([](Cancion c) {
            cout << "- " << c.getTitulo() << endl;
            });
    }

    // Verificar si está en favoritos
    bool esFavorita(string titulo) {
        bool encontrado = false;

        favoritas.recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo) {
                encontrado = true;
            }
            });

        return encontrado;
    }

    // Obtener lista
    Lista<Cancion>& getLista() {
        return favoritas;
    }
};