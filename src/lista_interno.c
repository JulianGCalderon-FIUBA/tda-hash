#include "lista_interno.h"

nodo_t *buscar_nodo_posicion(nodo_t *inicial, size_t posicion)
{
    if (!inicial)
    {
        return NULL;
    }

    nodo_t *nodo = inicial;
    for (int i = 0; i < posicion; i++)
    {
        nodo = nodo->siguiente;

        if (!nodo)
        {
            return NULL;
        }
    }

    return nodo;
}
