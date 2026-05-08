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
// -- Usuarios --
bool    crearNuevoUsuario(string nombre, string email);
Usuario* loginUsuario(string nombre);
// -- Disco --
void    crearEstructuraCarpetas();
bool    cargarAlmacenCanciones(Lista<Cancion>* canciones);
bool    cargarAlbumesDelDisco(Lista<Album>* albumes);
void    guardarADisco(Usuario* usuario);
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
void    mostrarDetallePlaylist(Usuario* usuarioActual, string nombrePL);

// ============================================================
// UTILIDADES DE UI  (no cuentan en las 36 funciones)
// ============================================================

// Filas validas: 0 a MAX_ROW (console buffer = 36 filas → 0..35)
static const int MAX_ROW = 35;
static const int MAX_COL = 99;

void gotoxy(int x, int y) {
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > MAX_COL) x = MAX_COL;
    if (y > MAX_ROW) y = MAX_ROW;
    Console::SetCursorPosition(x, y);
}

void Imprimir(int x, int y, string txt, ConsoleColor color = ConsoleColor::White) {
    if (y < 0 || y > MAX_ROW) return;   // fila fuera de rango: ignorar silenciosamente
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
    int sel   = 0;
    int n     = (int)opciones.size();
    if (n == 0) return -1;

    // Cuantas filas hay desde y hasta la fila 27 (la 28 es footer de cada tab)
    int maxVisible = 27 - y + 1;          // ej: y=6 → maxVisible=22
    if (maxVisible < 1)  maxVisible = 1;
    if (maxVisible > n)  maxVisible = n;

    int offset = 0;   // primer indice visible

    if (titulo != "") Imprimir(x, y - 2, titulo, ConsoleColor::Yellow);

    // Limpia la zona del menu antes de dibujar
    string blancos(50, ' ');
    auto limpiarMenu = [&]() {
        for (int i = 0; i < maxVisible + 2; i++)
            Imprimir(x, y - 1 + i, blancos);
    };

    auto dibujar = [&]() {
        limpiarMenu();
        // Indicador "mas arriba"
        if (offset > 0)
            Imprimir(x, y - 1, "  ^ mas resultados ^", ConsoleColor::DarkGray);

        for (int i = 0; i < maxVisible; i++) {
            int idx = offset + i;
            if (idx >= n) break;
            string linea = Truncar(opciones[idx].texto, 46);
            if (idx == sel)
                Imprimir(x, y + i, "> " + linea, ConsoleColor::Green);
            else
                Imprimir(x, y + i, "  " + linea, ConsoleColor::White);
        }

        // Indicador "mas abajo"
        if (offset + maxVisible < n)
            Imprimir(x, y + maxVisible, "  v mas resultados v", ConsoleColor::DarkGray);
    };

    dibujar();

    while (true) {
        ConsoleKeyInfo t = Console::ReadKey(true);

        if (t.Key == ConsoleKey::UpArrow) {
            if (sel > 0) {
                sel--;
                if (sel < offset) offset = sel;
                dibujar();
            }
        }
        else if (t.Key == ConsoleKey::DownArrow) {
            if (sel < n - 1) {
                sel++;
                if (sel >= offset + maxVisible) offset = sel - maxVisible + 1;
                dibujar();
            }
        }
        else if (t.Key == ConsoleKey::Enter)  return opciones[sel].id;
        else if (t.Key == ConsoleKey::Escape) return -1;
    }
}

// -- Helpers reutilizables --

// Maneja flechas <- -> para cambiar tab y Q para logout. Retorna true si el tab debe retornar.
bool ManejarNavTab(ConsoleKeyInfo t) {
    if (t.Key == ConsoleKey::LeftArrow)  { gTabActual = (gTabActual + 6) % 7; return true; }
    if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return true; }
    if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return true; }
    return false;
}

// Carga canciones en la cola global desde 'desde' y reproduce la primera.
void reproducirDesde(Lista<Cancion>* fuente, int desde) {
    delete gColaActual;
    gColaActual = new Lista<Cancion>();
    gIndiceActual = 0;
    for (int i = desde; i < (int)fuente->longitud(); i++)
        gColaActual->agregarFinal(fuente->obtenerPos(i));
    iniciarReproduccion(fuente->obtenerPos(desde));
}

// Muestra menu con canciones de una lista. Retorna indice elegido o -1.
int MenuSeleccionCancion(Lista<Cancion>* lista, string titulo) {
    LimpiarZona(23, 3, 75, 28);
    vector<OpcionMenu> ops;
    int idx = 0;
    lista->recorrer([&](Cancion c) {
        ops.push_back({ Truncar(c.getTitulo(), 28) + " - " + Truncar(c.getArtista(), 18), idx++ });
    });
    ops.push_back({ "Cancelar", -1 });
    return MenuFlechas(ops, 26, 6, titulo);
}

// Muestra menu SI/NO. Retorna true si el usuario eligio SI.
bool MenuConfirmacion(int x, int y, string pregunta) {
    vector<OpcionMenu> ops = { {"SI", 1}, {"NO, cancelar", 0} };
    return MenuFlechas(ops, x, y, pregunta) == 1;
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
// FUNCIONES PRINCIPALES
// ============================================================

// Pantalla inicial: LOGIN(0), REGISTRO(1) o SALIR(2)
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

// Login: solicita nombre, valida, carga datos. Retorna 1=exito, 0=fallo
int mostrarPantallaLogin() {
    Console::Clear();
    Imprimir(30, 2, string(42, '='), ConsoleColor::Green);
    Imprimir(36, 3, "INICIAR SESION", ConsoleColor::White);
    Imprimir(30, 4, string(42, '='), ConsoleColor::Green);

    string nombre = LeerTextoSinIgnore(30, 7, "Nombre de usuario: ");

    if (nombre.empty()) return 0;

    if (!gGestor.existeNombre(nombre)) {
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

// Registro: solicita nombre/email, crea cuenta y login automatico. Retorna 1=exito, 0=fallo
int mostrarPantallaRegistro() {
    Console::Clear();
    Imprimir(30, 2, string(42, '='), ConsoleColor::Green);
    Imprimir(36, 3, "CREAR CUENTA", ConsoleColor::White);
    Imprimir(30, 4, string(42, '='), ConsoleColor::Green);

    string nombre = LeerTextoSinIgnore(30, 7, "Nombre de usuario: ");
    if (nombre.empty()) return 0;

    if (gGestor.existeNombre(nombre)) {
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

// Menu principal: loop de 7 tabs, dibuja shell y delega al tab activo. Q=logout.
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

    guardarADisco(usuarioActual);
}

// Tab Biblioteca: lista todas las canciones. ENTER=reproducir, F=favorito, A=agregar a playlist
void mostrarTabBiblioteca(Usuario* usuarioActual) {
    if (!gCanciones || gCanciones->esVacia()) {
        Imprimir(24, 4, "Sin canciones. Agregue datos en almacenCanciones.txt", ConsoleColor::DarkGray);
        while (true) {
            ConsoleKeyInfo t = Console::ReadKey(true);
            if (ManejarNavTab(t)) return;
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

            bool esFav = usuarioActual->getFavoritos().esFavorita(c.getTitulo());
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
        else if (ManejarNavTab(t)) return;

        else if (t.Key == ConsoleKey::Enter) {
            reproducirDesde(gCanciones, sel);
            DibujarShell(usuarioActual);
        }
        else if ((char)tolower((int)t.KeyChar) == 'f') {
            Cancion c = gCanciones->obtenerPos(sel);
            if (usuarioActual->getFavoritos().esFavorita(c.getTitulo()))
                usuarioActual->eliminarFavorito(c.getTitulo());
            else
                usuarioActual->agregarFavorito(c);
            guardarADisco(usuarioActual);
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
                    GestorPlaylists::agregarCancionAPlaylist(*usuarioActual, nombrePL, c);
                    guardarADisco(usuarioActual);
                }
                DibujarShell(usuarioActual);
            }
        }
    }
}

// Tab Buscar: busqueda por titulo/artista/genero con Buscador. NUM=reproducir resultado
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

        if (ManejarNavTab(t)) { delete resultados; return; }

        // Cambiar tipo con 1/2/3
        if (t.KeyChar == '1') tipoBusqueda = 0;
        else if (t.KeyChar == '2') tipoBusqueda = 1;
        else if (t.KeyChar == '3') tipoBusqueda = 2;

        else if (t.Key == ConsoleKey::Enter) {
            // Solicitar query
            LimpiarZona(24, 6, 72, 1);
            query = LeerTextoSinIgnore(24, 6, "Busqueda: ");

            delete resultados;
            if (tipoBusqueda == 0) resultados = Buscador::buscarPorTitulo(gCanciones, query);
            else if (tipoBusqueda == 1) resultados = Buscador::buscarPorArtista(gCanciones, query);
            else                        resultados = Buscador::buscarPorGenero(gCanciones, query);

            DibujarShell(usuarioActual);
        }
        // Reproducir resultado por numero
        else if (t.KeyChar >= '1' && t.KeyChar <= '9' && resultados) {
            int idx = (int)(t.KeyChar - '1');
            if (idx < (int)resultados->longitud()) {
                reproducirDesde(resultados, idx);
                DibujarShell(usuarioActual);
            }
        }
    }
}

// Tab Playlists: listar, crear(C), eliminar(D), ver detalle(ENTER)
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

        if (ManejarNavTab(t)) return;

        if (t.Key == ConsoleKey::UpArrow) { if (sel > 0) sel--; }
        if (t.Key == ConsoleKey::DownArrow) { if (sel < total - 1) sel++; }

        // Crear playlist
        if ((char)tolower((int)t.KeyChar) == 'c') {
            LimpiarZona(23, 3, 75, 28);
            string nombre = LeerTexto(24, 5, "Nombre de la nueva playlist: ");
            if (!nombre.empty()) {
                usuarioActual->crearPlaylist(Playlist(nombre));
                guardarADisco(usuarioActual);
                DibujarShell(usuarioActual);
            }
        }

        // Eliminar playlist
        else if ((char)tolower((int)t.KeyChar) == 'd' && total > 0) {
            Playlist p = usuarioActual->getPlaylists().obtenerPos(sel);
            LimpiarZona(23, 20, 75, 8);
            if (MenuConfirmacion(28, 22, "Eliminar playlist: " + p.getNombre() + "?")) {
                usuarioActual->eliminarPlaylist(p.getNombre());
                guardarADisco(usuarioActual);
                sel = max(0, sel - 1);
            }
            DibujarShell(usuarioActual);
        }

        // Ver playlist
        else if (t.Key == ConsoleKey::Enter && total > 0) {
            string nombrePL = usuarioActual->getPlaylists().obtenerPos(sel).getNombre();
            mostrarDetallePlaylist(usuarioActual, nombrePL);
            DibujarShell(usuarioActual);
        }
    }
}

// Sub-pantalla de playlist: A=agregar, D=eliminar, S=shuffle, O=ordenar, P=play, ESC=volver
void mostrarDetallePlaylist(Usuario* usuarioActual, string nombrePL) {
    while (true) {
        LimpiarZona(23, 3, 75, 28);
        Imprimir(24, 3, "PLAYLIST: " + nombrePL, ConsoleColor::Yellow);
        Imprimir(24, 4, string(72, '-'), ConsoleColor::DarkGreen);

        Playlist plActual = usuarioActual->buscarPlaylist(nombrePL);
        int totalC = (int)plActual.getCanciones().longitud();
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

        char k = (char)tolower((int)tc.KeyChar);

        if (k == 'a') {
            if (!gCanciones || gCanciones->esVacia()) {
                Imprimir(24, 29, "No hay canciones en el almacen.", ConsoleColor::Red);
                Console::ReadKey(true);
            }
            else {
                int elegida = MenuSeleccionCancion(gCanciones, "Selecciona cancion a agregar:");
                if (elegida >= 0) {
                    GestorPlaylists::agregarCancionAPlaylist(*usuarioActual, nombrePL, gCanciones->obtenerPos(elegida));
                    guardarADisco(usuarioActual);
                }
                DibujarShell(usuarioActual);
            }
        }
        else if (k == 'd' && totalC > 0) {
            Lista<Cancion>& cancs = plActual.getCanciones();
            int elegida = MenuSeleccionCancion(&cancs, "Selecciona cancion a eliminar:");
            if (elegida >= 0) {
                GestorPlaylists::eliminarCancionDePlaylist(*usuarioActual, nombrePL, cancs.obtenerPos(elegida).getTitulo());
                guardarADisco(usuarioActual);
            }
            DibujarShell(usuarioActual);
        }
        else if (k == 's') {
            Lista<Playlist>& pls2 = usuarioActual->getPlaylists();
            for (uint i = 0; i < pls2.longitud(); i++) {
                Playlist pRef = pls2.obtenerPos(i);
                if (pRef.getNombre() == nombrePL) {
                    Playlist::mezclarFisherYates(&pRef.getCanciones());
                    pls2.modificarPos(i, pRef);
                    break;
                }
            }
            guardarADisco(usuarioActual);
            Imprimir(24, 29, "Playlist mezclada!", ConsoleColor::Green);
            Console::ReadKey(true);
        }
        else if (k == 'o') {
            Lista<Playlist>& pls2 = usuarioActual->getPlaylists();
            for (uint i = 0; i < pls2.longitud(); i++) {
                Playlist pRef = pls2.obtenerPos(i);
                if (pRef.getNombre() == nombrePL) {
                    Playlist::ordenarBubbleSort(&pRef.getCanciones());
                    pls2.modificarPos(i, pRef);
                    break;
                }
            }
            guardarADisco(usuarioActual);
            Imprimir(24, 29, "Playlist ordenada A-Z!", ConsoleColor::Green);
            Console::ReadKey(true);
        }
        else if (k == 'p' && totalC > 0) {
            delete gColaActual;
            gColaActual = new Lista<Cancion>();
            gIndiceActual = 0;
            plActual.getCanciones().recorrer([&](Cancion c) {
                gColaActual->agregarFinal(c);
                });
            iniciarReproduccion(gColaActual->obtenerPos(0));
            DibujarShell(usuarioActual);
        }
    }
}

// Tab Albumes: lista albumes con tracklist preview. ENTER=reproducir album
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

        if (ManejarNavTab(t)) return;

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

// Tab Artistas: lista artistas unicos. ENTER=ver canciones, NUM=reproducir
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

        if (ManejarNavTab(t)) return;

        if (t.Key == ConsoleKey::UpArrow) { if (sel > 0) sel--; }
        if (t.Key == ConsoleKey::DownArrow) { if (sel < total - 1) sel++; }

        if (t.Key == ConsoleKey::Enter && total > 0) {
            string artista = artistas[sel];
            Lista<Cancion>* cancArtista = Buscador::buscarPorArtista(gCanciones, artista);

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
                        reproducirDesde(cancArtista, idx);
                        if (gUsuarioActual) DibujarShell(gUsuarioActual);
                        break;
                    }
                }
            }
            delete cancArtista;
        }
    }
}

// Tab Favoritos: muestra favoritos en LIFO. NUM=reproducir, D=eliminar
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

        if (ManejarNavTab(t)) return;

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
                usuarioActual->eliminarFavorito(cElim.getTitulo());
                guardarADisco(usuarioActual);
            }
            DibujarShell(usuarioActual);
        }
    }
}

// Tab Configuracion: perfil, volumen, modo aleatorio, eliminar cuenta
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
            LimpiarZona(23, 3, 75, 28);
            if (MenuConfirmacion(28, 10, "Eliminar cuenta " + usuarioActual->getNombre() + "?")) {
                gGestor.eliminar(usuarioActual->getId());
                gMenuActivo = false;
                return;
            }
        }
        else if (accion == 5 || accion == -1) {
            guardarADisco(usuarioActual);
            gTabActual = (gTabActual + 6) % 7;
            return;
        }

        DibujarShell(usuarioActual);
    }
}


// Crea carpetas usuarios_data/, albumes/ y almacenCanciones.txt si no existen
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

// Lee almacenCanciones.txt (Titulo|Artista|Genero|Duracion) y puebla la lista
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

// Lee albumes/[nombre]/config.txt y canciones.txt para cargar cada album
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


// Registra usuario nuevo con nombre y email. No hace login automatico.
bool crearNuevoUsuario(string nombre, string email) {
    if (nombre.empty() || email.empty()) return false;
    if (gGestor.existeNombre(nombre))  return false;

    gGestor.registrar(nombre, email);
    return gGestor.existeNombre(nombre);
}

// Busca usuario por nombre, carga playlists/favoritos. Retorna puntero (delete responsabilidad del llamador)
Usuario* loginUsuario(string nombre) {
    Usuario encontrado = gGestor.buscarPorNombre(nombre);
    if (encontrado.getId() == 0) return nullptr;

    Usuario* ptr = new Usuario(encontrado);
    return ptr;
}


// Establece cancion actual, mezcla cola si modo aleatorio, y llama play()
void iniciarReproduccion(Cancion cancion) {
    gCancionActual = cancion;
    gPausado = false;

    if (gAleatorio && gColaActual && !gColaActual->esVacia())
        Playlist::mezclarFisherYates(gColaActual);

    gReproductor.agregarCancion(cancion);
    gReproductor.play();
}

// Pausa la reproduccion (no-op si ya pausada)
void pausarReproduccion() {
    if (gPausado) return;
    gPausado = true;
    gReproductor.pause();
}

// Reanuda la reproduccion (no-op si no pausada)
void reanudarReproduccion() {
    if (!gPausado) return;
    gPausado = false;
    gReproductor.play();
}

// Avanza a la siguiente cancion en gColaActual
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

// Retrocede a la cancion anterior en gColaActual
void cancionAnterior() {
    if (!gColaActual || gColaActual->esVacia()) return;

    if (gIndiceActual > 0) {
        gIndiceActual--;
        gCancionActual = gColaActual->obtenerPos(gIndiceActual);
        gPausado = false;
        gReproductor.anterior();
    }
}

// Sube o baja el volumen (clamped 0-100)
void cambiarVolumen(int delta) {
    gVolumen += delta;
    if (gVolumen > 100) gVolumen = 100;
    if (gVolumen < 0) gVolumen = 0;
}


// Persiste todos los datos del usuario a disco (config, playlists, favoritos)
void guardarADisco(Usuario* usuario) {
    if (!usuario) return;
    gGestor.setUsuarioActual(usuario);
    gGestor.guardarUsuarioActual();
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