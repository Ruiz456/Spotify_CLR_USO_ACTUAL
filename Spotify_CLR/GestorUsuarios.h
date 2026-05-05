#pragma once
#include <iostream>
#include "Lista.h"
#include "Usuario.h"

using namespace std;

class GestorUsuarios {
private:
    Lista<Usuario> usuarios;

public:
    // Registrar usuario
    void registrar(Usuario u) {
        usuarios.agregarFinal(u);
        cout << "Usuario registrado: " << u.getNombre() << endl;
    }

    // Eliminar usuario por id
    void eliminar(int id) {
        Lista<Usuario> nueva;

        usuarios.recorrer([&](Usuario u) {
            if (u.getId() != id) {
                nueva.agregarFinal(u);
            }
            });

        usuarios = nueva;
        cout << "Usuario eliminado (ID): " << id << endl;
    }

    // Buscar usuario
    Usuario buscar(int id) {
        Usuario encontrado;

        usuarios.recorrer([&](Usuario u) {
            if (u.getId() == id) {
                encontrado = u;
            }
            });

        return encontrado;
    }

    // Mostrar usuarios
    void mostrar() {
        cout << "Usuarios registrados:" << endl;

        usuarios.recorrer([](Usuario u) {
            cout << "- " << u.getNombre() << endl;
            });
    }
};