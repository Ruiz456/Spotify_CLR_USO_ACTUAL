// Spotify_CLR.cpp -> el main principal

#include "pch.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include "BibliotecaMusical.h"
#include "Reproductor.h"
#include "FiltroCanciones.h"
#include "MotorRecomendacion.h"
#include "Favoritos.h"
#include "Usuario.h"
#include "Buscador.h"
#include "GestorPlaylists.h"
#include "GestorUsuarios.h"

using namespace System;
using namespace System::Threading;
using namespace std;


///////////////////////////
// FUNCIONES DE JOSSELYN
////////////////////////////
void gotoxy(int x, int y) {
    Console::SetCursorPosition(x, y);
}


void Imprimir(int x, int y, string txt, ConsoleColor color = ConsoleColor::White) {
    gotoxy(x, y);
    Console::ForegroundColor = color;
    Console::Write(gcnew String(txt.c_str()));
    Console::ResetColor();
}

void LimpiarLinea(int x, int y, int longitud) {
    gotoxy(x, y);
    for (int i = 0; i < longitud; i++) {
        Console::Write(" ");
    }
}

void Pausar() {
    Imprimir(25, 22, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
    while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
}

struct OpcionMenu {
    string texto;
    int id;
};

int MenuTeclado(vector<OpcionMenu>& opciones, int startX, int startY, string titulo = "") {
    int seleccion = 0;
    ConsoleKeyInfo tecla;
    int totalOpciones = (int)opciones.size();

    if (titulo != "") {
        Imprimir(startX, startY - 2, titulo, ConsoleColor::Yellow);
    }

    for (int i = 0; i < totalOpciones; i++) {
        if (i == seleccion)
            Imprimir(startX, startY + i, "> " + opciones[i].texto + " <", ConsoleColor::Green);
        else
            Imprimir(startX, startY + i, "  " + opciones[i].texto + "   ", ConsoleColor::White);
    }

    while (true) {
        tecla = Console::ReadKey(true);

        if (tecla.Key == ConsoleKey::UpArrow) {
            Imprimir(startX, startY + seleccion, "  " + opciones[seleccion].texto + "   ", ConsoleColor::White);
            seleccion--;
            if (seleccion < 0) seleccion = totalOpciones - 1;
            Imprimir(startX, startY + seleccion, "> " + opciones[seleccion].texto + " <", ConsoleColor::Green);
        }
        else if (tecla.Key == ConsoleKey::DownArrow) {
            Imprimir(startX, startY + seleccion, "  " + opciones[seleccion].texto + "   ", ConsoleColor::White);
            seleccion++;
            if (seleccion >= totalOpciones) seleccion = 0;
            Imprimir(startX, startY + seleccion, "> " + opciones[seleccion].texto + " <", ConsoleColor::Green);
        }
        else if (tecla.Key == ConsoleKey::Enter) {
            return opciones[seleccion].id;
        }
    }
}

string InputTexto(int x, int y, string pregunta) {
    string respuesta;
    Imprimir(x, y, pregunta, ConsoleColor::White);
    gotoxy(x + (int)pregunta.length(), y);
    Console::ForegroundColor = ConsoleColor::White;
    Console::CursorVisible = true;
    cin.ignore();
    getline(cin, respuesta);
    Console::CursorVisible = false;
    Console::ResetColor();
    return respuesta;
}

string InputTextoSinIgnore(int x, int y, string pregunta) {
    string respuesta;
    Imprimir(x, y, pregunta, ConsoleColor::White);
    gotoxy(x + (int)pregunta.length(), y);
    Console::ForegroundColor = ConsoleColor::White;
    Console::CursorVisible = true;
    getline(cin, respuesta);
    Console::CursorVisible = false;
    Console::ResetColor();
    return respuesta;
}

int InputNumero(int x, int y, string pregunta) {
    string respuesta;
    Imprimir(x, y, pregunta, ConsoleColor::White);
    gotoxy(x + (int)pregunta.length(), y);
    Console::ForegroundColor = ConsoleColor::White;
    Console::CursorVisible = true;
    getline(cin, respuesta);
    Console::CursorVisible = false;
    Console::ResetColor();
    try { return stoi(respuesta); }
    catch (...) { return 0; }
}

//Pantalla de bienvenida
void MostrarBienvenida() {
    Console::Clear();
    Console::BackgroundColor = ConsoleColor::Black;
    system("color 0A");

    Imprimir(35, 5, "#####   ########", ConsoleColor::Green);
    Imprimir(35, 6, "#          ##   ", ConsoleColor::Green);
    Imprimir(35, 7, "#####      ##   ", ConsoleColor::Green);
    Imprimir(35, 8, "   #       ##   ", ConsoleColor::Green);
    Imprimir(35, 9, "#####      ##   ", ConsoleColor::Green);

    Imprimir(32, 11, "  Spotify de Temu", ConsoleColor::DarkGreen);
    Imprimir(30, 12, "  Tu musica, tu estilo", ConsoleColor::DarkGray);

    // Espera 4 segundos
    Thread::Sleep(4000);
}

//Menu de usuario inicio de sesion o registro
Usuario PantallaRegistro(GestorUsuarios& gestor) {
    Console::Clear();

    Imprimir(25, 2, "============================================", ConsoleColor::Green);
    Imprimir(30, 3, "SPOTIFY DE TEMU - ST", ConsoleColor::White);
    Imprimir(25, 4, "============================================", ConsoleColor::Green);

    vector<OpcionMenu> opciones;
    opciones.push_back({ "Soy usuario nuevo", 1 });
    opciones.push_back({ "Ya tengo cuenta",   2 });

    int eleccion = MenuTeclado(opciones, 33, 7, "Selecciona una opcion:");

    Console::Clear();

    // si es usuario nuevo
    if (eleccion == 1) {
        Imprimir(25, 2, "============================================", ConsoleColor::Green);
        Imprimir(32, 3, "CREAR CUENTA", ConsoleColor::White);
        Imprimir(25, 4, "============================================", ConsoleColor::Green);

        string nombre = InputTextoSinIgnore(28, 7, "Nombre: ");
        string email = InputTextoSinIgnore(28, 9, "Gmail:  ");

        Usuario u = gestor.registrar(nombre, email);

        Console::Clear();
        Imprimir(25, 2, "============================================", ConsoleColor::Green);
        Imprimir(30, 3, "CUENTA CREADA EXITOSAMENTE", ConsoleColor::White);
        Imprimir(25, 4, "============================================", ConsoleColor::Green);
        Imprimir(28, 6, "Nombre: " + u.getNombre(), ConsoleColor::White);
        Imprimir(28, 7, "Gmail:  " + u.getEmail(), ConsoleColor::White);
        Imprimir(28, 8, "ID:     " + to_string(u.getId()), ConsoleColor::Green);
        Imprimir(25, 10, "============================================", ConsoleColor::Green);
        Imprimir(28, 12, "GUARDA TU ID PARA INICIAR SESION", ConsoleColor::Yellow);
        Imprimir(28, 14, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);

        while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        return u;
    }

    // si es inicio de sesion
    Imprimir(25, 2, "============================================", ConsoleColor::Green);
    Imprimir(32, 3, "INICIAR SESION", ConsoleColor::White);
    Imprimir(25, 4, "============================================", ConsoleColor::Green);

    int idIngresado = InputNumero(28, 7, "Tu ID: ");

    Usuario u = gestor.buscar(idIngresado);

    if (u.getNombre().empty()) {
        Imprimir(28, 9, "ID no encontrado.", ConsoleColor::Red);
        Imprimir(28, 11, "Presiona ENTER para salir...", ConsoleColor::DarkGray);
        while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        return Usuario(0, "Desconocido", "");
    }

    Console::Clear();
    Imprimir(25, 2, "============================================", ConsoleColor::Green);
    Imprimir(30, 3, "BIENVENIDO DE VUELTA", ConsoleColor::White);
    Imprimir(25, 4, "============================================", ConsoleColor::Green);
    Imprimir(28, 6, "Hola, " + u.getNombre(), ConsoleColor::White);
    Imprimir(28, 7, "ID:   " + to_string(u.getId()), ConsoleColor::Green);
    Imprimir(28, 8, "Mail: " + u.getEmail(), ConsoleColor::DarkGray);
    Imprimir(25, 10, "============================================", ConsoleColor::Green);
    Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);

    while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
    return u;
}

// si deseas eliminar tu cuenta
void eliminarCuenta(GestorUsuarios& gestor, Usuario& u) {
    Console::Clear();
    Imprimir(25, 2, "============================================", ConsoleColor::Red);
    Imprimir(32, 3, "ELIMINAR CUENTA", ConsoleColor::White);
    Imprimir(25, 4, "============================================", ConsoleColor::Red);
    Imprimir(25, 6, "Esta seguro? Se eliminaran TODOS sus datos.", ConsoleColor::Yellow);
    Imprimir(28, 8, "ID: " + to_string(u.getId()), ConsoleColor::White);
    Imprimir(28, 9, "Nombre: " + u.getNombre(), ConsoleColor::White);

    vector<OpcionMenu> opciones;
    opciones.push_back({ "SI, eliminar mi cuenta", 1 });
    opciones.push_back({ "NO, cancelar",           2 });

    int eleccion = MenuTeclado(opciones, 28, 11);

    if (eleccion == 1) {
        gestor.eliminar(u.getId());
        Console::Clear();
        Imprimir(30, 8, "Cuenta eliminada.", ConsoleColor::Green);
        Imprimir(25, 10, "Presiona ENTER para salir...", ConsoleColor::DarkGray);
        while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        exit(0);
    }
}

void VerCancionesPlaylist(GestorUsuarios& gestor, Usuario& u, string nombrePlaylist) {
    bool salir = false;

    do {
        Console::Clear();
        Imprimir(25, 1, "============================================", ConsoleColor::Green);
        Imprimir(28, 2, "PLAYLIST: " + nombrePlaylist, ConsoleColor::White);
        Imprimir(25, 3, "============================================", ConsoleColor::Green);

        Playlist p = u.buscarPlaylist(nombrePlaylist);
        if (p.getNombre() == "") {
            Imprimir(28, 5, "Playlist no encontrada.", ConsoleColor::Red);
            Pausar();
            return;
        }

        int y = 5;
        int totalC = p.getCanciones().longitud();

        if (totalC == 0) {
            Imprimir(28, y, "Esta playlist no tiene canciones.", ConsoleColor::DarkGray);
            y += 2;
        }
        else {
            Imprimir(28, y, "Canciones:", ConsoleColor::Yellow);
            y++;
            int idx = 1;
            p.getCanciones().recorrer([&](Cancion c) {
                string linea = "  " + to_string(idx) + ". " + c.getTitulo() + " - " + c.getArtista();
                Imprimir(28, y, linea, ConsoleColor::White);
                y++;
                idx++;
                });
            y++;
        }

        Imprimir(25, y, "============================================", ConsoleColor::Green);
        y += 2;

        vector<OpcionMenu> opciones;
        opciones.push_back({ "Agregar cancion",   1 });
        opciones.push_back({ "Eliminar cancion",  2 });
        opciones.push_back({ "Mezclar (Shuffle)", 3 });
        opciones.push_back({ "Volver",            4 });

        int accion = MenuTeclado(opciones, 28, y, "Que deseas hacer?");

        switch (accion) {
        case 1: {
            // Agregar cancion
            Console::Clear();
            Imprimir(25, 1, "============================================", ConsoleColor::Green);
            Imprimir(28, 2, "AGREGAR CANCION A: " + nombrePlaylist, ConsoleColor::White);
            Imprimir(25, 3, "============================================", ConsoleColor::Green);

            string titulo = InputTextoSinIgnore(28, 6, "Titulo:         ");
            string artista = InputTextoSinIgnore(28, 8, "Artista:        ");
            string genero = InputTextoSinIgnore(28, 10, "Genero:         ");
            string durStr = InputTextoSinIgnore(28, 12, "Duracion (min): ");

            float duracion = 0;
            try { duracion = stof(durStr); }
            catch (...) { duracion = 0; }

            Cancion nueva(titulo, genero, duracion, artista);

            Lista<Playlist>& pls = u.getPlaylists();
            int n = pls.longitud();
            for (int i = 0; i < n; i++) {
                Playlist pRef = pls.obtenerPos(i);
                if (pRef.getNombre() == nombrePlaylist) {
                    pRef.agregarCancion(nueva);
                    Lista<Playlist> nueva_lista;
                    for (int j = 0; j < n; j++) {
                        if (j == i) nueva_lista.agregarFinal(pRef);
                        else        nueva_lista.agregarFinal(pls.obtenerPos(j));
                    }
                    u.getPlaylists() = nueva_lista;
                    break;
                }
            }

            gestor.guardarUsuarioActual();
            Imprimir(28, 14, "Cancion agregada!", ConsoleColor::Green);
            Imprimir(28, 16, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            break;
        }
        case 2: {
            // Eliminar cancion
            if (totalC == 0) {
                Console::Clear();
                Imprimir(28, 10, "No hay canciones para eliminar.", ConsoleColor::Red);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
                break;
            }

            Console::Clear();
            Imprimir(25, 1, "============================================", ConsoleColor::Green);
            Imprimir(28, 2, "ELIMINAR CANCION DE: " + nombrePlaylist, ConsoleColor::White);
            Imprimir(25, 3, "============================================", ConsoleColor::Green);

            Playlist pActual = u.buscarPlaylist(nombrePlaylist);
            vector<OpcionMenu> opsCanciones;
            int idx2 = 0;
            pActual.getCanciones().recorrer([&](Cancion c) {
                opsCanciones.push_back({ c.getTitulo() + " - " + c.getArtista(), idx2 });
                idx2++;
                });
            opsCanciones.push_back({ "Cancelar", -1 });

            int elegida = MenuTeclado(opsCanciones, 28, 6, "Selecciona la cancion a eliminar:");

            if (elegida >= 0) {
                string tituloEliminar = pActual.getCanciones().obtenerPos(elegida).getTitulo();

                Lista<Playlist>& pls = u.getPlaylists();
                int n = pls.longitud();
                for (int i = 0; i < n; i++) {
                    Playlist pRef = pls.obtenerPos(i);
                    if (pRef.getNombre() == nombrePlaylist) {
                        pRef.eliminarCancion(tituloEliminar);
                        Lista<Playlist> nueva_lista;
                        for (int j = 0; j < n; j++) {
                            if (j == i) nueva_lista.agregarFinal(pRef);
                            else        nueva_lista.agregarFinal(pls.obtenerPos(j));
                        }
                        u.getPlaylists() = nueva_lista;
                        break;
                    }
                }

                gestor.guardarUsuarioActual();
                Console::Clear();
                Imprimir(28, 10, "Cancion eliminada!", ConsoleColor::Green);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            }
            break;
        }
        case 3: {
            // Shuffle
            Playlist pActual = u.buscarPlaylist(nombrePlaylist);
            if (pActual.getCanciones().longitud() <= 1) {
                Console::Clear();
                Imprimir(28, 10, "No hay suficientes canciones para mezclar.", ConsoleColor::Red);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
                break;
            }
            pActual.shuffle();

            Lista<Playlist>& pls = u.getPlaylists();
            int n = pls.longitud();
            for (int i = 0; i < n; i++) {
                if (pls.obtenerPos(i).getNombre() == nombrePlaylist) {
                    Lista<Playlist> nueva_lista;
                    for (int j = 0; j < n; j++) {
                        if (j == i) nueva_lista.agregarFinal(pActual);
                        else        nueva_lista.agregarFinal(pls.obtenerPos(j));
                    }
                    u.getPlaylists() = nueva_lista;
                    break;
                }
            }
            gestor.guardarUsuarioActual();
            Console::Clear();
            Imprimir(28, 10, "Playlist mezclada!", ConsoleColor::Green);
            Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            break;
        }
        case 4:
            salir = true;
            break;
        }

    } while (!salir);
}

void PantallaPlaylists(GestorUsuarios& gestor, Usuario& u) {
    bool salir = false;

    do {
        Console::Clear();
        Imprimir(25, 1, "============================================", ConsoleColor::Green);
        Imprimir(32, 2, "MIS PLAYLISTS", ConsoleColor::White);
        Imprimir(25, 3, "============================================", ConsoleColor::Green);

        int y = 5;
        int totalPL = u.totalPlaylists();

        if (totalPL == 0) {
            Imprimir(28, y, "No tienes playlists aun.", ConsoleColor::DarkGray);
            y += 2;
        }
        else {
            Imprimir(28, y, "Tus playlists:", ConsoleColor::Yellow);
            y++;
            u.getPlaylists().recorrer([&](Playlist p) {
                string info = "  - " + p.getNombre()
                    + " (" + to_string(p.getCanciones().longitud()) + " canciones)";
                Imprimir(28, y, info, ConsoleColor::White);
                y++;
                });
            y++;
        }

        Imprimir(25, y, "============================================", ConsoleColor::Green);
        y += 2;

        vector<OpcionMenu> opciones;
        opciones.push_back({ "Crear playlist",    1 });
        opciones.push_back({ "Ver playlist",      2 });
        opciones.push_back({ "Eliminar playlist", 3 });
        opciones.push_back({ "Volver",            4 });

        int accion = MenuTeclado(opciones, 28, y, "Que deseas hacer?");

        switch (accion) {
        case 1: {
            // Crear playlist
            Console::Clear();
            Imprimir(25, 2, "============================================", ConsoleColor::Green);
            Imprimir(32, 3, "CREAR PLAYLIST", ConsoleColor::White);
            Imprimir(25, 4, "============================================", ConsoleColor::Green);

            string nombre = InputTextoSinIgnore(28, 7, "Nombre de la playlist: ");
            GestorPlaylists::crearPlaylist(u, nombre);
            gestor.guardarUsuarioActual();
            Imprimir(28, 10, "Playlist creada!", ConsoleColor::Green);
            Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            break;
        }
        case 2: {
            // Ver playlist
            if (totalPL == 0) {
                Console::Clear();
                Imprimir(28, 10, "No tienes playlists.", ConsoleColor::Red);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
                break;
            }

            Console::Clear();
            Imprimir(25, 2, "============================================", ConsoleColor::Green);
            Imprimir(32, 3, "VER PLAYLIST", ConsoleColor::White);
            Imprimir(25, 4, "============================================", ConsoleColor::Green);

            vector<OpcionMenu> opsPL;
            int idx = 0;
            u.getPlaylists().recorrer([&](Playlist p) {
                opsPL.push_back({ p.getNombre(), idx });
                idx++;
                });
            opsPL.push_back({ "Cancelar", -1 });

            int elegida = MenuTeclado(opsPL, 28, 7, "Selecciona la playlist:");

            if (elegida >= 0) {
                string nombreElegido = u.getPlaylists().obtenerPos(elegida).getNombre();
                VerCancionesPlaylist(gestor, u, nombreElegido);
            }
            break;
        }
        case 3: {
            // Eliminar playlist
            if (totalPL == 0) {
                Console::Clear();
                Imprimir(28, 10, "No tienes playlists.", ConsoleColor::Red);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
                break;
            }

            Console::Clear();
            Imprimir(25, 2, "============================================", ConsoleColor::Green);
            Imprimir(32, 3, "ELIMINAR PLAYLIST", ConsoleColor::White);
            Imprimir(25, 4, "============================================", ConsoleColor::Green);

            vector<OpcionMenu> opsPL;
            int idx = 0;
            u.getPlaylists().recorrer([&](Playlist p) {
                opsPL.push_back({ p.getNombre(), idx });
                idx++;
                });
            opsPL.push_back({ "Cancelar", -1 });

            int elegida = MenuTeclado(opsPL, 28, 7, "Selecciona la playlist a eliminar:");

            if (elegida >= 0) {
                string nombreElegido = u.getPlaylists().obtenerPos(elegida).getNombre();
                GestorPlaylists::eliminarPlaylist(u, nombreElegido);
                gestor.guardarUsuarioActual();
                Console::Clear();
                Imprimir(28, 10, "Playlist eliminada!", ConsoleColor::Green);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            }
            break;
        }
        case 4:
            salir = true;
            break;
        } // fin switch

    } while (!salir);
}

void PantallaFavoritos(GestorUsuarios& gestor, Usuario& u) {
    while (true) {
        Console::Clear();
        Imprimir(25, 1, "============================================", ConsoleColor::Green);
        Imprimir(32, 2, "MIS FAVORITOS", ConsoleColor::White);
        Imprimir(25, 3, "============================================", ConsoleColor::Green);

        int y = 5;
        int totalFav = u.getFavoritos().getLista().longitud();

        if (totalFav == 0) {
            Imprimir(28, y, "No tienes favoritos aun.", ConsoleColor::DarkGray);
            y += 2;
        }
        else {
            for (int i = totalFav - 1; i >= 0; i--) {
                Cancion c = u.getFavoritos().getLista().obtenerPos(i);
                string linea = "  - " + c.getTitulo() + "  |  " + c.getArtista();
                Imprimir(28, y, linea, ConsoleColor::White);
                y++;
            }
            y++;
        }

        Imprimir(25, y, "============================================", ConsoleColor::Green);
        y += 2;

        vector<OpcionMenu> opciones;
        opciones.push_back({ "Eliminar cancion de favoritos", 1 });
        opciones.push_back({ "Volver",                        2 });

        int accion = MenuTeclado(opciones, 28, y);

        if (accion == 1) {
            if (totalFav == 0) {
                Console::Clear();
                Imprimir(28, 10, "No hay favoritos para eliminar.", ConsoleColor::Red);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
                continue;
            }

            Console::Clear();
            Imprimir(25, 2, "============================================", ConsoleColor::Green);
            Imprimir(32, 3, "ELIMINAR FAVORITO", ConsoleColor::White);
            Imprimir(25, 4, "============================================", ConsoleColor::Green);

            // Menu de canciones favoritas con flechas
            vector<OpcionMenu> opsFav;
            for (int i = totalFav - 1; i >= 0; i--) {
                Cancion c = u.getFavoritos().getLista().obtenerPos(i);
                opsFav.push_back({ c.getTitulo() + "  |  " + c.getArtista(), i });
            }
            opsFav.push_back({ "Cancelar", -1 });

            int elegida = MenuTeclado(opsFav, 28, 7, "Selecciona la cancion a eliminar:");

            if (elegida >= 0) {
                string tituloElim = u.getFavoritos().getLista().obtenerPos(elegida).getTitulo();
                u.eliminarFavorito(tituloElim);
                gestor.guardarUsuarioActual();
                Console::Clear();
                Imprimir(28, 10, "Eliminada de favoritos!", ConsoleColor::Green);
                Imprimir(28, 12, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
                while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            }
        }
        else if (accion == 2) {
            return;
        }
    }
}

void MenuPrincipal(GestorUsuarios& gestor, Usuario& usuarioActual) {
    vector<OpcionMenu> opciones;
    opciones.push_back({ "Inicio",          10 });
    opciones.push_back({ "Buscar",          20 });
    opciones.push_back({ "Mis Playlists",   30 });
    opciones.push_back({ "Mis Favoritos",   40 });
    opciones.push_back({ "Eliminar Cuenta", 50 });
    opciones.push_back({ "Salir",           99 });

    while (true) {
        Console::Clear();

        Imprimir(2, 0, "ST", ConsoleColor::Green);
        Imprimir(5, 0, " | ", ConsoleColor::DarkGreen);
        Imprimir(8, 0, usuarioActual.getNombre() + " (ID: " + to_string(usuarioActual.getId()) + ")", ConsoleColor::White);
        Imprimir(2, 1, "------------------------------------------------------------", ConsoleColor::DarkGreen);

        int accion = MenuTeclado(opciones, 33, 5, "MENU PRINCIPAL");

        if (accion == 10) {
            Console::Clear();
            Imprimir(25, 2, "============================================", ConsoleColor::Green);
            Imprimir(30, 3, "INICIO", ConsoleColor::White);
            Imprimir(25, 4, "============================================", ConsoleColor::Green);
            Imprimir(28, 7, "Bienvenido a ST - Spotify de Temu", ConsoleColor::Green);
            Imprimir(28, 9, "Tienes " + to_string(usuarioActual.totalPlaylists()) + " playlists", ConsoleColor::White);
            Imprimir(28, 10, "Tienes " + to_string(usuarioActual.getFavoritos().getLista().longitud()) + " favoritos", ConsoleColor::White);
            Imprimir(25, 12, "============================================", ConsoleColor::Green);
            Imprimir(28, 14, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        }
        else if (accion == 20) {
            Console::Clear();
            Imprimir(25, 2, "============================================", ConsoleColor::Green);
            Imprimir(30, 3, "BUSCADOR", ConsoleColor::White);
            Imprimir(25, 4, "============================================", ConsoleColor::Green);
            Imprimir(28, 7, "En desarrollo...", ConsoleColor::Yellow);
            Imprimir(28, 9, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        }
        else if (accion == 30) {
            PantallaPlaylists(gestor, usuarioActual);
        }
        else if (accion == 40) {
            PantallaFavoritos(gestor, usuarioActual);
        }
        else if (accion == 50) {
            eliminarCuenta(gestor, usuarioActual);
        }
        else if (accion == 99) {
            Console::Clear();
            Imprimir(30, 10, "Gracias por usar ST!", ConsoleColor::Green);
            Imprimir(28, 12, "Presiona ENTER para salir...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            exit(0);
        }
    }
}
//------------------------------------------------------------------------------------------------


int main() {

    Console::Clear();
    Console::BackgroundColor = ConsoleColor::Black;
    Console::CursorVisible = false;
    system("color 0A");

    MostrarBienvenida();

    GestorUsuarios gestor;

    Usuario usuarioActual = PantallaRegistro(gestor);
    gestor.setUsuarioActual(&usuarioActual);

    if (usuarioActual.getNombre().empty() || usuarioActual.getNombre() == "Desconocido") {
        return 0;
    }


    

    return 0;
 
}

