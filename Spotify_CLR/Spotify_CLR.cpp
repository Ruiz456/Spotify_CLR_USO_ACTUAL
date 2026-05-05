#include "pch.h"
#include <iostream>
#include <windows.h>
#include "BibliotecaMusical.h"
#include "Reproductor.h"
#include "FiltroCanciones.h"
#include "MotorRecomendacion.h"
#include "Favoritos.h"
#include "Usuario.h"
#include "Buscador.h"
#include "GestorPlaylists.h"
#include <vector>

using namespace System;
using namespace std;

struct OpcionMouse
{
    string texto;
    int x;
    int y;
    int ancho;
    int id;
};

vector<OpcionMouse> opciones;

void DibujarOpcion(OpcionMouse o, bool hover)
{
    Console::SetCursorPosition(o.x, o.y);

    if (hover) {
        Console::ForegroundColor = ConsoleColor::Black;
        Console::BackgroundColor = ConsoleColor::Green;
    }
    else {
        Console::ForegroundColor = ConsoleColor::White;
        Console::BackgroundColor = ConsoleColor::Black;
    }

    Console::Write(" ");
    Console::Write(gcnew String(o.texto.c_str()));
    Console::Write(" ");

    Console::ResetColor();
}

void AgregarOpcion(string texto, int x, int y, int id)
{
    OpcionMouse o;
    o.texto = texto;
    o.x = x;
    o.y = y;
    o.ancho = (int)texto.length() + 2;
    o.id = id;

    opciones.push_back(o);

    DibujarOpcion(o, false);
}

int ProcesarMouse(HANDLE hIn)
{
    static int oldHover = -2;

    INPUT_RECORD input;
    DWORD eventos;

    ReadConsoleInput(hIn, &input, 1, &eventos);

    if (input.EventType != MOUSE_EVENT)
        return -1;

    int mx = input.Event.MouseEvent.dwMousePosition.X;
    int my = input.Event.MouseEvent.dwMousePosition.Y;

    int hover = -1;

    for (int i = 0; i < (int)opciones.size(); i++)
    {
        OpcionMouse o = opciones[i];

        if (my == o.y && mx >= o.x && mx < o.x + o.ancho) {
            hover = i;
            break;
        }
    }

    if (hover != oldHover)
    {
        for (int i = 0; i < (int)opciones.size(); i++)
            DibujarOpcion(opciones[i], i == hover);

        oldHover = hover;
    }

    if (input.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        if (hover != -1)
            return opciones[hover].id;
    }

    return -1;
}

int main()
{
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD mode;
    GetConsoleMode(hIn, &mode);

    mode |= ENABLE_MOUSE_INPUT;
    mode &= ~ENABLE_QUICK_EDIT_MODE;

    SetConsoleMode(hIn, mode);

    Console::Clear();

    AgregarOpcion("Inicio", 5, 5, 10);
    AgregarOpcion("Buscar", 18, 5, 20);
    AgregarOpcion("Playlists", 31, 5, 30);

    while (true)
    {
        int accion = ProcesarMouse(hIn);

        if (accion == 10) {
            Console::SetCursorPosition(5, 8);
            Console::Write("Click en Inicio       ");
        }
        else if (accion == 20) {
            Console::Clear();

            opciones.clear();

            string matriz[15] = {
                "+----------------------------------------------------------+",
                "| Buscar                                                   |",
                "| Ingrese lo que quiere buscar:                            |",
                "| -------------------------------------------------------- |",
                "| Resultados principales                                   |",
                "|                                                          |",
                "|                                                          |",
                "|                                                          |",
                "|                                                          |",
                "|                                                          |",
                "|                                                          |",
                "|                                                          |",
                "|                                                          |",
                "|  Esc: Volver    Click: Seleccionar                       |",
                "+----------------------------------------------------------+"
            };

            for (int i = 0; i < 15; i++)
                Console::WriteLine(gcnew String(matriz[i].c_str()));

            string canciones[4] = {
                "Blinding Lights",
                "Save Your Tears",
                "Starboy",
                "Labyrinth"
            };

            Buscador::dibujarResultados(canciones, 3, 4, 4);

            for (int i = 0; i < 3; i++)
            {
                AgregarOpcion(canciones[i], 4, 4 + i, i + 1);
            }
            AgregarOpcion("Buscar musica", 2, 1, 10);


        }
        else if (accion == 30) {
            Console::SetCursorPosition(5, 8);
            Console::Write("Click en Playlists    ");
        }
    }

    return 0;
}