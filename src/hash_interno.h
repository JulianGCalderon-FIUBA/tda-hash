#ifndef __HASH_INTERNO_H__
#define __HASH_INTERNO_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "lista.h"
#include "hash.h"

#define ERROR -1
#define EXITO 0
#define CAPACIDAD_INICIAL_MINIMA 3
#define FACTOR_DE_CARGA_MAXIMO 0.75
#define REHASH_FACTOR_DE_MULTIPLICIDAD 2

typedef struct par
{
    char *clave;
    void *elemento;
} par_t;

struct hash
{
    lista_t **tabla;
    size_t capacidad;
    size_t cantidad;

    void (*destructor)(void *);
};

/**
 * Devuelve la sumatoria de:
 *   caracteres del STRING (su valor como ASCII), 
 *   multiplicados por su indice (partiendo del 1).
 * 
 * Devuelve 0 si el STRING es NULL.
 * 
 * El ultimo caracter del STRING debe ser el caracter nulo.
 */
size_t funcion_hash(const char *string);

/**
 * Devuelve el mayor de dos numeros.
 */
int max(int numero1, int numero2);

/**
 * Devuelve una copia de la STRING, almacenada en el HEAP.
 * 
 * Devuelve NULL en caso de error.
 * 
 * El ultimo caracter del STRING debe ser el caracter nulo.
 */
char *duplicar_string(const char *string);

/**
 * Devuelve la lista de la tabla que puede contener la clave especificada
 * Devuelve NULL si los parametros son invalidos.
 */
lista_t *obtener_lista(lista_t **tabla, const char *clave, size_t capacidad);

/**
 * Devuelve la posicion de la lista que contenga la clave indicada.
 * Devuelve ERROR en caso de no encontrar la clave.
 */
int indice_de_clave_en_lista(lista_t *lista, const char *clave);

/**
 * Devuelve un PAR_T* inicializado, almacenado en el HEAP.
 * Devuelve NULL en caso de error.
 */
par_t *crear_par(char *clave, void *elemento);

/**
 * Libera la memoria reservada para la clave del par.
 * Aplica la funcion destructor en el elemento del par.
 * Libera la memoria reservada para el par.
 */
void destruir_par(par_t *par, void (*destructor)(void *));

/**
 * Inserta un PAR_T* al final de la lista, con los valores indicados.
 * 
 * Devuelve la lista si pudo insertar.
 * Devuelve NULL en caso de error.
 */
lista_t *insertar_par_en_lista(lista_t *lista, const char *clave, void *elemento);

/**
 * Libera toda la memoria reservada para la tabla.
 * Si la funcion destructor no es NULL, la invoca en el elemento de cada par.
 */
void destruir_tabla(lista_t **tabla, size_t capacidad, void (*destructor)(void *));

/**
 * Devuelve TRUE si se debe realizar un rehash.
 * Devuelve FALSE de lo contrario.
 * 
 * 'capacidad' debe ser mayor a cero.
 */
bool debo_rehashear(size_t cantidad, size_t capacidad);

/**
 * Aumenta la capacidad de la tabla del hash.
 * Reorganiza los valores acorde a la nueva capacidad.
 * 
 * Devuelve EXITO, o ERROR en caso de error.
 */
int rehash(struct hash *hash);

#endif // __HASH_INTERNO_H__
