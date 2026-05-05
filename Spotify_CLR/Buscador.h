#pragma once
//Buscador.h
#include <functional>
#include "Lista.h"
#include "Cancion.h"

using namespace System;
using namespace std;


class Buscador {
public:
    // Búsqueda genérica con criterio (lambda)
    static Lista<Cancion> buscar(Lista<Cancion>& lista, function<bool(Cancion)> criterio) {
        Lista<Cancion> resultado;

        lista.recorrer([&](Cancion c) {
            if (criterio(c)) {
                resultado.agregarFinal(c);
            }
            });

        return resultado;
    }

    // Buscar por título
    static Lista<Cancion> porTitulo(Lista<Cancion>& lista, string titulo) {
        return buscar(lista, [=](Cancion c) {
            return c.getTitulo() == titulo;
            });
    }

    // Buscar por artista
    static Lista<Cancion> porArtista(Lista<Cancion>& lista, string artista) {
        return buscar(lista, [=](Cancion c) {
            return c.getArtista() == artista;
            });
    }

    // Buscar por género
    static Lista<Cancion> porGenero(Lista<Cancion>& lista, string genero) {
        return buscar(lista, [=](Cancion c) {
            return c.getGenero() == genero;
            });
    }

    static void dibujarResultados(string canciones[], int cantidad, int x, int y)
    {
        for (int i = 0; i < cantidad; i++)
        {
            Console::SetCursorPosition(x, y + i);

            string linea = to_string(i + 1) + ". " + canciones[i];

            Console::Write("                                                    ");
            Console::SetCursorPosition(x, y + i);
            Console::Write(gcnew String(linea.c_str()));
        }
    }
};

