#pragma once
#include <iostream>
#include "Usuario.h"
#include "Playlist.h"

using namespace std;

class GestorPlaylists {
public:
    // Crear playlist para usuario
    static void crearPlaylist(Usuario& u, string nombre) {
        Playlist nueva(nombre);
        u.crearPlaylist(nueva);
        cout << "Playlist creada: " << nombre << endl;
    }

    // Eliminar playlist
    static void eliminarPlaylist(Usuario& u, string nombre) {
        u.eliminarPlaylist(nombre);
        cout << "Playlist eliminada: " << nombre << endl;
    }

    // Agregar cancion a playlist especifica
    static void agregarCancionAPlaylist(Usuario& u, string nombrePlaylist, Cancion c) {
        Playlist p = u.buscarPlaylist(nombrePlaylist);
        if (p.getNombre() != "") {
            p.agregarCancion(c);
            cout << "Cancion agregada a " << nombrePlaylist << endl;
        }
        else {
            cout << "Playlist no encontrada." << endl;
        }
    }

	// Buscar en que playlist esta una cancion (recursivo)
    static string buscarCancionEnPlaylists(Lista<Playlist>& playlists, string titulo, int indice = 0) {
        // Caso base: ya no hay mas playlists
        if (indice >= (int)playlists.longitud())
            return "";

        Playlist p = playlists.obtenerPos(indice);

        // Lambda para verificar si la cancion esta en esta playlist
        bool encontrada = false;
        p.getCanciones().recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrada = true;
            });

        if (encontrada)
            return p.getNombre();

        // Caso recursivo: buscar en la siguiente playlist
        return buscarCancionEnPlaylists(playlists, titulo, indice + 1);
    }

    // Contar cuantas playlists contienen una cancion (recursivo)
    static int contarApariciones(Lista<Playlist>& playlists, string titulo, int indice = 0) {
        if (indice >= (int)playlists.longitud())
            return 0;

        Playlist p = playlists.obtenerPos(indice);

        // Lambda para verificar
        bool encontrada = false;
        p.getCanciones().recorrer([&](Cancion c) {
            if (c.getTitulo() == titulo)
                encontrada = true;
            });

        int cuenta = encontrada ? 1 : 0;

        // Caso recursivo
        return cuenta + contarApariciones(playlists, titulo, indice + 1);
    }

    // Filtrar playlists con al menos N canciones (usa lambda)
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