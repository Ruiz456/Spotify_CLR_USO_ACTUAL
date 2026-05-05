#pragma once
#include <iostream>
#include "ColaReproduccion.h"
#include "HistorialReproduccion.h"

using namespace std;

class Reproductor {
private:
    ColaReproduccion cola;
    HistorialReproduccion historial;
    bool enPausa;

public:
    Reproductor() {
        enPausa = false;
    }

    // Agregar canción a la cola
    void agregarCancion(Cancion c) {
        cola.encolar(c);
    }

    // Reproducir
    void play() {
        if (cola.esVacia()) {
            cout << "No hay canciones en la cola" << endl;
            return;
        }

        enPausa = false;
        Cancion actual = cola.frente();

        cout << "Reproduciendo: " << actual.getTitulo() << endl;

        // Guardar en historial
        historial.agregar(actual);
    }

    // Pausar
    void pause() {
        if (!enPausa) {
            enPausa = true;
            cout << "Reproduccion pausada" << endl;
        }
    }

    // Siguiente canción
    void siguiente() {
        if (!cola.esVacia()) {
            cola.desencolar();

            if (!cola.esVacia()) {
                play();
            }
            else {
                cout << "Fin de la cola" << endl;
            }
        }
    }

    // Canción anterior
    void anterior() {
        Cancion anterior = historial.obtenerUltima();

        if (anterior.getTitulo() != "") {
            cout << "Reproduciendo anterior: " << anterior.getTitulo() << endl;
        }
    }

    // Mostrar cola
    void mostrarCola() {
        cola.mostrarCola();
    }

    //Dar acceso al historial desde Reproductor
    HistorialReproduccion& getHistorial() {
        return historial;
    }
};