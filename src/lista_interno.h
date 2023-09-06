#ifndef __LISTA_INTERNO_H__
#define __LISTA_INTERNO_H__

#include <stddef.h>

typedef struct nodo
{
    void *elemento;
    struct nodo *siguiente;
} nodo_t;

struct lista
{
    nodo_t *nodo_inicio;
    nodo_t *nodo_fin;
    size_t cantidad;
};

struct lista_iterador
{
    nodo_t *corriente;
    struct lista *lista;
};

/**
 * Devuelve el nodo que este en la posicion especificada, tomando el nodo inicial
 *  como la posicion 0.
 * 
 * Devuelve si la posicion no existe
 */
nodo_t *buscar_nodo_posicion(nodo_t *inicial, size_t posicion);

#endif // __LISTA_INTERNO_H__
