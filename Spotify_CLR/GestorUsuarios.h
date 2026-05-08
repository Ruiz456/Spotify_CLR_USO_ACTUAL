#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <direct.h>
#include <windows.h>
#include "Lista.h"
#include "Usuario.h"
#include "Playlist.h"
#include "Favoritos.h"
#include "Cancion.h"

using namespace std;

/*
 * Clase: ColaUsuarios
 * Proposito: Cola FIFO de objetos Usuario implementada con nodos enlazados.
 *            Se usa en GestorUsuarios para encolar registros pendientes
 *            que se procesaran en lote con procesarCola().
 */
class ColaUsuarios {
private:
    struct Nodo {
        Usuario dato;
        Nodo* sig;
        Nodo(Usuario d) : dato(d), sig(nullptr) {}
    };
    Nodo* frente;
    Nodo* fin;
    int tam;

public:
    /*
     * Constructor: inicializa la cola vacia.
     */
    ColaUsuarios() : frente(nullptr), fin(nullptr), tam(0) {}

    /*
     * Funcion: encolar()
     * Proposito: Insertar un usuario al final de la cola (FIFO).
     * Parametros:
     *   - Usuario u: usuario a encolar
     */
    void encolar(Usuario u) {
        Nodo* n = new Nodo(u);
        if (!fin) frente = fin = n;
        else { fin->sig = n; fin = n; }
        tam++;
    }

    /*
     * Funcion: desencolar()
     * Proposito: Extraer y retornar el usuario del frente de la cola.
     *            Retorna Usuario() por defecto si la cola esta vacia.
     * Retorno: Usuario extraido del frente
     */
    Usuario desencolar() {
        if (!frente) return Usuario();
        Nodo* aux = frente;
        Usuario d = aux->dato;
        frente = frente->sig;
        if (!frente) fin = nullptr;
        delete aux;
        tam--;
        return d;
    }

    /*
     * Funcion: esVacia()
     * Proposito: Verificar si la cola no tiene elementos.
     * Retorno: bool true si la cola esta vacia
     */
    bool esVacia() { return frente == nullptr; }

    /*
     * Funcion: longitud()
     * Proposito: Retornar el numero de usuarios en la cola.
     * Retorno: int cantidad de elementos
     */
    int longitud() { return tam; }
};

/*
 * Clase: GestorUsuarios
 * Proposito: Administra el ciclo de vida completo de los usuarios.
 *            Cada usuario tiene su propia carpeta en disco con la estructura:
 *
 *              usuarios_data/[nombre]/
 *              ├── config.txt             → nombre|email|id
 *              ├── playlists/
 *              │   ├── Rock.txt           → Titulo|Artista|Genero|Duracion
 *              │   └── Trap.txt
 *              └── favoritos/
 *                  └── favoritos.txt      → Titulo|Artista|Genero|Duracion
 *
 *            Los usuarios se cargan escaneando las subcarpetas de usuarios_data/
 *            al construirse. Los datos completos (playlists, favoritos) se cargan
 *            solo al hacer login (buscarPorNombre / buscar).
 */
class GestorUsuarios {
private:
    Lista<Usuario>  usuarios;
    ColaUsuarios    colaRegistro;
    string          carpeta = "usuarios_data/";
    Usuario*        usuarioActual = nullptr;

    /*
     * Funcion privada: crearCarpetasUsuario()
     * Proposito: Crear la jerarquia de carpetas de un usuario si no existe:
     *              usuarios_data/[nombre]/
     *              usuarios_data/[nombre]/playlists/
     *              usuarios_data/[nombre]/favoritos/
     * Parametros:
     *   - string nombre: nombre del usuario (usado como nombre de carpeta)
     */
    void crearCarpetasUsuario(const string& nombre) {
        (void)_mkdir(carpeta.c_str());
        string base = carpeta + nombre;
        (void)_mkdir(base.c_str());
        (void)_mkdir((base + "/playlists").c_str());
        (void)_mkdir((base + "/favoritos").c_str());
    }

    /*
     * Funcion privada: generarId()
     * Proposito: Generar un ID de 4 digitos unico que no exista en la lista actual.
     * Retorno: int ID entre 1000 y 9999
     */
    int generarId() {
        srand((unsigned int)time(nullptr));
        int id;
        bool existe;
        do {
            id = rand() % 9000 + 1000;
            existe = false;
            usuarios.recorrer([&](Usuario u) {
                if (u.getId() == id) existe = true;
                });
        } while (existe);
        return id;
    }

    /*
     * Funcion privada: parsearCancion()
     * Proposito: Convertir una linea "Titulo|Artista|Genero|Duracion" en un objeto Cancion.
     *            Si falta la duracion se usa 0.0f por defecto.
     * Parametros:
     *   - const string& linea: linea del archivo a parsear
     * Retorno: Cancion construida con los datos de la linea
     */
    Cancion parsearCancion(const string& lineaRaw) {
        if (lineaRaw.empty()) return Cancion();

        // Quitar \r de lineas con fin de linea Windows (\r\n)
        string linea = lineaRaw;
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) return Cancion();

        stringstream ss(linea);
        string titulo, artista, genero, durStr;
        getline(ss, titulo,  '|');
        getline(ss, artista, '|');
        getline(ss, genero,  '|');
        getline(ss, durStr,  '|');

        // Limpiar \r residual del ultimo campo
        if (!durStr.empty() && durStr.back() == '\r') durStr.pop_back();
        if (!genero.empty()  && genero.back()  == '\r') genero.pop_back();

        float dur = 0.0f;
        if (!durStr.empty()) {
            try { dur = stof(durStr); } catch (...) { dur = 0.0f; }
        }
        return Cancion(titulo, genero, dur, artista);
    }

    /*
     * Funcion privada: guardarConfig()
     * Proposito: Escribir (o sobreescribir) el archivo config.txt del usuario.
     *            Formato: nombre|email|id
     * Parametros:
     *   - Usuario& u: usuario cuyo config se guardara
     */
    void guardarConfig(Usuario& u) {
        string path = carpeta + u.getNombre() + "/config.txt";
        ofstream f(path);
        f << u.getNombre() << "|" << u.getEmail() << "|" << u.getId() << "\n";
        f.close();
    }

    /*
     * Funcion privada: limpiarPlaylistsDisco()
     * Proposito: Eliminar todos los archivos .txt de la carpeta playlists/ del usuario.
     *            Se llama antes de re-guardar para que las playlists eliminadas en
     *            memoria no reaparezcan al cargar nuevamente.
     * Parametros:
     *   - const string& nombre: nombre del usuario
     */
    void limpiarPlaylistsDisco(const string& nombre) {
        string folder = carpeta + nombre + "/playlists/";
        WIN32_FIND_DATAA ffd;
        HANDLE hFind = FindFirstFileA((folder + "*.txt").c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE) return;
        do {
            if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                remove((folder + ffd.cFileName).c_str());
        } while (FindNextFileA(hFind, &ffd));
        FindClose(hFind);
    }

    /*
     * Funcion privada: guardarDatosUsuario()
     * Proposito: Persistir en disco todos los datos del usuario:
     *            config.txt, todas las playlists y favoritos.
     *            Primero limpia los .txt de playlists/ para evitar archivos huerfanos
     *            de playlists que fueron eliminadas en memoria.
     * Parametros:
     *   - Usuario& u: usuario cuyos datos se guardaran
     */
    void guardarDatosUsuario(Usuario& u) {
        crearCarpetasUsuario(u.getNombre());
        guardarConfig(u);

        // Limpiar playlists antiguas y re-guardar todas las actuales
        limpiarPlaylistsDisco(u.getNombre());
        string plFolder = carpeta + u.getNombre() + "/playlists/";
        u.getPlaylists().recorrer([&](Playlist p) {
            ofstream f(plFolder + p.getNombre() + ".txt");
            p.getCanciones().recorrer([&](Cancion c) {
                f << c.getTitulo()  << "|" << c.getArtista() << "|"
                  << c.getGenero()  << "|" << c.getDuracion() << "\n";
                });
            f.close();
            });

        // Guardar favoritos
        string favPath = carpeta + u.getNombre() + "/favoritos/favoritos.txt";
        ofstream fav(favPath);
        u.getFavoritos().getLista().recorrer([&](Cancion c) {
            fav << c.getTitulo()  << "|" << c.getArtista() << "|"
                << c.getGenero()  << "|" << c.getDuracion() << "\n";
            });
        fav.close();
    }

    /*
     * Funcion privada: cargarPlaylists()
     * Proposito: Leer todos los archivos .txt de usuarios_data/[nombre]/playlists/
     *            y cargar cada uno como una Playlist con sus canciones.
     *            El nombre del archivo (sin .txt) es el nombre de la playlist.
     * Parametros:
     *   - Usuario& u: usuario al que se le cargaran las playlists
     */
    void cargarPlaylists(Usuario& u) {
        string folder = carpeta + u.getNombre() + "/playlists/";
        WIN32_FIND_DATAA ffd;
        HANDLE hFind = FindFirstFileA((folder + "*.txt").c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE) return;
        do {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            string archivo = ffd.cFileName;
            string nombrePl = archivo.substr(0, archivo.size() - 4); // quitar ".txt"

            Playlist p(nombrePl);
            ifstream f(folder + archivo);
            string linea;
            while (getline(f, linea)) {
                if (!linea.empty())
                    p.agregarCancion(parsearCancion(linea));
            }
            f.close();
            u.crearPlaylist(p);
        } while (FindNextFileA(hFind, &ffd));
        FindClose(hFind);
    }

    /*
     * Funcion privada: cargarFavoritos()
     * Proposito: Leer usuarios_data/[nombre]/favoritos/favoritos.txt
     *            y agregar cada cancion a los favoritos del usuario.
     * Parametros:
     *   - Usuario& u: usuario al que se le cargaran los favoritos
     */
    void cargarFavoritos(Usuario& u) {
        string path = carpeta + u.getNombre() + "/favoritos/favoritos.txt";
        ifstream f(path);
        if (!f.is_open()) return;
        string linea;
        while (getline(f, linea)) {
            if (!linea.empty())
                u.agregarFavorito(parsearCancion(linea));
        }
        f.close();
    }

    /*
     * Funcion privada: cargarDatosUsuario()
     * Proposito: Cargar las playlists y favoritos del usuario desde su carpeta.
     *            Se llama al hacer login (buscar / buscarPorNombre).
     * Parametros:
     *   - Usuario& u: usuario al que se le cargaran los datos (por referencia)
     */
    void cargarDatosUsuario(Usuario& u) {
        cargarPlaylists(u);
        cargarFavoritos(u);
    }

    /*
     * Funcion privada: cargarUsuarios()
     * Proposito: Escanear la carpeta usuarios_data/ buscando subcarpetas.
     *            Por cada subcarpeta lee config.txt (nombre|email|id) y agrega
     *            el usuario basico a la lista interna.
     *            Los datos completos se cargan al hacer login.
     */
    void cargarUsuarios() {
        (void)_mkdir(carpeta.c_str());
        WIN32_FIND_DATAA ffd;
        HANDLE hFind = FindFirstFileA((carpeta + "*").c_str(), &ffd);
        if (hFind == INVALID_HANDLE_VALUE) return;
        do {
            if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
            string nombre = ffd.cFileName;
            if (nombre == "." || nombre == "..") continue;

            string configPath = carpeta + nombre + "/config.txt";
            ifstream f(configPath);
            if (!f.is_open()) continue;

            string linea;
            getline(f, linea);
            f.close();

            stringstream ss(linea);
            string nom, email, idStr;
            getline(ss, nom,   '|');
            getline(ss, email, '|');
            getline(ss, idStr, '|');
            int id = (idStr.empty() || idStr == "0") ? generarId() : stoi(idStr);

            if (!nom.empty())
                usuarios.agregarFinal(Usuario(id, nom, email));
        } while (FindNextFileA(hFind, &ffd));
        FindClose(hFind);
    }

    /*
     * Funcion privada: eliminarCarpetaUsuario()
     * Proposito: Borrar fisicamente toda la estructura de carpetas del usuario:
     *            playlists/*.txt, favoritos/favoritos.txt, config.txt y la carpeta raiz.
     * Parametros:
     *   - string nombre: nombre del usuario cuya carpeta se eliminara
     */
    void eliminarCarpetaUsuario(const string& nombre) {
        string base      = carpeta + nombre;
        string plFolder  = base + "/playlists/";
        string favFolder = base + "/favoritos/";

        // Eliminar todos los .txt de playlists/
        WIN32_FIND_DATAA ffd;
        HANDLE hFind = FindFirstFileA((plFolder + "*").c_str(), &ffd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    remove((plFolder + ffd.cFileName).c_str());
            } while (FindNextFileA(hFind, &ffd));
            FindClose(hFind);
        }
        (void)_rmdir(plFolder.c_str());

        remove((favFolder + "favoritos.txt").c_str());
        (void)_rmdir(favFolder.c_str());

        remove((base + "/config.txt").c_str());
        (void)_rmdir(base.c_str());
    }

public:
    /*
     * Constructor: GestorUsuarios()
     * Proposito: Inicializar el gestor creando la carpeta usuarios_data/ si no existe
     *            y cargando el indice de usuarios escaneando las subcarpetas.
     */
    GestorUsuarios() {
        (void)_mkdir(carpeta.c_str());
        cargarUsuarios();
    }

    /*
     * Funcion: registrar()
     * Proposito: Crear un nuevo usuario: genera ID unico, crea sus carpetas en disco,
     *            escribe config.txt y lo agrega a la lista interna.
     * Parametros:
     *   - string nombre: nombre del nuevo usuario
     *   - string email : correo electronico
     * Retorno: Usuario recien creado (con ID generado)
     *
     * Ejemplo:
     *   Usuario u = gGestor.registrar("Joel", "joel@gmail.com");
     */
    Usuario registrar(string nombre, string email) {
        int id = generarId();
        Usuario u(id, nombre, email);
        crearCarpetasUsuario(nombre);
        guardarConfig(u);
        // Crear favoritos.txt vacio
        ofstream fav(carpeta + nombre + "/favoritos/favoritos.txt");
        fav.close();
        usuarios.agregarFinal(u);
        return u;
    }

    /*
     * Funcion: encolarRegistro()
     * Proposito: Agregar un usuario pendiente a la ColaUsuarios interna.
     *            Los usuarios encolados se registran al llamar procesarCola().
     * Parametros:
     *   - string nombre, string email: datos del usuario a encolar
     */
    void encolarRegistro(string nombre, string email) {
        colaRegistro.encolar(Usuario(0, nombre, email));
    }

    /*
     * Funcion: procesarCola()
     * Proposito: Registrar en lote todos los usuarios encolados con encolarRegistro().
     *            Vacia la cola al terminar.
     */
    void procesarCola() {
        while (!colaRegistro.esVacia()) {
            Usuario u = colaRegistro.desencolar();
            registrar(u.getNombre(), u.getEmail());
        }
    }

    /*
     * Funcion: eliminar()
     * Proposito: Borrar un usuario por ID: elimina su carpeta completa del disco
     *            y lo quita de la lista en memoria.
     * Parametros:
     *   - int id: ID del usuario a eliminar
     */
    void eliminar(int id) {
        string nombreElim = "";
        usuarios.recorrer([&](Usuario u) {
            if (u.getId() == id) nombreElim = u.getNombre();
            });
        if (!nombreElim.empty())
            eliminarCarpetaUsuario(nombreElim);

        Lista<Usuario> nueva;
        usuarios.recorrer([&](Usuario u) {
            if (u.getId() != id) nueva.agregarFinal(u);
            });
        usuarios = nueva;
        cout << "Usuario ID " << id << " eliminado." << endl;
    }

    /*
     * Funcion: buscar()
     * Proposito: Buscar un usuario por ID y cargar sus datos completos desde disco.
     *            Si no existe retorna un Usuario() con id=0.
     * Parametros:
     *   - int id: ID a buscar
     * Retorno: Usuario con playlists y favoritos cargados, o Usuario() si no existe
     */
    Usuario buscar(int id) {
        Usuario encontrado;
        usuarios.recorrer([&](Usuario u) {
            if (u.getId() == id) encontrado = u;
            });
        if (encontrado.getId() != 0)
            cargarDatosUsuario(encontrado);
        return encontrado;
    }

    /*
     * Funcion: buscarPorNombre()
     * Proposito: Buscar un usuario por nombre exacto (case-sensitive) y cargar
     *            sus datos completos desde disco. Se usa en el flujo de login.
     * Parametros:
     *   - string nombre: nombre exacto del usuario
     * Retorno: Usuario con datos cargados, o Usuario() con id=0 si no existe
     *
     * Ejemplo:
     *   Usuario u = gGestor.buscarPorNombre("Joel");
     *   if (u.getId() != 0) // login exitoso
     */
    Usuario buscarPorNombre(string nombre) {
        Usuario encontrado;
        usuarios.recorrer([&](Usuario u) {
            if (u.getNombre() == nombre) encontrado = u;
            });
        if (encontrado.getId() != 0)
            cargarDatosUsuario(encontrado);
        return encontrado;
    }

    /*
     * Funcion: existeNombre()
     * Proposito: Verificar rapidamente si un nombre de usuario ya esta registrado.
     *            Se usa antes del registro para evitar duplicados.
     * Parametros:
     *   - string nombre: nombre a verificar
     * Retorno: bool true si el nombre ya existe
     */
    bool existeNombre(string nombre) {
        bool existe = false;
        usuarios.recorrer([&](Usuario u) {
            if (u.getNombre() == nombre) existe = true;
            });
        return existe;
    }

    /*
     * Funcion: guardarUsuarioActual()
     * Proposito: Persistir en disco todos los datos del usuario en sesion
     *            (playlists, favoritos, config). No hace nada si usuarioActual es nullptr.
     */
    void guardarUsuarioActual() {
        if (usuarioActual != nullptr)
            guardarDatosUsuario(*usuarioActual);
    }

    /*
     * Funcion: setUsuarioActual()
     * Proposito: Establecer el puntero al usuario actualmente en sesion.
     *            Se llama desde guardarUsuarioADisco() antes de guardarUsuarioActual().
     * Parametros:
     *   - Usuario* u: puntero al usuario en sesion
     */
    void setUsuarioActual(Usuario* u) {
        usuarioActual = u;
    }

    /*
     * Funcion: getUsuarioActual()
     * Proposito: Retornar el puntero al usuario actualmente en sesion.
     * Retorno: Usuario* (puede ser nullptr si no hay sesion activa)
     */
    Usuario* getUsuarioActual() {
        return usuarioActual;
    }

    /*
     * Funcion: mostrar()
     * Proposito: Imprimir ID, nombre y email de todos los usuarios registrados.
     *            Util para depuracion.
     */
    void mostrar() {
        usuarios.recorrer([](Usuario u) {
            cout << "ID:" << u.getId()
                << "  " << u.getNombre()
                << "  " << u.getEmail() << endl;
            });
    }

    /*
     * Funcion: getLista()
     * Proposito: Retornar referencia a la lista interna de usuarios.
     * Retorno: Lista<Usuario>& referencia mutable a la lista
     */
    Lista<Usuario>& getLista() { return usuarios; }
};
