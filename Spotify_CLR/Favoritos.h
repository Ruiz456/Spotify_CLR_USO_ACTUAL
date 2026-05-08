#pragma once
#include <iostream>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

/*
 * Clase: Favoritos
 * Proposito: Gestiona la lista de canciones favoritas de un usuario.
 *            Las canciones se almacenan en orden de insercion (FIFO estructural),
 *            pero se muestran en orden LIFO (la ultima agregada aparece primero).
 */
class Favoritos {
private:
    Lista<Cancion> favoritas;

public:
    /*
     * Funcion: agregar()
     * Proposito: Agregar una cancion al final de la lista de favoritos.
     *            No verifica duplicados; eso es responsabilidad del llamador.
     * Parametros:
     *   - Cancion c: cancion a marcar como favorita
     * Retorno: void
     *
     * Ejemplo:
     *   usuario.getFavoritos().agregar(cancion);
     */
    void agregar(Cancion c) {
        favoritas.agregarFinal(c);
    }

    /*
     * Funcion: eliminar()
     * Proposito: Quitar de favoritos la cancion con el titulo dado.
     *            Reconstruye la lista excluyendo las canciones con ese titulo.
     * Parametros:
     *   - string titulo: titulo exacto de la cancion a eliminar
     * Retorno: void
     *
     * Ejemplo:
     *   usuario.getFavoritos().eliminar("Thriller");
     */
    void eliminar(string titulo) {
        Lista<Cancion> nueva;

        favoritas.recorrer([&](Cancion c) {
            if (c.getTitulo() != titulo)
                nueva.agregarFinal(c);
            });

        favoritas = nueva;
    }

    /*
     * Funcion: mostrar()
     * Proposito: Imprimir en consola las canciones favoritas en orden LIFO
     *            (la ultima agregada se muestra primero).
     * Retorno: void
     *
     * Ejemplo:
     *   usuario.getFavoritos().mostrar();
     */
    void mostrar() {
        cout << "Canciones favoritas:" << endl;

        for (int i = favoritas.longitud() - 1; i >= 0; i--) {
            Cancion c = favoritas.obtenerPos(i);
            cout << "- " << c.getTitulo() << " | " << c.getArtista() << endl;
        }
    }

    /*
     * Funcion: esFavorita()
     * Proposito: Verificar si una cancion con el titulo dado esta en favoritos.
     *            Busca por titulo exacto (case-sensitive).
     * Parametros:
     *   - string titulo: titulo a buscar
     * Retorno: bool  true si la cancion esta en favoritos, false en caso contrario
     *
     * Ejemplo:
     *   if (usuario.getFavoritos().esFavorita("Thriller")) cout << "<3";
     */
    bool esFavorita(string titulo) {
        bool encontrado = false;

        favoritas.recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrado = true;
            });

        return encontrado;
    }

    /*
     * Funcion: getLista()
     * Proposito: Retornar una referencia mutable a la lista interna de favoritos.
     *            Permite acceso directo para recorrer, contar o guardar en disco.
     * Retorno: Lista<Cancion>& referencia a la lista interna
     *
     * Ejemplo:
     *   int total = usuario.getFavoritos().getLista().longitud();
     */
    Lista<Cancion>& getLista() {
        return favoritas;
    }
};
