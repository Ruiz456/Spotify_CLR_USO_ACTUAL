#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class Playlist {
private:
    string nombre;
    Lista<Cancion> canciones;

    // Fisher-Yates Shuffle
    void fisherYates(Cancion arr[], int n) {
        srand((unsigned int)time(nullptr));
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Cancion temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

public:
    // Constructor
    Playlist(string nombre = "") {
        this->nombre = nombre;
    }

    // Getter
    string getNombre() const { return nombre; }

    // Setter
    void setNombre(string n) { nombre = n; }

    // Agregar cancion
    void agregarCancion(Cancion c) {
        canciones.agregarFinal(c);
    }

    // Mostrar canciones
    void mostrarCanciones() {
        if (canciones.esVacia()) {
            cout << "La playlist esta vacia." << endl;
            return;
        }
        cout << "=== Playlist: " << nombre << " ===" << endl;
        int i = 1;
        canciones.recorrer([&](Cancion c) {
            cout << i++ << ". " << c.getTitulo()
                << " - " << c.getArtista() << endl;
            });
    }

    // Shuffle con Fisher-Yates
    void shuffle() {
        int n = canciones.longitud();
        if (n <= 1) {
            cout << "No hay suficientes canciones para mezclar." << endl;
            return;
        }

        // Pasar a arreglo
        Cancion* arr = new Cancion[n];
        for (int i = 0; i < n; i++)
            arr[i] = canciones.obtenerPos(i);

        // Aplicar Fisher-Yates
        fisherYates(arr, n);

        // Reconstruir lista
        Lista<Cancion> nueva;
        for (int i = 0; i < n; i++)
            nueva.agregarFinal(arr[i]);

        canciones = nueva;
        delete[] arr;

        cout << "=== Playlist mezclada: " << nombre << " ===" << endl;
        mostrarCanciones();
    }

    // Buscar cancion
    Cancion buscarCancion(string titulo) {
        Cancion encontrada;
        canciones.recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrada = c;
            });
        return encontrada;
    }

    // Eliminar cancion
    void eliminarCancion(string titulo) {
        Lista<Cancion> nueva;
        canciones.recorrer([&](Cancion c) {
            if (c.getTitulo() != titulo)
                nueva.agregarFinal(c);
            });
        canciones = nueva;
    }

    // Obtener lista
    Lista<Cancion>& getCanciones() {
        return canciones;
    }

    int totalCanciones() {
        return canciones.longitud();
    }
};