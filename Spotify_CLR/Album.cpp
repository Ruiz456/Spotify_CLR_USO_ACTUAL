#pragma once
#include "pch.h"
#include <iostream>
#include <string>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class Album {
private:
    string nombre;
    int anio;
    Lista<Cancion> canciones;
    
public:
    // Constructor
    Album(string nombre = "", int anio = 0) {
        this->nombre = nombre;
        this->anio = anio;
    }

    // Getters
    string getNombre() const { return nombre; }
    int getAnio() const { return anio; }

    // Agregar canción
    void agregarCancion(Cancion c) {
        canciones.agregarFinal(c);
    }

    // Mostrar álbum
    void mostrar() {
        cout << "Album: " << nombre << " (" << anio << ")" << endl;

        canciones.recorrer([](Cancion c) {
            cout << "- " << c.getTitulo() << endl;
            });
    }

    // Obtener lista
    Lista<Cancion>& getCanciones() {
        return canciones;
    }
};