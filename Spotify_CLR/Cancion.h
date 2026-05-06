#pragma once
#include "pch.h"
#include <iostream>
#include <string>
#include <fstream> // para usar ofstream 

using namespace System;
using namespace std;

class Cancion {
private:
    string titulo;
    string genero;
    float duracion;
    string artista;

    // una txt donde se almacena todas las canciones
    Lista<Cancion*>* lst_canciones = new Lista<Cancion*>();


    // almancenCanciones.txt
    string almacenCanciones = "almacenCanciones.txt";

public:
    // Constructor
    Cancion(string titulo = "", string genero = "", float duracion = 0, string artista = "") {
        this->titulo = titulo;
        this->genero = genero;
        this->duracion = duracion;
        this->artista = artista;

        // creamos el almacenCanciones.txt (es un almacen de todas las canciones del mundo)
        crearArchivoSiNoExiste(almacenCanciones);
    }

    // Getters
    string getTitulo() const { return titulo; }
    string getGenero() const { return genero; }
    float getDuracion() const { return duracion; }
    string getArtista() const { return artista; }

    // Setters
    void setTitulo(string t) { titulo = t; }
    void setGenero(string g) { genero = g; }
    void setDuracion(float d) { duracion = d; }
    void setArtista(string a) { artista = a; }




    void mostrarCanciones()
    {
        int i = 0;

        lst_canciones->recorrer([&](Cancion* c) {
            cout << i << ". "
                << c->getTitulo() << " - "
                << c->getArtista() << " - "
                << c->getGenero() << endl;
            i++;
            });
    }


    // CAMBIARLO PARA A LA CLASE playlist
    void crearArchivoSiNoExiste(string nombreArchivo)
    {
        ifstream verificar(nombreArchivo);

        if (verificar.good()) {
            verificar.close();
            return;
        }

        verificar.close();


        ofstream archivo(nombreArchivo);
        archivo.close();
    }

    void guardarCanciones(Lista<Cancion*>* lista, string nombreArchivo)
    {
        ofstream archivo(nombreArchivo);

        lista->recorrer([&](Cancion* c) {
            archivo << c->getTitulo() << "|"
                << c->getArtista() << "|"
                << c->getGenero() << "\n";
            });

        archivo.close();
    }

    void eliminarPorNombre(string nombre)
    {
        for (unsigned int i = 0; i < lst_canciones->longitud(); i++)
        {
            Cancion* c = lst_canciones->obtenerPos(i);

            if (c && c->getTitulo() == nombre)
            {
                lst_canciones->eliminarEn(i);
                i--;
            }
        }
    }


    // Para agregar en la base de datos de almacenCanciones.txt
    void agregarCancion(string titulo, string artista, string genero, float duracion = 0)
    {
        Cancion* c = new Cancion(titulo, genero, duracion, artista);
        lst_canciones->agregarFinal(c);

        // ✅ Guarda automáticamente en almacenCanciones.txt
        ofstream archivo(almacenCanciones, ios::app);
        archivo << titulo << "|" << artista << "|" << genero << "\n";
        archivo.close();
    }

    

    ////////////////////////////////////////
    ////////////////////////////////////////
    // PARA MOSTRAS//// PARA PRUEBAS

    ////////////////////////////////////////
    ////////////////////////////////////////
    void cargarCancionesDesdeArchivo()
    {
        ifstream archivo(almacenCanciones);

        if (!archivo.is_open()) {
            cout << "No se pudo abrir: " << almacenCanciones << endl;
            return;
        }

        string linea;
        while (getline(archivo, linea))
        {
            if (linea.empty()) continue;

            int pos1 = linea.find('|');
            int pos2 = linea.find('|', pos1 + 1);

            string titulo = linea.substr(0, pos1);
            string artista = linea.substr(pos1 + 1, pos2 - pos1 - 1);
            string genero = linea.substr(pos2 + 1);

            Cancion* c = new Cancion(titulo, genero, 0, artista);
            lst_canciones->agregarFinal(c);
        }

        archivo.close();
    }
};