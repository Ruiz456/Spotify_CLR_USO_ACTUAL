#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <direct.h>
#include "Lista.h"
#include "Usuario.h"
#include "Playlist.h"
#include "Favoritos.h"
#include "Cancion.h"

using namespace std;

class ColaUsuarios {
private:
    struct Nodo {
        Usuario dato;
        Nodo* sig;
        Nodo(Usuario d) : dato(d), sig(nullptr) {}
    };
    Nodo* frente;
    Nodo* fin;
    int tam;
public:
    ColaUsuarios() : frente(nullptr), fin(nullptr), tam(0) {}

    void encolar(Usuario u) {
        Nodo* n = new Nodo(u);
        if (!fin) frente = fin = n;
        else { fin->sig = n; fin = n; }
        tam++;
    }

    Usuario desencolar() {
        if (!frente) return Usuario();
        Nodo* aux = frente;
        Usuario d = aux->dato;
        frente = frente->sig;
        if (!frente) fin = nullptr;
        delete aux;
        tam--;
        return d;
    }

    bool esVacia() { return frente == nullptr; }
    int longitud() { return tam; }
};

class GestorUsuarios {
private:
    Lista<Usuario>  usuarios;
    ColaUsuarios    colaRegistro;
    string          carpeta = "usuarios_data/";
    Usuario* usuarioActual = nullptr;

    void crearCarpeta() {
        _mkdir(carpeta.c_str());
    }

    int generarId() {
        srand((unsigned int)time(nullptr));
        int id;
        bool existe;
        do {
            id = rand() % 9000 + 1000;
            existe = false;
            usuarios.recorrer([&](Usuario u) {
                if (u.getId() == id) existe = true;
                });
        } while (existe);
        return id;
    }

    void guardarUsuarios() {
        ofstream f(carpeta + "usuarios.txt");
        usuarios.recorrer([&](Usuario u) {
            f << u.getId() << "|"
                << u.getNombre() << "|"
                << u.getEmail() << "\n";
            });
        f.close();
    }

    void cargarUsuarios() {
        ifstream f(carpeta + "usuarios.txt");
        if (!f.is_open()) return;
        string linea;
        while (getline(f, linea)) {
            if (linea.empty()) continue;
            stringstream ss(linea);
            string idStr, nombre, email;
            getline(ss, idStr, '|');
            getline(ss, nombre, '|');
            getline(ss, email, '|');
            if (!idStr.empty() && !nombre.empty())
                usuarios.agregarFinal(Usuario(stoi(idStr), nombre, email));
        }
        f.close();
    }

    string obtenerArchivoUsuario(int id) {
        return carpeta + "usuario_" + to_string(id) + ".txt";
    }

    void guardarDatosUsuario(Usuario& u) {
        string archivo = obtenerArchivoUsuario(u.getId());
        ofstream f(archivo);

        f << "FAVORITOS_START\n";
        u.getFavoritos().getLista().recorrer([&](Cancion c) {
            f << "CANCION|" << c.getTitulo() << "|"
                << c.getArtista() << "|"
                << c.getGenero() << "|"
                << c.getDuracion() << "\n";
            });
        f << "FAVORITOS_END\n";

        f << "PLAYLISTS_START\n";
        u.getPlaylists().recorrer([&](Playlist p) {
            f << "PLAYLIST|" << p.getNombre() << "\n";
            p.getCanciones().recorrer([&](Cancion c) {
                f << "CANCION|" << c.getTitulo() << "|"
                    << c.getArtista() << "|"
                    << c.getGenero() << "|"
                    << c.getDuracion() << "\n";
                });
            });
        f << "PLAYLISTS_END\n";

        f.close();
    }

    void cargarDatosUsuario(Usuario& u) {
        string archivo = obtenerArchivoUsuario(u.getId());
        ifstream f(archivo);
        if (!f.is_open()) return;

        string linea;
        Playlist playlistActual;
        bool enFavoritos = false, enPlaylists = false;

        while (getline(f, linea)) {
            if (linea == "FAVORITOS_START") {
                enFavoritos = true;
                enPlaylists = false;
                continue;
            }
            else if (linea == "FAVORITOS_END") {
                enFavoritos = false;
                continue;
            }
            else if (linea == "PLAYLISTS_START") {
                enPlaylists = true;
                enFavoritos = false;
                continue;
            }
            else if (linea == "PLAYLISTS_END") {
                enPlaylists = false;
                continue;
            }

            stringstream ss(linea);
            string tipo;
            getline(ss, tipo, '|');

            if (tipo == "CANCION") {
                string titulo, artista, genero, duracionStr;
                getline(ss, titulo, '|');
                getline(ss, artista, '|');
                getline(ss, genero, '|');
                getline(ss, duracionStr, '|');
                float duracion = stof(duracionStr);
                Cancion c(titulo, genero, duracion, artista);

                if (enFavoritos) {
                    u.agregarFavorito(c);
                }
                else if (enPlaylists) {
                    if (playlistActual.getNombre() != "") {
                        playlistActual.agregarCancion(c);
                    }
                }
            }
            else if (tipo == "PLAYLIST" && enPlaylists) {
                string nombrePlaylist;
                getline(ss, nombrePlaylist);
                playlistActual = Playlist(nombrePlaylist);
                u.crearPlaylist(playlistActual);
            }
        }
        f.close();
    }

    void eliminarArchivoUsuario(int id) {
        string archivo = obtenerArchivoUsuario(id);
        remove(archivo.c_str());
    }

public:
    GestorUsuarios() {
        crearCarpeta();
        cargarUsuarios();
    }

    Usuario registrar(string nombre, string email) {
        int id = generarId();
        Usuario u(id, nombre, email);
        usuarios.agregarFinal(u);
        guardarUsuarios();
        guardarDatosUsuario(u);
        return u;
    }

    void encolarRegistro(string nombre, string email) {
        colaRegistro.encolar(Usuario(0, nombre, email));
    }

    void procesarCola() {
        while (!colaRegistro.esVacia()) {
            Usuario u = colaRegistro.desencolar();
            registrar(u.getNombre(), u.getEmail());
        }
    }

    void eliminar(int id) {
        eliminarArchivoUsuario(id);
        Lista<Usuario> nueva;
        usuarios.recorrer([&](Usuario u) {
            if (u.getId() != id) nueva.agregarFinal(u);
            });
        usuarios = nueva;
        guardarUsuarios();
        cout << "Usuario ID " << id << " eliminado (incluyendo sus datos)." << endl;
    }

    Usuario buscar(int id) {
        Usuario encontrado;
        usuarios.recorrer([&](Usuario u) {
            if (u.getId() == id) encontrado = u;
            });
        if (encontrado.getId() != 0) {
            cargarDatosUsuario(encontrado);
        }
        return encontrado;
    }

    Usuario buscarPorNombre(string nombre) {
        Usuario encontrado;
        usuarios.recorrer([&](Usuario u) {
            if (u.getNombre() == nombre) encontrado = u;
            });
        if (encontrado.getId() != 0) {
            cargarDatosUsuario(encontrado);
        }
        return encontrado;
    }

    bool existeNombre(string nombre) {
        bool existe = false;
        usuarios.recorrer([&](Usuario u) {
            if (u.getNombre() == nombre) existe = true;
            });
        return existe;
    }

    void guardarUsuarioActual() {
        if (usuarioActual != nullptr) {
            guardarDatosUsuario(*usuarioActual);
        }
    }

    void setUsuarioActual(Usuario* u) {
        usuarioActual = u;
    }

    Usuario* getUsuarioActual() {
        return usuarioActual;
    }

    void mostrar() {
        usuarios.recorrer([](Usuario u) {
            cout << "ID:" << u.getId()
                << "  " << u.getNombre()
                << "  " << u.getEmail() << endl;
            });
    }

    Lista<Usuario>& getLista() { return usuarios; }
};