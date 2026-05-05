#pragma once
#include <iostream>
#include <string>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class Playlist {
private:
    string nombre;
    Lista<Cancion> canciones;

public:
    // Constructor
    Playlist(string nombre = "") {
        this->nombre = nombre;
    }

    // Getter
    string getNombre() const { return nombre; }

    // Setter
    void setNombre(string n) { nombre = n; }

    // Agregar canción
    void agregarCancion(Cancion c) {
        canciones.agregarFinal(c);
    }

    // Mostrar canciones
    void mostrarCanciones() {
        cout << "Playlist: " << nombre << endl;

        canciones.recorrer([](Cancion c) {
            cout << "- " << c.getTitulo() << endl;
            });
    }

    // Buscar canción
    Cancion buscarCancion(string titulo) {
        Cancion encontrada;

        canciones.recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo) {
                encontrada = c;
            }
            });

        return encontrada;
    }

    // Eliminar canción
    void eliminarCancion(string titulo) {
        Lista<Cancion> nueva;

        canciones.recorrer([&](Cancion c) {
            if (c.getTitulo() != titulo) {
                nueva.agregarFinal(c);
            }
            });

        canciones = nueva;
    }

    // Obtener lista (para otras clases)
    Lista<Cancion>& getCanciones() {
        return canciones;
    }
};