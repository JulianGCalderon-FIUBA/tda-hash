#include "lista.h"

lista_t *lista_crear()
{
    return (lista_t *)calloc(1, sizeof(lista_t));
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
    if (!lista)
    {
        return NULL;
    }

    nodo_t *nodo_a_insertar = (nodo_t *)calloc(1, sizeof(nodo_t));
    if (!nodo_a_insertar)
    {
        return NULL;
    }
    nodo_a_insertar->elemento = elemento;

    if (lista->cantidad == 0)
    {
        lista->nodo_inicio = nodo_a_insertar;
        lista->nodo_fin = nodo_a_insertar;
    }
    else
    {
        lista->nodo_fin->siguiente = nodo_a_insertar;
        lista->nodo_fin = nodo_a_insertar;
    }

    (lista->cantidad)++;
    return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento, size_t posicion)
{
    if (!lista)
    {
        return NULL;
    }
    if (posicion >= lista->cantidad)
    {
        return lista_insertar(lista, elemento);
    }

    nodo_t *nodo_a_insertar = (nodo_t *)calloc(1, sizeof(nodo_t));
    if (!nodo_a_insertar)
    {
        return NULL;
    }
    nodo_a_insertar->elemento = elemento;

    if (posicion == 0)
    {
        nodo_a_insertar->siguiente = lista->nodo_inicio;
        lista->nodo_inicio = nodo_a_insertar;
    }
    else
    {
        nodo_t *nodo_anterior = buscar_nodo_posicion(lista->nodo_inicio, posicion - 1);
        nodo_a_insertar->siguiente = nodo_anterior->siguiente;
        nodo_anterior->siguiente = nodo_a_insertar;
    }

    (lista->cantidad)++;
    return lista;
}

void *lista_quitar(lista_t *lista)
{
    if (!lista)
    {
        return NULL;
    }
    if (lista->cantidad == 0)
    {
        return NULL;
    }

    void *elemento;
    if (lista->cantidad == 1)
    {
        elemento = lista->nodo_fin->elemento;

        free(lista->nodo_fin);
        lista->nodo_fin = NULL;
        lista->nodo_inicio = NULL;
    }
    else
    {

        nodo_t *nodo_anterior = buscar_nodo_posicion(lista->nodo_inicio, lista->cantidad - 2);
        if (!nodo_anterior)
        {
            return NULL;
        }
        nodo_anterior->siguiente = NULL;

        elemento = lista->nodo_fin->elemento;

        free(lista->nodo_fin);
        lista->nodo_fin = nodo_anterior;
    }

    (lista->cantidad)--;
    return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
    if (!lista)
    {
        return NULL;
    }
    if (lista->cantidad == 0)
    {
        return NULL;
    }

    if (posicion > lista->cantidad || lista->cantidad == 1)
    {
        return lista_quitar(lista);
    }

    void *elemento;
    if (posicion == 0)
    {
        nodo_t *nodo_a_quitar = lista->nodo_inicio;
        nodo_t *nodo_siguiente = nodo_a_quitar->siguiente;

        elemento = nodo_a_quitar->elemento;

        free(nodo_a_quitar);
        lista->nodo_inicio = nodo_siguiente;
    }
    else
    {
        nodo_t *nodo_anterior = buscar_nodo_posicion(lista->nodo_inicio, posicion - 1);
        nodo_t *nodo_a_quitar = nodo_anterior->siguiente;
        nodo_t *nodo_siguiente = nodo_a_quitar->siguiente;

        elemento = nodo_a_quitar->elemento;

        free(nodo_a_quitar);
        nodo_anterior->siguiente = nodo_siguiente;
    }

    (lista->cantidad)--;
    return elemento;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
    if (!lista)
    {
        return NULL;
    }
    if (lista->cantidad == 0)
    {
        return NULL;
    }

    nodo_t *nodo_buscado = buscar_nodo_posicion(lista->nodo_inicio, posicion);
    if (!nodo_buscado)
    {
        return NULL;
    }

    return nodo_buscado->elemento;
}

void *lista_primero(lista_t *lista)
{
    if (!lista)
    {
        return NULL;
    }

    if (lista->cantidad == 0)
    {
        return NULL;
    }

    return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
    if (!lista)
    {
        return NULL;
    }

    if (lista->cantidad == 0)
    {
        return NULL;
    }

    return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t *lista)
{
    if (!lista)
    {
        return true;
    }

    return lista->cantidad == 0;
}

size_t lista_tamanio(lista_t *lista)
{
    if (!lista)
    {
        return 0;
    }

    return lista->cantidad;
}

void lista_destruir(lista_t *lista)
{
    if (!lista)
    {
        return;
    }

    if (!lista->nodo_inicio)
    {
        free(lista);
        return;
    }

    nodo_t *siguiente = lista->nodo_inicio->siguiente;
    free(lista->nodo_inicio);
    lista->nodo_inicio = siguiente;

    lista_destruir(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
    if (!lista)
    {
        return NULL;
    }

    lista_iterador_t *it = (lista_iterador_t *)malloc(sizeof(lista_iterador_t));
    if (!it)
    {
        return NULL;
    }
    it->lista = lista;
    it->corriente = lista->nodo_inicio;

    return it;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
    if (!iterador)
    {
        return false;
    }

    return iterador->corriente;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
    if (!iterador)
    {
        return false;
    }
    if (!iterador->corriente)
    {
        return false;
    }

    iterador->corriente = iterador->corriente->siguiente;

    return iterador->corriente;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
    if (!iterador)
    {
        return NULL;
    }

    if (!iterador->corriente)
    {
        return NULL;
    }

    return iterador->corriente->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
    free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *), void *contexto)
{
    if (!lista || !funcion || !contexto)
    {
        return 0;
    }

    bool finalizo = false;
    size_t cantidad = 0;

    nodo_t *nodo_actual = lista->nodo_inicio;

    while (nodo_actual && !finalizo)
    {
        finalizo = !funcion(nodo_actual->elemento, contexto);
        cantidad++;

        nodo_actual = nodo_actual->siguiente;
    }

    return cantidad;
}
