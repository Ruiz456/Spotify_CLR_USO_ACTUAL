#pragma once
#include <functional>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

class FiltroCanciones {
public:
    // Método genérico con lambda
    static Lista<Cancion> filtrar(Lista<Cancion>& lista, function<bool(Cancion)> criterio) {
        Lista<Cancion> resultado;

        lista.recorrer([&](Cancion c) {
            if (criterio(c)) {
                resultado.agregarFinal(c);
            }
            });

        return resultado;
    }

    // Filtro por género
    static Lista<Cancion> porGenero(Lista<Cancion>& lista, string genero) {
        return filtrar(lista, [=](Cancion c) {
            return c.getGenero() == genero;
            });
    }

    // Filtro por artista
    static Lista<Cancion> porArtista(Lista<Cancion>& lista, string artista) {
        return filtrar(lista, [=](Cancion c) {
            return c.getArtista() == artista;
            });
    }

    // Filtro por duración mayor a X
    static Lista<Cancion> duracionMayor(Lista<Cancion>& lista, float min) {
        return filtrar(lista, [=](Cancion c) {
            return c.getDuracion() > min;
            });
    }
};