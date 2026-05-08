#pragma once
#include <iostream>
#include <string>
#include <functional>
#include "Lista.h"
#include "Playlist.h"
#include "Favoritos.h"

using namespace std;

class Usuario {
private:
    int id;
    string nombre;
    string email;
    Lista<Playlist> playlists;

public:
    // Constructor
    Usuario(int id = 0, string nombre = "", string email = "") {
        this->id = id;
        this->nombre = nombre;
        this->email = email;
    }

    // Getters
    int getId() const { return id; }
    string getNombre() const { return nombre; }
    string getEmail() const { return email; }

    // Setters
    void setId(int i) { id = i; }
    void setNombre(string n) { nombre = n; }
    void setEmail(string e) { email = e; }

    // Crear playlist
    void crearPlaylist(Playlist p) {
        playlists.agregarFinal(p);
    }

    // Mostrar playlists
    void mostrarPlaylists() {
        if (playlists.esVacia()) {
            cout << "No tienes playlists aun." << endl;
            return;
        }
        cout << "=== Playlists de " << nombre << " ===" << endl;
        int i = 1;
        playlists.recorrer([&](Playlist p) {
            cout << i++ << ". " << p.getNombre()
                << " (" << p.getCanciones().longitud() << " canciones)" << endl;
            });
    }

    // Buscar playlist por nombre con lambda
    Playlist buscarPlaylist(string nombre) {
        Playlist resultado;
        playlists.recorrer([&](Playlist p) {
            if (p.getNombre() == nombre)
                resultado = p;
            });
        return resultado;
    }

    // Eliminar playlist con lambda
    void eliminarPlaylist(string nombre) {
        Lista<Playlist> nueva;
        playlists.recorrer([&](Playlist p) {
            if (p.getNombre() != nombre)
                nueva.agregarFinal(p);
            });
        playlists = nueva;
    }

    // Filtrar playlists con criterio lambda
    Lista<Playlist> filtrarPlaylists(function<bool(Playlist)> criterio) {
        Lista<Playlist> resultado;
        playlists.recorrer([&](Playlist p) {
            if (criterio(p))
                resultado.agregarFinal(p);
            });
        return resultado;
    }

    // Buscar cancion en todas las playlists con lambda
    string buscarCancionEnPlaylists(string titulo) {
        string playlistEncontrada = "";
        playlists.recorrer([&](Playlist p) {
            p.getCanciones().recorrer([&](Cancion c) {
                if (c.getTitulo() == titulo)
                    playlistEncontrada = p.getNombre();
                });
            });
        return playlistEncontrada;
    }

    // Contar total de canciones en todas las playlists con lambda
    int totalCanciones() {
        int total = 0;
        playlists.recorrer([&](Playlist p) {
            total += p.getCanciones().longitud();
            });
        return total;
    }

    // Obtener lista de playlists
    Lista<Playlist>& getPlaylists() {
        return playlists;
    }

    int totalPlaylists() {
        return playlists.longitud();
    }

    // Favoritos
    Favoritos favoritos;

    Favoritos& getFavoritos() { return favoritos; }
    void agregarFavorito(Cancion c) { favoritos.agregar(c); }
    void eliminarFavorito(string titulo) { favoritos.eliminar(titulo); }
    void mostrarFavoritos() { favoritos.mostrar(); }

};