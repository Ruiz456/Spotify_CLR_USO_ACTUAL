#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Lista.h"
#include "Cancion.h"

using namespace std;

/*
 * Clase: Playlist
 * Proposito: Representa una lista de reproduccion de canciones.
 *            Contiene el nombre de la playlist y una lista enlazada de canciones.
 *            Ofrece operaciones CRUD sobre canciones y algoritmos de ordenacion/mezcla.
 */
class Playlist {
private:
    string nombre;
    Lista<Cancion> canciones;

public:
    /*
     * Constructor de Playlist
     * Parametros:
     *   - string nombre: nombre de la playlist (por defecto cadena vacia)
     */
    Playlist(string nombre = "") {
        this->nombre = nombre;
    }

    /*
     * Funcion: getNombre()
     * Proposito: Retornar el nombre de la playlist.
     * Retorno: string con el nombre actual.
     */
    string getNombre() const { return nombre; }

    /*
     * Funcion: setNombre()
     * Proposito: Cambiar el nombre de la playlist.
     * Parametros:
     *   - string n: nuevo nombre
     */
    void setNombre(string n) { nombre = n; }

    /*
     * Funcion: agregarCancion()
     * Proposito: Insertar una cancion al final de la playlist.
     * Parametros:
     *   - Cancion c: cancion a agregar
     */
    void agregarCancion(Cancion c) {
        canciones.agregarFinal(c);
    }

    /*
     * Funcion: mostrarCanciones()
     * Proposito: Imprimir en consola todas las canciones de la playlist numeradas.
     *            Si esta vacia muestra un mensaje informativo.
     */
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

    /*
     * Funcion: shuffle()
     * Proposito: Reordenar aleatoriamente las canciones de la playlist.
     *            Delega en Playlist::mezclarFisherYates para operar in-place
     *            sobre la lista enlazada interna usando modificarPos().
     */
    void shuffle() {
        if (canciones.longitud() <= 1) {
            cout << "No hay suficientes canciones para mezclar." << endl;
            return;
        }
        mezclarFisherYates(&canciones);
    }

    /*
     * Funcion: buscarCancion()
     * Proposito: Encontrar una cancion por titulo exacto dentro de la playlist.
     * Parametros:
     *   - string titulo: titulo exacto a buscar
     * Retorno: Cancion encontrada (o Cancion() vacia si no existe)
     */
    Cancion buscarCancion(string titulo) {
        Cancion encontrada;
        canciones.recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrada = c;
            });
        return encontrada;
    }

    /*
     * Funcion: eliminarCancion()
     * Proposito: Eliminar la primera cancion cuyo titulo coincida exactamente.
     *            Reconstruye la lista sin el elemento eliminado.
     * Parametros:
     *   - string titulo: titulo de la cancion a eliminar
     */
    void eliminarCancion(string titulo) {
        Lista<Cancion> nueva;
        canciones.recorrer([&](Cancion c) {
            if (c.getTitulo() != titulo)
                nueva.agregarFinal(c);
            });
        canciones = nueva;
    }

    /*
     * Funcion: getCanciones()
     * Proposito: Retornar una referencia mutable a la lista interna de canciones.
     *            Permite acceso directo para algoritmos externos (sort, shuffle).
     * Retorno: Lista<Cancion>& referencia a la lista interna
     */
    Lista<Cancion>& getCanciones() {
        return canciones;
    }

    /*
     * Funcion: totalCanciones()
     * Proposito: Consultar cuantas canciones tiene la playlist.
     * Retorno: int numero de canciones
     */
    int totalCanciones() {
        return canciones.longitud();
    }

    // ============================================================
    // ALGORITMOS ESTATICOS (operan sobre cualquier Lista<Cancion>*)
    // ============================================================

    /*
     * Funcion estatica: mezclarFisherYates()
     * Proposito: Reordenar aleatoriamente una lista de canciones usando el
     *            algoritmo Fisher-Yates O(n). Usa modificarPos() para operar
     *            in-place sobre la lista enlazada, sin copiar a arreglo.
     * Parametros:
     *   - Lista<Cancion>* lista: lista a mezclar (se modifica directamente)
     * Retorno: void
     *
     * Algoritmo:
     *   Para i desde n-1 hasta 1:
     *     j = aleatorio entre 0 e i
     *     intercambiar lista[i] con lista[j]  (via modificarPos)
     *
     * Ejemplo:
     *   Playlist::mezclarFisherYates(gColaActual);
     */
    static void mezclarFisherYates(Lista<Cancion>* lista) {
        if (!lista || lista->esVacia()) return;

        srand((unsigned int)time(nullptr));
        int n = (int)lista->longitud();

        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);

            Cancion temp = lista->obtenerPos(i);
            lista->modificarPos(i, lista->obtenerPos(j));
            lista->modificarPos(j, temp);
        }
    }

    /*
     * Funcion estatica: ordenarBubbleSort()
     * Proposito: Ordenar alfabeticamente (A-Z) las canciones de una lista por titulo.
     *            Implementa Bubble Sort optimizado con bandera de corte temprano:
     *            si en un pase no hubo intercambios, la lista ya esta ordenada (O(n) mejor caso).
     * Parametros:
     *   - Lista<Cancion>* lista: lista a ordenar (se modifica directamente via modificarPos)
     * Retorno: void
     *
     * Ejemplo:
     *   Playlist::ordenarBubbleSort(&usuario->buscarPlaylist("Rock").getCanciones());
     */
    static void ordenarBubbleSort(Lista<Cancion>* lista) {
        if (!lista || lista->esVacia()) return;

        int n = (int)lista->longitud();

        for (int i = 0; i < n - 1; i++) {
            bool yaOrdenado = true;

            for (int j = 0; j < n - (i + 1); j++) {
                Cancion actual    = lista->obtenerPos(j);
                Cancion siguiente = lista->obtenerPos(j + 1);

                if (actual.getTitulo() > siguiente.getTitulo()) {
                    lista->modificarPos(j,     siguiente);
                    lista->modificarPos(j + 1, actual);
                    yaOrdenado = false;
                }
            }
            if (yaOrdenado) break;
        }
    }
};
