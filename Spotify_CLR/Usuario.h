#pragma once
#include <iostream>
#include <string>
#include "Lista.h"
#include "Playlist.h"

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
        cout << "Playlists de " << nombre << ":" << endl;

        playlists.recorrer([](Playlist p) {
            cout << "- " << p.getNombre() << endl;
            });
    }

    // Buscar playlist por nombre (usa lambda indirectamente)
    Playlist buscarPlaylist(string nombre) {
        Playlist resultado;

        playlists.recorrer([&](Playlist p) {
            if (p.getNombre() == nombre) {
                resultado = p;
            }
            });

        return resultado;
    }

    // Eliminar playlist (básico)
    void eliminarPlaylist(string nombre) {
        Lista<Playlist> nueva;

        playlists.recorrer([&](Playlist p) {
            if (p.getNombre() != nombre) {
                nueva.agregarFinal(p);
            }
            });

        playlists = nueva;
    }
};