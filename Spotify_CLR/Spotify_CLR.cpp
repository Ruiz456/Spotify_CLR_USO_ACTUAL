// SpotifyConsola.cpp - MAIN PRINCIPAL
// Spotify en Consola - Implementacion completa
// Requiere: Entidad.h, Lista.h (con modificarPos), Cancion.h, Album.h,
//           Usuario.h, Playlist.h, Reproductor.h,
//           GestorUsuarios.h, GestorPlaylists.h, Buscador.h, Favoritos.h

#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "Lista.h"
#include "Nodo.h"
#include "Cancion.h"
#include "Album.h"
#include "Usuario.h"
#include "Playlist.h"
#include "Reproductor.h"
#include "GestorUsuarios.h"
#include "GestorPlaylists.h"
#include "Buscador.h"
#include "Favoritos.h"

using namespace System;
using namespace std;

// ============================================================
// ESTRUCTURA UI
// ============================================================
struct OpcionMenu { string texto; int id; };

// ============================================================
// VARIABLES GLOBALES
// ============================================================
Lista<Cancion>* gCanciones = nullptr;
Lista<Album>* gAlbumes = nullptr;
GestorUsuarios  gGestor;
Reproductor     gReproductor;

int     gVolumen = 70;
bool    gPausado = false;
bool    gAleatorio = false;
int     gTabActual = 0;
bool    gMenuActivo = true;
Cancion gCancionActual;
Usuario* gUsuarioActual = nullptr;

// Cola personal de reproduccion
Lista<Cancion>* gColaActual = nullptr;
int             gIndiceActual = 0;

// ============================================================
// DECLARACIONES ANTICIPADAS (forward declarations)
// Necesarias porque las funciones se llaman entre si antes de
// estar definidas (ej. ManejarTeclaGlobal llama pausarReproduccion
// que esta definida cientos de lineas despues).
// ============================================================
void    DibujarShell(Usuario* u);
bool    ManejarTeclaGlobal(ConsoleKeyInfo t, Usuario* u);
// -- Reproductor --
void    iniciarReproduccion(Cancion cancion);
void    pausarReproduccion();
void    reanudarReproduccion();
void    siguienteCancion();
void    cancionAnterior();
void    cambiarVolumen(int delta);
// -- Algoritmos --
void    mezclarCancionesConFisherYates(Lista<Cancion>* lista);
void    ordenarCancionesConBubbleSort(Lista<Cancion>* lista);
// -- Busqueda --
Lista<Cancion>* buscarCancionesPorTitulo(Lista<Cancion>* canciones, string titulo);
Lista<Cancion>* buscarCancionesPorArtista(Lista<Cancion>* canciones, string artista);
Lista<Cancion>* buscarCancionesPorGenero(Lista<Cancion>* canciones, string genero);
// -- Favoritos --
bool    agregarAFavoritos(Usuario* usuario, Cancion cancion);
bool    eliminarDeFavoritos(Usuario* usuario, Cancion cancion);
bool    esCancionFavorita(Usuario* usuario, Cancion cancion);
// -- Playlists --
bool    crearPlaylist(Usuario* usuario, string nombre);
bool    agregarCancionAPlaylist(Usuario* usuario, string nombrePlaylist, Cancion cancion);
bool    eliminarCancionDePlaylist(Usuario* usuario, string nombrePlaylist, string tituloCancion);
// -- Usuarios --
bool    verificarUsuarioExiste(string nombre);
bool    crearNuevoUsuario(string nombre, string email);
Usuario* loginUsuario(string nombre);
// -- Disco --
void    crearEstructuraCarpetas();
bool    cargarAlmacenCanciones(Lista<Cancion>* canciones);
bool    cargarUsuariosDelDisco(Lista<Usuario>* usuarios);
bool    cargarAlbumesDelDisco(Lista<Album>* albumes);
bool    guardarUsuarioADisco(Usuario* usuario);
bool    guardarPlaylistADisco(Usuario* usuario, Playlist playlist);
bool    guardarFavoritosADisco(Usuario* usuario);
// -- Pantallas --
int     mostrarPantallaInicio();
int     mostrarPantallaLogin();
int     mostrarPantallaRegistro();
void    mostrarMenuPrincipal(Usuario* usuarioActual);
void    mostrarTabBiblioteca(Usuario* usuarioActual);
void    mostrarTabBuscar(Usuario* usuarioActual);
void    mostrarTabPlaylists(Usuario* usuarioActual);
void    mostrarTabAlbumes();
void    mostrarTabArtistas();
void    mostrarTabFavoritos(Usuario* usuarioActual);
void    mostrarTabConfiguracion(Usuario* usuarioActual);

// ============================================================
// UTILIDADES DE UI  (no cuentan en las 36 funciones)
// ============================================================

void gotoxy(int x, int y) { Console::SetCursorPosition(x, y); }

void Imprimir(int x, int y, string txt, ConsoleColor color = ConsoleColor::White) {
    gotoxy(x, y);
    Console::ForegroundColor = color;
    Console::Write(gcnew String(txt.c_str()));
    Console::ResetColor();
}

void LimpiarZona(int x, int y, int ancho, int alto) {
    string linea(ancho, ' ');
    for (int row = 0; row < alto; row++)
        Imprimir(x, y + row, linea);
}

string Truncar(string s, int maxLen) {
    return (s.size() > (size_t)maxLen) ? s.substr(0, maxLen - 2) + ".." : s;
}

string LeerTexto(int x, int y, string etiqueta) {
    Imprimir(x, y, etiqueta);
    gotoxy(x + (int)etiqueta.size(), y);
    Console::CursorVisible = true;
    Console::ForegroundColor = ConsoleColor::Yellow;
    string resp;
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, resp);
    Console::CursorVisible = false;
    Console::ResetColor();
    return resp;
}

string LeerTextoSinIgnore(int x, int y, string etiqueta) {
    Imprimir(x, y, etiqueta);
    gotoxy(x + (int)etiqueta.size(), y);
    Console::CursorVisible = true;
    Console::ForegroundColor = ConsoleColor::Yellow;
    string resp;
    getline(cin, resp);
    Console::CursorVisible = false;
    Console::ResetColor();
    return resp;
}

int MenuFlechas(vector<OpcionMenu>& opciones, int x, int y, string titulo = "") {
    int sel = 0;
    int n = (int)opciones.size();
    if (titulo != "") Imprimir(x, y - 2, titulo, ConsoleColor::Yellow);

    auto dibujar = [&]() {
        for (int i = 0; i < n; i++) {
            if (i == sel)
                Imprimir(x, y + i, "> " + opciones[i].texto, ConsoleColor::Green);
            else
                Imprimir(x, y + i, "  " + opciones[i].texto + "  ", ConsoleColor::White);
        }
        };
    dibujar();

    while (true) {
        ConsoleKeyInfo t = Console::ReadKey(true);
        if (t.Key == ConsoleKey::UpArrow) { sel = (sel - 1 + n) % n; dibujar(); }
        else if (t.Key == ConsoleKey::DownArrow) { sel = (sel + 1) % n;     dibujar(); }
        else if (t.Key == ConsoleKey::Enter)       return opciones[sel].id;
        else if (t.Key == ConsoleKey::Escape)      return -1;
    }
}

// Dibuja el marco persistente (header + tabs + panel izq + footer)
void DibujarShell(Usuario* u) {
    Console::Clear();

    // Barra superior
    string linea100(98, '=');
    Imprimir(0, 0, linea100, ConsoleColor::DarkGreen);
    string header = " SPOTIFY EN CONSOLA | " + u->getNombre() + " | Vol: " + to_string(gVolumen) + "%";
    Imprimir(1, 0, header, ConsoleColor::Green);

    // Barra de tabs
    string tabNames[] = { "BIBLIOTECA","BUSCAR","PLAYLISTS","ALBUMES","ARTISTAS","FAVORITOS","CONFIG" };
    int tx = 1;
    for (int i = 0; i < 7; i++) {
        if (i == gTabActual)
            Imprimir(tx, 1, "[" + tabNames[i] + "]", ConsoleColor::Green);
        else
            Imprimir(tx, 1, " " + tabNames[i] + " ", ConsoleColor::DarkGray);
        tx += (int)tabNames[i].size() + 3;
    }

    // Separador horizontal superior
    Imprimir(0, 2, string(98, '-'), ConsoleColor::DarkGreen);

    // Separador vertical (col 22)
    for (int row = 3; row <= 30; row++)
        Imprimir(22, row, "|", ConsoleColor::DarkGreen);

    // Panel izquierdo: Reproductor NOW PLAYING
    Imprimir(1, 3, "  NOW PLAYING       ", ConsoleColor::Green);
    Imprimir(1, 4, "--------------------", ConsoleColor::DarkGreen);

    string tit = Truncar(gCancionActual.getTitulo(), 18);
    string art = Truncar(gCancionActual.getArtista(), 18);
    string gen = Truncar(gCancionActual.getGenero(), 18);

    if (tit.empty()) {
        Imprimir(1, 5, "  (sin cancion)     ", ConsoleColor::DarkGray);
        Imprimir(1, 6, "                    ", ConsoleColor::DarkGray);
        Imprimir(1, 7, "                    ", ConsoleColor::DarkGray);
    }
    else {
        Imprimir(1, 5, "  " + tit, ConsoleColor::White);
        Imprimir(1, 6, "  " + art, ConsoleColor::DarkGray);
        Imprimir(1, 7, "  [" + gen + "]", ConsoleColor::DarkCyan);
    }

    // Barra de progreso (simulada)
    int lleno = gPausado ? 4 : 7;
    string barra = "  [";
    for (int i = 0; i < 12; i++) barra += (i < lleno ? "#" : "-");
    barra += "]";
    Imprimir(1, 8, barra, ConsoleColor::Green);

    string estado = gPausado ? "  [PAUSADO]  " : "  [PLAYING]  ";
    Imprimir(1, 9, estado, gPausado ? ConsoleColor::DarkYellow : ConsoleColor::Green);
    Imprimir(1, 10, "  Vol: " + to_string(gVolumen) + "%   ", ConsoleColor::White);

    string modoStr = gAleatorio ? "  [ALEATORIO] " : "  [SECUENCIAL]";
    Imprimir(1, 11, modoStr, ConsoleColor::DarkCyan);
    Imprimir(1, 12, "--------------------", ConsoleColor::DarkGreen);

    // Info cola
    if (gColaActual && !gColaActual->esVacia()) {
        int restantes = (int)gColaActual->longitud() - gIndiceActual - 1;
        Imprimir(1, 13, "  En cola: " + to_string(restantes), ConsoleColor::DarkGray);
    }

    // Separador horizontal inferior
    Imprimir(0, 31, string(98, '-'), ConsoleColor::DarkGreen);

    // Barra de controles
    Imprimir(0, 32, " P:Play/Pausa  N:Sig  B:Ant  +:Vol+  -:Vol-  A:Agregar  F:Fav  D:Del  Q:Logout",
        ConsoleColor::DarkGray);
    Imprimir(0, 33, " <-:Tab ant   ->:Tab sig   ENTER:Seleccionar   ESC:Volver",
        ConsoleColor::DarkGray);
}

// Maneja teclas globales del reproductor; retorna true si consumio la tecla
bool ManejarTeclaGlobal(ConsoleKeyInfo t, Usuario* u) {
    char kc = (char)tolower((int)t.KeyChar);
    if (kc == 'p') {
        if (gPausado) reanudarReproduccion(); else pausarReproduccion();
        DibujarShell(u); return true;
    }
    if (kc == 'n') { siguienteCancion();   DibujarShell(u); return true; }
    if (kc == 'b') { cancionAnterior();    DibujarShell(u); return true; }
    if (kc == '+' || t.Key == ConsoleKey::Add) { cambiarVolumen(10);  DibujarShell(u); return true; }
    if (kc == '-' || t.Key == ConsoleKey::Subtract) { cambiarVolumen(-10); DibujarShell(u); return true; }
    return false;
}

// ============================================================
// ============================================================
//   FUNCIONES REQUERIDAS  (total: 37)
// ============================================================
// ============================================================


// ============================================================
// BLOQUE 1: PANTALLAS (11 funciones)
// ============================================================

/*
 * Funcion: mostrarPantallaInicio()
 * Proposito: Mostrar el menu inicial con opciones LOGIN, REGISTRO y SALIR.
 *            Es la primera pantalla que ve el usuario al abrir la app.
 * Parametros: void
 * Retorno: int  0=LOGIN  1=REGISTRO  2=SALIR
 *
 * Elementos mostrados:
 *   - Logo ASCII de SPOTIFY EN CONSOLA
 *   - Tres opciones navegables con flechas
 *
 * Ejemplo:
 *   int op = mostrarPantallaInicio();
 *   if (op == 0) mostrarPantallaLogin();
 */
int mostrarPantallaInicio() {
    Console::Clear();
    Console::SetWindowSize(100, 36);
    Console::CursorVisible = false;

    Imprimir(30, 3, "  ######  ########   #####  ########  #### ######## ##    ##", ConsoleColor::Green);
    Imprimir(30, 4, " ##       ##     ## ##   ##    ##      ##  ##        ##  ## ", ConsoleColor::Green);
    Imprimir(30, 5, "  #####   ########  ##   ##    ##      ##  ######     ####  ", ConsoleColor::Green);
    Imprimir(30, 6, "       ## ##        ##   ##    ##      ##  ##          ##   ", ConsoleColor::Green);
    Imprimir(30, 7, "  ######  ##         #####     ##     #### ##          ##   ", ConsoleColor::Green);
    Imprimir(32, 9, "EN CONSOLA - Tu musica, tu estilo", ConsoleColor::DarkGreen);
    Imprimir(32, 10, string(36, '-'), ConsoleColor::DarkGreen);

    vector<OpcionMenu> opciones = {
        { "  Iniciar Sesion  ", 0 },
        { "  Registrarse     ", 1 },
        { "  Salir           ", 2 }
    };
    return MenuFlechas(opciones, 38, 13, "Selecciona una opcion:");
}

/*
 * Funcion: mostrarPantallaLogin()
 * Proposito: Solicitar el nombre de usuario y validarlo contra el disco.
 *            Carga todos los datos del usuario (playlists, favoritos).
 *            Si el login es exitoso, asigna gUsuarioActual.
 * Parametros: void
 * Retorno: int  1=exito  0=fallo o cancelado
 *
 * Operaciones internas:
 *   1. Solicitar nombre de usuario
 *   2. Verificar con verificarUsuarioExiste()
 *   3. Si existe: llamar loginUsuario() y asignar gUsuarioActual
 *   4. Mostrar resultado
 *
 * Ejemplo:
 *   if (mostrarPantallaLogin() == 1) mostrarMenuPrincipal(gUsuarioActual);
 */
int mostrarPantallaLogin() {
    Console::Clear();
    Imprimir(30, 2, string(42, '='), ConsoleColor::Green);
    Imprimir(36, 3, "INICIAR SESION", ConsoleColor::White);
    Imprimir(30, 4, string(42, '='), ConsoleColor::Green);

    string nombre = LeerTextoSinIgnore(30, 7, "Nombre de usuario: ");

    if (nombre.empty()) return 0;

    if (!verificarUsuarioExiste(nombre)) {
        Imprimir(30, 9, "Usuario no encontrado.", ConsoleColor::Red);
        Imprimir(30, 11, "Presiona ENTER para volver...", ConsoleColor::DarkGray);
        while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        return 0;
    }

    gUsuarioActual = loginUsuario(nombre);

    if (!gUsuarioActual) return 0;

    gGestor.setUsuarioActual(gUsuarioActual);

    Console::Clear();
    Imprimir(30, 2, string(42, '='), ConsoleColor::Green);
    Imprimir(32, 3, "BIENVENIDO DE VUELTA, " + nombre + "!", ConsoleColor::White);
    Imprimir(30, 4, string(42, '='), ConsoleColor::Green);
    Imprimir(30, 6, "ID   : " + to_string(gUsuarioActual->getId()), ConsoleColor::White);
    Imprimir(30, 7, "Email: " + gUsuarioActual->getEmail(), ConsoleColor::DarkGray);
    Imprimir(30, 9, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
    while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
    return 1;
}

/*
 * Funcion: mostrarPantallaRegistro()
 * Proposito: Solicitar nombre y email para crear una cuenta nueva.
 *            Valida que el nombre no exista ya en el sistema.
 *            Tras registrar, asigna gUsuarioActual para entrar directo al menu.
 * Parametros: void
 * Retorno: int  1=exito  0=fallo o cancelado
 *
 * Operaciones internas:
 *   1. Solicitar nombre y email
 *   2. Verificar que el nombre no exista (verificarUsuarioExiste)
 *   3. Crear usuario (crearNuevoUsuario)
 *   4. Hacer login automatico (loginUsuario)
 *
 * Ejemplo:
 *   if (mostrarPantallaRegistro() == 1) mostrarMenuPrincipal(gUsuarioActual);
 */
int mostrarPantallaRegistro() {
    Console::Clear();
    Imprimir(30, 2, string(42, '='), ConsoleColor::Green);
    Imprimir(36, 3, "CREAR CUENTA", ConsoleColor::White);
    Imprimir(30, 4, string(42, '='), ConsoleColor::Green);

    string nombre = LeerTextoSinIgnore(30, 7, "Nombre de usuario: ");
    if (nombre.empty()) return 0;

    if (verificarUsuarioExiste(nombre)) {
        Imprimir(30, 9, "Ese nombre ya esta en uso. Elige otro.", ConsoleColor::Red);
        Imprimir(30, 11, "Presiona ENTER para volver...", ConsoleColor::DarkGray);
        while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        return 0;
    }

    string email = LeerTextoSinIgnore(30, 9, "Email            : ");

    if (!crearNuevoUsuario(nombre, email)) {
        Imprimir(30, 12, "Error al crear usuario.", ConsoleColor::Red);
        Imprimir(30, 14, "Presiona ENTER para volver...", ConsoleColor::DarkGray);
        while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
        return 0;
    }

    gUsuarioActual = loginUsuario(nombre);
    if (!gUsuarioActual) return 0;

    gGestor.setUsuarioActual(gUsuarioActual);

    Console::Clear();
    Imprimir(30, 2, string(42, '='), ConsoleColor::Green);
    Imprimir(34, 3, "CUENTA CREADA EXITOSAMENTE", ConsoleColor::White);
    Imprimir(30, 4, string(42, '='), ConsoleColor::Green);
    Imprimir(30, 6, "Nombre: " + nombre, ConsoleColor::White);
    Imprimir(30, 7, "Email : " + email, ConsoleColor::DarkGray);
    Imprimir(30, 8, "ID    : " + to_string(gUsuarioActual->getId()), ConsoleColor::Green);
    Imprimir(30, 10, "Presiona ENTER para continuar...", ConsoleColor::DarkGray);
    while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
    return 1;
}

/*
 * Funcion: mostrarMenuPrincipal()
 * Proposito: Mostrar el menu principal con 7 tabs navegables.
 *            Dibuja el marco persistente (header, reproductor, footer)
 *            y delega el contenido al tab activo.
 *            Loop hasta que el usuario presione Q (logout).
 * Parametros:
 *   - Usuario* usuarioActual: puntero al usuario con sesion iniciada
 * Retorno: void
 *
 * Navegacion:
 *   <- -> : cambiar tab
 *   P N B +/- : controles del reproductor
 *   Q        : logout y guardar datos
 *
 * Ejemplo:
 *   mostrarMenuPrincipal(gUsuarioActual);
 */

void mostrarMenuPrincipal(Usuario* usuarioActual) {
    gMenuActivo = true;
    gTabActual = 0;

    while (gMenuActivo) {
        DibujarShell(usuarioActual);

        switch (gTabActual) {
        case 0: mostrarTabBiblioteca(usuarioActual);    break;
        case 1: mostrarTabBuscar(usuarioActual);        break;
        case 2: mostrarTabPlaylists(usuarioActual);     break;
        case 3: mostrarTabAlbumes();                    break;
        case 4: mostrarTabArtistas();                   break;
        case 5: mostrarTabFavoritos(usuarioActual);     break;
        case 6: mostrarTabConfiguracion(usuarioActual); break;
        }
    }

    guardarUsuarioADisco(usuarioActual);
}

/*
 * Funcion: mostrarTabBiblioteca()
 * Proposito: Mostrar todas las canciones del almacen global agrupadas por genero.
 *            Permite reproducir una cancion seleccionada con ENTER.
 *            Tambien permite agregar a favoritos con F o a playlist con A.
 * Parametros:
 *   - Usuario* usuarioActual: usuario en sesion (para favoritos/playlists)
 * Retorno: void
 *
 * Navegacion:
 *   UP/DOWN: moverse por canciones
 *   ENTER  : reproducir cancion seleccionada
 *   F      : agregar/quitar de favoritos
 *   A      : agregar a playlist
 *   <- ->  : cambiar tab
 *
 * Ejemplo:
 *   mostrarTabBiblioteca(usuarioActual);
 */
void mostrarTabBiblioteca(Usuario* usuarioActual) {
    if (!gCanciones || gCanciones->esVacia()) {
        Imprimir(24, 4, "Sin canciones. Agregue datos en almacenCanciones.txt", ConsoleColor::DarkGray);
        while (true) {
            ConsoleKeyInfo t = Console::ReadKey(true);
            if (t.Key == ConsoleKey::LeftArrow)  { gTabActual = (gTabActual + 6) % 7; return; }
            if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return; }
            if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return; }
        }
    }

    int sel = 0;
    int total = (int)gCanciones->longitud();

    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "BIBLIOTECA  (" + to_string(total) + " canciones)", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        // Mostrar canciones con selector
        int inicioVista = max(0, sel - 10);
        int finVista = min(total, inicioVista + 22);
        int y = 5;

        for (int i = inicioVista; i < finVista; i++) {
            Cancion c = gCanciones->obtenerPos(i);
            string linea = Truncar(c.getTitulo(), 28) + " - " + Truncar(c.getArtista(), 18)
                + "  [" + Truncar(c.getGenero(), 10) + "]";

            bool esFav = esCancionFavorita(usuarioActual, c);
            string fav = esFav ? " <3" : "   ";

            if (i == sel)
                Imprimir(24, y, "> " + Truncar(linea, 68) + fav, ConsoleColor::Green);
            else
                Imprimir(25, y, Truncar(linea, 68) + fav, ConsoleColor::White);
            y++;
        }

        Imprimir(24, 28, "ENTER:Reproducir  F:Favorito  A:Agregar a playlist", ConsoleColor::DarkGray);

        ConsoleKeyInfo t = Console::ReadKey(true);

        if (ManejarTeclaGlobal(t, usuarioActual)) continue;

        if (t.Key == ConsoleKey::UpArrow) { if (sel > 0) sel--; }
        else if (t.Key == ConsoleKey::DownArrow) { if (sel < total - 1) sel++; }
        else if (t.Key == ConsoleKey::LeftArrow) { gTabActual = (gTabActual + 6) % 7; return; }
        else if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return; }
        else if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return; }

        else if (t.Key == ConsoleKey::Enter) {
            Cancion c = gCanciones->obtenerPos(sel);
            delete gColaActual;
            gColaActual = new Lista<Cancion>();
            gIndiceActual = 0;
            // cargar todas las canciones desde sel en adelante
            for (int i = sel; i < total; i++)
                gColaActual->agregarFinal(gCanciones->obtenerPos(i));
            iniciarReproduccion(c);
            DibujarShell(usuarioActual);
        }
        else if ((char)tolower((int)t.KeyChar) == 'f') {
            Cancion c = gCanciones->obtenerPos(sel);
            if (esCancionFavorita(usuarioActual, c))
                eliminarDeFavoritos(usuarioActual, c);
            else
                agregarAFavoritos(usuarioActual, c);
            guardarFavoritosADisco(usuarioActual);
            DibujarShell(usuarioActual);
        }
        else if ((char)tolower((int)t.KeyChar) == 'a') {
            // Agregar a playlist
            int totalPL = usuarioActual->totalPlaylists();
            if (totalPL == 0) {
                Imprimir(24, 29, "No tienes playlists. Crea una en el tab PLAYLISTS.", ConsoleColor::Red);
                Console::ReadKey(true);
            }
            else {
                vector<OpcionMenu> ops;
                int idx = 0;
                usuarioActual->getPlaylists().recorrer([&](Playlist p) {
                    ops.push_back({ p.getNombre(), idx++ });
                    });
                ops.push_back({ "Cancelar", -1 });
                LimpiarZona(23, 3, 75, 28);
                int elegida = MenuFlechas(ops, 28, 6, "Agregar a cual playlist?");
                if (elegida >= 0) {
                    string nombrePL = usuarioActual->getPlaylists().obtenerPos(elegida).getNombre();
                    Cancion c = gCanciones->obtenerPos(sel);
                    agregarCancionAPlaylist(usuarioActual, nombrePL, c);
                    guardarPlaylistADisco(usuarioActual, usuarioActual->getPlaylists().obtenerPos(elegida));
                }
                DibujarShell(usuarioActual);
            }
        }
    }
}

/*
 * Funcion: mostrarTabBuscar()
 * Proposito: Permitir al usuario buscar canciones por titulo, artista o genero.
 *            Muestra los resultados con navegacion y permite reproducirlos.
 * Parametros:
 *   - Usuario* usuarioActual: usuario en sesion
 * Retorno: void
 *
 * Operaciones internas:
 *   1. Mostrar tipo de busqueda (titulo/artista/genero)
 *   2. Leer texto de busqueda
 *   3. Llamar buscarCancionesPorXxx() segun tipo
 *   4. Mostrar resultados navegables
 *   5. ENTER en resultado llama iniciarReproduccion()
 *
 * Ejemplo:
 *   mostrarTabBuscar(usuarioActual);
 */
void mostrarTabBuscar(Usuario* usuarioActual) {
    int tipoBusqueda = 0; // 0=titulo 1=artista 2=genero
    string query = "";
    Lista<Cancion>* resultados = nullptr;

    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "BUSCAR", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        // Selector tipo de busqueda
        string tipos[] = { "[Titulo]", "[Artista]", "[Genero]" };
        for (int i = 0; i < 3; i++) {
            if (i == tipoBusqueda) Imprimir(24 + i * 12, 5, tipos[i], ConsoleColor::Green);
            else                   Imprimir(24 + i * 12, 5, tipos[i], ConsoleColor::DarkGray);
        }

        Imprimir(24, 6, "Busqueda: " + query, ConsoleColor::White);
        Imprimir(24, 7, string(72, '-'), ConsoleColor::DarkGreen);

        // Mostrar resultados si los hay
        if (resultados && !resultados->esVacia()) {
            int tot = (int)resultados->longitud();
            Imprimir(24, 8, to_string(tot) + " resultado(s):", ConsoleColor::DarkCyan);
            int y = 9;
            for (int i = 0; i < min(tot, 18); i++) {
                Cancion c = resultados->obtenerPos(i);
                string lin = to_string(i + 1) + ". " + Truncar(c.getTitulo(), 25)
                    + " - " + Truncar(c.getArtista(), 18)
                    + "  [" + c.getGenero() + "]";
                Imprimir(24, y++, lin, ConsoleColor::White);
            }
        }
        else if (!query.empty()) {
            Imprimir(24, 9, "Sin resultados para: " + query, ConsoleColor::DarkGray);
        }

        Imprimir(24, 28, "Tab1/2/3:Tipo  ENTER:Buscar  NUM+ENTER:Reproducir  <- ->:Tabs", ConsoleColor::DarkGray);

        ConsoleKeyInfo t = Console::ReadKey(true);

        if (ManejarTeclaGlobal(t, usuarioActual)) continue;

        if (t.Key == ConsoleKey::LeftArrow) { delete resultados; gTabActual = (gTabActual + 6) % 7; return; }
        if (t.Key == ConsoleKey::RightArrow) { delete resultados; gTabActual = (gTabActual + 1) % 7; return; }
        if ((char)tolower((int)t.KeyChar) == 'q') { delete resultados; gMenuActivo = false; return; }

        // Cambiar tipo con 1/2/3
        if (t.KeyChar == '1') tipoBusqueda = 0;
        else if (t.KeyChar == '2') tipoBusqueda = 1;
        else if (t.KeyChar == '3') tipoBusqueda = 2;

        else if (t.Key == ConsoleKey::Enter) {
            // Solicitar query
            LimpiarZona(24, 6, 72, 1);
            query = LeerTextoSinIgnore(24, 6, "Busqueda: ");

            delete resultados;
            if (tipoBusqueda == 0) resultados = buscarCancionesPorTitulo(gCanciones, query);
            else if (tipoBusqueda == 1) resultados = buscarCancionesPorArtista(gCanciones, query);
            else                        resultados = buscarCancionesPorGenero(gCanciones, query);

            DibujarShell(usuarioActual);
        }
        // Reproducir resultado por numero
        else if (t.KeyChar >= '1' && t.KeyChar <= '9' && resultados) {
            int idx = (int)(t.KeyChar - '1');
            if (idx < (int)resultados->longitud()) {
                Cancion c = resultados->obtenerPos(idx);
                delete gColaActual;
                gColaActual = new Lista<Cancion>();
                gIndiceActual = 0;
                for (int i = idx; i < (int)resultados->longitud(); i++)
                    gColaActual->agregarFinal(resultados->obtenerPos(i));
                iniciarReproduccion(c);
                DibujarShell(usuarioActual);
            }
        }
    }
}

/*
 * Funcion: mostrarTabPlaylists()
 * Proposito: Gestionar las playlists del usuario.
 *            Crear, ver, eliminar playlists y manipular sus canciones.
 *            Permite shuffle y ordenamiento alfabetico.
 * Parametros:
 *   - Usuario* usuarioActual: usuario propietario de las playlists
 * Retorno: void
 *
 * Operaciones internas:
 *   1. Listar playlists del usuario
 *   2. Opcion de crear nueva playlist
 *   3. Al seleccionar playlist: ver/agregar/eliminar/mezclar canciones
 *   4. Guardar cambios a disco automaticamente
 *
 * Ejemplo:
 *   mostrarTabPlaylists(usuarioActual);
 */
void mostrarTabPlaylists(Usuario* usuarioActual) {
    int sel = 0;

    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "MIS PLAYLISTS", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        int total = usuarioActual->totalPlaylists();
        int y = 5;

        if (total == 0) {
            Imprimir(25, y++, "(sin playlists - presiona C para crear una)", ConsoleColor::DarkGray);
        }
        else {
            Lista<Playlist>& pls = usuarioActual->getPlaylists();
            for (int i = 0; i < total; i++) {
                Playlist p = pls.obtenerPos(i);
                string lin = p.getNombre() + "  (" + to_string(p.getCanciones().longitud()) + " canciones)";
                if (i == sel)
                    Imprimir(24, y++, "> " + Truncar(lin, 68), ConsoleColor::Green);
                else
                    Imprimir(25, y++, Truncar(lin, 68), ConsoleColor::White);
            }
        }

        Imprimir(24, 28, "C:Nueva  ENTER:Ver  D:Eliminar  <- ->:Tabs", ConsoleColor::DarkGray);

        ConsoleKeyInfo t = Console::ReadKey(true);

        if (ManejarTeclaGlobal(t, usuarioActual)) continue;

        if (t.Key == ConsoleKey::LeftArrow) { gTabActual = (gTabActual + 6) % 7; return; }
        if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return; }
        if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return; }

        if (t.Key == ConsoleKey::UpArrow) { if (sel > 0) sel--; }
        if (t.Key == ConsoleKey::DownArrow) { if (sel < total - 1) sel++; }

        // Crear playlist
        if ((char)tolower((int)t.KeyChar) == 'c') {
            LimpiarZona(23, 3, 75, 28);
            string nombre = LeerTexto(24, 5, "Nombre de la nueva playlist: ");
            if (!nombre.empty()) {
                crearPlaylist(usuarioActual, nombre);
                DibujarShell(usuarioActual);
            }
        }

        // Eliminar playlist
        else if ((char)tolower((int)t.KeyChar) == 'd' && total > 0) {
            Playlist p = usuarioActual->getPlaylists().obtenerPos(sel);
            LimpiarZona(23, 20, 75, 8);
            vector<OpcionMenu> ops = { {"SI, eliminar", 1}, {"NO, cancelar", 0} };
            int conf = MenuFlechas(ops, 28, 22, "Eliminar playlist: " + p.getNombre() + "?");
            if (conf == 1) {
                usuarioActual->eliminarPlaylist(p.getNombre());
                guardarUsuarioADisco(usuarioActual);
                sel = max(0, sel - 1);
            }
            DibujarShell(usuarioActual);
        }

        // Ver playlist
        else if (t.Key == ConsoleKey::Enter && total > 0) {
            Playlist pl = usuarioActual->getPlaylists().obtenerPos(sel);
            string nombrePL = pl.getNombre();

            // Sub-pantalla de la playlist
            while (true) {
                LimpiarZona(23, 3, 75, 28);
                Imprimir(24, 3, "PLAYLIST: " + nombrePL, ConsoleColor::Yellow);
                Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

                Playlist plActual = usuarioActual->buscarPlaylist(nombrePL);
                int totalC = (int)plActual.getCanciones().longitud();
                int selC = 0;
                int yC = 5;

                if (totalC == 0) {
                    Imprimir(25, yC, "(vacia - presiona A para agregar)", ConsoleColor::DarkGray);
                }
                else {
                    for (int i = 0; i < totalC; i++) {
                        Cancion c = plActual.getCanciones().obtenerPos(i);
                        string lin = to_string(i + 1) + ". " + Truncar(c.getTitulo(), 28) + " - " + Truncar(c.getArtista(), 18);
                        Imprimir(25, yC++, lin, ConsoleColor::White);
                    }
                }

                Imprimir(24, 28, "A:Agregar  D:Eliminar  S:Shuffle  O:Ordenar  PLAY:Reproducir  ESC:Volver",
                    ConsoleColor::DarkGray);

                ConsoleKeyInfo tc = Console::ReadKey(true);

                if (ManejarTeclaGlobal(tc, usuarioActual)) { DibujarShell(usuarioActual); continue; }
                if (tc.Key == ConsoleKey::Escape) break;

                if ((char)tolower((int)tc.KeyChar) == 'a') {
                    // Agregar cancion desde almacen
                    if (!gCanciones || gCanciones->esVacia()) {
                        Imprimir(24, 29, "No hay canciones en el almacen.", ConsoleColor::Red);
                        Console::ReadKey(true);
                    }
                    else {
                        LimpiarZona(23, 3, 75, 28);
                        vector<OpcionMenu> ops;
                        int idx = 0;
                        gCanciones->recorrer([&](Cancion c) {
                            ops.push_back({ Truncar(c.getTitulo(), 28) + " - " + Truncar(c.getArtista(), 18), idx++ });
                            });
                        ops.push_back({ "Cancelar", -1 });
                        int elegida = MenuFlechas(ops, 26, 6, "Selecciona cancion a agregar:");
                        if (elegida >= 0) {
                            Cancion c = gCanciones->obtenerPos(elegida);
                            agregarCancionAPlaylist(usuarioActual, nombrePL, c);
                            guardarPlaylistADisco(usuarioActual, usuarioActual->buscarPlaylist(nombrePL));
                        }
                        DibujarShell(usuarioActual);
                    }
                }
                else if ((char)tolower((int)tc.KeyChar) == 'd' && totalC > 0) {
                    LimpiarZona(23, 3, 75, 28);
                    vector<OpcionMenu> ops;
                    int idx2 = 0;
                    plActual.getCanciones().recorrer([&](Cancion c) {
                        ops.push_back({ Truncar(c.getTitulo(), 28) + " - " + Truncar(c.getArtista(), 18), idx2++ });
                        });
                    ops.push_back({ "Cancelar", -1 });
                    int elegida = MenuFlechas(ops, 26, 6, "Selecciona cancion a eliminar:");
                    if (elegida >= 0) {
                        Cancion cElim = plActual.getCanciones().obtenerPos(elegida);
                        eliminarCancionDePlaylist(usuarioActual, nombrePL, cElim.getTitulo());
                        guardarPlaylistADisco(usuarioActual, usuarioActual->buscarPlaylist(nombrePL));
                    }
                    DibujarShell(usuarioActual);
                }
                else if ((char)tolower((int)tc.KeyChar) == 's') {
                    // Shuffle
                    Lista<Playlist>& pls2 = usuarioActual->getPlaylists();
                    for (uint i = 0; i < pls2.longitud(); i++) {
                        Playlist pRef = pls2.obtenerPos(i);
                        if (pRef.getNombre() == nombrePL) {
                            mezclarCancionesConFisherYates(&pRef.getCanciones());
                            pls2.modificarPos(i, pRef);
                            break;
                        }
                    }
                    guardarPlaylistADisco(usuarioActual, usuarioActual->buscarPlaylist(nombrePL));
                    DibujarShell(usuarioActual);
                    Imprimir(24, 29, "Playlist mezclada!", ConsoleColor::Green);
                    Console::ReadKey(true);
                    DibujarShell(usuarioActual);
                }
                else if ((char)tolower((int)tc.KeyChar) == 'o') {
                    // Ordenar A-Z
                    Lista<Playlist>& pls2 = usuarioActual->getPlaylists();
                    for (uint i = 0; i < pls2.longitud(); i++) {
                        Playlist pRef = pls2.obtenerPos(i);
                        if (pRef.getNombre() == nombrePL) {
                            ordenarCancionesConBubbleSort(&pRef.getCanciones());
                            pls2.modificarPos(i, pRef);
                            break;
                        }
                    }
                    guardarPlaylistADisco(usuarioActual, usuarioActual->buscarPlaylist(nombrePL));
                    DibujarShell(usuarioActual);
                    Imprimir(24, 29, "Playlist ordenada A-Z!", ConsoleColor::Green);
                    Console::ReadKey(true);
                    DibujarShell(usuarioActual);
                }
                else if ((char)tolower((int)tc.KeyChar) == 'p' && totalC > 0) {
                    // Reproducir playlist
                    delete gColaActual;
                    gColaActual = new Lista<Cancion>();
                    gIndiceActual = 0;
                    plActual.getCanciones().recorrer([&](Cancion c) {
                        gColaActual->agregarFinal(c);
                        });
                    Cancion primera = gColaActual->obtenerPos(0);
                    iniciarReproduccion(primera);
                    DibujarShell(usuarioActual);
                }
            }
            DibujarShell(usuarioActual);
        }
    }
}

/*
 * Funcion: mostrarTabAlbumes()
 * Proposito: Mostrar todos los albumes cargados desde disco.
 *            Permite expandir un album para ver su tracklist y reproducirlo.
 * Parametros: void (usa gAlbumes y gUsuarioActual globales)
 * Retorno: void
 *
 * Navegacion:
 *   UP/DOWN: moverse por albumes
 *   ENTER  : expandir/contraer album
 *   P      : reproducir album seleccionado
 *   <- ->  : cambiar tab
 *
 * Ejemplo:
 *   mostrarTabAlbumes();
 */
void mostrarTabAlbumes() {
    int sel = 0;
    int total = gAlbumes ? (int)gAlbumes->longitud() : 0;

    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "ALBUMES  (" + to_string(total) + ")", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        int y = 5;
        if (total == 0) {
            Imprimir(25, y, "(sin albumes - agrega carpetas en albumes/)", ConsoleColor::DarkGray);
        }
        else {
            for (int i = 0; i < min(total, 20); i++) {
                Album alb = gAlbumes->obtenerPos(i);
                string lin = Truncar(alb.getNombre(), 28) + " - " + Truncar(alb.getArtista(), 18)
                    + " (" + to_string(alb.getAnio()) + ") ["
                    + to_string(alb.getCanciones().longitud()) + " pistas]";
                if (i == sel)
                    Imprimir(24, y++, "> " + Truncar(lin, 68), ConsoleColor::Green);
                else
                    Imprimir(25, y++, Truncar(lin, 68), ConsoleColor::White);
            }
        }

        // Si hay album seleccionado, mostrar tracklist abajo
        if (total > 0 && sel < total) {
            Album alb = gAlbumes->obtenerPos(sel);
            int totalPistas = (int)alb.getCanciones().longitud();
            if (totalPistas > 0) {
                Imprimir(24, 26, "Tracklist:", ConsoleColor::DarkCyan);
                string tracklist = "";
                for (int i = 0; i < min(totalPistas, 4); i++) {
                    Cancion c = alb.getCanciones().obtenerPos(i);
                    tracklist += to_string(i + 1) + "." + Truncar(c.getTitulo(), 15) + "  ";
                }
                Imprimir(25, 27, Truncar(tracklist, 70), ConsoleColor::DarkGray);
            }
        }

        Imprimir(24, 28, "ENTER:Reproducir album  <- ->:Tabs", ConsoleColor::DarkGray);

        ConsoleKeyInfo t = Console::ReadKey(true);

        if (gUsuarioActual && ManejarTeclaGlobal(t, gUsuarioActual)) continue;

        if (t.Key == ConsoleKey::LeftArrow) { gTabActual = (gTabActual + 6) % 7; return; }
        if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return; }
        if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return; }

        if (t.Key == ConsoleKey::UpArrow) { if (sel > 0) sel--; }
        if (t.Key == ConsoleKey::DownArrow) { if (sel < total - 1) sel++; }

        if (t.Key == ConsoleKey::Enter && total > 0) {
            Album alb = gAlbumes->obtenerPos(sel);
            if (!alb.getCanciones().esVacia()) {
                delete gColaActual;
                gColaActual = new Lista<Cancion>();
                gIndiceActual = 0;
                alb.getCanciones().recorrer([&](Cancion c) {
                    gColaActual->agregarFinal(c);
                    });
                Cancion primera = gColaActual->obtenerPos(0);
                iniciarReproduccion(primera);
                if (gUsuarioActual) DibujarShell(gUsuarioActual);
            }
        }
    }
}

/*
 * Funcion: mostrarTabArtistas()
 * Proposito: Listar artistas unicos extraidos del almacen de canciones.
 *            Al seleccionar un artista muestra todas sus canciones.
 * Parametros: void (usa gCanciones y gUsuarioActual globales)
 * Retorno: void
 *
 * Operaciones internas:
 *   1. Extraer artistas unicos de gCanciones
 *   2. Mostrar lista navegable
 *   3. ENTER expande canciones del artista
 *   4. ENTER en cancion la reproduce
 *
 * Ejemplo:
 *   mostrarTabArtistas();
 */
void mostrarTabArtistas() {
    // Extraer artistas unicos
    vector<string> artistas;
    if (gCanciones) {
        gCanciones->recorrer([&](Cancion c) {
            bool existe = false;
            for (auto& a : artistas) if (a == c.getArtista()) { existe = true; break; }
            if (!existe && !c.getArtista().empty()) artistas.push_back(c.getArtista());
            });
    }

    int sel = 0;
    int total = (int)artistas.size();

    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "ARTISTAS  (" + to_string(total) + ")", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        int y = 5;
        if (total == 0) {
            Imprimir(25, y, "(sin artistas)", ConsoleColor::DarkGray);
        }
        else {
            for (int i = 0; i < min(total, 18); i++) {
                // Contar canciones del artista
                int numC = 0;
                if (gCanciones) gCanciones->recorrer([&](Cancion c) {
                    if (c.getArtista() == artistas[i]) numC++;
                    });
                string lin = artistas[i] + "  (" + to_string(numC) + " canciones)";
                if (i == sel)
                    Imprimir(24, y++, "> " + Truncar(lin, 68), ConsoleColor::Green);
                else
                    Imprimir(25, y++, Truncar(lin, 68), ConsoleColor::White);
            }
        }

        Imprimir(24, 28, "ENTER:Ver canciones  <- ->:Tabs", ConsoleColor::DarkGray);

        ConsoleKeyInfo t = Console::ReadKey(true);

        if (gUsuarioActual && ManejarTeclaGlobal(t, gUsuarioActual)) continue;

        if (t.Key == ConsoleKey::LeftArrow) { gTabActual = (gTabActual + 6) % 7; return; }
        if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return; }
        if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return; }

        if (t.Key == ConsoleKey::UpArrow) { if (sel > 0) sel--; }
        if (t.Key == ConsoleKey::DownArrow) { if (sel < total - 1) sel++; }

        if (t.Key == ConsoleKey::Enter && total > 0) {
            // Mostrar canciones del artista seleccionado
            string artista = artistas[sel];
            Lista<Cancion>* cancArtista = buscarCancionesPorArtista(gCanciones, artista);

            LimpiarZona(23, 3, 75, 28);
            Imprimir(24, 3, "ARTISTA: " + artista, ConsoleColor::Yellow);
            Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

            int y2 = 5;
            int totC = (int)cancArtista->longitud();
            for (int i = 0; i < min(totC, 20); i++) {
                Cancion c = cancArtista->obtenerPos(i);
                Imprimir(25, y2++, to_string(i + 1) + ". " + c.getTitulo() + " [" + c.getGenero() + "]", ConsoleColor::White);
            }

            Imprimir(24, 28, "NUM:Reproducir  ESC:Volver", ConsoleColor::DarkGray);

            while (true) {
                ConsoleKeyInfo t2 = Console::ReadKey(true);
                if (t2.Key == ConsoleKey::Escape) break;
                if (gUsuarioActual && ManejarTeclaGlobal(t2, gUsuarioActual)) {
                    DibujarShell(gUsuarioActual); break;
                }
                if (t2.KeyChar >= '1' && t2.KeyChar <= '9') {
                    int idx = (int)(t2.KeyChar - '1');
                    if (idx < totC) {
                        delete gColaActual;
                        gColaActual = new Lista<Cancion>();
                        gIndiceActual = 0;
                        for (int i = idx; i < totC; i++)
                            gColaActual->agregarFinal(cancArtista->obtenerPos(i));
                        iniciarReproduccion(cancArtista->obtenerPos(idx));
                        if (gUsuarioActual) DibujarShell(gUsuarioActual);
                        break;
                    }
                }
            }
            delete cancArtista;
        }
    }
}

/*
 * Funcion: mostrarTabFavoritos()
 * Proposito: Mostrar las canciones marcadas como favoritas por el usuario.
 *            Visualizacion en orden LIFO (ultima agregada primero).
 *            Permite eliminar favoritos y reproducir canciones.
 * Parametros:
 *   - Usuario* usuarioActual: usuario dueno de los favoritos
 * Retorno: void
 *
 * Navegacion:
 *   UP/DOWN: moverse por favoritos
 *   ENTER  : reproducir cancion
 *   D      : eliminar de favoritos
 *   <- ->  : cambiar tab
 *
 * Ejemplo:
 *   mostrarTabFavoritos(usuarioActual);
 */
void mostrarTabFavoritos(Usuario* usuarioActual) {
    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "MIS FAVORITOS  <3", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        int total = (int)usuarioActual->getFavoritos().getLista().longitud();
        int y = 5;

        if (total == 0) {
            Imprimir(25, y, "(sin favoritos - presiona F en cualquier cancion)", ConsoleColor::DarkGray);
        }
        else {
            // Mostrar en LIFO (del ultimo al primero)
            for (int i = total - 1; i >= max(0, total - 22); i--) {
                Cancion c = usuarioActual->getFavoritos().getLista().obtenerPos(i);
                string lin = to_string(total - i) + ". <3  "
                    + Truncar(c.getTitulo(), 26) + " - " + Truncar(c.getArtista(), 18);
                Imprimir(25, y++, Truncar(lin, 68), ConsoleColor::White);
            }
        }

        Imprimir(24, 28, "NUM:Reproducir  D:Eliminar  <- ->:Tabs", ConsoleColor::DarkGray);

        ConsoleKeyInfo t = Console::ReadKey(true);

        if (ManejarTeclaGlobal(t, usuarioActual)) continue;

        if (t.Key == ConsoleKey::LeftArrow) { gTabActual = (gTabActual + 6) % 7; return; }
        if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return; }
        if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return; }

        // Reproducir por numero (1=ultimo favorito, etc.)
        if (t.KeyChar >= '1' && t.KeyChar <= '9' && total > 0) {
            int posDesde = total - (int)(t.KeyChar - '0');
            if (posDesde >= 0) {
                delete gColaActual;
                gColaActual = new Lista<Cancion>();
                gIndiceActual = 0;
                for (int i = posDesde; i >= 0; i--)
                    gColaActual->agregarFinal(usuarioActual->getFavoritos().getLista().obtenerPos(i));
                iniciarReproduccion(usuarioActual->getFavoritos().getLista().obtenerPos(posDesde));
                DibujarShell(usuarioActual);
            }
        }

        // Eliminar un favorito
        if ((char)tolower((int)t.KeyChar) == 'd' && total > 0) {
            LimpiarZona(23, 3, 75, 28);
            vector<OpcionMenu> ops;
            for (int i = total - 1; i >= 0; i--) {
                Cancion c = usuarioActual->getFavoritos().getLista().obtenerPos(i);
                ops.push_back({ "<3 " + Truncar(c.getTitulo(), 26) + " - " + Truncar(c.getArtista(), 18), i });
            }
            ops.push_back({ "Cancelar", -1 });
            int elegida = MenuFlechas(ops, 26, 6, "Eliminar de favoritos:");
            if (elegida >= 0) {
                Cancion cElim = usuarioActual->getFavoritos().getLista().obtenerPos(elegida);
                eliminarDeFavoritos(usuarioActual, cElim);
                guardarFavoritosADisco(usuarioActual);
            }
            DibujarShell(usuarioActual);
        }
    }
}

/*
 * Funcion: mostrarTabConfiguracion()
 * Proposito: Mostrar y modificar ajustes del usuario y la aplicacion.
 *            Incluye: volumen, modo reproduccion, info de perfil,
 *            opcion de eliminar cuenta y cerrar sesion.
 * Parametros:
 *   - Usuario* usuarioActual: usuario en sesion
 * Retorno: void
 *
 * Opciones:
 *   - Subir/Bajar volumen
 *   - Activar/Desactivar modo aleatorio
 *   - Ver info de perfil
 *   - Eliminar cuenta
 *
 * Ejemplo:
 *   mostrarTabConfiguracion(usuarioActual);
 */
void mostrarTabConfiguracion(Usuario* usuarioActual) {
    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "CONFIGURACION", ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        // Info de perfil
        Imprimir(24, 6, "Perfil:", ConsoleColor::DarkCyan);
        Imprimir(26, 7, "Nombre : " + usuarioActual->getNombre(), ConsoleColor::White);
        Imprimir(26, 8, "Email  : " + usuarioActual->getEmail(), ConsoleColor::DarkGray);
        Imprimir(26, 9, "ID     : " + to_string(usuarioActual->getId()), ConsoleColor::DarkGray);
        Imprimir(26, 10, "Playlists: " + to_string(usuarioActual->totalPlaylists()), ConsoleColor::DarkGray);
        int numFav = (int)usuarioActual->getFavoritos().getLista().longitud();
        Imprimir(26, 11, "Favoritos: " + to_string(numFav), ConsoleColor::DarkGray);

        // Ajustes
        Imprimir(24, 13, "Ajustes:", ConsoleColor::DarkCyan);
        Imprimir(26, 14, "Volumen: " + to_string(gVolumen) + "%", ConsoleColor::White);
        string modoStr = gAleatorio ? "Aleatorio (ACTIVO)" : "Secuencial";
        Imprimir(26, 15, "Modo   : " + modoStr, ConsoleColor::White);

        Imprimir(24, 17, "Canciones en almacen: " + (gCanciones ? to_string(gCanciones->longitud()) : "0"),
            ConsoleColor::DarkGray);
        Imprimir(24, 18, "Albums cargados     : " + (gAlbumes ? to_string(gAlbumes->longitud()) : "0"),
            ConsoleColor::DarkGray);

        // Acciones
        vector<OpcionMenu> opciones = {
            { "Subir volumen (+10)"     , 1 },
            { "Bajar volumen (-10)"     , 2 },
            { "Activar/Desact. Aleatorio",3 },
            { "Eliminar mi cuenta"      , 4 },
            { "Guardar y volver"        , 5 }
        };
        int accion = MenuFlechas(opciones, 28, 20, "");

        if (accion == 1) { cambiarVolumen(10); }
        else if (accion == 2) { cambiarVolumen(-10); }
        else if (accion == 3) { gAleatorio = !gAleatorio; }
        else if (accion == 4) {
            // Confirmar eliminacion
            LimpiarZona(23, 3, 75, 28);
            vector<OpcionMenu> conf = {
                { "SI, eliminar permanentemente", 1 },
                { "NO, cancelar",                 0 }
            };
            int respuesta = MenuFlechas(conf, 28, 10, "Eliminar cuenta " + usuarioActual->getNombre() + "?");
            if (respuesta == 1) {
                gGestor.eliminar(usuarioActual->getId());
                gMenuActivo = false;
                return;
            }
        }
        else if (accion == 5 || accion == -1) {
            guardarUsuarioADisco(usuarioActual);
            gTabActual = (gTabActual + 6) % 7;
            return;
        }

        DibujarShell(usuarioActual);
    }
}


// ============================================================
// BLOQUE 2: CARGA DE DATOS (3 funciones + crearEstructuraCarpetas)
// ============================================================

/*
 * Funcion: crearEstructuraCarpetas()
 * Proposito: Verificar y crear la estructura base de carpetas y archivos
 *            necesarios para que la aplicacion funcione:
 *              - Carpeta usuarios_data/
 *              - Carpeta albumes/
 *              - Archivo almacenCanciones.txt (si no existe)
 *            Se llama una sola vez al inicio desde main().
 * Parametros: void
 * Retorno: void
 *
 * Ejemplo:
 *   crearEstructuraCarpetas(); // primera instruccion de main()
 */
void crearEstructuraCarpetas() {
    (void)_mkdir("usuarios_data");
    (void)_mkdir("albumes");

    ifstream check("almacenCanciones.txt");
    if (!check.is_open()) {
        ofstream crear("almacenCanciones.txt");
        crear.close();
    }
    else {
        check.close();
    }
}

/*
 * Funcion: cargarAlmacenCanciones()
 * Proposito: Leer almacenCanciones.txt y poblar la lista global de canciones.
 *            Se ejecuta una sola vez al iniciar la aplicacion.
 * Parametros:
 *   - Lista<Cancion>* canciones: lista destino donde se agregan las canciones
 * Retorno: bool (true si cargo al menos una cancion)
 *
 * Formato del archivo (4 campos):
 *   Titulo|Artista|Genero|Duracion
 *   Bohemian Rhapsody|Queen|Rock|5.20
 *   (la duracion es opcional; si falta se usa 0.0)
 *
 * Ejemplo:
 *   Lista<Cancion>* gCanciones = new Lista<Cancion>();
 *   cargarAlmacenCanciones(gCanciones);
 */
bool cargarAlmacenCanciones(Lista<Cancion>* canciones) {
    ifstream archivo("almacenCanciones.txt");
    if (!archivo.is_open()) return false;

    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;

        stringstream ss(linea);
        string titulo, artista, genero, durStr;
        getline(ss, titulo,  '|');
        getline(ss, artista, '|');
        getline(ss, genero,  '|');
        getline(ss, durStr,  '|');
        if (!durStr.empty() && durStr.back() == '\r') durStr.pop_back();
        if (!genero.empty()  && genero.back()  == '\r') genero.pop_back();

        float dur = 0.0f;
        if (!durStr.empty()) {
            try { dur = stof(durStr); } catch (...) { dur = 0.0f; }
        }

        if (!titulo.empty())
            canciones->agregarFinal(Cancion(titulo, genero, dur, artista));
    }
    archivo.close();
    return !canciones->esVacia();
}

/*
 * Funcion: cargarUsuariosDelDisco()
 * Proposito: Cargar el listado basico de usuarios desde usuarios_data/usuarios.txt.
 *            Solo carga id, nombre y email (sin playlists ni favoritos).
 *            Los datos completos se cargan al hacer login.
 * Parametros:
 *   - Lista<Usuario>* usuarios: lista destino donde se agregan los usuarios
 * Retorno: bool (true si cargo al menos un usuario)
 *
 * Archivo leido:
 *   usuarios_data/usuarios.txt  formato: id|nombre|email
 *
 * Ejemplo:
 *   Lista<Usuario>* usuarios = new Lista<Usuario>();
 *   cargarUsuariosDelDisco(usuarios);
 */
bool cargarUsuariosDelDisco(Lista<Usuario>* usuarios) {
    Lista<Usuario>& listaGestor = gGestor.getLista();
    if (listaGestor.esVacia()) return false;

    listaGestor.recorrer([&](Usuario u) {
        usuarios->agregarFinal(u);
        });
    return !usuarios->esVacia();
}

/*
 * Funcion: cargarAlbumesDelDisco()
 * Proposito: Leer la carpeta albumes/ y cargar cada album con su tracklist.
 *            Cada subcarpeta debe tener config.txt y canciones.txt.
 * Parametros:
 *   - Lista<Album>* albumes: lista destino donde se agregan los albumes
 * Retorno: bool (true si cargo al menos un album)
 *
 * Estructura esperada:
 *   albumes/[NombreAlbum]/config.txt    -> nombre|artista|anio
 *   albumes/[NombreAlbum]/canciones.txt -> titulo|artista|genero|duracion
 *
 * Ejemplo:
 *   Lista<Album>* gAlbumes = new Lista<Album>();
 *   cargarAlbumesDelDisco(gAlbumes);
 */
bool cargarAlbumesDelDisco(Lista<Album>* albumes) {
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA("albumes\\*", &ffd);
    if (hFind == INVALID_HANDLE_VALUE) return false;

    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        string carpeta = ffd.cFileName;
        if (carpeta == "." || carpeta == "..") continue;

        string configPath = "albumes\\" + carpeta + "\\config.txt";
        ifstream configF(configPath);
        if (!configF.is_open()) continue;

        string linea;
        getline(configF, linea);
        configF.close();

        stringstream ss(linea);
        string nombre, artista, anioStr;
        getline(ss, nombre, '|');
        getline(ss, artista, '|');
        getline(ss, anioStr, '|');
        int anio = anioStr.empty() ? 0 : stoi(anioStr);

        Album album(nombre, artista, anio);

        string cancionesPath = "albumes\\" + carpeta + "\\canciones.txt";
        ifstream cancionesF(cancionesPath);
        if (cancionesF.is_open()) {
            string cLinea;
            while (getline(cancionesF, cLinea)) {
                if (cLinea.empty()) continue;
                stringstream css(cLinea);
                string cTit, cArt, cGen, cDurStr;
                getline(css, cTit,    '|');
                getline(css, cArt,    '|');
                getline(css, cGen,    '|');
                getline(css, cDurStr, '|');
                if (!cDurStr.empty() && cDurStr.back() == '\r') cDurStr.pop_back();
                if (!cGen.empty()    && cGen.back()    == '\r') cGen.pop_back();
                float dur = 0.0f;
                if (!cDurStr.empty()) {
                    try { dur = stof(cDurStr); } catch (...) { dur = 0.0f; }
                }
                album.agregarCancion(Cancion(cTit, cGen, dur, cArt));
            }
            cancionesF.close();
        }
        albumes->agregarFinal(album);

    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);
    return !albumes->esVacia();
}


// ============================================================
// BLOQUE 3: GESTION DE USUARIOS (3 funciones)
// ============================================================

/*
 * Funcion: verificarUsuarioExiste()
 * Proposito: Comprobar si un nombre de usuario ya esta registrado en el sistema.
 *            Se usa antes de login (verificar que existe) y antes de registro
 *            (verificar que NO existe para evitar duplicados).
 * Parametros:
 *   - string nombre: nombre de usuario a buscar (case-sensitive)
 * Retorno: bool (true si el nombre ya existe en el sistema)
 *
 * Ejemplo:
 *   if (!verificarUsuarioExiste("Joel")) crearNuevoUsuario("Joel", "j@mail.com");
 */
bool verificarUsuarioExiste(string nombre) {
    return gGestor.existeNombre(nombre);
}

/*
 * Funcion: crearNuevoUsuario()
 * Proposito: Registrar un usuario nuevo con nombre y email.
 *            Genera un ID unico de 4 digitos y crea sus carpetas en disco.
 *            NO hace login automatico; llamar loginUsuario() despues.
 * Parametros:
 *   - string nombre: nombre de usuario deseado (debe ser unico)
 *   - string email : correo electronico del usuario
 * Retorno: bool (true si se registro correctamente)
 *
 * Archivos creados:
 *   usuarios_data/usuarios.txt (actualizado)
 *   usuarios_data/usuario_[id].txt (datos iniciales vacios)
 *
 * Ejemplo:
 *   if (crearNuevoUsuario("Maria", "maria@mail.com"))
 *       loginUsuario("Maria");
 */
bool crearNuevoUsuario(string nombre, string email) {
    if (nombre.empty() || email.empty()) return false;
    if (verificarUsuarioExiste(nombre))  return false;

    gGestor.registrar(nombre, email);
    return verificarUsuarioExiste(nombre);
}

/*
 * Funcion: loginUsuario()
 * Proposito: Buscar el usuario por nombre, cargar sus playlists y favoritos
 *            desde disco, y retornar un puntero al objeto Usuario cargado.
 *            El llamador es responsable de liberar la memoria (delete).
 * Parametros:
 *   - string nombre: nombre del usuario a autenticar
 * Retorno: Usuario* (puntero al usuario con datos cargados, o nullptr si no existe)
 *
 * Operaciones internas:
 *   1. Llamar GestorUsuarios::buscarPorNombre() que carga datos completos
 *   2. Verificar que el id sea valido (> 0)
 *   3. Copiar al heap y retornar puntero
 *
 * Ejemplo:
 *   Usuario* u = loginUsuario("Joel");
 *   if (u) mostrarMenuPrincipal(u);
 *   delete u;
 */
Usuario* loginUsuario(string nombre) {
    Usuario encontrado = gGestor.buscarPorNombre(nombre);
    if (encontrado.getId() == 0) return nullptr;

    Usuario* ptr = new Usuario(encontrado);
    return ptr;
}


// ============================================================
// BLOQUE 4: GESTION DE PLAYLISTS (3 funciones)
// ============================================================

/*
 * Funcion: crearPlaylist()
 * Proposito: Crear una playlist vacia con el nombre dado y asociarla al usuario.
 *            Guarda los cambios a disco automaticamente.
 * Parametros:
 *   - Usuario* usuario    : puntero al usuario propietario
 *   - string  nombre      : nombre de la nueva playlist (debe ser unico para el usuario)
 * Retorno: bool (true si se creo correctamente)
 *
 * Operaciones internas:
 *   1. Validar parametros
 *   2. Crear objeto Playlist(nombre)
 *   3. Llamar usuario->crearPlaylist()
 *   4. Guardar a disco con guardarUsuarioADisco()
 *
 * Ejemplo:
 *   crearPlaylist(usuarioActual, "Rock Favoritos");
 */
bool crearPlaylist(Usuario* usuario, string nombre) {
    if (!usuario || nombre.empty()) return false;

    usuario->crearPlaylist(Playlist(nombre));
    guardarUsuarioADisco(usuario);
    return true;
}

/*
 * Funcion: agregarCancionAPlaylist()
 * Proposito: Agregar una cancion a una playlist existente del usuario.
 *            Usa modificarPos() para actualizar el nodo en la lista enlazada.
 * Parametros:
 *   - Usuario* usuario      : propietario de la playlist
 *   - string  nombrePlaylist: nombre exacto de la playlist destino
 *   - Cancion cancion       : objeto cancion a insertar
 * Retorno: bool (true si se agrego, false si la playlist no existe)
 *
 * Operaciones internas:
 *   1. Recorrer lista de playlists del usuario
 *   2. Localizar la playlist por nombre
 *   3. Llamar playlist.agregarCancion()
 *   4. Actualizar nodo con modificarPos()
 *
 * Ejemplo:
 *   Cancion c("Bohemian Rhapsody","Rock",5.2f,"Queen");
 *   agregarCancionAPlaylist(usuario, "Rock Classics", c);
 */
bool agregarCancionAPlaylist(Usuario* usuario, string nombrePlaylist, Cancion cancion) {
    if (!usuario) return false;

    Lista<Playlist>& pls = usuario->getPlaylists();
    for (uint i = 0; i < pls.longitud(); i++) {
        Playlist p = pls.obtenerPos(i);
        if (p.getNombre() == nombrePlaylist) {
            p.agregarCancion(cancion);
            pls.modificarPos(i, p);
            return true;
        }
    }
    return false;
}

/*
 * Funcion: eliminarCancionDePlaylist()
 * Proposito: Eliminar una cancion (por titulo) de una playlist del usuario.
 *            Usa modificarPos() para actualizar el nodo en la lista enlazada.
 * Parametros:
 *   - Usuario* usuario      : propietario de la playlist
 *   - string  nombrePlaylist: nombre de la playlist donde esta la cancion
 *   - string  tituloCancion : titulo exacto de la cancion a eliminar
 * Retorno: bool (true si se elimino, false si no se encontro playlist o cancion)
 *
 * Operaciones internas:
 *   1. Localizar playlist por nombre
 *   2. Llamar playlist.eliminarCancion(titulo)
 *   3. Actualizar nodo con modificarPos()
 *
 * Ejemplo:
 *   eliminarCancionDePlaylist(usuario, "Rock Classics", "Bohemian Rhapsody");
 */
bool eliminarCancionDePlaylist(Usuario* usuario, string nombrePlaylist, string tituloCancion) {
    if (!usuario) return false;

    Lista<Playlist>& pls = usuario->getPlaylists();
    for (uint i = 0; i < pls.longitud(); i++) {
        Playlist p = pls.obtenerPos(i);
        if (p.getNombre() == nombrePlaylist) {
            p.eliminarCancion(tituloCancion);
            pls.modificarPos(i, p);
            return true;
        }
    }
    return false;
}


// ============================================================
// BLOQUE 5: GESTION DE FAVORITOS (3 funciones)
// ============================================================

/*
 * Funcion: agregarAFavoritos()
 * Proposito: Marcar una cancion como favorita para el usuario.
 *            Evita duplicados usando esCancionFavorita() primero.
 * Parametros:
 *   - Usuario* usuario: dueno de la lista de favoritos
 *   - Cancion  cancion: cancion a agregar a favoritos
 * Retorno: bool (true si se agrego, false si ya era favorita o error)
 *
 * Ejemplo:
 *   Cancion c = gCanciones->obtenerPos(sel);
 *   if (!esCancionFavorita(usuario, c)) agregarAFavoritos(usuario, c);
 */
bool agregarAFavoritos(Usuario* usuario, Cancion cancion) {
    if (!usuario) return false;
    if (esCancionFavorita(usuario, cancion)) return false;

    usuario->agregarFavorito(cancion);
    return true;
}

/*
 * Funcion: eliminarDeFavoritos()
 * Proposito: Quitar una cancion de la lista de favoritos del usuario.
 * Parametros:
 *   - Usuario* usuario: dueno de la lista de favoritos
 *   - Cancion  cancion: cancion a eliminar (se busca por titulo)
 * Retorno: bool (true si se elimino, false si no estaba o error)
 *
 * Ejemplo:
 *   eliminarDeFavoritos(usuario, cancionSeleccionada);
 */
bool eliminarDeFavoritos(Usuario* usuario, Cancion cancion) {
    if (!usuario) return false;
    if (!esCancionFavorita(usuario, cancion)) return false;

    usuario->eliminarFavorito(cancion.getTitulo());
    return true;
}

/*
 * Funcion: esCancionFavorita()
 * Proposito: Verificar si una cancion ya esta en la lista de favoritos del usuario.
 *            Busca por titulo de la cancion.
 * Parametros:
 *   - Usuario* usuario: dueno de la lista de favoritos
 *   - Cancion  cancion: cancion a verificar
 * Retorno: bool (true si la cancion esta en favoritos)
 *
 * Ejemplo:
 *   if (esCancionFavorita(usuario, c)) mostrar("<3");
 */
bool esCancionFavorita(Usuario* usuario, Cancion cancion) {
    if (!usuario) return false;
    return usuario->getFavoritos().esFavorita(cancion.getTitulo());
}


// ============================================================
// BLOQUE 6: BUSQUEDA (3 funciones)
// ============================================================

/*
 * Funcion: buscarCancionesPorTitulo()
 * Proposito: Buscar canciones cuyo titulo coincida exactamente con el query.
 *            Retorna una nueva lista con los resultados (el llamador debe hacer delete).
 * Parametros:
 *   - Lista<Cancion>* canciones: lista fuente donde buscar
 *   - string titulo            : texto a comparar con el titulo de cada cancion
 * Retorno: Lista<Cancion>* con las canciones que coinciden (puede estar vacia)
 *
 * Ejemplo:
 *   Lista<Cancion>* res = buscarCancionesPorTitulo(gCanciones, "Thriller");
 *   res->recorrer([](Cancion c){ cout << c.getTitulo(); });
 *   delete res;
 */
Lista<Cancion>* buscarCancionesPorTitulo(Lista<Cancion>* canciones, string titulo) {
    // Delega en Buscador::buscarPorTitulo (busqueda parcial case-insensitive)
    return Buscador::buscarPorTitulo(canciones, titulo);
}

/*
 * Funcion: buscarCancionesPorArtista()
 * Proposito: Buscar canciones cuyo artista coincida con el query.
 *            Retorna una nueva lista con los resultados (el llamador debe hacer delete).
 * Parametros:
 *   - Lista<Cancion>* canciones: lista fuente donde buscar
 *   - string artista           : nombre del artista a buscar
 * Retorno: Lista<Cancion>* con las canciones del artista (puede estar vacia)
 *
 * Ejemplo:
 *   Lista<Cancion>* res = buscarCancionesPorArtista(gCanciones, "Queen");
 *   delete res;
 */
Lista<Cancion>* buscarCancionesPorArtista(Lista<Cancion>* canciones, string artista) {
    // Delega en Buscador::buscarPorArtista (busqueda parcial case-insensitive)
    return Buscador::buscarPorArtista(canciones, artista);
}

/*
 * Funcion: buscarCancionesPorGenero()
 * Proposito: Buscar canciones pertenecientes a un genero musical especifico.
 *            Retorna una nueva lista con los resultados (el llamador debe hacer delete).
 * Parametros:
 *   - Lista<Cancion>* canciones: lista fuente donde buscar
 *   - string genero            : genero musical a filtrar
 * Retorno: Lista<Cancion>* con las canciones del genero (puede estar vacia)
 *
 * Ejemplo:
 *   Lista<Cancion>* res = buscarCancionesPorGenero(gCanciones, "Rock");
 *   delete res;
 */
Lista<Cancion>* buscarCancionesPorGenero(Lista<Cancion>* canciones, string genero) {
    // Delega en Buscador::buscarPorGenero (busqueda parcial case-insensitive)
    return Buscador::buscarPorGenero(canciones, genero);
}


// ============================================================
// BLOQUE 7: REPRODUCCION (6 funciones)
// ============================================================

/*
 * Funcion: iniciarReproduccion()
 * Proposito: Establecer la cancion actual y comenzar la reproduccion.
 *            Actualiza gCancionActual, gPausado y agrega al Reproductor.
 *            Si hay modo aleatorio activo, mezcla gColaActual primero.
 * Parametros:
 *   - Cancion cancion: cancion a reproducir
 * Retorno: void
 *
 * Ejemplo:
 *   iniciarReproduccion(gCanciones->obtenerPos(0));
 */
void iniciarReproduccion(Cancion cancion) {
    gCancionActual = cancion;
    gPausado = false;

    if (gAleatorio && gColaActual && !gColaActual->esVacia())
        mezclarCancionesConFisherYates(gColaActual);

    gReproductor.agregarCancion(cancion);
    gReproductor.play();
}

/*
 * Funcion: pausarReproduccion()
 * Proposito: Pausar la reproduccion activa. No hace nada si ya esta pausada.
 *            Actualiza la bandera global gPausado.
 * Parametros: void
 * Retorno: void
 *
 * Ejemplo:
 *   if (!gPausado) pausarReproduccion();
 */
void pausarReproduccion() {
    if (gPausado) return;
    gPausado = true;
    gReproductor.pause();
}

/*
 * Funcion: reanudarReproduccion()
 * Proposito: Reanudar la reproduccion que estaba en pausa.
 *            No hace nada si no estaba pausada.
 * Parametros: void
 * Retorno: void
 *
 * Ejemplo:
 *   if (gPausado) reanudarReproduccion();
 */
void reanudarReproduccion() {
    if (!gPausado) return;
    gPausado = false;
    gReproductor.play();
}

/*
 * Funcion: siguienteCancion()
 * Proposito: Avanzar a la siguiente cancion en gColaActual.
 *            Actualiza gCancionActual y gIndiceActual.
 *            Si se llega al final de la cola, se detiene.
 * Parametros: void
 * Retorno: void
 *
 * Ejemplo:
 *   siguienteCancion();  // al presionar N
 */
void siguienteCancion() {
    if (!gColaActual || gColaActual->esVacia()) return;

    int total = (int)gColaActual->longitud();
    if (gIndiceActual < total - 1) {
        gIndiceActual++;
        gCancionActual = gColaActual->obtenerPos(gIndiceActual);
        gPausado = false;
        gReproductor.siguiente();
    }
}

/*
 * Funcion: cancionAnterior()
 * Proposito: Retroceder a la cancion anterior en gColaActual.
 *            Si gIndiceActual ya es 0, no hace nada.
 * Parametros: void
 * Retorno: void
 *
 * Ejemplo:
 *   cancionAnterior();  // al presionar B
 */
void cancionAnterior() {
    if (!gColaActual || gColaActual->esVacia()) return;

    if (gIndiceActual > 0) {
        gIndiceActual--;
        gCancionActual = gColaActual->obtenerPos(gIndiceActual);
        gPausado = false;
        gReproductor.anterior();
    }
}

/*
 * Funcion: cambiarVolumen()
 * Proposito: Incrementar o decrementar el volumen global de la aplicacion.
 *            El volumen se mantiene entre 0 y 100.
 * Parametros:
 *   - int delta: cantidad a sumar al volumen (negativo para bajar, positivo para subir)
 * Retorno: void
 *
 * Ejemplo:
 *   cambiarVolumen(10);   // sube 10%
 *   cambiarVolumen(-10);  // baja 10%
 */
void cambiarVolumen(int delta) {
    gVolumen += delta;
    if (gVolumen > 100) gVolumen = 100;
    if (gVolumen < 0) gVolumen = 0;
}


// ============================================================
// BLOQUE 8: ALGORITMOS (2 funciones)
// ============================================================

/*
 * Funcion: mezclarCancionesConFisherYates()
 * Proposito: Reordenar aleatoriamente una lista de canciones usando Fisher-Yates.
 *            Se usa cuando el usuario activa el modo ALEATORIO (shuffle).
 * Parametros:
 *   - Lista<Cancion>* lista: lista de canciones a mezclar (se modifica in-place)
 * Retorno: void
 *
 * Algoritmo Fisher-Yates O(n):
 *   Para i desde n-1 hasta 1:
 *     j = numero aleatorio entre 0 e i
 *     intercambiar lista[i] con lista[j]
 *
 * Ejemplo:
 *   mezclarCancionesConFisherYates(gColaActual);
 */
void mezclarCancionesConFisherYates(Lista<Cancion>* lista) {
    // Delega en Playlist::mezclarFisherYates (implementacion canonica del algoritmo)
    Playlist::mezclarFisherYates(lista);
}

/*
 * Funcion: ordenarCancionesConBubbleSort()
 * Proposito: Ordenar alfabeticamente (A-Z) las canciones de una lista por titulo.
 *            Se usa cuando el usuario selecciona "Ordenar A-Z" en una playlist.
 * Parametros:
 *   - Lista<Cancion>* lista: lista de canciones a ordenar (se modifica in-place)
 * Retorno: void
 *
 * Algoritmo Bubble Sort Optimizado O(n^2) peor caso, O(n) mejor caso:
 *   Si en un pase completo no hubo intercambios, la lista ya esta ordenada.
 *
 * Ejemplo:
 *   ordenarCancionesConBubbleSort(&playlist.getCanciones());
 */
void ordenarCancionesConBubbleSort(Lista<Cancion>* lista) {
    // Delega en Playlist::ordenarBubbleSort (implementacion canonica del algoritmo)
    Playlist::ordenarBubbleSort(lista);
}


// ============================================================
// BLOQUE 9: SINCRONIZACION CON DISCO (3 funciones)
// ============================================================

/*
 * Funcion: guardarUsuarioADisco()
 * Proposito: Persistir todos los datos del usuario (playlists + favoritos) a disco.
 *            Delega en GestorUsuarios::guardarUsuarioActual().
 * Parametros:
 *   - Usuario* usuario: puntero al usuario cuya sesion se guardara
 * Retorno: bool (true si guardo correctamente)
 *
 * Archivos escritos:
 *   usuarios_data/usuario_[id].txt
 *
 * Ejemplo:
 *   guardarUsuarioADisco(gUsuarioActual);
 */
bool guardarUsuarioADisco(Usuario* usuario) {
    if (!usuario) return false;
    gGestor.setUsuarioActual(usuario);
    gGestor.guardarUsuarioActual();
    return true;
}

/*
 * Funcion: guardarPlaylistADisco()
 * Proposito: Guardar los datos de una playlist especifica del usuario.
 *            En la implementacion actual guarda todo el usuario (la estructura
 *            de GestorUsuarios no permite guardar una playlist de forma aislada).
 * Parametros:
 *   - Usuario*  usuario : propietario de la playlist
 *   - Playlist  playlist: objeto playlist a guardar (parametro informativo)
 * Retorno: bool (true si guardo correctamente)
 *
 * Ejemplo:
 *   Playlist pl = usuario->buscarPlaylist("Rock");
 *   guardarPlaylistADisco(usuario, pl);
 */
bool guardarPlaylistADisco(Usuario* usuario, Playlist playlist) {
    if (!usuario) return false;
    gGestor.setUsuarioActual(usuario);
    gGestor.guardarUsuarioActual();
    return true;
}

/*
 * Funcion: guardarFavoritosADisco()
 * Proposito: Persistir la lista de favoritos del usuario a disco.
 *            En la implementacion actual guarda todo el usuario (favoritos
 *            estan en el mismo archivo que las playlists).
 * Parametros:
 *   - Usuario* usuario: dueno de los favoritos a guardar
 * Retorno: bool (true si guardo correctamente)
 *
 * Ejemplo:
 *   guardarFavoritosADisco(gUsuarioActual);
 */
bool guardarFavoritosADisco(Usuario* usuario) {
    if (!usuario) return false;
    gGestor.setUsuarioActual(usuario);
    gGestor.guardarUsuarioActual();
    return true;
}


// ============================================================
// MAIN
// ============================================================

int main() {
    Console::Clear();
    Console::CursorVisible = false;

    try {
        Console::SetWindowSize(100, 36);
        Console::SetBufferSize(100, 36);
    }
    catch (...) {}

    // 1. Crear estructura de carpetas y archivos base si no existen
    crearEstructuraCarpetas();

    // 2. Inicializar listas globales
    gCanciones = new Lista<Cancion>();
    gAlbumes = new Lista<Album>();
    gColaActual = new Lista<Cancion>();

    // 3. Cargar datos del disco a memoria
    cargarAlmacenCanciones(gCanciones);
    cargarAlbumesDelDisco(gAlbumes);

    // Loop principal: inicio de sesion
    while (true) {
        int opcion = mostrarPantallaInicio();

        if (opcion == 0) {
            // LOGIN
            if (mostrarPantallaLogin() == 1 && gUsuarioActual) {
                mostrarMenuPrincipal(gUsuarioActual);
                delete gUsuarioActual;
                gUsuarioActual = nullptr;
            }
        }
        else if (opcion == 1) {
            // REGISTRO
            if (mostrarPantallaRegistro() == 1 && gUsuarioActual) {
                mostrarMenuPrincipal(gUsuarioActual);
                delete gUsuarioActual;
                gUsuarioActual = nullptr;
            }
        }
        else {
            // SALIR
            Console::Clear();
            Imprimir(35, 12, "Gracias por usar Spotify en Consola!", ConsoleColor::Green);
            Imprimir(35, 14, "Presiona ENTER para salir...", ConsoleColor::DarkGray);
            while (Console::ReadKey(true).Key != ConsoleKey::Enter) {}
            break;
        }
    }

    // Limpieza
    delete gCanciones;
    delete gAlbumes;
    delete gColaActual;

    return 0;
}