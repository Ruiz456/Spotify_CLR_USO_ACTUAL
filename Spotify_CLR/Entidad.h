#pragma once
#include <string>
using namespace std;

/*
 * Clase: Entidad (CLASE PADRE ABSTRACTA)
 * Proposito: Clase base para elementos multimedia del sistema.
 *            Define la interfaz comun para Cancion y Album.
 * Campos:
 *   - id         : identificador numerico unico del elemento
 *   - nombre     : nombre principal del elemento
 *   - descripcion: descripcion adicional (opcional)
 * Metodos virtuales puros:
 *   - mostrar()       : imprime la informacion del elemento
 *   - obtenerNombre() : retorna el nombre del elemento
 */
class Entidad {
protected:
    int    id;
    string nombre;
    string descripcion;

public:
    Entidad() : id(0), nombre(""), descripcion("") {}

    virtual void   mostrar() = 0;
    virtual string obtenerNombre() = 0;
    virtual ~Entidad() {}
};