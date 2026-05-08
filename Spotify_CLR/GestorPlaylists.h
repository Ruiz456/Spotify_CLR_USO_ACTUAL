#pragma once
#include <iostream>
#include "Usuario.h"
#include "Playlist.h"

using namespace std;

/*
 * Clase: GestorPlaylists
 * Proposito: Coleccion de metodos estaticos para administrar las playlists
 *            de un usuario. Encapsula la logica de creacion, eliminacion,
 *            busqueda y modificacion de playlists y sus canciones.
 *            Todos los metodos reciben el usuario por referencia para que
 *            los cambios persistan en el objeto original.
 */
class GestorPlaylists {
public:

    /*
     * Funcion: crearPlaylist()
     * Proposito: Crear una playlist vacia con el nombre dado y asociarla al usuario.
     * Parametros:
     *   - Usuario& u    : usuario propietario (por referencia para modificarlo)
     *   - string nombre : nombre de la nueva playlist
     * Retorno: void
     *
     * Ejemplo:
     *   GestorPlaylists::crearPlaylist(u, "Rock Clasico");
     */
    static void crearPlaylist(Usuario& u, string nombre) {
        Playlist nueva(nombre);
        u.crearPlaylist(nueva);
        cout << "Playlist creada: " << nombre << endl;
    }

    /*
     * Funcion: eliminarPlaylist()
     * Proposito: Eliminar la playlist de nombre exacto del usuario.
     *            Si no existe, no produce efecto (la lista queda intacta).
     * Parametros:
     *   - Usuario& u    : usuario propietario
     *   - string nombre : nombre de la playlist a eliminar
     * Retorno: void
     *
     * Ejemplo:
     *   GestorPlaylists::eliminarPlaylist(u, "Rock Clasico");
     */
    static void eliminarPlaylist(Usuario& u, string nombre) {
        u.eliminarPlaylist(nombre);
        cout << "Playlist eliminada: " << nombre << endl;
    }

    /*
     * Funcion: agregarCancionAPlaylist()
     * Proposito: Agregar una cancion a una playlist existente del usuario.
     *            Usa modificarPos() para escribir de vuelta el nodo modificado
     *            en la lista enlazada (fix: evita el error de modificar solo la copia).
     * Parametros:
     *   - Usuario& u            : propietario de la playlist
     *   - string nombrePlaylist : nombre exacto de la playlist destino
     *   - Cancion c             : cancion a insertar
     * Retorno: void
     *
     * Nota de implementacion:
     *   obtenerPos() retorna por valor (copia). Despues de modificar la copia,
     *   se usa modificarPos(i, p) para reemplazar el nodo original en la lista.
     *
     * Ejemplo:
     *   GestorPlaylists::agregarCancionAPlaylist(u, "Favoritos", cancion);
     */
    static void agregarCancionAPlaylist(Usuario& u, string nombrePlaylist, Cancion c) {
        Lista<Playlist>& pls = u.getPlaylists();
        for (uint i = 0; i < pls.longitud(); i++) {
            Playlist p = pls.obtenerPos(i);
            if (p.getNombre() == nombrePlaylist) {
                p.agregarCancion(c);
                pls.modificarPos(i, p);   // escribir la copia modificada de vuelta
                cout << "Cancion agregada a " << nombrePlaylist << endl;
                return;
            }
        }
        cout << "Playlist no encontrada." << endl;
    }

    /*
     * Funcion: eliminarCancionDePlaylist()
     * Proposito: Eliminar una cancion (por titulo) de una playlist del usuario.
     *            Usa modificarPos() para persistir el cambio en el nodo original.
     * Parametros:
     *   - Usuario& u            : propietario de la playlist
     *   - string nombrePlaylist : nombre de la playlist donde esta la cancion
     *   - string titulo         : titulo exacto de la cancion a eliminar
     * Retorno: void
     *
     * Ejemplo:
     *   GestorPlaylists::eliminarCancionDePlaylist(u, "Rock", "Bohemian Rhapsody");
     */
    static void eliminarCancionDePlaylist(Usuario& u, string nombrePlaylist, string titulo) {
        Lista<Playlist>& pls = u.getPlaylists();
        for (uint i = 0; i < pls.longitud(); i++) {
            Playlist p = pls.obtenerPos(i);
            if (p.getNombre() == nombrePlaylist) {
                p.eliminarCancion(titulo);
                pls.modificarPos(i, p);   // escribir la copia modificada de vuelta
                cout << "Cancion eliminada de " << nombrePlaylist << endl;
                return;
            }
        }
        cout << "Playlist no encontrada." << endl;
    }

    /*
     * Funcion: buscarCancionEnPlaylists()  [recursiva]
     * Proposito: Buscar en que playlist se encuentra una cancion por titulo.
     *            Recorre la lista de playlists de forma recursiva.
     * Parametros:
     *   - Lista<Playlist>& playlists: lista de playlists del usuario
     *   - string titulo             : titulo de la cancion a buscar
     *   - int indice                : posicion actual (parametro recursivo, defecto 0)
     * Retorno: string con el nombre de la playlist que contiene la cancion,
     *          o "" si no se encontro en ninguna.
     *
     * Ejemplo:
     *   string pl = GestorPlaylists::buscarCancionEnPlaylists(u.getPlaylists(), "Thriller");
     */
    static string buscarCancionEnPlaylists(Lista<Playlist>& playlists, string titulo, int indice = 0) {
        if (indice >= (int)playlists.longitud())
            return "";

        Playlist p = playlists.obtenerPos(indice);

        bool encontrada = false;
        p.getCanciones().recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrada = true;
            });

        if (encontrada)
            return p.getNombre();

        return buscarCancionEnPlaylists(playlists, titulo, indice + 1);
    }

    /*
     * Funcion: contarApariciones()  [recursiva]
     * Proposito: Contar en cuantas playlists aparece una cancion con el titulo dado.
     *            Recorre la lista recursivamente acumulando la cuenta.
     * Parametros:
     *   - Lista<Playlist>& playlists: lista de playlists del usuario
     *   - string titulo             : titulo de la cancion a contar
     *   - int indice                : posicion actual (parametro recursivo, defecto 0)
     * Retorno: int numero de playlists que contienen la cancion
     *
     * Ejemplo:
     *   int n = GestorPlaylists::contarApariciones(u.getPlaylists(), "Thriller");
     */
    static int contarApariciones(Lista<Playlist>& playlists, string titulo, int indice = 0) {
        if (indice >= (int)playlists.longitud())
            return 0;

        Playlist p = playlists.obtenerPos(indice);

        bool encontrada = false;
        p.getCanciones().recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrada = true;
            });

        int cuenta = encontrada ? 1 : 0;

        return cuenta + contarApariciones(playlists, titulo, indice + 1);
    }

    /*
     * Funcion: mostrarPlaylistsConMin()
     * Proposito: Imprimir solo las playlists que tengan al menos N canciones.
     *            Usa una lambda como criterio de filtrado al recorrer la lista.
     * Parametros:
     *   - Lista<Playlist>& playlists: lista de playlists del usuario
     *   - int minCanciones          : cantidad minima de canciones requerida
     * Retorno: void
     *
     * Ejemplo:
     *   GestorPlaylists::mostrarPlaylistsConMin(u.getPlaylists(), 5);
     */
    static void mostrarPlaylistsConMin(Lista<Playlist>& playlists, int minCanciones) {
        cout << "=== Playlists con al menos " << minCanciones << " canciones ===" << endl;
        bool hayAlguna = false;

        playlists.recorrer([&](Playlist p) {
            if ((int)p.getCanciones().longitud() >= minCanciones) {
                cout << "- " << p.getNombre()
                    << " (" << p.getCanciones().longitud() << " canciones)" << endl;
                hayAlguna = true;
            }
            });

        if (!hayAlguna)
            cout << "Ninguna playlist cumple el criterio." << endl;
    }
};
