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

    // Agregar canción a playlist específica
    static void agregarCancionAPlaylist(Usuario& u, string nombrePlaylist, Cancion c) {
        Playlist p = u.buscarPlaylist(nombrePlaylist);

        if (p.getNombre() != "") {
            p.agregarCancion(c);
            cout << "Cancion agregada a " << nombrePlaylist << endl;
        }
        else {
            cout << "Playlist no encontrada" << endl;
        }
    }
};