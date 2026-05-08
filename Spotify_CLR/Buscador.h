#pragma once
#include <functional>
#include "Lista.h"
#include "Cancion.h"

using namespace System;
using namespace std;

/*
 * Clase: Buscador
 * Proposito: Coleccion de metodos estaticos para buscar canciones dentro de
 *            una Lista<Cancion>. Ofrece dos familias de metodos:
 *              1. Por referencia (retornan Lista<Cancion> por valor) — busqueda exacta.
 *              2. Por puntero   (retornan Lista<Cancion>* dinamico) — busqueda parcial
 *                 e insensible a mayusculas. El llamador es responsable de hacer delete.
 */
class Buscador {
public:

    // ----------------------------------------------------------------
    // FAMILIA 1: busqueda generica con criterio lambda (retorna por valor)
    // ----------------------------------------------------------------

    /*
     * Funcion: buscar()
     * Proposito: Filtrar canciones de una lista usando un criterio lambda generico.
     *            Base para porTitulo(), porArtista() y porGenero().
     * Parametros:
     *   - Lista<Cancion>& lista          : lista fuente (por referencia, no se modifica)
     *   - function<bool(Cancion)> criterio: lambda que retorna true si la cancion cumple
     * Retorno: Lista<Cancion> nueva con las canciones que cumplen el criterio
     *
     * Ejemplo:
     *   auto res = Buscador::buscar(lista, [](Cancion c){ return c.getDuracion() > 4; });
     */
    static Lista<Cancion> buscar(Lista<Cancion>& lista, function<bool(Cancion)> criterio) {
        Lista<Cancion> resultado;

        lista.recorrer([&](Cancion c) {
            if (criterio(c))
                resultado.agregarFinal(c);
            });

        return resultado;
    }

    /*
     * Funcion: porTitulo()
     * Proposito: Buscar canciones cuyo titulo sea identico al parametro (case-sensitive exacto).
     * Parametros:
     *   - Lista<Cancion>& lista: lista fuente
     *   - string titulo        : titulo exacto a comparar
     * Retorno: Lista<Cancion> con las canciones coincidentes
     *
     * Ejemplo:
     *   Lista<Cancion> res = Buscador::porTitulo(lista, "Thriller");
     */
    static Lista<Cancion> porTitulo(Lista<Cancion>& lista, string titulo) {
        return buscar(lista, [=](Cancion c) {
            return c.getTitulo() == titulo;
            });
    }

    /*
     * Funcion: porArtista()
     * Proposito: Buscar canciones cuyo artista sea identico al parametro (case-sensitive exacto).
     * Parametros:
     *   - Lista<Cancion>& lista: lista fuente
     *   - string artista       : nombre exacto del artista
     * Retorno: Lista<Cancion> con las canciones del artista
     *
     * Ejemplo:
     *   Lista<Cancion> res = Buscador::porArtista(lista, "Queen");
     */
    static Lista<Cancion> porArtista(Lista<Cancion>& lista, string artista) {
        return buscar(lista, [=](Cancion c) {
            return c.getArtista() == artista;
            });
    }

    /*
     * Funcion: porGenero()
     * Proposito: Buscar canciones cuyo genero sea identico al parametro (case-sensitive exacto).
     * Parametros:
     *   - Lista<Cancion>& lista: lista fuente
     *   - string genero        : genero exacto a comparar
     * Retorno: Lista<Cancion> con las canciones del genero
     *
     * Ejemplo:
     *   Lista<Cancion> res = Buscador::porGenero(lista, "Rock");
     */
    static Lista<Cancion> porGenero(Lista<Cancion>& lista, string genero) {
        return buscar(lista, [=](Cancion c) {
            return c.getGenero() == genero;
            });
    }

    // ----------------------------------------------------------------
    // FAMILIA 2: busqueda parcial e insensible a mayusculas (retorna puntero)
    //            El llamador debe hacer delete sobre el puntero retornado.
    // ----------------------------------------------------------------

    /*
     * Funcion: buscarPorTitulo()
     * Proposito: Buscar canciones cuyo titulo contenga el texto buscado,
     *            sin importar mayusculas/minusculas (busqueda parcial case-insensitive).
     *            Retorna un puntero dinamico; el llamador debe hacer delete.
     * Parametros:
     *   - Lista<Cancion>* lista: lista fuente (puntero, puede ser nullptr)
     *   - string query         : texto a buscar dentro del titulo
     * Retorno: Lista<Cancion>* nueva con las coincidencias (nunca nullptr)
     *
     * Ejemplo:
     *   Lista<Cancion>* res = Buscador::buscarPorTitulo(gCanciones, "thrill");
     *   // encuentra "Thriller", "Chiller Thriller", etc.
     *   delete res;
     */
    static Lista<Cancion>* buscarPorTitulo(Lista<Cancion>* lista, string query) {
        Lista<Cancion>* resultado = new Lista<Cancion>();
        if (!lista) return resultado;

        string q = toLower(query);
        lista->recorrer([&](Cancion c) {
            string t = toLower(c.getTitulo());
            if (t.find(q) != string::npos)
                resultado->agregarFinal(c);
            });
        return resultado;
    }

    /*
     * Funcion: buscarPorArtista()
     * Proposito: Buscar canciones cuyo artista contenga el texto buscado,
     *            sin importar mayusculas/minusculas (busqueda parcial case-insensitive).
     *            Retorna un puntero dinamico; el llamador debe hacer delete.
     * Parametros:
     *   - Lista<Cancion>* lista: lista fuente (puntero, puede ser nullptr)
     *   - string query         : texto a buscar dentro del nombre del artista
     * Retorno: Lista<Cancion>* nueva con las coincidencias (nunca nullptr)
     *
     * Ejemplo:
     *   Lista<Cancion>* res = Buscador::buscarPorArtista(gCanciones, "queen");
     *   delete res;
     */
    static Lista<Cancion>* buscarPorArtista(Lista<Cancion>* lista, string query) {
        Lista<Cancion>* resultado = new Lista<Cancion>();
        if (!lista) return resultado;

        string q = toLower(query);
        lista->recorrer([&](Cancion c) {
            string a = toLower(c.getArtista());
            if (a.find(q) != string::npos)
                resultado->agregarFinal(c);
            });
        return resultado;
    }

    /*
     * Funcion: buscarPorGenero()
     * Proposito: Buscar canciones cuyo genero contenga el texto buscado,
     *            sin importar mayusculas/minusculas (busqueda parcial case-insensitive).
     *            Retorna un puntero dinamico; el llamador debe hacer delete.
     * Parametros:
     *   - Lista<Cancion>* lista: lista fuente (puntero, puede ser nullptr)
     *   - string query         : texto a buscar dentro del genero musical
     * Retorno: Lista<Cancion>* nueva con las coincidencias (nunca nullptr)
     *
     * Ejemplo:
     *   Lista<Cancion>* res = Buscador::buscarPorGenero(gCanciones, "pop");
     *   delete res;
     */
    static Lista<Cancion>* buscarPorGenero(Lista<Cancion>* lista, string query) {
        Lista<Cancion>* resultado = new Lista<Cancion>();
        if (!lista) return resultado;

        string q = toLower(query);
        lista->recorrer([&](Cancion c) {
            string g = toLower(c.getGenero());
            if (g.find(q) != string::npos)
                resultado->agregarFinal(c);
            });
        return resultado;
    }

    // ----------------------------------------------------------------
    // UTILIDADES DE DIBUJO
    // ----------------------------------------------------------------

    /*
     * Funcion: dibujarResultados()
     * Proposito: Imprimir un arreglo de strings de resultados en la consola
     *            en la posicion (x, y) indicada, numerados desde 1.
     * Parametros:
     *   - string canciones[]: arreglo de strings con los textos a mostrar
     *   - int cantidad       : cuantos elementos tiene el arreglo
     *   - int x, int y       : columna y fila de inicio en la consola
     * Retorno: void
     *
     * Ejemplo:
     *   Buscador::dibujarResultados(lineas, n, 26, 6);
     */
    static void dibujarResultados(string canciones[], int cantidad, int x, int y) {
        for (int i = 0; i < cantidad; i++) {
            Console::SetCursorPosition(x, y + i);
            string linea = to_string(i + 1) + ". " + canciones[i];
            Console::Write("                                                    ");
            Console::SetCursorPosition(x, y + i);
            Console::Write(gcnew String(linea.c_str()));
        }
    }

private:
    /*
     * Funcion privada: toLower()
     * Proposito: Convertir un string a minusculas para comparacion insensible a caso.
     * Parametros:
     *   - string s: string de entrada
     * Retorno: string en minusculas
     */
    static string toLower(string s) {
        for (auto& ch : s) ch = (char)tolower((unsigned char)ch);
        return s;
    }
};
