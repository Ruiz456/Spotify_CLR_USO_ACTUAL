#pragma once
#include "pch.h"
#include <iostream>
#include <string>
#include <fstream>
#include "Entidad.h"
#include "Lista.h"

using namespace System;
using namespace std;

/*
 * Clase: Cancion (CLASE DERIVADA de Entidad)
 * Proposito: Representa una cancion individual del sistema.
 * Hereda de: Entidad
 * Campos adicionales: artista, genero, duracion
 * El campo 'nombre' (de Entidad) almacena el titulo de la cancion.
 */
class Cancion : public Entidad {
private:
    string genero;
    float  duracion;
    string artista;

    // Lista interna usada para gestion del almacen de canciones
    Lista<Cancion*>* lst_canciones = new Lista<Cancion*>();

    string almacenCanciones = "almacenCanciones.txt";

public:
    /*
     * Constructor de Cancion
     * Parametros:
     *   - string titulo  : titulo de la cancion (va a Entidad::nombre)
     *   - string genero  : genero musical
     *   - float  duracion: duracion en minutos
     *   - string artista : nombre del artista
     */
    Cancion(string titulo = "", string genero = "", float duracion = 0, string artista = "") {
        this->nombre = titulo;   // campo heredado de Entidad
        this->id = 0;
        this->descripcion = "";
        this->genero = genero;
        this->duracion = duracion;
        this->artista = artista;

        crearArchivoSiNoExiste(almacenCanciones);
    }

    // Implementacion de metodos virtuales de Entidad
    void mostrar() override {
        cout << nombre << " - " << artista << " (" << genero << ")" << endl;
    }

    string obtenerNombre() override { return nombre; }

    // Getters
    string getTitulo()   const { return nombre; }
    string getGenero()   const { return genero; }
    float  getDuracion() const { return duracion; }
    string getArtista()  const { return artista; }

    // Setters
    void setTitulo(string t) { nombre = t; }
    void setGenero(string g) { genero = g; }
    void setDuracion(float d) { duracion = d; }
    void setArtista(string a) { artista = a; }

    void mostrarCanciones() {
        int i = 0;
        lst_canciones->recorrer([&](Cancion* c) {
            cout << i << ". " << c->getTitulo() << " - " << c->getArtista() << endl;
            i++;
            });
    }

    void crearArchivoSiNoExiste(string nombreArchivo) {
        ifstream verificar(nombreArchivo);
        if (verificar.good()) { verificar.close(); return; }
        verificar.close();
        ofstream archivo(nombreArchivo);
        archivo.close();
    }

    void guardarCanciones(Lista<Cancion*>* lista, string nombreArchivo) {
        ofstream archivo(nombreArchivo);
        lista->recorrer([&](Cancion* c) {
            archivo << c->getTitulo() << "|"
                << c->getArtista() << "|"
                << c->getGenero() << "\n";
            });
        archivo.close();
    }

    void eliminarPorNombre(string nombre_buscado) {
        for (unsigned int i = 0; i < lst_canciones->longitud(); i++) {
            Cancion* c = lst_canciones->obtenerPos(i);
            if (c && c->getTitulo() == nombre_buscado) {
                lst_canciones->eliminarEn(i);
                i--;
            }
        }
    }

    /*
     * Funcion: agregarCancion()
     * Proposito: Agregar una cancion al almacen interno y persistirla en almacenCanciones.txt.
     *            Formato escrito: Titulo|Artista|Genero|Duracion (4 campos).
     * Parametros:
     *   - string titulo, artista_param, genero_param: datos de la cancion
     *   - float  duracion_param: duracion en minutos (0 por defecto)
     */
    void agregarCancion(string titulo, string artista_param, string genero_param, float duracion_param = 0) {
        Cancion* c = new Cancion(titulo, genero_param, duracion_param, artista_param);
        lst_canciones->agregarFinal(c);

        ofstream archivo(almacenCanciones, ios::app);
        archivo << titulo << "|" << artista_param << "|"
                << genero_param << "|" << duracion_param << "\n";
        archivo.close();
    }

    void cargarCancionesDesdeArchivo() {
        ifstream archivo(almacenCanciones);
        if (!archivo.is_open()) { cout << "No se pudo abrir: " << almacenCanciones << endl; return; }

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            int pos1 = (int)linea.find('|');
            int pos2 = (int)linea.find('|', pos1 + 1);

            string t = linea.substr(0, pos1);
            string a = linea.substr(pos1 + 1, pos2 - pos1 - 1);
            string g = linea.substr(pos2 + 1);

            Cancion* c = new Cancion(t, g, 0, a);
            lst_canciones->agregarFinal(c);
        }
        archivo.close();
    }
};