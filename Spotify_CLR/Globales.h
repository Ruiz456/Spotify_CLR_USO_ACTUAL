#pragma once
#include <string>
#include <vector>
#include "Lista.h"
#include "Cancion.h"
#include "Album.h"
#include "Reproductor.h"
#include "GestorUsuarios.h"

using namespace std;

// Estructura para opciones de menu con flechas
struct OpcionMenu { string texto; int id; };

// Variables globales (declaraciones extern — definidas en Spotify_CLR.cpp)
extern Lista<Cancion>*  gCanciones;
extern Lista<Album>*    gAlbumes;
extern GestorUsuarios   gGestor;
extern Reproductor      gReproductor;

extern int      gVolumen;
extern bool     gPausado;
extern bool     gAleatorio;
extern int      gTabActual;
extern bool     gMenuActivo;
extern Cancion  gCancionActual;
extern Usuario* gUsuarioActual;

// Cola de reproduccion
extern Lista<Cancion>* gColaActual;
extern int             gIndiceActual;
