#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 10000

struct FarmaSalud {
    struct NodoFarmacia *farmacias;
    struct NodoProveedores *proveedores;
};

struct NodoFarmacia {
    struct Farmacia *farmacia;
    struct NodoFarmacia *ant, *sig;
};

struct Farmacia {
    int idFarmacia;
    int codigoCiudad;
    int codigoRegion;
    int capacidadAlmacenamiento;
    char *nombreFarmacia;
    struct arregloProductos *inventario;
    struct NodoOrdenCompra *comprasProveedor;
    struct NodoVenta *ventas;
    struct NodoProveedores *proveedores;
};

struct arregloProductos {
    struct Producto *arrProductos[MAX];
    int pLibre;
};

struct Producto {
    int codigoProducto;
    int precioProducto;
    int *numeroEntrega;
    int requiereReceta;
    int stock;
    int umbral;
    char *nombreProducto;
    char *descripcionProducto;
    char *categoria;
    int diaCaducidad;
    int mesCaducidad;
    int anioCaducidad;
    char *nombreLote;
    struct Proveedor *proveedor;
};

struct NodoProveedores {
    struct Proveedor *proveedor;
    struct NodoProveedores *sig;
};

struct Proveedor {
    int idProveedor;
    int numeroTelefonico;
    char *nombreProveedor;
    char *email;
    struct NodoLotes *lotes;
};

struct NodoLotes {
    struct Lote *LoteProducto;
    struct NodoLotes *izq, *der;
};

struct Lote {
    struct Producto *productoDentroLote;
    int codigoLote;
    int precioLote;
    int cantidadProductoDelLote;
    char *nombreLote;
    int diaCaducidadLote;
    int mesCaducidadLote;
    int anioCaducidadLote;
};

struct NodoVenta {
    struct NodoVenta *sig;
    struct Venta *ventas;
};

struct Venta {
    int idFarmacia;
    int codigoProductoVendido;
    int cantidadVendida;
    int numeroBoleta;
    int montoPagado;
    int clienteTrajoReceta;
    char *nombreProductoVendido;
    char *rutCliente;
    int diaVenta;
    int mesVenta;
    int anioVenta;
};


struct ProductoOrden {
    int codigoProducto;
    int cantidad;
    struct ProductoOrden *sig;
};

struct OrdenCompra {
    int idOrden;
    int montoComprado;
    int dia;
    int mes;
    int anio;
    int idFarmacia;
    struct ProductoOrden *productos;
};

struct NodoOrdenCompra {
    struct OrdenCompra *compraProveedor;
    struct NodoOrdenCompra *sig;
};


//0. Funciones Auxiliares.--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Farmacia *seleccionarFarmacia(struct NodoFarmacia *headFarmacias, int idFarmacia)
{
    struct NodoFarmacia *rec = NULL;
    if (headFarmacias == NULL)
        return NULL;

    rec = headFarmacias;
    while (rec != NULL) {  // Línea 129
        if (rec->farmacia->idFarmacia == idFarmacia) {  // Línea 130
            return rec->farmacia;
        }
        rec = rec->sig;
    }
    return NULL;
}


float calcularPromedioVentas(struct Farmacia *farmacias, int codigoProducto, int mesVenta)
{
    int totalVentas = 0, cantidadesVendidas = 0;
    struct NodoVenta *recVentas = NULL;

    if (farmacias == NULL)
        return 0.0;

    recVentas = farmacias->ventas;  // Línea 147

    while (recVentas != NULL) {
        if (recVentas->ventas->codigoProductoVendido == codigoProducto && recVentas->ventas->mesVenta == mesVenta) {
            cantidadesVendidas += recVentas->ventas->cantidadVendida;
        }
        totalVentas++;
        recVentas = recVentas->sig;
    }

    if (totalVentas == 0)
        return 0.0;

    return (float)cantidadesVendidas / totalVentas;
}


int verificarReabastecimiento(struct Farmacia *farmacia, struct Venta *ventaSeleccionada) {

    int i;
    struct Producto *producto = NULL;
    float promedioVentas = 0.0;

    if(farmacia == NULL || ventaSeleccionada == NULL)
        return 0;

    for (i = 0; i < farmacia->inventario->pLibre; i++) {
        if (farmacia->inventario->arrProductos[i]->codigoProducto == ventaSeleccionada->codigoProductoVendido) {
            producto = farmacia->inventario->arrProductos[i];
        }
    }

    if (producto == NULL) {
        return 0;
    }

    promedioVentas = calcularPromedioVentas(farmacia, ventaSeleccionada->codigoProductoVendido, ventaSeleccionada->mesVenta);

    if (promedioVentas >= producto->umbral && producto->stock < producto->umbral) {
        return 1;
    } else {
        return 0;
    }
}


int reducirStockProducto(struct arregloProductos *arreglo, int codigoProducto, int cantidad)
{
    int i;
    for (i = 0; i < arreglo->pLibre; i++)
    {
        if (arreglo->arrProductos[i]->codigoProducto == codigoProducto)
        {
            if (arreglo->arrProductos[i]->stock >= cantidad)
            {
                arreglo->arrProductos[i]->stock -= cantidad;
            }
            else
            {
                return 0;
            }
            return 1;
        }

    }
    return 0;
}


int contarProductosDiferentes(struct arregloProductos *productos) {
    int i, j, contador = 0, esDiferente = 1;

    for (i = 0; i < productos->pLibre; i++) {

        for (j = 0; j < productos->pLibre; j++) {
            if (i != j && productos->arrProductos[i]->codigoProducto == productos->arrProductos[j]->codigoProducto) {
                esDiferente = 0;
            }
        }
        if (esDiferente == 1) {
            contador++;
        }
    }
    return contador;
}


int obtenerCantidadDeProducto(struct arregloProductos *arreglo, int codigoProducto) {
    int i, contador = 0;

    for (i = 0; i < arreglo->pLibre; i++) {
        if (arreglo->arrProductos[i]->codigoProducto == codigoProducto) {
            contador += arreglo->arrProductos[i]->stock;
        }
    }
    return contador;
}


int sumaCantidadDeProductosTotales(struct arregloProductos *arreglo) {
    int i, total = 0;

    for (i = 0; i < arreglo->pLibre; i++) {
        total += arreglo->arrProductos[i]->stock;
    }
    return total;
}


int buscarProducto(struct arregloProductos *arreglo, int codigoProducto);


int calcularMesesRestantes(int mesActual, int anioActual, int mesLlegada, int anioLlegada) {
    return (anioLlegada - anioActual) * 12 + (mesLlegada - mesActual);
}


//1. Funciones de Creación.---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Producto *crearProducto(int codigo, int precio, char *nombre, char *descripcion, char *categoria, int diaCaducidad, int mesCaducidad, int anioCaducidad, struct Proveedor *proveedor, char *nombreLote, int *numeroEntrega, int requiereReceta, int stock, int umbral) {
    struct Producto *nuevoProducto = NULL;

    nuevoProducto = (struct Producto *)malloc(sizeof(struct Producto));

    nuevoProducto->codigoProducto = codigo;
    nuevoProducto->precioProducto = precio;
    nuevoProducto->nombreProducto = strdup(nombre);
    nuevoProducto->descripcionProducto = strdup(descripcion);
    nuevoProducto->categoria = strdup(categoria);
    nuevoProducto->diaCaducidad = diaCaducidad;
    nuevoProducto->mesCaducidad = mesCaducidad;
    nuevoProducto->anioCaducidad = anioCaducidad;
    nuevoProducto->proveedor = proveedor;
    nuevoProducto->nombreLote = strdup(nombreLote);
    nuevoProducto->numeroEntrega = numeroEntrega;
    nuevoProducto->requiereReceta = requiereReceta;
    nuevoProducto->stock = stock;
    nuevoProducto->umbral = umbral;
    return nuevoProducto;
}


struct arregloProductos *crearArregloProductos() {
    struct arregloProductos *arreglo = NULL;
    arreglo = (struct arregloProductos *)malloc(sizeof(struct arregloProductos));
    arreglo->pLibre = 0;
    return arreglo;
}


struct Farmacia *crearFarmacia(int idFarmacia, int codigoCiudad, int codigoRegion, char *nombreFarmacia, int capacidadAlmacenamiento, struct NodoProveedores *proveedores, struct arregloProductos *arregloProductos) {
    struct Farmacia *nuevaFarmacia = NULL;

    nuevaFarmacia = (struct Farmacia *)malloc(sizeof(struct Farmacia));
    nuevaFarmacia->idFarmacia = idFarmacia;
    nuevaFarmacia->codigoCiudad = codigoCiudad;
    nuevaFarmacia->codigoRegion = codigoRegion;
    nuevaFarmacia->nombreFarmacia = strdup(nombreFarmacia);
    nuevaFarmacia->capacidadAlmacenamiento = capacidadAlmacenamiento;
    nuevaFarmacia->inventario = arregloProductos; // Asigna el arreglo de productos específico
    nuevaFarmacia->comprasProveedor = NULL;
    nuevaFarmacia->ventas = NULL;
    nuevaFarmacia->proveedores = proveedores; // Asigna la lista de proveedores
    return nuevaFarmacia;
}


struct Lote *crearLote(int codigoLote, struct Producto *producto, int precioLote, int cantidadProductoLote, char *nombreLote, int diaCaducidadLote, int mesCaducidadLote, int anioCaducidadLote) {

    struct Lote *nuevoLote = NULL;

    nuevoLote = (struct Lote *)malloc(sizeof(struct Lote));
    nuevoLote->codigoLote = codigoLote;
    nuevoLote->productoDentroLote = producto;
    nuevoLote->precioLote = precioLote;
    nuevoLote->cantidadProductoDelLote = cantidadProductoLote;
    nuevoLote->nombreLote = strdup(nombreLote);
    nuevoLote->diaCaducidadLote = diaCaducidadLote;
    nuevoLote->mesCaducidadLote = mesCaducidadLote;
    nuevoLote->anioCaducidadLote = anioCaducidadLote;
    return nuevoLote;
}


struct Proveedor *crearProveedor(int idProveedor, char *nombreProveedor, int numeroTelefono, char *email) {
    struct Proveedor *nuevoProveedor = NULL;
    nuevoProveedor = (struct Proveedor *)malloc(sizeof(struct Proveedor));
    nuevoProveedor->idProveedor = idProveedor;
    nuevoProveedor->nombreProveedor = strdup(nombreProveedor);
    nuevoProveedor->numeroTelefonico = numeroTelefono;
    nuevoProveedor->email = strdup(email);
    nuevoProveedor->lotes = NULL; // Inicializar con NULL o asignar segun sea necesario
    return nuevoProveedor;
}


struct Venta *crearVenta(int idFarmacia, int codigoProductoVendido, char *nombreProductoVendido, int cantidadVendida, int numeroBoleta, int montoPagado, char *rutCliente, int clienteTrajoReceta, int diaVenta, int mesVenta, int anioVenta) {

    struct Venta *nuevaVenta = NULL;
    nuevaVenta = (struct Venta *)malloc(sizeof(struct Venta));
    nuevaVenta->idFarmacia = idFarmacia;
    nuevaVenta->codigoProductoVendido = codigoProductoVendido;
    nuevaVenta->nombreProductoVendido = strdup(nombreProductoVendido);
    nuevaVenta->cantidadVendida = cantidadVendida;
    nuevaVenta->numeroBoleta = numeroBoleta;
    nuevaVenta->montoPagado = montoPagado;
    nuevaVenta->rutCliente = strdup(rutCliente);
    nuevaVenta->clienteTrajoReceta = clienteTrajoReceta;
    nuevaVenta->diaVenta = diaVenta;
    nuevaVenta->mesVenta = mesVenta;
    nuevaVenta->anioVenta = anioVenta;
    return nuevaVenta;
}


struct OrdenCompra *crearOrdenCompra(int idOrden, int montoComprado, int dia, int mes, int anio, int idFarmacia) {
    struct OrdenCompra *nuevaOrden = NULL;
    nuevaOrden = (struct OrdenCompra *)malloc(sizeof(struct OrdenCompra));
    nuevaOrden->idOrden = idOrden;
    nuevaOrden->montoComprado = montoComprado;
    nuevaOrden->dia = dia;
    nuevaOrden->mes = mes;
    nuevaOrden->anio = anio;
    nuevaOrden->idFarmacia = idFarmacia;
    nuevaOrden->productos = NULL;
    return nuevaOrden;
}


struct ProductoOrden *crearProductoOrden(int codigoProducto, int cantidad) {
    struct ProductoOrden *nuevoProductoOrden = NULL;
    nuevoProductoOrden = (struct ProductoOrden *)malloc(sizeof(struct ProductoOrden));
    nuevoProductoOrden->codigoProducto = codigoProducto;
    nuevoProductoOrden->cantidad = cantidad;
    nuevoProductoOrden->sig = NULL;
    return nuevoProductoOrden;
}


//3. Funciones de insertar.---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int agregarProductoAlArreglo(struct arregloProductos *arreglo, struct Producto *producto) {
    if (arreglo->pLibre < MAX) {
        arreglo->arrProductos[arreglo->pLibre++] = producto;
        return 1;
    }
    return 0;
}


int insertarFarmacia(struct NodoFarmacia **headFarmacias, struct Farmacia *nuevaFarmacia) {
    struct NodoFarmacia *rec = NULL;

    if (*headFarmacias == NULL) {
        (*headFarmacias) = (struct NodoFarmacia *)malloc(sizeof(struct NodoFarmacia));
        (*headFarmacias)->farmacia = nuevaFarmacia;
        (*headFarmacias)->ant = NULL;
        (*headFarmacias)->sig = NULL;
        return 1;
    } else {
        rec = *headFarmacias;
        while (rec->sig != NULL)
            rec = rec->sig;
        rec->sig = (struct NodoFarmacia *)malloc(sizeof(struct NodoFarmacia));
        rec->sig->farmacia = nuevaFarmacia;
        rec->sig->ant = rec;
        rec->sig->sig = NULL;
        return 1;
    }
}


void agregarFarmacia(struct FarmaSalud *farmaSalud, struct Farmacia *nuevaFarmacia) {
    insertarFarmacia(&farmaSalud->farmacias, nuevaFarmacia);
}


int insertarLote(struct NodoLotes **arbol, struct Lote *nuevoLote) {

    if ((*arbol) == NULL) {
        (*arbol) = (struct NodoLotes *)malloc(sizeof(struct NodoLotes));
        (*arbol)->LoteProducto = nuevoLote;
        (*arbol)->izq = NULL;
        (*arbol)->der = NULL;
        return 1;
    }

    if ((*arbol) != NULL) {
        if (nuevoLote->codigoLote < (*arbol)->LoteProducto->codigoLote) {
            return insertarLote(&((*arbol)->izq), nuevoLote);
        } else if (nuevoLote->codigoLote > (*arbol)->LoteProducto->codigoLote) {
            return insertarLote(&((*arbol)->der), nuevoLote);
        }
    }
    return 0;
}


int insertarProveedor(struct NodoProveedores **headProveedores, struct Proveedor *nuevoProveedor)
{
    struct NodoProveedores *rec = NULL;
    if ((*headProveedores) == NULL)
    {
        (*headProveedores) = (struct NodoProveedores *)malloc(sizeof(struct NodoProveedores));
        (*headProveedores)->proveedor = nuevoProveedor;
        (*headProveedores)->sig = NULL;
        return 1;
    }
    else
    {
        rec = *headProveedores;
        while (rec->sig != NULL)
        {
            rec = rec->sig;
        }
        rec->sig = (struct NodoProveedores *)malloc(sizeof(struct NodoProveedores));
        rec->sig->proveedor = nuevoProveedor;
        rec->sig->sig = NULL;
        return 1;
    }
}


void agregarProveedor(struct FarmaSalud *farmaSalud, struct Proveedor *nuevoProveedor) {
    insertarProveedor(&farmaSalud->proveedores, nuevoProveedor);
}


int insertarVenta(struct NodoVenta **headVentas, struct Venta *nuevaVenta, struct arregloProductos *arreglo) {
    int indiceProducto = 0;
    struct NodoVenta *rec = NULL;

    if (headVentas == NULL || nuevaVenta == NULL || arreglo == NULL) {
        return 0; // Fracaso
    }

    indiceProducto = buscarProducto(arreglo, nuevaVenta->codigoProductoVendido);
    if (indiceProducto != -1) {
        if (arreglo->arrProductos[indiceProducto]->stock < nuevaVenta->cantidadVendida) {
            return 0; // Fracaso
        }
        reducirStockProducto(arreglo, arreglo->arrProductos[indiceProducto]->codigoProducto, nuevaVenta->cantidadVendida);
    } else {
        return 0; // Fracaso
    }
    if (*headVentas == NULL) {
        *headVentas = (struct NodoVenta *)malloc(sizeof(struct NodoVenta));
        (*headVentas)->ventas = nuevaVenta;
        (*headVentas)->sig = NULL;
    } else {
        rec = *headVentas;
        while (rec->sig != NULL)
            rec = rec->sig;
        rec->sig = (struct NodoVenta *)malloc(sizeof(struct NodoVenta));
        rec->sig->ventas = nuevaVenta;
        rec->sig->sig = NULL;
    }
    return 1; // Éxito
}


void agregarProductoAOrden(struct OrdenCompra *orden, struct ProductoOrden *nuevoProductoOrden) {

    struct ProductoOrden *rec = NULL;
    if (orden->productos == NULL) {
        orden->productos = nuevoProductoOrden;
    } else {
        rec = orden->productos;
        while (rec->sig != NULL) {
            rec = rec->sig;
        }
        rec->sig = nuevoProductoOrden;
    }
}


int insertarOrdenCompra(struct NodoOrdenCompra **headOrdenes, struct OrdenCompra *nuevaOrden)
{
    struct NodoOrdenCompra *rec = NULL;
    if (*headOrdenes == NULL)
    {
        *headOrdenes = (struct NodoOrdenCompra *)malloc(sizeof(struct NodoOrdenCompra));
        (*headOrdenes)->compraProveedor = nuevaOrden;
        (*headOrdenes)->sig = NULL;
        return 1;
    }
    else
    {
        rec = *headOrdenes;
        while (rec->sig != NULL)
            rec = rec->sig;
        rec->sig = (struct NodoOrdenCompra *)malloc(sizeof(struct NodoOrdenCompra));
        rec->sig->compraProveedor = nuevaOrden;
        rec->sig->sig = NULL;
        return 1;
    }
}


int insertarVentaEnFarmacia(struct NodoFarmacia *headFarmacias, struct Venta* venta)
{
    struct NodoFarmacia *recFarmacias = NULL;
    if(headFarmacias == NULL)
        return 0;
    recFarmacias = headFarmacias;

    while (recFarmacias != NULL)
    {
        if (recFarmacias->farmacia->idFarmacia == venta->idFarmacia)
        {
            struct NodoVenta *nuevoNodo = (struct NodoVenta *) malloc(sizeof(struct NodoVenta));
            nuevoNodo->ventas = venta;
            nuevoNodo->sig = recFarmacias->farmacia->ventas;
            recFarmacias->farmacia->ventas = nuevoNodo;
            return 1;
        }
        recFarmacias = recFarmacias->sig;
    }
    return 0;
}


//4. Funciones buscar.--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int buscarProducto(struct arregloProductos *arreglo, int codigoProducto) {
    int i;

    for (i = 0; i < arreglo->pLibre; i++) {
        if (arreglo->arrProductos[i] && arreglo->arrProductos[i]->codigoProducto == codigoProducto) {
            return i;
        }
    }
    return -1;
}


struct Farmacia *buscarFarmacia(struct NodoFarmacia *headFarmacias, int idFarmacia) {
    struct NodoFarmacia *rec = NULL;

    if(headFarmacias == NULL)
        return NULL;

    rec = headFarmacias;

    while (rec != NULL) {
        if (rec->farmacia->idFarmacia == idFarmacia) {
            return rec->farmacia;
        }
        rec = rec->sig;
    }
    return NULL;
}


struct Proveedor *buscarProveedor(struct NodoProveedores *headProveedores, char *nombreProveedor) {
    struct NodoProveedores *rec = NULL;
    if(headProveedores == NULL)
        return NULL;

    rec = headProveedores;

    while (rec != NULL) {
        if (strcmp(rec->proveedor->nombreProveedor, nombreProveedor) == 0) {
            return rec->proveedor;
        }
        rec = rec->sig;
    }
    return NULL;
}


struct Proveedor *buscarProveedorPorId(struct NodoProveedores *headProveedores, int idProveedor) {
    struct NodoProveedores *rec = NULL;
    if(headProveedores == NULL)
        return NULL;

    rec = headProveedores;

    while (rec != NULL) {
        if (rec->proveedor->idProveedor == idProveedor) {
            return rec->proveedor;
        }
        rec = rec->sig;
    }
    return NULL;
}


struct Lote *buscarLote(struct NodoLotes *arbol, int codigoLote) {
    if (arbol == NULL) {
        return NULL;
    }

    if (arbol->LoteProducto->codigoLote == codigoLote) {
        return arbol->LoteProducto;
    }

    if (codigoLote < arbol->LoteProducto->codigoLote) {
        return buscarLote(arbol->izq, codigoLote);
    } else {
        return buscarLote(arbol->der, codigoLote);
    }
}


struct Venta *buscarVentaPorCodigo(struct NodoVenta *headVentas, int codigoProductoVendido) {
    struct NodoVenta *recVentas = NULL;
    if(headVentas == NULL)
        return NULL;


    if (headVentas->ventas->codigoProductoVendido == codigoProductoVendido) {
        return headVentas->ventas;
    }

    recVentas = headVentas->sig;

    while (recVentas != NULL) {
        if (recVentas->ventas->codigoProductoVendido == codigoProductoVendido) {
            return recVentas->ventas;
        }
        recVentas = recVentas->sig;
    }
    return NULL;
}


struct Venta *buscarVentaPorRut(struct NodoVenta *headVentas, char *rutCliente) {
    struct NodoVenta *recVentas = NULL;
    if(headVentas == NULL)
        return NULL;

    if (strcmp(headVentas->ventas->rutCliente, rutCliente) == 0) {
        return headVentas->ventas;
    }

    recVentas = headVentas->sig;

    while (recVentas != NULL) {
        if (strcmp(recVentas->ventas->rutCliente, rutCliente) == 0) {
            return recVentas->ventas;
        }
        recVentas = recVentas->sig;
    }
    return NULL;
}


struct Venta *buscarVentaPorBoleta(struct NodoVenta *headVentas, int numeroBoleta) {
    struct NodoVenta *recVenta = NULL;
    if(headVentas == NULL)
        return NULL;

    if (headVentas->ventas->numeroBoleta == numeroBoleta) {
        return headVentas->ventas;
    }

    recVenta = headVentas->sig;

    while (recVenta != NULL) {
        if (recVenta->ventas->numeroBoleta == numeroBoleta) {
            return recVenta->ventas;
        }
        recVenta = recVenta->sig;
    }
    return NULL;
}


struct Venta *buscarProductoMasVendido(struct NodoVenta *headVentas) {
    struct Venta *ventaMasVendida = NULL;
    struct NodoVenta *recVenta = NULL;

    if (headVentas == NULL) {
        return NULL;
    }

    ventaMasVendida = headVentas->ventas;
    recVenta = headVentas->sig;

    while (recVenta != NULL) {
        if (recVenta->ventas->cantidadVendida > ventaMasVendida->cantidadVendida) {
            ventaMasVendida = recVenta->ventas;
        }
        recVenta = recVenta->sig;
    }
    return ventaMasVendida;
}


int seguimientoEnvios(struct NodoOrdenCompra *headOrdenes, int idOrden, int mesActual, int anioActual) {

    int mesesRestantes;
    struct NodoOrdenCompra *recOrdenes = NULL;
    struct OrdenCompra *orden = NULL;

    if(headOrdenes == NULL)
        return 0;

    recOrdenes = headOrdenes;

    while (recOrdenes != NULL) {
        orden = recOrdenes->compraProveedor;
        if (orden->idOrden == idOrden) {
            mesesRestantes = calcularMesesRestantes(mesActual, anioActual, orden->mes, orden->anio);
            if (mesesRestantes > 0) {
                printf("La orden de compra con ID %d llegara en %d mes(es).\n", idOrden, mesesRestantes);
            } else if (mesesRestantes == 0) {
                printf("La orden de compra con ID %d llega este mes.\n", idOrden);
            } else {
                printf("La orden de compra con ID %d ya ha llegado.\n", idOrden);
            }
            return 1;
        }
        recOrdenes = recOrdenes->sig;
    }
    return 0;
}

//5. Funciones eliminar.------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void eliminarProducto(struct arregloProductos *arreglo, int indice) {
    int i;
    if (indice >= 0 && indice < arreglo->pLibre) {
        arreglo->arrProductos[indice] = NULL;
        for (i = indice; i < arreglo->pLibre - 1; i++) {
            arreglo->arrProductos[i] = arreglo->arrProductos[i + 1];
        }
        arreglo->pLibre--;
    }
}


int eliminarFarmacia(struct NodoFarmacia **headFarmacias, int idFarmacia)
{
    struct NodoFarmacia *recFarmacias = NULL;
    struct NodoFarmacia *anterior = NULL;

    if ((*headFarmacias) == NULL)
        return 0;

    recFarmacias = (*headFarmacias);
    while (recFarmacias != NULL)
    {
        if (recFarmacias->farmacia->idFarmacia == idFarmacia)
        {
            if (anterior == NULL)
            {
                (*headFarmacias) = recFarmacias->sig;
                if ((*headFarmacias) != NULL)
                    (*headFarmacias)->ant = NULL;
            }
            else
            {
                anterior->sig = recFarmacias->sig;
                if (recFarmacias->sig != NULL)
                    recFarmacias->sig->ant = anterior;
            }
            recFarmacias->farmacia = NULL;
            recFarmacias->sig = NULL;
            recFarmacias->ant = NULL;
            return 1;
        }

        anterior = recFarmacias;
        recFarmacias = recFarmacias->sig;
    }
    return 0;
}


int eliminarProveedor(struct NodoProveedores **headProveedores, char *nombreProveedor) {
    struct NodoProveedores *recProvedores = NULL;

    if (*headProveedores == NULL) {
        return 0;
    }

    if (strcmp((*headProveedores)->proveedor->nombreProveedor, nombreProveedor) == 0) {
        *headProveedores = (*headProveedores)->sig;
        return 1;
    }

    recProvedores = *headProveedores;

    while (recProvedores->sig != NULL) {
        if (strcmp(recProvedores->sig->proveedor->nombreProveedor, nombreProveedor) == 0) {
            recProvedores->sig = recProvedores->sig->sig;
            return 1;
        }
        recProvedores = recProvedores->sig;
    }
    return 0;
}


int eliminarProveedorPorId(struct NodoProveedores **headProveedores, int idProveedor) {
    struct NodoProveedores *recProvedores = NULL;

    if (*headProveedores == NULL) {
        return 0;
    }

    if ((*headProveedores)->proveedor->idProveedor == idProveedor) {
        *headProveedores = (*headProveedores)->sig;
        return 1;
    }

    recProvedores = *headProveedores;

    while (recProvedores->sig != NULL) {
        if (recProvedores->sig->proveedor->idProveedor == idProveedor) {
            recProvedores->sig = recProvedores->sig->sig;
            return 1;
        }
        recProvedores = recProvedores->sig;
    }
    return 0;
}


struct NodoLotes *eliminarLote(struct NodoLotes *arbol, int codigoLote) //rezar para que funcione
{
    struct NodoLotes *temp = NULL;
    if (arbol == NULL)
        return NULL;

    if (codigoLote < arbol->LoteProducto->codigoLote)
        arbol->izq = eliminarLote(arbol->izq, codigoLote);
    else if (codigoLote > arbol->LoteProducto->codigoLote)
        arbol->der = eliminarLote(arbol->der, codigoLote);

    else
    {
        if (arbol->izq == NULL)
        {
            temp = arbol->der;
            return temp;
        } else if (arbol->der == NULL)
        {
            temp = arbol->izq;
            return temp;
        }
        temp = arbol->der;
        while (temp && temp->izq != NULL)
        {
            temp = temp->izq;
        }
        arbol->LoteProducto = temp->LoteProducto;
        arbol->der = eliminarLote(arbol->der, temp->LoteProducto->codigoLote);
    }
    return arbol;
}


int eliminarVenta(struct NodoVenta **headVentas, int copiaNumeroBoleta)
{
    struct NodoVenta *recVentas = NULL;
    if (*headVentas == NULL)

        return 0;

    if ((*headVentas)->ventas->numeroBoleta == copiaNumeroBoleta)
    {
        *headVentas = (*headVentas)->sig;
        return 1;
    }
    recVentas = (*headVentas);
    while (recVentas->sig != NULL)
    {
        if (recVentas->sig->ventas->numeroBoleta == copiaNumeroBoleta) {
            recVentas->sig = recVentas->sig->sig;
            return 1;
        }
        recVentas = recVentas->sig;
    }
    return 0;
}


void eliminarProductosCaducados(struct arregloProductos *arreglo, int dia, int mes, int anio) {
    int i;

    struct Producto *producto = NULL;
    for (i = 0; i < arreglo->pLibre; i++) {
        producto = arreglo->arrProductos[i];

        if (producto->anioCaducidad < anio ||
            (producto->anioCaducidad == anio && producto->mesCaducidad < mes) ||
            (producto->anioCaducidad == anio && producto->mesCaducidad == mes && producto->diaCaducidad <= dia)) {
            eliminarProducto(arreglo, i);
            i--;
        }
    }
}

//7. Funciones mostrar.-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void mostrarProductos(struct Farmacia *farmaciaSeleccionada) {
    int i;
    struct Producto *producto = NULL;
    if (farmaciaSeleccionada == NULL){
        return;
    }

    for (i = 0; i < farmaciaSeleccionada->inventario->pLibre; i++) {
        producto = farmaciaSeleccionada->inventario->arrProductos[i];
        printf("Producto %d:\n", i + 1);
        printf("  Codigo: %d\n", producto->codigoProducto);
        printf("  Nombre: %s\n", producto->nombreProducto);
        printf("  Descripcion: %s\n", producto->descripcionProducto);
        printf("  Categoria: %s\n", producto->categoria);
        printf("  Fecha de caducidad: %d-%d-%d\n", producto->diaCaducidad, producto->mesCaducidad, producto->anioCaducidad);
        printf("  Precio: %d\n", producto->precioProducto);
        printf("  Requiere receta: %s\n", producto->requiereReceta ? "Si" : "No");
        printf("  Stock: %d\n", producto->stock);
        printf("  Umbral: %d\n", producto->umbral);
        printf("\n");
    }
}


void mostrarFarmacias(struct NodoFarmacia *headFarmacias) {
    struct NodoFarmacia *rec = NULL;
    struct Farmacia *farmacia = NULL;

    rec = headFarmacias;
    while (rec != NULL) {
        farmacia = rec->farmacia;
        printf("Farmacia:\n");
        printf("  ID: %d\n", farmacia->idFarmacia);
        printf("  Nombre: %s\n", farmacia->nombreFarmacia);
        printf("  Codigo Ciudad: %d\n", farmacia->codigoCiudad);
        printf("  Codigo Region: %d\n", farmacia->codigoRegion);
        printf("  Capacidad de Almacenamiento: %d\n", farmacia->capacidadAlmacenamiento);
        printf("\n");
        rec = rec->sig;
    }
}


void mostrarProveedores(struct NodoProveedores *headProveedores) {
    struct NodoProveedores *rec = NULL;
    struct Proveedor *proveedor = NULL;
    rec = headProveedores;

    while (rec != NULL) {
        proveedor = rec->proveedor;
        printf("Proveedor:\n");
        printf("  ID: %d\n", proveedor->idProveedor);
        printf("  Nombre: %s\n", proveedor->nombreProveedor);
        printf("  Email: %s\n", proveedor->email);
        printf("  Numero Telefonico: %d\n", proveedor->numeroTelefonico);
        printf("\n");
        rec = rec->sig;
    }
}


void mostrarLotes(struct NodoLotes *arbol, struct Farmacia *farmaciaSeleccionada)
{
    struct Lote *lote = NULL;

    if (arbol != NULL) {
        mostrarLotes(arbol->izq, farmaciaSeleccionada);

        lote = arbol->LoteProducto;

        printf("Lote:\n");
        printf("  Codigo: %d\n", lote->codigoLote);
        printf("  Nombre: %s\n", lote->nombreLote);
        printf("  Fecha de Caducidad: %d-%d-%d\n", lote->diaCaducidadLote, lote->mesCaducidadLote, lote->anioCaducidadLote);
        printf("  Precio: %d\n", lote->precioLote);
        printf("  Cantidad de Producto del Lote: %d\n", lote->cantidadProductoDelLote);
        printf("\n");
        mostrarLotes(arbol->der, farmaciaSeleccionada);
    }
}


void mostrarVentas(struct NodoVenta *headVentas, struct Farmacia *farmaciaSeleccionada) {
    int encontrado = 0;
    struct NodoVenta *rec = NULL;
    struct Venta *venta = NULL;

    if (farmaciaSeleccionada == NULL || headVentas == NULL) {
        return;
    }
    rec = headVentas;

    encontrado = 0;

    while (rec != NULL) {
        venta = rec->ventas;
        if (venta->idFarmacia == farmaciaSeleccionada->idFarmacia) {
            encontrado = 1;
            printf("Venta:\n");
            printf("  Nombre Producto Vendido: %s\n", venta->nombreProductoVendido);
            printf("  Cantidad Vendida: %d\n", venta->cantidadVendida);
            printf("  Numero Boleta: %d\n", venta->numeroBoleta);
            printf("  Monto Pagado: %d\n", venta->montoPagado);
            printf("  RUT Cliente: %s\n", venta->rutCliente);
            printf("  Cliente Trajo Receta: %s\n", venta->clienteTrajoReceta ? "Si" : "No");
            printf("  Fecha de compra %d-%d-%d\n", venta->diaVenta, venta->mesVenta, venta->anioVenta);
            printf("\n");
        }
        rec = rec->sig;
    }
    if (encontrado == 0) {
        printf("No se encontraron ventas para la farmacia seleccionada.\n");
    }
}


void mostrarProductosBajoUmbral(struct arregloProductos *arreglo) {
    int encontrado = 0, i;
    struct Producto *producto = NULL;

    if (arreglo == NULL) {
        return;
    }
    for (i = 0; i < arreglo->pLibre; i++) {
        producto = arreglo->arrProductos[i];

        if (producto->stock <= producto->umbral / 2) {
            encontrado = 1;
            printf("Producto bajo umbral:\n");
            printf("  Codigo: %d\n", producto->codigoProducto);
            printf("  Nombre: %s\n", producto->nombreProducto);
            printf("  Descripcion: %s\n", producto->descripcionProducto);
            printf("  Categoria: %s\n", producto->categoria);
            printf("  Fecha de caducidad: %d-%d-%d\n", producto->diaCaducidad, producto->mesCaducidad, producto->anioCaducidad);
            printf("  Precio: %d\n", producto->precioProducto);
            printf("  Stock: %d\n", producto->stock);
            printf("  Umbral: %d\n", producto->umbral);
            printf("\n");
        }
    }

    if (encontrado == 0) {
        printf("No hay productos bajo el umbral.\n");
    }
}


void mostrarOrdenesCompra(struct NodoOrdenCompra *headOrdenes, struct Farmacia *farmaciaSeleccionada)
{
    int encontrado = 0;
    struct NodoOrdenCompra *rec = NULL;
    struct OrdenCompra *orden = NULL;
    struct ProductoOrden *productoOrden = NULL;

    if (farmaciaSeleccionada == NULL || headOrdenes == NULL)
    {
        printf("No hay una farmacia seleccionada o no hay ordenes seleccionada.\n");
        return;
    }

    rec = headOrdenes;

    while (rec != NULL)
    {
        orden = rec->compraProveedor;

        if (orden->idFarmacia == farmaciaSeleccionada->idFarmacia)
        {
            encontrado = 1;
            printf("Orden de Compra:\n");
            printf("  ID: %d\n", orden->idOrden);
            printf("  Monto Comprado: %d\n", orden->montoComprado);
            printf("  Fecha de Llegada: %d-%d-%d\n", orden->dia, orden->mes, orden->anio);  // Mostrar los nuevos campos de fecha
            printf("  Productos:\n");
            productoOrden = orden->productos;
            while (productoOrden != NULL)
            {
                printf("    Codigo Producto: %d, Cantidad: %d\n", productoOrden->codigoProducto, productoOrden->cantidad);
                productoOrden = productoOrden->sig;
            }
            printf("\n");
        }
        rec = rec->sig;
    }
    if (encontrado == 0)
        printf("No se encontraron ordenes de compra para la farmacia seleccionada.\n");
}


void mostrarProductosVendidosConReceta(struct NodoVenta *headVentas, struct Farmacia *farmaciaSeleccionada)
{
    struct NodoVenta *rec = NULL;
    struct  Venta *venta = NULL;
    int encontrado = 0;

    if(headVentas == NULL || farmaciaSeleccionada == NULL){
        printf("No se encuentras ventas seleccionadas o farmacias seleccionas. \n");
        return;
    }

    rec = headVentas;
    while (rec != NULL) {
        venta = rec->ventas;
        if (venta->clienteTrajoReceta && venta->idFarmacia == farmaciaSeleccionada->idFarmacia) {
            encontrado = 1;
            printf("Venta con receta:\n");
            printf("  Nombre Producto Vendido: %s\n", venta->nombreProductoVendido);
            printf("  Cantidad Vendida: %d\n", venta->cantidadVendida);
            printf("  Numero Boleta: %d\n", venta->numeroBoleta);
            printf("  Monto Pagado: %d\n", venta->montoPagado);
            printf("  RUT Cliente: %s\n", venta->rutCliente);
            printf("  Fecha de compra: %d-%d-%d\n", venta->diaVenta, venta->mesVenta, venta->anioVenta);
            printf("\n");
            encontrado = 1;
        }
        rec = rec->sig;
    }
    if (encontrado == 0) {
        printf("No se encontraron productos vendidos con receta para la farmacia seleccionada.\n");
    }
}


int mostrarProductosCaducanEnFecha(struct arregloProductos *arreglo, int dia, int mes, int anio)
{
    struct Producto *producto = NULL;
    int i;

    for (i = 0; i < arreglo->pLibre; i++) {
        producto = arreglo->arrProductos[i];
        if (producto->diaCaducidad == dia && producto->mesCaducidad == mes && producto->anioCaducidad == anio) {
            printf("Producto:\n");
            printf("  Codigo: %d\n", producto->codigoProducto);
            printf("  Nombre: %s\n", producto->nombreProducto);
            printf("  Descripcion: %s\n", producto->descripcionProducto);
            printf("  Categoria: %s\n", producto->categoria);
            printf("  Fecha de caducidad: %d-%d-%d\n", producto->diaCaducidad, producto->mesCaducidad, producto->anioCaducidad);
            printf("  Precio: %d\n", producto->precioProducto);
            printf("  Requiere receta: %s\n", producto->requiereReceta ? "Si" : "No");
            printf("  Stock: %d\n", producto->stock);
            printf("  Umbral: %d\n", producto->umbral);
            printf("\n");
            return 1;
        }
    }
    return 0;
}

void informeTendenciasVentas(struct NodoVenta *headVentas, int mesInicio, int mesFin, struct Farmacia *farmaciaSeleccionada)
{
    int totalProductos = 0, indiceProducto = 0, encontrado = 0, i;
    struct NodoVenta *rec = NULL;
    struct ProductoOrden *productosVendidos[MAX];
    if(headVentas==NULL)
        return;

    rec = headVentas;

    while (rec != NULL) {

        if (rec->ventas->idFarmacia == farmaciaSeleccionada->idFarmacia) {
            indiceProducto = buscarProducto(farmaciaSeleccionada->inventario, rec->ventas->codigoProductoVendido);

            if (indiceProducto != -1 && rec->ventas->mesVenta >= mesInicio && rec->ventas->mesVenta <= mesFin) {
                encontrado = 0;
                for (i = 0; i < totalProductos; i++) {
                    if (productosVendidos[i]->codigoProducto == rec->ventas->codigoProductoVendido) {
                        productosVendidos[i]->cantidad += rec->ventas->cantidadVendida;
                        encontrado = 1;
                    }
                }
                if (encontrado == 0) {
                    productosVendidos[totalProductos] = (struct ProductoOrden *)malloc(sizeof(struct ProductoOrden));
                    productosVendidos[totalProductos]->codigoProducto = rec->ventas->codigoProductoVendido;
                    productosVendidos[totalProductos]->cantidad = rec->ventas->cantidadVendida;
                    productosVendidos[totalProductos]->sig = NULL;
                    totalProductos++;
                }
            }
        }
        rec = rec->sig;
    }

    printf("Informe de Tendencias de Ventas para la farmacia %s:\n", farmaciaSeleccionada->nombreFarmacia);

    for (i = 0; i < totalProductos; i++) {
        indiceProducto = buscarProducto(farmaciaSeleccionada->inventario, productosVendidos[i]->codigoProducto);
        if (indiceProducto != -1) {
            printf("Producto: %s (Codigo: %d) - Cantidad Vendida: %d\n", farmaciaSeleccionada->inventario->arrProductos[indiceProducto]->nombreProducto, productosVendidos[i]->codigoProducto, productosVendidos[i]->cantidad);
        }
    }
}


void imprimirDetallesVenta(struct Venta *venta)
{
    printf("Venta encontrada:\n");
    printf("  Codigo Producto: %d\n", venta->codigoProductoVendido);
    printf("  Cantidad Vendida: %d\n", venta->cantidadVendida);
    printf("  Numero Boleta: %d\n", venta->numeroBoleta);
    printf("  Monto Pagado: %d\n", venta->montoPagado);
    printf("  Nombre Producto: %s\n", venta->nombreProductoVendido);
    printf("  RUT Cliente: %s\n", venta->rutCliente);
    printf("  Fecha Venta: %d-%d-%d\n\n", venta->diaVenta, venta->mesVenta, venta->anioVenta);
}


void patronesEstacionales(struct NodoFarmacia *farmacias, int mes1, int mes2, int anio, int idFarmacia)
{
    struct NodoFarmacia *recFarmacias = NULL;
    struct NodoVenta *recVentas = NULL;
    struct Venta *venta = NULL;
    int ventasEncontradas = 0;

    if(farmacias == NULL)
        return;

    recFarmacias = farmacias;
    while (recFarmacias != NULL) {
        recVentas = recFarmacias->farmacia->ventas;
        while (recVentas != NULL) {
            venta = recVentas->ventas;

            if (venta->idFarmacia == idFarmacia) {
                if (venta->mesVenta >= mes1 && venta->mesVenta <= mes2 && venta->anioVenta == anio) {
                    ventasEncontradas = 1;
                    imprimirDetallesVenta(venta);
                }
            }
            recVentas = recVentas->sig;
        }
        recFarmacias = recFarmacias->sig;
    }
    if (ventasEncontradas == 0) {
        printf("No se encontraron ventas en el intervalo dado para la farmacia con id %d.\n", idFarmacia);
    }
}


void productosAltaBajaRotacion(struct arregloProductos *arreglo) {

    int altaRotacion = 0, bajaRotacion = 0, i;
    struct Producto *productoAltaRotacion = NULL;
    struct Producto *productoBajaRotacion = NULL;

    printf("Productos de Alta Rotacion (stock por debajo del umbral):\n");
    for (i = 0; i < arreglo->pLibre; i++) {
        productoAltaRotacion = arreglo->arrProductos[i];
        if (productoAltaRotacion->stock <= productoAltaRotacion->umbral / 2) {
            printf("Producto: %s (Codigo: %d) - Stock: %d\n", productoAltaRotacion->nombreProducto, productoAltaRotacion->codigoProducto, productoAltaRotacion->stock);
            altaRotacion++;
        }
    }
    if (altaRotacion == 0) {
        printf("No hay productos de alta rotacion.\n");
    }

    printf("\nProductos de Baja Rotacion (stock muy por encima del umbral):\n");
    for (i = 0; i < arreglo->pLibre; i++) {
        productoBajaRotacion = arreglo->arrProductos[i];
        if (productoBajaRotacion->stock > 2 * productoBajaRotacion->umbral) {
            printf("Producto: %s (Codigo: %d) - Stock: %d\n", productoBajaRotacion->nombreProducto, productoBajaRotacion->codigoProducto, productoBajaRotacion->stock);
            bajaRotacion++;
        }
    }
    if (bajaRotacion == 0) {
        printf("No hay productos de baja rotacion.\n");
    }
}


//8. Funciones modificar.-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void modificarProducto(struct arregloProductos *arregloDeProductos, int codigoProductoModificar, struct Producto *productoModificado)
{
    int i;
    struct Producto *productoModificar = NULL;

    for (i = 0; i < arregloDeProductos->pLibre; i++) {
        if (arregloDeProductos->arrProductos[i]->codigoProducto == codigoProductoModificar) {
            productoModificar = arregloDeProductos->arrProductos[i];

            productoModificar->nombreProducto = strdup(productoModificado->nombreProducto);
            productoModificar->descripcionProducto = strdup(productoModificado->descripcionProducto);
            productoModificar->categoria = strdup(productoModificado->categoria);
            productoModificar->nombreLote = strdup(productoModificado->nombreLote);

            productoModificar->precioProducto = productoModificado->precioProducto;
            productoModificar->numeroEntrega = productoModificado->numeroEntrega;
            productoModificar->requiereReceta = productoModificado->requiereReceta;
            productoModificar->stock = productoModificado->stock;
            productoModificar->umbral = productoModificado->umbral;
            productoModificar->diaCaducidad = productoModificado->diaCaducidad;
            productoModificar->mesCaducidad = productoModificado->mesCaducidad;
            productoModificar->anioCaducidad = productoModificado->anioCaducidad;
            productoModificar->proveedor = productoModificado->proveedor;
            return;
        }
    }
    printf("Producto con codigo %d no encontrado.\n", codigoProductoModificar);
}


void modificarFarmacia(struct NodoFarmacia *farmacias, int idFarmacia, struct Farmacia *farmaciaModificada) {

    struct NodoFarmacia *rec = NULL;
    struct Farmacia *farmaciaModificar = NULL;
    if(farmacias == NULL)
        return;

    rec = farmacias;

    while (rec != NULL) {
        if (rec->farmacia->idFarmacia == idFarmacia) {
            farmaciaModificar = rec->farmacia;
            farmaciaModificar->codigoCiudad = farmaciaModificada->codigoCiudad;
            farmaciaModificar->codigoRegion = farmaciaModificada->codigoRegion;
            farmaciaModificar->capacidadAlmacenamiento = farmaciaModificada->capacidadAlmacenamiento;
            farmaciaModificar->nombreFarmacia = strdup(farmaciaModificada->nombreFarmacia);
            farmaciaModificar->inventario = farmaciaModificada->inventario;
            farmaciaModificar->comprasProveedor = farmaciaModificada->comprasProveedor;
            farmaciaModificar->ventas = farmaciaModificada->ventas;
            return;
        }
        rec = rec->sig;
    }
    printf("Farmacia con ID %d no encontrada.\n", idFarmacia);
}


void modificarLote(struct NodoLotes *arbol, int codigoLote, struct Lote *loteModificado)
{
    struct Lote *loteModificar = NULL;
    if (arbol == NULL) {
        printf("Lote con codigo %d no encontrado.\n", codigoLote);
        return;
    }

    if (arbol->LoteProducto->codigoLote == codigoLote) {
        loteModificar = arbol->LoteProducto;
        loteModificar->productoDentroLote = loteModificado->productoDentroLote;
        loteModificar->precioLote = loteModificado->precioLote;
        loteModificar->cantidadProductoDelLote = loteModificado->cantidadProductoDelLote;
        loteModificar->nombreLote = strdup(loteModificado->nombreLote);
        loteModificar->diaCaducidadLote = loteModificado->diaCaducidadLote;
        loteModificar->mesCaducidadLote = loteModificado->mesCaducidadLote;
        loteModificar->anioCaducidadLote = loteModificado->anioCaducidadLote;
    } else if (codigoLote < arbol->LoteProducto->codigoLote) {
        modificarLote(arbol->izq, codigoLote, loteModificado);
    } else {
        modificarLote(arbol->der, codigoLote, loteModificado);
    }
}


int modificarProveedor(struct NodoProveedores *headProveedores, int idProveedor, struct Proveedor *proveedorModificado) {
    struct NodoProveedores *rec = NULL;
    struct Proveedor *proveedorModificar = NULL;

    if(headProveedores == NULL)
        return 0;

    rec = headProveedores;

    while (rec != NULL) {
        if (rec->proveedor->idProveedor == idProveedor) {
            proveedorModificar = rec->proveedor;
            proveedorModificar->numeroTelefonico = proveedorModificado->numeroTelefonico;

            proveedorModificar->nombreProveedor = strdup(proveedorModificado->nombreProveedor);
            proveedorModificar->email = strdup(proveedorModificado->email);
            proveedorModificar->lotes = proveedorModificado->lotes;

            return 1;
        }
        rec = rec->sig;
    }
    return 0;
}


int modificarVenta(struct NodoVenta *headVentas, int numeroBoleta, struct Venta *ventaModificada) {
    struct NodoVenta *rec = NULL;

    if(headVentas == NULL)
        return 0;

    rec = headVentas;

    while (rec != NULL)
    {
        if (rec->ventas->numeroBoleta == numeroBoleta)
        {
            rec->ventas->codigoProductoVendido = ventaModificada->codigoProductoVendido;
            rec->ventas->cantidadVendida = ventaModificada->cantidadVendida;
            rec->ventas->numeroBoleta = ventaModificada->numeroBoleta;
            rec->ventas->montoPagado = ventaModificada->montoPagado;
            rec->ventas->clienteTrajoReceta = ventaModificada->clienteTrajoReceta;
            rec->ventas->nombreProductoVendido = strdup(ventaModificada->nombreProductoVendido);
            rec->ventas->rutCliente = strdup(ventaModificada->rutCliente);
            rec->ventas->diaVenta = ventaModificada->diaVenta;
            rec->ventas->mesVenta = ventaModificada->mesVenta;
            rec->ventas->anioVenta = ventaModificada->anioVenta;
            return 1;
        }
        rec = rec->sig;
    }
    return 0;
}


int modificarOrdenCompra(struct NodoOrdenCompra *headOrdenes, int idOrden, struct OrdenCompra *ordenModificada) { //revisar
    struct NodoOrdenCompra *recOrdenCompra = NULL;
    struct OrdenCompra *ordenModificar = NULL ;
    struct ProductoOrden *productoActual = NULL;
    struct ProductoOrden *ultimoProducto = NULL;
    struct ProductoOrden *nuevoProducto = NULL;

    recOrdenCompra = headOrdenes;

    if(headOrdenes == NULL)
        return 0;

    while (recOrdenCompra != NULL) {
        if (recOrdenCompra->compraProveedor->idOrden == idOrden) {
            ordenModificar = recOrdenCompra->compraProveedor;

            ordenModificar->montoComprado = ordenModificada->montoComprado;
            ordenModificar->dia = ordenModificada->dia;
            ordenModificar->mes = ordenModificada->mes;
            ordenModificar->anio = ordenModificada->anio;
            ordenModificar->productos = NULL;

            productoActual = ordenModificada->productos;
            ultimoProducto = NULL;

            while (productoActual != NULL) {
                nuevoProducto = (struct ProductoOrden *)malloc(sizeof(struct ProductoOrden));
                nuevoProducto->codigoProducto = productoActual->codigoProducto;
                nuevoProducto->cantidad = productoActual->cantidad;
                nuevoProducto->sig = NULL;

                if (ultimoProducto == NULL) {
                    ordenModificar->productos = nuevoProducto;
                } else {
                    ultimoProducto->sig = nuevoProducto;
                }

                ultimoProducto = nuevoProducto;
                productoActual = productoActual->sig;
            }

            return 1;
        }
        recOrdenCompra = recOrdenCompra->sig;
    }
    return 0;
}

//9. Menú de opciones.--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void mostrarMenu()
{
    printf("Menu de Opciones:\n");

    printf("\nFunciones de Producto:\n");
    printf("1. Agregar Producto\n");
    printf("2. Buscar Producto\n");
    printf("3. Eliminar Producto\n");
    printf("4. Listar Productos\n");
    printf("5. Mostrar Productos Bajo Umbral\n");
    printf("6. Contar Productos Diferentes\n");
    printf("7. Obtener Cantidad de Producto\n");
    printf("8. Sumar Cantidad de Productos Totales\n");
    printf("9. Eliminar Productos Caducados\n");
    printf("10. Mostrar Productos que Caducan en Fecha\n");
    printf("11. Modificar Producto\n");

    printf("\nFunciones de Farmacia:\n");
    printf("12. Crear Nueva Farmacia\n");
    printf("13. Buscar Farmacia por ID\n");
    printf("14. Eliminar Farmacia\n");
    printf("15. Mostrar Farmacias\n");
    printf("16. Modificar Farmacia\n");

    printf("\nFunciones de Lote:\n");
    printf("17. Crear Nuevo Lote\n");
    printf("18. Buscar Lote por Codigo\n");
    printf("19. Eliminar Lote\n");
    printf("20. Mostrar Lotes\n");
    printf("21. Modificar Lote\n");

    printf("\nFunciones de Proveedor:\n");
    printf("22. Crear Nuevo Proveedor\n");
    printf("23. Buscar Proveedor por Nombre\n");
    printf("24. Eliminar Proveedor\n");
    printf("25. Mostrar Proveedores\n");
    printf("26. Modificar Proveedor\n");

    printf("\nFunciones de Venta:\n");
    printf("27. Crear Nueva Venta\n");
    printf("28. Buscar Venta por Codigo de Producto Vendido\n");
    printf("29. Buscar Venta por RUT del Cliente\n");
    printf("30. Buscar Venta por Numero de Boleta\n");
    printf("31. Buscar Producto Mas Vendido\n");
    printf("32. Eliminar Venta por Numero de Boleta\n");
    printf("33. Mostrar Ventas\n");
    printf("34. Modificar Venta\n");
    printf("35. Mostrar productos que se vendieron y requerian receta\n");

    printf("\nFunciones de Orden de Compra:\n");
    printf("36. Crear Nueva Orden de Compra\n");
    printf("37. Mostrar Ordenes de Compra\n");
    printf("38. Modificar Orden de Compra\n");
    printf("39. Seguimiento de envios\n");

    printf("\nOtras Funciones:\n");
    printf("40. Verificar Reabastecimiento\n");
    printf("41. Informe de Tendencias de Ventas\n");
    printf("42. Patrones Estacionales\n");
    printf("43. Productos de Alta o Baja Rotacion\n");
    printf("44. Seleccione farmacia a trabajar\n");

    printf("0. Salir\n");
    printf("Seleccione una opcion: ");
}


//10. Programa principal.---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
    struct NodoProveedores *headProveedores = NULL;
    struct NodoLotes *arbolLotes = NULL;
    struct NodoOrdenCompra *headOrdenes = NULL;
    struct NodoFarmacia *headFarmacias = NULL;
    struct NodoVenta *headVentas = NULL;

    int idFarmacia;
    struct Farmacia *farmaciaSeleccionada = NULL;
    struct FarmaSalud *farmaSalud;

    struct Farmacia *farmacia1;

    struct arregloProductos *arregloFarmaciaUno;
    struct Producto *producto1;
    struct Producto *producto2;
    struct Producto *producto3;
    struct Producto *producto4;
    struct Producto *producto5;

    struct Venta *venta1;
    struct Venta *venta2;
    struct Venta *venta3;
    // Crear proveedores
    struct Proveedor *proveedor1;
    struct Proveedor *proveedor2;
    int opcion;

    farmaSalud = (struct FarmaSalud * )malloc(sizeof(struct FarmaSalud));
    farmaSalud->farmacias = NULL;
    farmaSalud->proveedores = NULL;

    arregloFarmaciaUno = crearArregloProductos();
    proveedor1 = crearProveedor(1, "Proveedor Uno", 123456789, "proveedor1@mail.com");
    proveedor2 = crearProveedor(2, "Proveedor Dos", 987654321, "proveedor2@mail.com");

    insertarProveedor(&headProveedores, proveedor1);
    insertarProveedor(&headProveedores, proveedor2);
    agregarProveedor(farmaSalud, proveedor1);
    agregarProveedor(farmaSalud, proveedor2);
    // Crear arreglo de productos específico para "Farmacia Uno"



    // Crear productos y asignarlos al arreglo de "Farmacia Uno"
    producto1 = crearProducto(1001, 50, "Ibuprofeno", "Analgesico y antipiretico", "Medicamento", 30, 12, 2024, proveedor1, "Lote1", NULL, 1, 500, 250);
    producto2 = crearProducto(1002, 30, "Paracetamol", "Analgesico y antipiretico", "Medicamento", 31, 12, 2024, proveedor1, "Lote2", NULL, 1, 1000, 500);
    producto3 = crearProducto(1003, 70, "Amoxicilina", "Antibiotico", "Medicamento", 31, 12, 2024, proveedor1, "Lote3", NULL, 1, 800, 400);
    producto4 = crearProducto(1004, 90, "Omeprazol", "Inhibidor de la bomba de protones", "Medicamento", 31, 12, 2027, proveedor2, "Lote4", NULL, 0, 300, 150);
    producto5 = crearProducto(1005, 150, "Lorazepam", "Ansiolitico", "Medicamento", 31, 12, 2024, proveedor2, "Lote5", NULL, 1, 99, 100);

    agregarProductoAlArreglo(arregloFarmaciaUno, producto1);
    agregarProductoAlArreglo(arregloFarmaciaUno, producto2);
    agregarProductoAlArreglo(arregloFarmaciaUno, producto3);
    agregarProductoAlArreglo(arregloFarmaciaUno, producto4);
    agregarProductoAlArreglo(arregloFarmaciaUno, producto5);

    // Crear "Farmacia Uno" y asignar el arreglo específico
    farmacia1 = crearFarmacia(1, 101, 202, "Farmacia Uno", 500, headProveedores, arregloFarmaciaUno);

    // Insertar farmacia en la lista de farmacias
    insertarFarmacia(&headFarmacias, farmacia1);
    agregarFarmacia(farmaSalud, farmacia1);

    // Crear ventas

    venta1 = crearVenta(1, 1001, "Ibuprofeno", 10, 1, 500, "12345678-9", 1, 10, 6, 2026);
    venta2 = crearVenta(1, 1002, "Paracetamol", 5, 2, 150, "98765432-1", 0, 7, 8, 2024);
    venta3 = crearVenta(1,1003,"Amoxicilina",10,3,500,"13123613-2",1,10,12,2024);


    insertarVenta(&headVentas, venta1, arregloFarmaciaUno);
    insertarVenta(&headVentas, venta2, arregloFarmaciaUno);
    insertarVenta(&headVentas, venta3, arregloFarmaciaUno);

    insertarVentaEnFarmacia(headFarmacias,venta1);
    insertarVentaEnFarmacia(headFarmacias,venta2);
    insertarVentaEnFarmacia(headFarmacias,venta3);

    // Seleccionar farmacia
    while (farmaciaSeleccionada == NULL) {
        printf("Ingrese id de farmacia a trabajar:\n");
        scanf("%d", &idFarmacia);
        farmaciaSeleccionada = seleccionarFarmacia(headFarmacias, idFarmacia);
    }


    do {
        if (farmaciaSeleccionada != NULL) {
            do
            {
                mostrarMenu();
                scanf("%d", &opcion);
                switch (opcion)
                {
                    case 1:
                    {
                        int codigo, precio, requiereReceta, stock, umbral, dia, mes, anio, idProveedor;
                        char nombre[100], descripcion[200], categoria[100], nombreLote[50];
                        struct Proveedor *proveedor = NULL;
                        struct Producto *nuevoProducto = NULL;
                        printf("Ingrese codigo de producto: ");
                        scanf("%d", &codigo);
                        printf("Ingrese precio de producto: ");
                        scanf("%d", &precio);
                        printf("Ingrese nombre de producto: ");
                        scanf("%s", nombre);
                        printf("Ingrese descripcion de producto: ");
                        scanf("%s", descripcion);
                        printf("Ingrese categoria de producto: ");
                        scanf("%s", categoria);
                        printf("Ingrese dia de caducidad: ");
                        scanf("%d", &dia);
                        printf("Ingrese mes de caducidad: ");
                        scanf("%d", &mes);
                        printf("Ingrese anio de caducidad: ");
                        scanf("%d", &anio);
                        printf("Ingrese nombre de lote: ");
                        scanf("%s", nombreLote);
                        printf("Requiere receta (1: Si, 0: No): ");
                        scanf("%d", &requiereReceta);
                        printf("Ingrese stock de producto: ");
                        scanf("%d", &stock);
                        printf("Ingrese umbral de producto: ");
                        scanf("%d", &umbral);

                        printf("Ingrese ID del proveedor: ");
                        scanf("%d", &idProveedor);
                        proveedor = buscarProveedorPorId(headProveedores, idProveedor);
                        if (proveedor == NULL) {
                            printf("Proveedor con ID %d no encontrado.\n", idProveedor);
                            break;
                        }
                        nuevoProducto = crearProducto(codigo, precio, nombre, descripcion, categoria, dia, mes, anio, proveedor, nombreLote, NULL, requiereReceta, stock, umbral);
                        agregarProductoAlArreglo(farmaciaSeleccionada->inventario, nuevoProducto);
                        printf("Producto agregado a la farmacia %s exitosamente.\n", farmaciaSeleccionada->nombreFarmacia);
                        break;
                    }
                    case 2:
                    {
                        int codigoProducto;
                        int indice;
                        printf("Ingrese codigo de producto a buscar: ");
                        scanf("%d", &codigoProducto);
                        indice = buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                        if (indice != -1) {
                            printf("Producto encontrado en el indice %d.\n", indice);
                        } else {
                            printf("Producto no encontrado.\n");
                        }
                        break;
                    }
                    case 3:
                    {
                        int codigoProducto;
                        int indice;
                        printf("Ingrese codigo de producto a eliminar: ");
                        scanf("%d", &codigoProducto);
                        indice = buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                        if (indice != -1) {
                            eliminarProducto(farmaciaSeleccionada->inventario, indice);
                            printf("Producto eliminado exitosamente.\n");
                        } else {
                            printf("Producto no encontrado.\n");
                        }
                        break;
                    }
                    case 4:
                    {
                        if (farmaciaSeleccionada != NULL)
                        {
                            if (farmaciaSeleccionada->inventario != NULL && farmaciaSeleccionada->inventario->pLibre > 0)
                                mostrarProductos(farmaciaSeleccionada);
                            else
                                printf("No hay productos en la farmacia seleccionada.\n");
                        }
                        else
                            printf("No hay una farmacia seleccionada o la farmacia seleccionada ha sido eliminada.\n");
                        break;
                    }
                    case 5:
                    {
                        mostrarProductosBajoUmbral(farmaciaSeleccionada->inventario);
                        break;
                    }
                    case 6:
                    {
                        int totalDiferentes;
                        totalDiferentes = contarProductosDiferentes(farmaciaSeleccionada->inventario);
                        printf("Total productos diferentes: %d\n", totalDiferentes);
                        break;
                    }
                    case 7:
                    {
                        int codigoProducto;
                        int cantidad;
                        printf("Ingrese codigo de producto para contar stock: ");
                        scanf("%d", &codigoProducto);
                        cantidad = obtenerCantidadDeProducto(farmaciaSeleccionada->inventario, codigoProducto);
                        printf("Cantidad total de producto %d: %d\n", codigoProducto, cantidad);
                        break;
                    }
                    case 8:
                    {
                        int totalProductos;
                        totalProductos = sumaCantidadDeProductosTotales(farmaciaSeleccionada->inventario);
                        printf("Cantidad total de todos los productos: %d\n", totalProductos);
                        break;
                    }
                    case 9:
                    {
                        int dia, mes, anio;
                        printf("Ingrese dia actual: ");
                        scanf("%d", &dia);
                        printf("Ingrese mes actual: ");
                        scanf("%d", &mes);
                        printf("Ingrese anio actual: ");
                        scanf("%d", &anio);
                        eliminarProductosCaducados(farmaciaSeleccionada->inventario, dia, mes, anio);
                        printf("Productos caducados hasta la fecha %d-%d-%d han sido eliminados.\n", dia, mes, anio);
                        break;
                    }

                    case 10:
                    {
                        int dia, mes, anio;
                        printf("Ingrese el dia de caducidad: ");
                        scanf("%d", &dia);
                        printf("Ingrese el mes de caducidad: ");
                        scanf("%d", &mes);
                        printf("Ingrese el anio de caducidad: ");
                        scanf("%d", &anio);
                        mostrarProductosCaducanEnFecha(farmaciaSeleccionada->inventario, dia, mes, anio);
                        break;
                    }
                    case 11:
                    {
                        int codigoProducto;
                        int indiceProducto;
                        struct Producto *productoModificado = NULL;
                        struct Producto *productoOriginal = NULL;
                        printf("Ingrese codigo del producto a modificar: ");
                        scanf("%d", &codigoProducto);



                        printf("Ingrese nuevo precio del producto: ");
                        scanf("%d", &productoModificado->precioProducto);
                        printf("Ingrese nueva cantidad de stock del producto: ");
                        scanf("%d", &productoModificado->stock);
                        printf("Ingrese nuevo umbral del producto: ");
                        scanf("%d", &productoModificado->umbral);

                        productoModificado->nombreProducto = (char *)malloc(100 * sizeof(char));
                        productoModificado->descripcionProducto = (char *)malloc(200 * sizeof(char));
                        productoModificado->categoria = (char *)malloc(100 * sizeof(char));
                        productoModificado->nombreLote = (char *)malloc(50 * sizeof(char));

                        printf("Ingrese nuevo nombre del producto: ");
                        scanf("%99s", productoModificado->nombreProducto);

                        printf("Ingrese nueva descripcion del producto: ");
                        scanf("%199s", productoModificado->descripcionProducto);

                        printf("Ingrese nueva categoria del producto: ");
                        scanf("%99s", productoModificado->categoria);

                        printf("Ingrese nuevo dia de caducidad del producto: ");
                        scanf("%d", &productoModificado->diaCaducidad);
                        printf("Ingrese nuevo mes de caducidad del producto: ");
                        scanf("%d", &productoModificado->mesCaducidad);
                        printf("Ingrese nuevo anio de caducidad del producto: ");
                        scanf("%d", &productoModificado->anioCaducidad);

                        printf("Ingrese nuevo nombre del lote del producto: ");
                        scanf("%49s", productoModificado->nombreLote);

                        printf("Ingrese si el producto requiere receta (1: Si, 0: No): ");
                        scanf("%d", &productoModificado->requiereReceta);

                        indiceProducto = buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                        if (indiceProducto != -1)
                        {
                            productoOriginal = farmaciaSeleccionada->inventario->arrProductos[indiceProducto];
                            productoModificado->proveedor = productoOriginal->proveedor;  // Mantener la referencia al proveedor
                            productoModificado->numeroEntrega = productoOriginal->numeroEntrega;

                            modificarProducto(farmaciaSeleccionada->inventario, codigoProducto, productoModificado);
                            printf("Producto con codigo %d modificado exitosamente.\n", codigoProducto);
                        }
                        else
                            printf("Producto con codigo %d no encontrado.\n");
                        break;
                    }
                    case 12:
                    {
                        int idFarmacia, codigoCiudad, codigoRegion, capacidadAlmacenamiento;
                        char nombreFarmacia[100];
                        struct arregloProductos *arregloNuevaFarmacia = NULL;
                        struct Farmacia *nuevaFarmacia = NULL;

                        printf("Ingrese ID de la farmacia: ");
                        scanf("%d", &idFarmacia);
                        printf("Ingrese codigo de ciudad: ");
                        scanf("%d", &codigoCiudad);
                        printf("Ingrese codigo de region: ");
                        scanf("%d", &codigoRegion);
                        printf("Ingrese nombre de la farmacia: ");
                        scanf("%s", nombreFarmacia);
                        printf("Ingrese capacidad de almacenamiento: ");
                        scanf("%d", &capacidadAlmacenamiento);

                        // Crear un arreglo específico para la nueva farmacia
                        arregloNuevaFarmacia = crearArregloProductos();

                        nuevaFarmacia = crearFarmacia(idFarmacia, codigoCiudad, codigoRegion, nombreFarmacia, capacidadAlmacenamiento, headProveedores, arregloNuevaFarmacia);

                        insertarFarmacia(&headFarmacias, nuevaFarmacia);
                        printf("Nueva farmacia creada exitosamente.\n");

                        farmaciaSeleccionada = nuevaFarmacia;
                        printf("Farmacia seleccionada: %s\n", farmaciaSeleccionada->nombreFarmacia);
                        break;
                    }
                    case 13:
                    {
                        int idFarmacia;
                        struct Farmacia *farmacia = NULL;
                        printf("Ingrese ID de la farmacia a buscar: ");
                        scanf("%d", &idFarmacia);
                        farmacia = buscarFarmacia(headFarmacias, idFarmacia);
                        if (farmacia != NULL)
                            printf("Farmacia encontrada: ID: %d, Nombre: %s, Codigo Ciudad: %d, Codigo Region: %d, Capacidad de Almacenamiento: %d\n", farmacia->idFarmacia, farmacia->nombreFarmacia, farmacia->codigoCiudad, farmacia->codigoRegion, farmacia->capacidadAlmacenamiento);
                        else
                            printf("Farmacia no encontrada.\n");
                        break;
                    }
                    case 14:
                    {
                        int idFarmacia, codigoCiudad, codigoRegion, capacidadAlmacenamiento;
                        char nombreFarmacia[100];
                        struct Farmacia *farmaciaSeleccionada;
                        struct arregloProductos *arregloNuevaFarmacia = NULL;
                        struct Farmacia *nuevaFarmacia = NULL;
                        printf("Ingrese ID de la farmacia a eliminar: ");
                        scanf("%d", &idFarmacia);
                        if (eliminarFarmacia(&headFarmacias, idFarmacia))
                        {
                            printf("Farmacia eliminada exitosamente.\n");
                            if (farmaciaSeleccionada != NULL && farmaciaSeleccionada->idFarmacia == idFarmacia)
                            {
                                farmaciaSeleccionada = NULL;
                                if (headFarmacias != NULL)
                                {
                                    printf("Seleccione una nueva farmacia.\n");
                                    do {
                                        printf("Ingrese id de farmacia a trabajar:\n");
                                        scanf("%d", &idFarmacia);
                                        farmaciaSeleccionada = seleccionarFarmacia(headFarmacias, idFarmacia);
                                    }while (farmaciaSeleccionada == NULL);

                                }
                                else
                                {
                                    printf("No hay farmacias disponibles. Creando una nueva farmacia...\n");
                                    printf("Ingrese ID de la farmacia: ");
                                    scanf("%d", &idFarmacia);
                                    printf("Ingrese codigo de ciudad: ");
                                    scanf("%d", &codigoCiudad);
                                    printf("Ingrese codigo de region: ");
                                    scanf("%d", &codigoRegion);
                                    printf("Ingrese nombre de la farmacia: ");
                                    scanf("%s", nombreFarmacia);
                                    printf("Ingrese capacidad de almacenamiento: ");
                                    scanf("%d", &capacidadAlmacenamiento);

                                    arregloNuevaFarmacia = crearArregloProductos();

                                    nuevaFarmacia = crearFarmacia(idFarmacia, codigoCiudad, codigoRegion, nombreFarmacia, capacidadAlmacenamiento, headProveedores, arregloNuevaFarmacia);

                                    insertarFarmacia(&headFarmacias, nuevaFarmacia);
                                    printf("Nueva farmacia creada exitosamente.\n");

                                    farmaciaSeleccionada = nuevaFarmacia;
                                    printf("Farmacia seleccionada: %s\n", farmaciaSeleccionada->nombreFarmacia);
                                }
                            }
                        }
                        else
                        {
                            printf("Farmacia con ID %d no encontrada.\n", idFarmacia);
                            break;
                        }
                    }
                    case 15:
                    {
                        mostrarFarmacias(headFarmacias);
                        break;
                    }
                    case 16:
                    {
                        int idFarmacia;
                        struct Farmacia *farmaciaEncontrada = NULL;
                        struct Farmacia farmaciaModificada;
                        char nombreTemp[100];
                        printf("Ingrese ID de la farmacia a modificar: ");
                        scanf("%d", &idFarmacia);

                        farmaciaEncontrada = buscarFarmacia(headFarmacias, idFarmacia);

                        if (farmaciaEncontrada == NULL)
                        {
                            printf("ID de farmacia %d no encontrada.\n", idFarmacia);
                            break;
                        }

                        memset(&farmaciaModificada, 0, sizeof(struct Farmacia));
                        printf("Ingrese nueva capacidad de almacenamiento: ");
                        scanf("%d", &farmaciaModificada.capacidadAlmacenamiento);

                        printf("Ingrese nuevo codigo de ciudad: ");
                        scanf("%d", &farmaciaModificada.codigoCiudad);

                        printf("Ingrese nuevo codigo de region: ");
                        scanf("%d", &farmaciaModificada.codigoRegion);

                        printf("Ingrese nuevo nombre de la farmacia: ");
                        scanf("%s", nombreTemp);
                        farmaciaModificada.nombreFarmacia = strdup(nombreTemp);

                        // Mantener la referencia a los proveedores
                        farmaciaModificada.proveedores = farmaciaEncontrada->proveedores;

                        modificarFarmacia(headFarmacias, idFarmacia, &farmaciaModificada);

                        printf("La modificacion de la farmacia ha sido exitosa.\n");
                        break;
                    }
                    case 17:
                    {
                        struct Producto *producto = NULL;
                        struct Lote *nuevoLote = NULL;
                        int codigoLote, precioLote, cantidadProductoLote, dia, mes, anio,indice;
                        char nombreLote[100];
                        int codigoProducto;
                        printf("Ingrese codigo del lote: ");
                        scanf("%d", &codigoLote);
                        printf("Ingrese codigo del producto: ");
                        scanf("%d", &codigoProducto);
                        indice = buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                        if (indice != -1) {
                            producto = farmaciaSeleccionada->inventario->arrProductos[indice];
                            printf("Ingrese precio del lote: ");
                            scanf("%d", &precioLote);
                            printf("Ingrese cantidad de producto en el lote: ");
                            scanf("%d", &cantidadProductoLote);
                            printf("Ingrese nombre del lote: ");
                            scanf("%s", nombreLote);
                            printf("Ingrese dia de caducidad: ");
                            scanf("%d", &dia);
                            printf("Ingrese mes de caducidad: ");
                            scanf("%d", &mes);
                            printf("Ingrese anio de caducidad: ");
                            scanf("%d", &anio);
                            nuevoLote = crearLote(codigoLote, producto, precioLote, cantidadProductoLote, nombreLote, dia, mes, anio);
                            insertarLote(&arbolLotes, nuevoLote);
                            printf("Nuevo lote creado exitosamente.\n");
                        } else
                            printf("Producto con codigo %d no encontrado.\n", codigoProducto);
                        break;
                    }
                    case 18:
                    {
                        int codigoLote;
                        struct Lote *lote = NULL;
                        printf("Ingrese codigo del lote a buscar: ");
                        scanf("%d", &codigoLote);

                        lote = buscarLote(arbolLotes, codigoLote);
                        if (lote != NULL) {
                            printf("Lote encontrado: Codigo: %d, Nombre: %s, Fecha de Caducidad: %d-%d-%d, Precio: %d, Cantidad de Producto del Lote: %d\n", lote->codigoLote, lote->nombreLote, lote->diaCaducidadLote, lote->mesCaducidadLote, lote->anioCaducidadLote, lote->precioLote, lote->cantidadProductoDelLote);
                        } else {
                            printf("Lote no encontrado.\n");
                        }
                        break;
                    }
                    case 19:
                    {
                        int codigoLote;
                        printf("Ingrese codigo del lote a eliminar: ");
                        scanf("%d", &codigoLote);
                        arbolLotes = eliminarLote(arbolLotes, codigoLote);
                        printf("Lote eliminado exitosamente.\n");
                        break;
                    }
                    case 20:
                    {
                        mostrarLotes(arbolLotes, farmaciaSeleccionada);
                        break;
                    }
                    case 21:
                    {
                        struct Lote *loteModificado = NULL;
                        int codigoLote;
                        int codigoProducto;
                        int indiceProducto;

                        printf("Ingrese codigo del lote a modificar: ");
                        scanf("%d", &codigoLote);

                        loteModificado = (struct Lote *)malloc(sizeof(struct Lote));

                        printf("Ingrese nuevo precio del lote: ");
                        scanf("%d", &loteModificado->precioLote);
                        printf("Ingrese nueva cantidad de producto en el lote: ");
                        scanf("%d", &loteModificado->cantidadProductoDelLote);

                        loteModificado->nombreLote = (char *)malloc(100 * sizeof(char));

                        printf("Ingrese nuevo nombre del lote: ");
                        scanf("%99s", loteModificado->nombreLote);

                        printf("Ingrese nuevo dia de caducidad del lote: ");
                        scanf("%d", &loteModificado->diaCaducidadLote);
                        printf("Ingrese nuevo mes de caducidad del lote: ");
                        scanf("%d", &loteModificado->mesCaducidadLote);
                        printf("Ingrese nuevo anio de caducidad del lote: ");
                        scanf("%d", &loteModificado->anioCaducidadLote);


                        printf("Ingrese codigo del producto en el lote: ");
                        scanf("%d", &codigoProducto);
                        indiceProducto= buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                        if (indiceProducto != -1) {
                            loteModificado->productoDentroLote = farmaciaSeleccionada->inventario->arrProductos[indiceProducto];
                        } else {
                            printf("Producto con codigo %d no encontrado.\n", codigoProducto);
                            break;
                        }

                        modificarLote(arbolLotes, codigoLote, loteModificado);
                        printf("Lote con codigo %d modificado exitosamente.\n", codigoLote);


                        break;
                    }
                    case 22:
                    {
                        struct Proveedor *nuevoProveedor = NULL;
                        int idProveedor, numeroTelefono;
                        char nombreProveedor[100], email[100];
                        printf("Ingrese ID del proveedor: ");
                        scanf("%d", &idProveedor);
                        printf("Ingrese nombre del proveedor: ");
                        scanf("%s", nombreProveedor);
                        printf("Ingrese numero telefonico: ");
                        scanf("%d", &numeroTelefono);
                        printf("Ingrese email del proveedor: ");
                        scanf("%s", email);
                        nuevoProveedor = crearProveedor(idProveedor, nombreProveedor, numeroTelefono, email);
                        insertarProveedor(&headProveedores, nuevoProveedor);
                        printf("Nuevo proveedor creado exitosamente.\n");
                        break;
                    }
                    case 23:
                    {
                        struct Proveedor *proveedor = NULL;
                        char nombreProveedor[100];
                        printf("Ingrese nombre del proveedor a buscar: ");
                        scanf("%s", nombreProveedor);
                        proveedor = buscarProveedor(headProveedores, nombreProveedor);
                        if (proveedor != NULL) {
                            printf("Proveedor encontrado: Nombre: %s, Email: %s, Numero Telefonico: %d\n", proveedor->nombreProveedor, proveedor->email, proveedor->numeroTelefonico);
                        } else {
                            printf("Proveedor no encontrado.\n");
                        }
                        break;
                    }
                    case 24:
                    {
                        char nombreProveedor[100];
                        printf("Ingrese nombre del proveedor a eliminar: ");
                        scanf("%s", nombreProveedor);
                        eliminarProveedor(&headProveedores, nombreProveedor);
                        printf("Proveedor eliminado exitosamente.\n");
                        break;
                    }
                    case 25:
                    {
                        mostrarProveedores(headProveedores);
                        break;
                    }
                    case 26:
                    {
                        int idProveedor;
                        struct Proveedor *proveedorModificado = NULL;
                        printf("Ingrese ID del proveedor a modificar: ");
                        scanf("%d", &idProveedor);

                        proveedorModificado = (struct Proveedor *)malloc(sizeof(struct Proveedor));
                        memset(proveedorModificado, 0, sizeof(struct Proveedor));

                        printf("Ingrese nuevo numero telefonico del proveedor: ");
                        scanf("%d", &proveedorModificado->numeroTelefonico);

                        proveedorModificado->nombreProveedor = (char *)malloc(100 * sizeof(char));
                        proveedorModificado->email = (char *)malloc(100 * sizeof(char));

                        printf("Ingrese nuevo nombre del proveedor: ");
                        scanf("%s", proveedorModificado->nombreProveedor);
                        printf("Ingrese nuevo email del proveedor: ");
                        scanf("%s", proveedorModificado->email);

                        modificarProveedor(headProveedores, idProveedor, proveedorModificado);

                        printf("Proveedor con ID %d modificado exitosamente.\n");
                        break;
                    }
                    case 27:
                    {
                        int idFarmaciaVenta, indice;
                        struct Farmacia *farmaciaVenta = NULL;
                        struct Producto *producto = NULL;
                        struct Venta *nuevaVenta = NULL;
                        int codigoProductoVendido, cantidadVendida, numeroBoleta, montoPagado, clienteTrajoReceta, dia, mes, anio;
                        char nombreProductoVendido[100], rutCliente[20];

                        printf("Ingrese ID de la farmacia para la venta: ");
                        scanf("%d", &idFarmaciaVenta);
                        farmaciaVenta = buscarFarmacia(headFarmacias, idFarmaciaVenta);
                        if (farmaciaVenta == NULL) {
                            printf("Farmacia con ID %d no encontrada.\n", idFarmaciaVenta);
                            break;
                        }

                        printf("Ingrese codigo de producto vendido: ");
                        scanf("%d", &codigoProductoVendido);
                        indice = buscarProducto(farmaciaVenta->inventario, codigoProductoVendido);

                        if (indice != -1) {
                            producto = farmaciaVenta->inventario->arrProductos[indice];
                            strcpy(nombreProductoVendido, producto->nombreProducto);
                            printf("Ingrese cantidad vendida: ");
                            scanf("%d", &cantidadVendida);
                            printf("Ingrese numero de boleta: ");
                            scanf("%d", &numeroBoleta);
                            printf("Ingrese monto pagado: ");
                            scanf("%d", &montoPagado);
                            printf("Ingrese RUT del cliente: ");
                            scanf("%s", rutCliente);

                            if (producto->requiereReceta) {
                                printf("El cliente trajo receta (1: Si, 0: No): ");
                                scanf("%d", &clienteTrajoReceta);
                                if (clienteTrajoReceta == 0) {
                                    printf("Venta no realizada. El producto requiere receta y el cliente no trajo receta.\n");
                                    break;
                                }
                            } else {
                                clienteTrajoReceta = 0;
                            }

                            printf("Ingrese dia de compra:\n");
                            scanf("%d", &dia);
                            printf("Ingrese mes de compra:\n");
                            scanf("%d", &mes);
                            printf("Ingrese anio de compra:\n");
                            scanf("%d", &anio);

                            nuevaVenta = crearVenta(idFarmaciaVenta, codigoProductoVendido, nombreProductoVendido, cantidadVendida, numeroBoleta, montoPagado, rutCliente, clienteTrajoReceta, dia, mes, anio);
                            if (insertarVenta(&headVentas, nuevaVenta, farmaciaVenta->inventario)) {
                                insertarVentaEnFarmacia(headFarmacias,nuevaVenta);
                                printf("Nueva venta creada exitosamente.\n");
                            } else {
                                printf("No se pudo realizar la venta.\n");
                            }
                        } else {
                            printf("Producto con codigo %d no encontrado.\n", codigoProductoVendido);
                        }
                        break;
                    }
                    case 28:
                    {
                        int codigoProductoVendido;
                        struct Venta *ventaEncontrada = NULL;
                        printf("Ingrese codigo de producto vendido: ");
                        scanf("%d", &codigoProductoVendido);
                        ventaEncontrada = buscarVentaPorCodigo(headVentas, codigoProductoVendido);
                        if (ventaEncontrada != NULL) {
                            printf("Venta encontrada: Producto Vendido: %s, Cantidad Vendida: %d, Numero Boleta: %d, Monto Pagado: %d, RUT Cliente: %s\n", ventaEncontrada->nombreProductoVendido, ventaEncontrada->cantidadVendida, ventaEncontrada->numeroBoleta, ventaEncontrada->montoPagado, ventaEncontrada->rutCliente);
                        } else {
                            printf("Venta no encontrada.\n");
                        }
                        break;
                    }
                    case 29:
                    {
                        char rutCliente[20];
                        struct Venta *ventaEncontrada = NULL;
                        printf("Ingrese RUT del cliente: ");
                        scanf("%s", rutCliente);
                        ventaEncontrada = buscarVentaPorRut(headVentas, rutCliente);
                        if (ventaEncontrada != NULL) {
                            printf("Venta encontrada: Producto Vendido: %s, Cantidad Vendida: %d, Numero Boleta: %d, Monto Pagado: %d, RUT Cliente: %s\n", ventaEncontrada->nombreProductoVendido, ventaEncontrada->cantidadVendida, ventaEncontrada->numeroBoleta, ventaEncontrada->montoPagado, ventaEncontrada->rutCliente);
                        } else {
                            printf("Venta no encontrada.\n");
                        }
                        break;
                    }
                    case 30:
                    {
                        int numeroBoleta;
                        struct Venta *ventaEncontrada = NULL;
                        printf("Ingrese numero de boleta: ");
                        scanf("%d", &numeroBoleta);
                        if (ventaEncontrada != NULL) {
                            printf("Venta encontrada: Producto Vendido: %s, Cantidad Vendida: %d, Numero Boleta: %d, Monto Pagado: %d, RUT Cliente: %s\n", ventaEncontrada->nombreProductoVendido, ventaEncontrada->cantidadVendida, ventaEncontrada->numeroBoleta, ventaEncontrada->montoPagado, ventaEncontrada->rutCliente);
                        } else {
                            printf("Venta no encontrada.\n");
                        }
                        break;
                    }
                    case 31:
                    {
                        struct Venta *ventaMasVendida = buscarProductoMasVendido(headVentas);
                        if (ventaMasVendida != NULL) {
                            printf("Producto mas vendido: Producto Vendido: %s, Cantidad Vendida: %d, Numero Boleta: %d, Monto Pagado: %d, RUT Cliente: %s\n", ventaMasVendida->nombreProductoVendido, ventaMasVendida->cantidadVendida, ventaMasVendida->numeroBoleta, ventaMasVendida->montoPagado, ventaMasVendida->rutCliente);
                        } else {
                            printf("No se encontraron ventas.\n");
                        }
                        break;
                    }
                    case 32:
                    {
                        int numeroBoleta;
                        printf("Ingrese numero de boleta de la venta a eliminar: ");
                        scanf("%d", &numeroBoleta);
                        eliminarVenta(&headVentas, numeroBoleta);
                        printf("Venta eliminada exitosamente.\n");
                        break;
                    }
                    case 33:
                    {
                        mostrarVentas(headVentas, farmaciaSeleccionada);
                        break;
                    }
                    case 34:
                    {
                        int numeroBoleta, indiceProducto;
                        struct Venta *ventaModificada = NULL;
                        printf("Ingrese numero de boleta de la venta a modificar: ");
                        scanf("%d", &numeroBoleta);

                        ventaModificada = (struct Venta *)malloc(sizeof(struct Venta));
                        ventaModificada->nombreProductoVendido = (char *)malloc(100 * sizeof(char));
                        ventaModificada->rutCliente = (char *)malloc(20 * sizeof(char));

                        printf("Ingrese nuevo codigo de producto vendido: ");
                        scanf("%d", &ventaModificada->codigoProductoVendido);
                        printf("Ingrese nuevo nombre del producto vendido: ");
                        scanf("%s", ventaModificada->nombreProductoVendido);
                        printf("Ingrese nueva cantidad vendida: ");
                        scanf("%d", &ventaModificada->cantidadVendida);
                        printf("Ingrese nuevo numero de boleta: ");
                        scanf("%d", &ventaModificada->numeroBoleta);
                        printf("Ingrese nuevo monto pagado: ");
                        scanf("%d", &ventaModificada->montoPagado);
                        printf("Ingrese nuevo RUT del cliente: ");
                        scanf("%s", ventaModificada->rutCliente);
                        printf("Ingrese si el cliente trajo receta (1: Si, 0: No): ");
                        scanf("%d", &ventaModificada->clienteTrajoReceta);
                        printf("Ingrese nuevo dia de venta: ");
                        scanf("%d", &ventaModificada->diaVenta);
                        printf("Ingrese nuevo mes de venta: ");
                        scanf("%d", &ventaModificada->mesVenta);
                        printf("Ingrese nuevo anio de venta: ");
                        scanf("%d", &ventaModificada->anioVenta);

                        indiceProducto = buscarProducto(farmaciaSeleccionada->inventario, ventaModificada->codigoProductoVendido);
                        if (indiceProducto != -1) {
                            modificarVenta(headVentas, numeroBoleta, ventaModificada);
                            printf("Venta con numero de boleta %d modificada exitosamente.\n", numeroBoleta);
                        } else {
                            printf("Producto con codigo %d no encontrado.\n", ventaModificada->codigoProductoVendido);
                        }

                        break;
                    }
                    case 35:
                    {
                        mostrarProductosVendidosConReceta(headVentas, farmaciaSeleccionada);
                        break;
                    }
                    case 36:
                    {
                        struct OrdenCompra *nuevaOrden = NULL;
                        int idOrden, montoComprado = 0, dia, mes, anio, idFarmacia,indiceProducto, codigoProducto, cantidad,agregarMas;
                        struct ProductoOrden *nuevoProductoOrden =NULL;
                        struct Producto *producto = NULL;
                        printf("Ingrese ID de la orden de compra: ");
                        scanf("%d", &idOrden);
                        printf("Ingrese dia de llegada: ");
                        scanf("%d", &dia);
                        printf("Ingrese mes de llegada: ");
                        scanf("%d", &mes);
                        printf("Ingrese anio de llegada: ");
                        scanf("%d", &anio);
                        printf("Ingrese ID de la farmacia: ");
                        scanf("%d", &idFarmacia); // Añadir la entrada para idFarmacia

                        nuevaOrden = crearOrdenCompra(idOrden, montoComprado, dia, mes, anio, idFarmacia); // Añadir idFarmacia a la funcion


                        do {
                            printf("Ingrese codigo del producto a reponer: ");
                            scanf("%d", &codigoProducto);
                            indiceProducto = buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                            if (indiceProducto != -1) {
                                producto = farmaciaSeleccionada->inventario->arrProductos[indiceProducto];
                                printf("Ingrese cantidad a reponer: ");
                                scanf("%d", &cantidad);
                                nuevoProductoOrden = crearProductoOrden(codigoProducto, cantidad);
                                agregarProductoAOrden(nuevaOrden, nuevoProductoOrden);
                                montoComprado += producto->precioProducto * cantidad;
                            } else {
                                printf("Producto con codigo %d no encontrado.\n", codigoProducto);
                            }
                            printf("¿Desea agregar otro producto a la orden? (1: Si, 0: No): ");
                            scanf("%d", &agregarMas);
                        } while (agregarMas);

                        nuevaOrden->montoComprado = montoComprado;
                        insertarOrdenCompra(&headOrdenes, nuevaOrden);
                        printf("Nueva orden de compra creada exitosamente.\n");
                        break;
                    }
                    case 37:
                    {
                        mostrarOrdenesCompra(headOrdenes, farmaciaSeleccionada);
                        break;
                    }
                    case 38:
                    {
                        int idOrden, montoComprado, dia, mes, anio,codigoProducto, cantidad, indiceProducto, agregarMas;
                        struct OrdenCompra *ordenModificada = NULL;
                        struct ProductoOrden *nuevoProductoOrden = NULL;
                        printf("Ingrese ID de la orden de compra a modificar: ");
                        scanf("%d", &idOrden);
                        printf("Ingrese nuevo monto comprado: ");
                        scanf("%d", &montoComprado);
                        printf("Ingrese nuevo dia de llegada: ");
                        scanf("%d", &dia);
                        printf("Ingrese nuevo mes de llegada: ");
                        scanf("%d", &mes);
                        printf("Ingrese nuevo anio de llegada: ");
                        scanf("%d", &anio);

                        ordenModificada = crearOrdenCompra(idOrden, montoComprado, dia, mes, anio, farmaciaSeleccionada->idFarmacia);

                        do {
                            printf("Ingrese codigo del producto a agregar/modificar: ");
                            scanf("%d", &codigoProducto);
                            indiceProducto = buscarProducto(farmaciaSeleccionada->inventario, codigoProducto);
                            if (indiceProducto != -1) {
                                printf("Ingrese cantidad de producto a agregar/modificar: ");
                                scanf("%d", &cantidad);
                                nuevoProductoOrden = crearProductoOrden(codigoProducto, cantidad);
                                agregarProductoAOrden(ordenModificada, nuevoProductoOrden);
                            } else {
                                printf("Producto con codigo %d no encontrado.\n", codigoProducto);
                            }
                            printf("¿Desea agregar/modificar otro producto? (1: Si, 0: No): ");
                            scanf("%d", &agregarMas);
                        } while (agregarMas == 1);

                        modificarOrdenCompra(headOrdenes, idOrden, ordenModificada);
                        printf("Orden de compra modificada exitosamente.\n");

                        break;
                    }
                    case 39:
                    {
                        int idOrden, mesActual, anioActual;
                        printf("Ingrese ID de la orden de compra: ");
                        scanf("%d", &idOrden);
                        printf("Ingrese el mes actual: ");
                        scanf("%d", &mesActual);
                        printf("Ingrese el anio actual: ");
                        scanf("%d", &anioActual);
                        seguimientoEnvios(headOrdenes, idOrden, mesActual, anioActual);
                        break;
                    }
                    case 40:
                    {
                        int codigoVenta;
                        struct Venta *ventaSeleccionada = NULL;
                        printf("Ingrese el codigo de la venta a verificar reabastecimiento: ");
                        scanf("%d", &codigoVenta);

                        ventaSeleccionada = buscarVentaPorCodigo(headVentas, codigoVenta);
                        if (ventaSeleccionada != NULL) {
                            verificarReabastecimiento(farmaciaSeleccionada, ventaSeleccionada);
                        } else {
                            printf("Venta no encontrada.\n");
                        }
                        break;
                    }
                    case 41:
                    {
                        int mesInicio, mesFin;
                        printf("Ingrese el mes de inicio: ");
                        scanf("%d", &mesInicio);
                        printf("Ingrese el mes de fin: ");
                        scanf("%d", &mesFin);
                        informeTendenciasVentas(headVentas, mesInicio, mesFin, farmaciaSeleccionada);
                        break;
                    }
                    case 42:
                    {
                        int mes1, mes2, anio;
                        printf("Ingrese ID de la farmacia para trabajar: ");
                        scanf("%d", &idFarmacia);
                        printf("Ingrese el primer mes a comparar: ");
                        scanf("%d", &mes1);
                        printf("Ingrese el segundo mes a comparar: ");
                        scanf("%d", &mes2);
                        printf("Ingrese el anio de caducidad: ");
                        scanf("%d", &anio);

                        printf("Mes 1: %d, Mes 2: %d, Anio: %d\n", mes1, mes2, anio);

                        patronesEstacionales(headFarmacias, mes1, mes2, anio, idFarmacia);
                        break;
                    }
                    case 43:
                    {
                        productosAltaBajaRotacion(farmaciaSeleccionada->inventario);
                        break;
                    }
                    case 44:
                    {
                        int idFarmacia;
                        printf("Ingrese ID de la farmacia para trabajar: ");
                        scanf("%d", &idFarmacia);
                        farmaciaSeleccionada = seleccionarFarmacia(headFarmacias, idFarmacia);
                        if (farmaciaSeleccionada != NULL) {
                            printf("Farmacia seleccionada: %s\n", farmaciaSeleccionada->nombreFarmacia);
                        } else {
                            printf("Farmacia con ID %d no encontrada. Por favor, ingrese un ID valido.\n", idFarmacia);
                        }
                        break;
                    }
                    case 0:
                    {
                        printf("Saliendo...\n");
                        break;
                    }
                    default:
                        printf("Opcion invalida. Intente de nuevo.\n");
                }
            } while (opcion != 0);

        } //Llave del if arriba

    }while (farmaciaSeleccionada != NULL && opcion != 0);
    return 0;
}