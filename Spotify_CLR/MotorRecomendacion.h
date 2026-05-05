#pragma once
#include <iostream>
#include "Lista.h"
#include "Cancion.h"
#include "HistorialReproduccion.h"

using namespace std;


class MotorRecomendacion {
public:
    // Recomienda basado en la última canción escuchada
    static Cancion recomendar(HistorialReproduccion& historial, Lista<Cancion>& biblioteca) {
        Cancion ultima = historial.obtenerUltima();

        if (ultima.getTitulo() == "") {
            cout << "No hay historial, recomendando primera cancion..." << endl;
            return biblioteca.obtenerPos(0);
        }

        // Buscar canción del mismo género
        Cancion recomendada;

        biblioteca.recorrer([&](Cancion c) {
            if (c.getGenero() == ultima.getGenero() &&
                c.getTitulo() != ultima.getTitulo()) {

                recomendada = c;
            }
            });

        if (recomendada.getTitulo() != "") {
            cout << "Recomendado por genero: " << recomendada.getTitulo() << endl;
            return recomendada;
        }

        // fallback (esto es un mecanismo de respaldo o método alternativo que se activa automáticamente
        // cuando un componente principal falla, garantizando la continuidad operativa
        // y minimizando interrupcionesa asi que no le muevan XDXDXD) 
        return biblioteca.obtenerPos(0);
    }
};