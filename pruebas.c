#include <stdbool.h>
#include <string.h>

#include "pa2mm.h"
#include "src/hash.h"
#include "src/hash_interno.h"
#include "src/lista.h"

//AUXILIARES

bool sumar_claves(hash_t *hash, const char *clave, void *_suma)
{
    int *suma = _suma;
    *suma += funcion_hash(clave);

    return false;
}

bool sumar_claves_hasta_corte(hash_t *hash, const char *clave, void *_suma)
{
    int *suma = _suma;
    *suma += funcion_hash(clave);

    if (strcmp(clave, "corte") == 0)
    {
        return true;
    }

    return false;
}

void destructor_incrementar(void *_numero)
{
    int *numero = _numero;
    (*numero)++;
}

void insertar_numero(hash_t *hash, char *clave, int numero, bool *error)
{
    int *p_numero = malloc(sizeof(int));
    if (!p_numero)
    {
        *error = true;
        return;
    }
    *p_numero = numero;

    int retorno = hash_insertar(hash, clave, p_numero);
    if (retorno == ERROR)
    {
        free(p_numero);
        *error = true;
    }
}

void insertar_puntero(hash_t *hash, char *clave, void *elemento, bool *error)
{
    int retorno = hash_insertar(hash, clave, elemento);
    if (retorno == ERROR)
    {
        *error = true;
    }
}

//PRUEBAS

void puedoCrearUnHash()
{
    hash_t *hash = hash_crear(destructor_incrementar, 5);
    if (!hash)
    {
        return;
    }

    bool todas_vacias = true;
    for (int i = 0; i < hash->capacidad; i++)
    {
        if (hash->tabla[i]->cantidad != 0)
        {
            todas_vacias = false;
        }
    }

    pa2m_afirmar(hash, "Puedo crear un HASH");
    pa2m_afirmar(hash->destructor == destructor_incrementar, " Se crea con destructor indicado");
    pa2m_afirmar(hash->capacidad == 5, " Se crea con capacidad inicial indicada");
    pa2m_afirmar(hash->cantidad == 0, " Se crea con cantidad de elementos CERO");
    pa2m_afirmar(todas_vacias, " Se crea con todas las listas de la tabla vacias");

    hash_destruir(hash);
}

void puedoCrearUnHash_conCapacidadInicialMenorAlMinimo()
{
    hash_t *hash = hash_crear(free, 0);
    if (!hash)
    {
        return;
    }

    pa2m_afirmar(hash, "Puedo crear un HASH con menor capacidad inicial que la minima");
    pa2m_afirmar(hash->capacidad == CAPACIDAD_INICIAL_MINIMA, " Se crea con capacidad inicial minima");

    hash_destruir(hash);
}

void dadoUnHashNulo_cantidadDevuelveCero()
{
    hash_t *hash = NULL;

    pa2m_afirmar(hash_cantidad(hash) == 0, "Dado un HASH nulo, cantidad devuelve CERO");
}

void dadoUnHashVacio_cantidadDevuelveCero()
{
    hash_t *hash = hash_crear(NULL, 5);
    if (!hash)
    {
        return;
    }

    pa2m_afirmar(hash_cantidad(hash) == 0, "Dado un HASH vacio, cantidad devuelve CERO");

    hash_destruir(hash);
}

void dadoUnHashNoVacio_cantidadDevuelveCorrectamente()
{
    hash_t *hash = hash_crear(NULL, 5);
    if (!hash)
    {
        return;
    }

    hash->cantidad = 3;

    pa2m_afirmar(hash_cantidad(hash) == 3, "Dado un HASH no vacio, cantidad devuelve correctamente");

    hash_destruir(hash);
}

void dadoUnHashNulo_alInsertar_devuelveError()
{
    hash_t *hash = NULL;

    int elemento = 10;

    pa2m_afirmar(hash_insertar(hash, "clave", &elemento) == ERROR, "Dado un HASH nulo, al insertar, devuelve ERROR");
}

void dadoUnHash_puedoInsertarElementos()
{
    hash_t *hash = hash_crear(free, 5);
    if (!hash)
    {
        return;
    }

    bool error = false;

    insertar_numero(hash, "AA", 10, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    pa2m_afirmar(hash_contiene(hash, "AA") == true, "Dado un hash vacio, puedo insertar un elemento");
    pa2m_afirmar(*(int *)hash_obtener(hash, "AA") == 10, " Puedo acceder al elemento insertado");
    pa2m_afirmar(hash_cantidad(hash) == 1, " La cantidad de elementos del hash aumenta correctamente");

    insertar_numero(hash, "BC", 20, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    pa2m_afirmar(hash_contiene(hash, "BC") == true, "Puedo insertar otro elemento");
    pa2m_afirmar(*(int *)hash_obtener(hash, "BC") == 20, " Puedo acceder al elemento insertado");
    pa2m_afirmar(hash_cantidad(hash) == 2, " La cantidad de elementos del hash aumenta correctamente");

    insertar_numero(hash, "BC", 40, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    pa2m_afirmar(hash_contiene(hash, "BC") == true, "Puedo modificar un elemento");
    pa2m_afirmar(*(int *)hash_obtener(hash, "BC") == 40, " Puedo acceder al elemento modificado");
    pa2m_afirmar(hash_cantidad(hash) == 2, " La cantidad de elementos se mantiene");

    pa2m_afirmar(hash_contiene(hash, "AA"), "Aun contiene el primer elemento insetado");
    pa2m_afirmar(hash_contiene(hash, "AA"), " Aun puedo acceder al primer elemento insetado");

    hash_destruir(hash);
}

void dadoUnHash_puedoAplicarDestructor()
{
    hash_t *hash = hash_crear(destructor_incrementar, 5);
    if (!hash)
    {
        return;
    }

    int elemento1 = 10;
    int elemento2 = 20;
    int elemento3 = 30;

    bool error = false;
    insertar_puntero(hash, "a", &elemento1, &error);
    insertar_puntero(hash, "b", &elemento2, &error);
    insertar_puntero(hash, "c", &elemento3, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    hash_destruir(hash);

    pa2m_afirmar(elemento1 == 11 && elemento2 == 21 && elemento3 == 31,
                 "Al destruir un hash con elementos, el destructor se aplica correctamente a cada elemento");
}

void dadoUnHashNull_puedoDestruirSinErrores()
{
    hash_t *hash = NULL;

    hash_destruir(hash);
}

void dadoUnHashNulo_alVerificarSiContieneUnElemento_devuelveFalse()
{
    hash_t *hash = NULL;

    pa2m_afirmar(!hash_contiene(hash, "clave"), "Al verificar si un hash nulo tiene un elemento, devuelve FALSE");
}

void dadoUnHashVacio_alVerificarSiContieneUnElemento_devuelveFalse()
{
    hash_t *hash = hash_crear(NULL, 5);
    if (!hash)
    {
        return;
    }

    pa2m_afirmar(!hash_contiene(hash, "clave"), "Al verificar si un hash vacio tiene un elemento, devuelve FALSE");

    hash_destruir(hash);
}

void dadoUnHashNoVacio_AlVerificarSiContieneUnElemento_devuelveCorrectamente()
{
    hash_t *hash = hash_crear(free, 5);
    if (!hash)
    {
        return;
    }

    bool error = false;
    insertar_numero(hash, "AA", 10, &error);
    insertar_numero(hash, "BC", 20, &error);
    insertar_numero(hash, "DD", 30, &error);
    insertar_numero(hash, "EF", 40, &error);
    insertar_numero(hash, "GG", 50, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    pa2m_afirmar(hash_contiene(hash, "BC"), "Dado un hash con las claves [AA, BC, DD, EF, GG], al verificar si contiene BC devuelve TRUE");
    pa2m_afirmar(hash_contiene(hash, "EF"), " Al verificar si contiene EF, devuelve TRUE");
    pa2m_afirmar(!hash_contiene(hash, "ZZ"), " Al verificar si contiene ZZ, devuelve FALSE");

    hash_destruir(hash);
}

void dadoUnHashNulo_alObtenerUnElemento_devuelveNULL()
{
    hash_t *hash = NULL;

    pa2m_afirmar(!hash_obtener(hash, "clave"), "Dado un HASH nulo, al obtener un elemento, devuelve NULL");
}

void dadoUnHashVacio_alObtenerUnElemento_devuelveNULL()
{
    hash_t *hash = hash_crear(NULL, 5);
    if (!hash)
    {
        return;
    }

    pa2m_afirmar(!hash_obtener(hash, "clave"), "Dado un HASH vacio, al obtener un elemento, devuelve NULL");

    hash_destruir(hash);
}

void dadoUnHashNoVacio_alObtenerUnElemento_devuelveCorrectamente()
{
    hash_t *hash = hash_crear(free, 5);
    if (!hash)
    {
        return;
    }

    bool error = false;
    insertar_numero(hash, "AA", 10, &error);
    insertar_numero(hash, "BC", 20, &error);
    insertar_numero(hash, "DD", 30, &error);
    insertar_numero(hash, "EF", 40, &error);
    insertar_numero(hash, "GG", 50, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    pa2m_afirmar(*(int *)hash_obtener(hash, "AA") == 10,
                 "Dado un hash con las claves [AA, BC, DD, EF, GG], al obtener AA devuelve correctamente");

    pa2m_afirmar(*(int *)hash_obtener(hash, "EF") == 40, " Al obtener EF, devuelve correctamente");

    pa2m_afirmar(!hash_obtener(hash, "ZZ"), " Al obtener ZZ, devuelve NULL");

    hash_destruir(hash);
}

void dadoUnHashNulo_alQuitarUnElemento_devuelveError()
{
    hash_t *hash = NULL;

    pa2m_afirmar(hash_quitar(hash, "clave") == ERROR, "Al quitar un elemento de un HASH nulo, devuelve ERROR");
}

void dadoUnHashVacio_alQuitarUnElemento_devuelveError()
{
    hash_t *hash = hash_crear(NULL, 5);
    if (!hash)
    {
        return;
    }

    pa2m_afirmar(hash_quitar(hash, "clave") == ERROR, "Al quitar un elemento de un HASH vacio, devuelve ERROR");

    hash_destruir(hash);
}

void dadoUnHashNoVacio_puedoQuitarElementos()
{
    hash_t *hash = hash_crear(free, 5);
    if (!hash)
    {
        return;
    }

    bool error = false;
    insertar_numero(hash, "AA", 10, &error);
    insertar_numero(hash, "BC", 20, &error);
    insertar_numero(hash, "DD", 30, &error);
    insertar_numero(hash, "EF", 40, &error);
    insertar_numero(hash, "GG", 50, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    hash_quitar(hash, "AA");
    pa2m_afirmar(!hash_contiene(hash, "AA"), "Dado un HASH no vacio, puedo quitar un elemento");

    hash_quitar(hash, "DD");
    pa2m_afirmar(!hash_contiene(hash, "DD"), " Puedo quitar otro elemento");

    hash_quitar(hash, "GG");
    pa2m_afirmar(!hash_contiene(hash, "GG"), " Puedo quitar otro elemento");

    pa2m_afirmar(hash_cantidad(hash) == 2, "La cantidad de elementos se disminuye correctamente al quitar");

    hash_destruir(hash);
}

void dadoUnHashNulo_alLlamarUnaFuncionConCadaClave_devuelveCERO()
{
    hash_t *hash = NULL;

    int suma = 0;
    int contador = hash_con_cada_clave(hash, sumar_claves, &suma);
    pa2m_afirmar(contador == 0, "Dado un hash nulo, al llamar a una funcion con cada clave, devuelve CERO");
}

void dadoUnHashVacio_alLlamarUnaFuncionConCadaClave_devuelveCERO()
{
    hash_t *hash = hash_crear(NULL, 5);
    if (!hash)
    {
        return;
    }

    int suma = 0;
    int contador = hash_con_cada_clave(hash, sumar_claves, &suma);
    pa2m_afirmar(contador == 0, "Dado un hash vacio, al llamar a una funcion con cada clave, devuelve CERO");

    hash_destruir(hash);
}

void dadoUnHashNoVacio_alLlamarUnaFuncionConCadaClave_devuelveCorrectamente()
{
    hash_t *hash = hash_crear(free, 5);
    if (!hash)
    {
        return;
    }

    bool error = false;
    insertar_numero(hash, "AA", 10, &error);
    insertar_numero(hash, "BC", 20, &error);
    insertar_numero(hash, "DD", 30, &error);
    insertar_numero(hash, "EF", 40, &error);
    insertar_numero(hash, "GG", 50, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    int suma_esperada = funcion_hash("AA") + funcion_hash("BC") +
                        funcion_hash("DD") + funcion_hash("EF") +
                        funcion_hash("GG");

    int suma = 0;
    int contador = hash_con_cada_clave(hash, sumar_claves, &suma);
    pa2m_afirmar(contador == 5, "Dado un hash no vacio, al llamar a una funcion con cada clave, devuelve correctamente");
    pa2m_afirmar(suma == suma_esperada, " Las funciones se aplican correctamente");

    hash_destruir(hash);
}

void dadoUnHashNoVacio_alLlamarUnaFuncionConCadaClave_devuelveCorrectamenteHastaCorte()
{
    hash_t *hash = hash_crear(free, 10);
    if (!hash)
    {
        return;
    }

    bool error = false;
    insertar_numero(hash, "GG", 1, &error);    // indice = 3 (no suma)
    insertar_numero(hash, "TY", 4, &error);    // indice = 2 (suma)
    insertar_numero(hash, "corte", 6, &error); // indice = 2 (suma) CORTE
    insertar_numero(hash, "DC", 5, &error);    // indice = 2 (no suma)
    insertar_numero(hash, "RE", 2, &error);    // indice = 0 (suma)
    insertar_numero(hash, "HT", 3, &error);    // indice = 0 (suma)
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    int suma_esperada = funcion_hash("TY") + funcion_hash("corte") +
                        funcion_hash("RE") + funcion_hash("HT");
    int contador_esperado = 4;

    int suma = 0;
    int contador = hash_con_cada_clave(hash, sumar_claves_hasta_corte, &suma);

    pa2m_afirmar(contador == contador_esperado, "Dado un hash no vacio, al llamar a una funcion con cada clave hasta corte, devuelve correctamente");
    pa2m_afirmar(suma == suma_esperada, " Las funciones se aplican correctamente");

    hash_destruir(hash);
}

void dadoUnHash_alAlcanzarElLimiteDeElementosImpuesto_SeHaceUnRehash()
{
    hash_t *hash = hash_crear(free, 5);
    if (!hash)
    {
        return;
    }

    bool error = false;
    insertar_numero(hash, "Algo2Mendez", 10, &error);
    insertar_numero(hash, "Bulbasaur", 20, &error);
    insertar_numero(hash, "Squirtle", 30, &error);
    insertar_numero(hash, "Charmander", 40, &error);
    insertar_numero(hash, "Venusaur", 50, &error);
    insertar_numero(hash, "Charizard", 60, &error);
    insertar_numero(hash, "Blastoise", 70, &error);
    if (error)
    {
        hash_destruir(hash);
        return;
    }

    pa2m_afirmar(hash->capacidad == 5 * REHASH_FACTOR_DE_MULTIPLICIDAD,
                 "Al rehashear, la capacidad maxima se aumenta correctamente");

    pa2m_afirmar(hash->cantidad == 7, "Al rehashear, conserva la cantidad de elementos");

    pa2m_afirmar(hash->destructor == free, "Al rehashear, conserva el destructor");

    bool contiene_elementos = hash_contiene(hash, "Algo2Mendez") && hash_contiene(hash, "Bulbasaur") &&
                              hash_contiene(hash, "Squirtle") && hash_contiene(hash, "Charmander") &&
                              hash_contiene(hash, "Venusaur") && hash_contiene(hash, "Charizard") &&
                              hash_contiene(hash, "Blastoise");

    pa2m_afirmar(contiene_elementos, "Al rehashear, aun contiene los elementos");

    hash_destruir(hash);
}

int main()
{
    pa2m_nuevo_grupo("HASH CREAR");
    puedoCrearUnHash();
    puedoCrearUnHash_conCapacidadInicialMenorAlMinimo();

    pa2m_nuevo_grupo("HASH CANTIDAD");
    dadoUnHashNulo_cantidadDevuelveCero();
    dadoUnHashVacio_cantidadDevuelveCero();
    dadoUnHashNoVacio_cantidadDevuelveCorrectamente();

    pa2m_nuevo_grupo("HASH INSERTAR");
    dadoUnHashNulo_alInsertar_devuelveError();
    dadoUnHash_puedoInsertarElementos();

    pa2m_nuevo_grupo("HASH DESTRUIR");
    dadoUnHash_puedoAplicarDestructor();
    dadoUnHashNull_puedoDestruirSinErrores();

    pa2m_nuevo_grupo("HASH CONTIENE");
    dadoUnHashNulo_alVerificarSiContieneUnElemento_devuelveFalse();
    dadoUnHashVacio_alVerificarSiContieneUnElemento_devuelveFalse();
    dadoUnHashNoVacio_AlVerificarSiContieneUnElemento_devuelveCorrectamente();

    pa2m_nuevo_grupo("HASH OBTENER");
    dadoUnHashNulo_alObtenerUnElemento_devuelveNULL();
    dadoUnHashVacio_alObtenerUnElemento_devuelveNULL();
    dadoUnHashNoVacio_alObtenerUnElemento_devuelveCorrectamente();

    pa2m_nuevo_grupo("HASH QUITAR");
    dadoUnHashNulo_alQuitarUnElemento_devuelveError();
    dadoUnHashVacio_alQuitarUnElemento_devuelveError();
    dadoUnHashNoVacio_puedoQuitarElementos();

    pa2m_nuevo_grupo("HASH CON CADA CLAVE");
    dadoUnHashNulo_alLlamarUnaFuncionConCadaClave_devuelveCERO();
    dadoUnHashVacio_alLlamarUnaFuncionConCadaClave_devuelveCERO();
    dadoUnHashNoVacio_alLlamarUnaFuncionConCadaClave_devuelveCorrectamente();
    dadoUnHashNoVacio_alLlamarUnaFuncionConCadaClave_devuelveCorrectamenteHastaCorte();

    pa2m_nuevo_grupo("HASH REHASH");
    dadoUnHash_alAlcanzarElLimiteDeElementosImpuesto_SeHaceUnRehash();

    return pa2m_mostrar_reporte();
}
