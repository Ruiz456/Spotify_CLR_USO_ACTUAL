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

public:
    // Constructor
    Cancion(string titulo = "", string genero = "", float duracion = 0, string artista = "") {
        this->titulo = titulo;
        this->genero = genero;
        this->duracion = duracion;
        this->artista = artista;
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

    // Mostrar info

    //////////////////////////////////////////////////////
    ////////////ELIMINAMOS ESTO: mostrar//////////////////
    //////////////////////////////////////////////////////
    void mostrar() const {
        cout << "Titulo: " << titulo << endl;
        cout << "Artista: " << artista << endl;
        cout << "Genero: " << genero << endl;
        cout << "Duracion: " << duracion << " min" << endl;
        cout << "------------------------" << endl;
    }


    // Operador de comparación (para Lista<T>)
    bool operator==(const Cancion& c) const {
        return this->titulo == c.titulo;
    }


    ///////////////////////////////////////////////////
    ////////////LO CAMBIAMOS POR ESTO//////////////////
    ///////////////////////////////////////////////////
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

};