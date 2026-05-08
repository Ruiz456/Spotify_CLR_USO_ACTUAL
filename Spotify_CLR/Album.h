#pragma once
#include <iostream>
#include <string>
#include "Entidad.h"
#include "Lista.h"
#include "Cancion.h"

using namespace std;

/*
 * Clase: Album (CLASE DERIVADA de Entidad)
 * Proposito: Representa una coleccion ordenada de canciones.
 * Hereda de: Entidad
 * Campos adicionales: artista, anio, canciones, portada
 * El campo 'nombre' (de Entidad) almacena el nombre del album.
 */
class Album : public Entidad {
private:
    string artista;
    int    anio;
    Lista<Cancion> canciones;
    string portada;

public:
    /*
     * Constructor de Album
     * Parametros:
     *   - string nombre  : nombre del album (va a Entidad::nombre)
     *   - string artista : artista del album
     *   - int    anio    : anio de lanzamiento
     *   - string portada : ruta/URL de la imagen de portada
     */
    Album(string nombre = "", string artista = "", int anio = 0, string portada = "") {
        this->nombre = nombre;   // campo heredado de Entidad
        this->id = 0;
        this->descripcion = "";
        this->artista = artista;
        this->anio = anio;
        this->portada = portada;
    }

    // Implementacion de metodos virtuales de Entidad
    void mostrar() override {
        cout << "Album: " << nombre << " - " << artista
            << " (" << anio << ")" << endl;
        canciones.recorrer([](Cancion c) {
            cout << "  - " << c.getTitulo() << endl;
            });
    }

    string obtenerNombre() override { return nombre; }

    // Getters
    string getNombre()  const { return nombre; }
    string getArtista() const { return artista; }
    int    getAnio()    const { return anio; }
    string getPortada() const { return portada; }

    // Agregar cancion al album
    void agregarCancion(Cancion c) {
        canciones.agregarFinal(c);
    }

    // Acceso a la lista interna
    Lista<Cancion>& getCanciones() { return canciones; }
};