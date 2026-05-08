#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Globales.h"

using namespace System;
using namespace std;

// Filas validas: 0 a MAX_ROW (console buffer = 36 filas -> 0..35)
static const int MAX_ROW = 35;
static const int MAX_COL = 99;

// Posiciona el cursor con proteccion de limites
void gotoxy(int x, int y) {
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > MAX_COL) x = MAX_COL;
    if (y > MAX_ROW) y = MAX_ROW;
    Console::SetCursorPosition(x, y);
}

// Imprime texto en (x,y) con color
void Imprimir(int x, int y, string txt, ConsoleColor color = ConsoleColor::White) {
    if (y < 0 || y > MAX_ROW) return;
    gotoxy(x, y);
    Console::ForegroundColor = color;
    Console::Write(gcnew String(txt.c_str()));
    Console::ResetColor();
}

// Limpia una zona rectangular de la consola
void LimpiarZona(int x, int y, int ancho, int alto) {
    string linea(ancho, ' ');
    for (int row = 0; row < alto; row++)
        Imprimir(x, y + row, linea);
}

// Trunca string a maxLen caracteres, agrega ".." si excede
string Truncar(string s, int maxLen) {
    return (s.size() > (size_t)maxLen) ? s.substr(0, maxLen - 2) + ".." : s;
}

// Lee texto del usuario con etiqueta, hace ignore de newline previo
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

// Lee texto del usuario sin ignorar newline previo
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

// Menu con flechas arriba/abajo, scroll si excede filas visibles. Retorna id o -1
int MenuFlechas(vector<OpcionMenu>& opciones, int x, int y, string titulo = "") {
    int sel   = 0;
    int n     = (int)opciones.size();
    if (n == 0) return -1;

    int maxVisible = 27 - y + 1;
    if (maxVisible < 1)  maxVisible = 1;
    if (maxVisible > n)  maxVisible = n;

    int offset = 0;

    if (titulo != "") Imprimir(x, y - 2, titulo, ConsoleColor::Yellow);

    string blancos(50, ' ');
    auto limpiarMenu = [&]() {
        for (int i = 0; i < maxVisible + 2; i++)
            Imprimir(x, y - 1 + i, blancos);
    };

    auto dibujar = [&]() {
        limpiarMenu();
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

// Maneja flechas <- -> para cambiar tab y Q para logout. Retorna true si el tab debe retornar.
bool ManejarNavTab(ConsoleKeyInfo t) {
    if (t.Key == ConsoleKey::LeftArrow)  { gTabActual = (gTabActual + 6) % 7; return true; }
    if (t.Key == ConsoleKey::RightArrow) { gTabActual = (gTabActual + 1) % 7; return true; }
    if ((char)tolower((int)t.KeyChar) == 'q') { gMenuActivo = false; return true; }
    return false;
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
