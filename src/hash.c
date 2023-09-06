#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash.h"
#include "hash_interno.h"
#include "lista.h"

hash_t *hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad_inicial)
{
    capacidad_inicial = max(capacidad_inicial, CAPACIDAD_INICIAL_MINIMA);

    hash_t *hash = malloc(sizeof(hash_t));
    lista_t **tabla = calloc(capacidad_inicial, sizeof(lista_t *));
    if (!hash || !tabla)
    {
        free(hash);
        return NULL;
    }

    int estado = EXITO;
    for (int i = 0; i < capacidad_inicial; i++)
    {
        tabla[i] = lista_crear();
        if (!tabla[i])
        {
            estado = ERROR;
        }
    }
    if (estado == ERROR)
    {
        destruir_tabla(tabla, capacidad_inicial, NULL);
        return NULL;
    }

    hash->tabla = tabla;
    hash->cantidad = 0;
    hash->capacidad = capacidad_inicial;
    hash->destructor = destruir_elemento;

    return hash;
}

int hash_insertar(hash_t *hash, const char *clave, void *elemento)
{
    if (!hash || !clave)
    {
        return ERROR;
    }

    lista_t *lista = obtener_lista(hash->tabla, clave, hash->capacidad);
    int posicion_lista = indice_de_clave_en_lista(lista, clave);

    if (posicion_lista != ERROR)
    {
        par_t *par = lista_elemento_en_posicion(lista, posicion_lista);

        if (hash->destructor)
        {
            hash->destructor(par->elemento);
        }

        par->elemento = elemento;
    }
    else
    {
        if (debo_rehashear(hash->cantidad + 1, hash->capacidad))
        {
            int retorno = rehash(hash);
            if (retorno == ERROR)
            {
                return ERROR;
            }
        }

        lista = obtener_lista(hash->tabla, clave, hash->capacidad);

        lista = insertar_par_en_lista(lista, clave, elemento);
        if (!lista)
        {
            return ERROR;
        }

        (hash->cantidad)++;
    }

    return EXITO;
}

int hash_quitar(hash_t *hash, const char *clave)
{
    if (!hash || !clave)
    {
        return ERROR;
    }

    lista_t *lista = obtener_lista(hash->tabla, clave, hash->capacidad);

    int posicion_lista = indice_de_clave_en_lista(lista, clave);
    if (posicion_lista == ERROR)
    {
        return ERROR;
    }

    void *elemento = lista_quitar_de_posicion(lista, posicion_lista);
    destruir_par(elemento, hash->destructor);

    (hash->cantidad)--;

    return EXITO;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
    if (!hash || !clave)
    {
        return false;
    }

    lista_t *lista = obtener_lista(hash->tabla, clave, hash->capacidad);

    int posicion_lista = indice_de_clave_en_lista(lista, clave);
    if (posicion_lista == ERROR)
    {
        return NULL;
    }

    par_t *par = lista_elemento_en_posicion(lista, posicion_lista);
    return par->elemento;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
    if (!hash || !clave)
    {
        return false;
    }

    lista_t *lista = obtener_lista(hash->tabla, clave, hash->capacidad);

    int posicion_lista = indice_de_clave_en_lista(lista, clave);

    return (posicion_lista == ERROR) ? false : true;
}

size_t hash_cantidad(hash_t *hash)
{
    if (!hash)
    {
        return 0;
    }

    return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
    if (hash)
    {
        destruir_tabla(hash->tabla, hash->capacidad, hash->destructor);
        free(hash);
    }
}

size_t hash_con_cada_clave(hash_t *hash, bool (*funcion)(hash_t *hash, const char *clave, void *aux), void *aux)
{
    if (!hash || !funcion)
    {
        return 0;
    }

    int indice_tabla = 0;
    int elementos_iterados = 0;
    bool cortar_ejecucion = false;

    while (indice_tabla < hash->capacidad && !cortar_ejecucion)
    {
        lista_t *lista = hash->tabla[indice_tabla];

        lista_iterador_t *it = lista_iterador_crear(lista);
        while (lista_iterador_tiene_siguiente(it) && !cortar_ejecucion)
        {
            par_t *par = lista_iterador_elemento_actual(it);
            cortar_ejecucion = funcion(hash, par->clave, aux);

            elementos_iterados++;
            lista_iterador_avanzar(it);
        }

        lista_iterador_destruir(it);

        indice_tabla++;
    }

    return elementos_iterados;
}
