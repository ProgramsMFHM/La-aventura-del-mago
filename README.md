# Umag-Proyecto_Prog_estructurada

## Comprendiendo el funcionamiento de allegro

### Módulos de funcionamiento
Para funcionar Allegro utiliza alginos módulos que se incluyen a modo de bibliotecas como por ejemplo:
```C++
#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h> // Figuras primitivas
    #include <allegro5/allegro_image.h> // Imágenes
    #include <allegro5/allegro_font.h> // Fentes
    #include <allegro5/allegro_ttf.h> // Uso de archivos ttf para fuentes
```

### Inicialización de Allegro
Para funcionar Allegro precisa de la inicialización de sus diferentes módulos, a continuación un ejemplo;
```C++
/*Inicialización allegro*/
al_init();
al_init_primitives_addon(); // Figuras
al_init_image_addon(); // Imagenes
al_init_font_addon(); // Texto
al_init_ttf_addon();
al_install_keyboard(); // Teclado
```
Podemos ver en el anterior ejemplo la inicialización.

### Eventos, Fuentes de eventos
Para poder monitorear el comportamiento de los distintos periféricos de un computador Allegro ocupa eventos, similar a lo que se puede encontrar en lenguajes como `JS`.

Lo primero que se debe hacer es inicializar una cola de eventos, un lugar al que llegarán todos ellos.
```C++
//Inicializar cola de eventos
ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
```

Luego debemos asignar a esta cola de eventos los input que llegarán a ella, por ejemplo
```C++
al_register_event_source(event_queue, al_get_display_event_source(ventana)); /*La ventana puede dar eventos*/
al_register_event_source(event_queue, al_get_keyboard_event_source());/*El teclado puede dar eventos*/
al_register_event_source(event_queue, al_get_timer_event_source(timer));/*El temporizador puede dar eventos*/
```
Como vemos usamos variables como `ventana` o `timer` que deben ser creadas previamente con funciones como `ALLEGRO_DISPLAY *ventana = al_create_display(ancho, largo)` o `ALLEGRO_TIMER *timer = al_create_timer(tasa de refresco por segundo)`.

Luego es posible crear una variable para almacenar un evento de la siguiente manera
```C++
ALLEGRO_EVENT ev; /*Creamos un evento que analizaremos posteriormente*/
```

Luego podemos con la función: `al_wait_for_event(event_queue, &ev);` hacer que la cola de eventos `evente_queue` espere a un evento y guarde la información de este en `ev`.

Luego mediante ciclos y sistemas de control podemos analizar el evento almacenado.

### Analizando un evento
Para analizar un evento podemos usar alguna de sus propiedades.

#### ev.type
Nos permite conocer qué clase de evento fué accionado algunos de estos pueden ser:
- `ALLEGRO_EVENT_KEY_DOWN` Una tecla del teclado fué presionada
- `ALLEGRO_EVENT_KEY_UP` Una tecla del teclado fué dejada de presionar
- `ALLEGRO_EVENT_TIMER` Un temporizador ha completado un ciclo

Una vez detectado el tipo de evento es posible analizarlo con otras propiedades.
#### ev.keyboard
##### ev.keyboard.keycode
Nos permite conocer el código que va relacionado a una tecla que fué presionada.
En Allegro todas las teclas tienen su propio código que es un numero entero pero a cada uno de esos números está relacionado una variable, algunas de esas son:

- ALLEGRO_KEY_A ... ALLEGRO_KEY_Z
- ALLEGRO_KEY_0 ... ALLEGRO_KEY_9
- ALLEGRO_KEY_PAD_0 ... ALLEGRO_KEY_PAD_9
- ALLEGRO_KEY_F1 ... ALLEGRO_KEY_F12
- ALLEGRO_KEY_ESCAPE
- ALLEGRO_KEY_TILDE
- ALLEGRO_KEY_MINUS
- ALLEGRO_KEY_EQUALS
- ALLEGRO_KEY_BACKSPACE
- ALLEGRO_KEY_TAB
- ALLEGRO_KEY_OPENBRACE
- ALLEGRO_KEY_CLOSEBRACE

Además existe el código `ALLEGRO_KEY_MAX` cuyo valor es siempre uno más que el del código de tecla más grande, esto nos permite, por ejemplo crear un arreglo con el que controlar cuáles teclas están presionadas en cada momento de la siguiente manera.
``` C++
bool pressed_keys[ALLEGRO_KEY_MAX];
```
Luego podemos, por ejemplo cambiar el valor de estos elementos a `true` o `false` a conveniencia.

## Tips usados en este proyecto
### Arreglo de booleanos
Como fué mencionado anteriormente el uso de un arreglo de booleanos para detectar teckas presionadas es una manera útil de usar los códigos de tecla de Allegro.
``` C++
bool pressed_keys[ALLEGRO_KEY_MAX];
```

En el caso del programa tenemos el siguiente fragmento de código:
``` C++
else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
{
    if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    ...
    else
    {
        keys[ev.keyboard.keycode] = true;
    }
}
```
Enfocándonos en la línea `keys[ev.keyboard.keycode] = true;` lo que hacemos es detectar qué tecla fué presionada y mandar esto en forma de `true` a nuestro arreglo de booleanos.

Por otra parte tenemos:
``` C++
else if (ev.type == ALLEGRO_EVENT_KEY_UP)
{
    keys[ev.keyboard.keycode] = false;
}
```
En caso de que una tecla se haya dejado de presionar entonces mandamos esto en forma de `false` a nuestro arreglo de booleanos.

Luego mediante un fragmento como el siguiente:
```C++
if (keys[ALLEGRO_KEY_UP])
{
    ...
}
else if (keys[ALLEGRO_KEY_DOWN])
{
    ...
}
else if (keys[ALLEGRO_KEY_LEFT])
{
    ...
}
else if (keys[ALLEGRO_KEY_RIGHT])
{
    ...
}
```
Decidimos qué hacer en caso de que una tecla esté presionada o no.

Esto es útil para poder tener un movimiento fluido que dependa del tiempo y que sea compatible con presionar una tecla por un periodo prolongado de tiempo.

### Scroll - un mapa más grande
Una de las funconalidades más curiosas es el poder moverte por un mapa de un tamaño distinto si así el nivel lo requiere; para hacer esto se siguieron los siguientes pasos.
1. Cada archivo de nivel tiene al inicio una indicación con 2 nuemros *Numero de filas* y *Numero de columnas*
2. Se crea una matriz que pueda almacenar cualquier mapa que se de,para esto se le asigna un tamaño de `MAXRPWS` y `MAXCOLS`
3. Al cargar un nuevo nivel lo primero es con la función `draw_background()` generar el BITMAP para el mapa que se crea del tamaño del mapa indicado en el archivo (información que se guarda en una estructura de tipo `gameInfo`).

A partir de este punto todo depende de la posición del jugador. Un jugador puede estar en la posición `3000` a pesar de que la ventana tenga solo `960px` de ancho, esto es porque el "mapa grande" abarca todas las posibles posiciones a nivel de pixeles. por lo que los siguientes pasos son:
4. Se calculan los límites de la ventana. Partiendo de la posición del personaje en cada iteración se calcula la posición en que se debe ubicar la ventana para que el personaje quede en el centro de la misma (Se tiene en cuenta que cuando la ventana toca un borde del mapa entonces el límite de la ventana es ese borde).
5. COn el paso anterior se consiguen dos valores fundamentales para la construcción del resto del mapa, estos son: parte de la estructua `gameInfo` y son `mapColStart` y `mapFilStart`; nos indican cuál es eñl primer pixel que debemos mostrar en ventana y de ahí se puede calcular el resto conocido el tamaño de la ventana deseada.
6. Conocidos estos desplazamientos se calcula a qué casillas de el `board` corresponden y se renderiza solamente esta "submatriz".

> 📘 Importante
>
> Un descubrimiento relevante para poder renderizar las imagenes de manera suave es que es posible imprimir un BITMAP en pantalla enb una posición negativa, es decir, con coordenadas que se salen de la pantalla. Podemos por ejemplo imprimir un mapa en la posición $(-50,-50)$ y se renderizará a partir de este punto el mapa mostrándose solo lo que quepa dentro de los pixeles asignados a la pantalla.
>
> Esto es útil puesto que para mostrar los sprites de los obstáculos no hace falta calcular "fracciones de casilla" sino que, puesto que se tiene el valor del lado de la casilla y el valor numérico de cada casilla dentro de la matriz `BOARD` al ejecutar algo como: "`(8*lado) - mapColStart`" obtendremos una posición exacta de dónde hay que imprimir un sprite para que se vea completo dentro de la pantalla.

## Numeración de mapas.
- **1** Personaje principal
- **21-40** Objetos
- **41-80** Muros - De 4 en 4 para usar el poder