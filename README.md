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

## Inicialización de Allegro
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

## Eventos, Fuentes de eventos
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