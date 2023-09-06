#include "hash_interno.h"

size_t funcion_hash(const char *string)
{
    if (!string)
    {
        return 0;
    }

    size_t i = 1;
    size_t suma = 0;
    while (*string)
    {
        suma += *string * i;

        string++;
        i++;
    }

    return suma;
}

int max(int numero1, int numero2)
{
    if (numero1 > numero2)
    {
        return numero1;
    }
    return numero2;
}

char *duplicar_string(const char *string)
{
    if (!string)
    {
        return NULL;
    }

    char *copia = malloc(strlen(string) + 1);
    if (!copia)
    {
        return NULL;
    }

    strcpy(copia, string);

    return copia;
}

lista_t *obtener_lista(lista_t **tabla, const char *clave, size_t capacidad)
{
    if (!tabla || !clave || capacidad == 0)
    {
        return NULL;
    }
    int indice = funcion_hash(clave) % capacidad;

    return tabla[indice];
}

/**
 * NOTA:
 * Si se le aplica esta funcion a cada elemento de una lista, entoces recorre
 *  todos los elementos de la lista hasta que encuentra la clave buscada.
 *
 * Esta funcion recibe un PAR_T* y un vector de VOID*.
 *  -> _parametros[0] es un STRING (clave buscada).
 *  -> _parametros[1] es un BOOL*
 *
 * Si la clave del PAR_T* es igual a STRING.
 *  -> Almacena el TRUE en el BOOL*.
 *  -> Devuelve FALSE.
 *
 * De lo contrario, devuelve TRUE.
 */
bool indice_de_clave_en_lista_aux(void *_par, void *_parametros)
{
    par_t *par = _par;

    void **parametros = _parametros;
    char *clave = parametros[0];
    int *encontro_clave = parametros[1];

    if (strcmp(par->clave, clave) == 0)
    {
        *encontro_clave = true;
        return false;
    }

    return true;
}

int indice_de_clave_en_lista(lista_t *lista, const char *clave)
{
    if (!lista || !clave)
    {
        return ERROR;
    }

    bool encontro_clave = false;
    void *parametros[2] = {(void *)clave, &encontro_clave};

    int elementos_recorridos = lista_con_cada_elemento(lista, indice_de_clave_en_lista_aux, parametros);
    int indice_lista = elementos_recorridos - 1;

    if (!encontro_clave)
    {
        return ERROR;
    }

    return indice_lista;
}

par_t *crear_par(char *clave, void *elemento)
{
    par_t *par = malloc(sizeof(par_t));
    if (!par)
    {
        return NULL;
    }

    par->clave = clave;
    par->elemento = elemento;

    return par;
}

void destruir_par(par_t *par, void (*destructor)(void *))
{
    if (par)
    {
        free(par->clave);

        if (destructor)
        {
            destructor(par->elemento);
        }

        free(par);
    }
}

lista_t *insertar_par_en_lista(lista_t *lista, const char *clave, void *elemento)
{
    if (!lista)
    {
        return NULL;
    }

    char *copia_clave = duplicar_string(clave);
    if (!copia_clave)
    {
        return NULL;
    }

    par_t *par = crear_par(copia_clave, elemento);
    if (!par)
    {
        free(copia_clave);
        return NULL;
    }

    lista = lista_insertar(lista, par);
    if (!lista)
    {
        destruir_par(par, NULL);
        return NULL;
    }

    return lista;
}

void destruir_tabla(lista_t **tabla, size_t capacidad, void (*destructor)(void *))
{
    if (tabla)
    {
        for (int i = 0; i < capacidad; i++)
        {
            lista_t *lista = tabla[i];

            lista_iterador_t *it;
            for (it = lista_iterador_crear(lista); lista_iterador_tiene_siguiente(it); lista_iterador_avanzar(it))
            {
                par_t *par = lista_iterador_elemento_actual(it);
                destruir_par(par, destructor);
            }
            lista_iterador_destruir(it);

            lista_destruir(lista);
        }

        free(tabla);
    }
}

bool debo_rehashear(size_t cantidad, size_t capacidad)
{
    if (cantidad >= 0 && capacidad > 0)
    {

        float factor_de_carga = ((float)cantidad) / (float)capacidad;

        return (factor_de_carga > FACTOR_DE_CARGA_MAXIMO);
    }

    return false;
}

/**
 * NOTA:
 * Si se le aplica esta funcion a cada elemento del hash, entonces
 *  inserta todos los elementos del mismo en una nueva tabla.
 * Corta la ejecucion si encuentra un error.
 *
 * Esta funcion recibe un vector de VOID*
 *  -> _parametros[0] es un vector de HASH_T* (nuevo hash)
 *  -> _parametros[1] es un BOOL* (inicialmente en false)
 *
 * Inserta el elemento del hash con la clave indicada en el nuevo hash
 * Si pudo insertar, devuelve FALSE
 *
 * En caso de error, almacena TRUE en BOOL* y devuelve TRUE
 */
bool rehash_aux(struct hash *hash, const char *clave, void *_parametros)
{
    void **parametros = _parametros;
    hash_t *nuevo_hash = parametros[0];
    bool *error = parametros[1];

    void *elemento = hash_obtener(hash, clave);

    lista_t *lista = obtener_lista(nuevo_hash->tabla, clave, nuevo_hash->capacidad);
    lista = insertar_par_en_lista(lista, clave, elemento);
    if (!lista)
    {
        *error = true;
        return true;
    }

    (nuevo_hash->cantidad)++;

    return false;
}

int rehash(struct hash *hash)
{
    size_t nueva_capacidad = hash->capacidad * REHASH_FACTOR_DE_MULTIPLICIDAD;

    hash_t *nuevo_hash = hash_crear(NULL, nueva_capacidad);

    bool error = false;
    void *parametros[3] = {nuevo_hash, &error};
    hash_con_cada_clave(hash, rehash_aux, parametros);

    if (error)
    {
        destruir_tabla(nuevo_hash->tabla, nuevo_hash->capacidad, NULL);
        return ERROR;
    }

    destruir_tabla(hash->tabla, hash->capacidad, NULL);

    hash->tabla = nuevo_hash->tabla;
    hash->capacidad = nuevo_hash->capacidad;

    free(nuevo_hash);

    return EXITO;
}
