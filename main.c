#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h> 
    #include <allegro5/allegro_ttf.h> 
#include <stdio.h>

#define lado 100
#define fil 10
#define col 15
#define font_size 30

struct _personaje {
    int position_fil;
    int position_col;
    int oldPosition_fil;
    int oldPosition_col;
    char direction; // U, D, L, R
} player1;
typedef struct _personaje personaje;

//Funciones lógicasM
int moveTo(int board[fil][col], personaje *pnj, int newfil, int newcol);
void power(int board[fil][col], personaje pnj);

//Funciones gráficas
void draw_rectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_board(ALLEGRO_BITMAP *bitmap, ALLEGRO_FONT *font, ALLEGRO_COLOR backColor, ALLEGRO_COLOR lineColor, ALLEGRO_COLOR numberColor);

int main()
{
    int i, j, num=1;

    // Inicializción colores
    ALLEGRO_COLOR color_black = al_map_rgb(0,0,0);
    ALLEGRO_COLOR color_white = al_map_rgb(255,255,255);
    ALLEGRO_COLOR color_blue = al_map_rgb(100,100,255);
    ALLEGRO_COLOR color_purple1 = al_map_rgb(87, 35, 100);
    ALLEGRO_COLOR color_purple2 = al_map_rgb(182, 149, 192);

    // Banderas
    int done = 0;

    int position[2];
    int board[fil][col];

    for(i=0; i<fil; i++)
        for(j=0; j<col; j++)
            board[i][j]=0;

    board[0][0]=1;
    player1.position_fil=0;
    player1.position_col=0;
    player1.direction='D';

    /*Inicialización allegro*/
    al_init();
    al_init_primitives_addon(); // Figuras
    al_init_image_addon(); // Imagenes
    al_init_font_addon(); // Texto
    al_init_ttf_addon();
    al_install_keyboard(); // Teclado

    /*Fuentes*/
    ALLEGRO_FONT *roboto = al_load_ttf_font("../src/fonts/Roboto/Roboto-Bold.ttf", font_size, 0);

    // Inicializando ventana
    ALLEGRO_DISPLAY *ventana = al_create_display(col*lado, fil*lado);
    al_set_target_backbuffer(ventana);

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue(); /*Creamos cola de eventos*/
    al_register_event_source(event_queue, al_get_display_event_source(ventana)); /*La ventana puede dar eventos*/
    al_register_event_source(event_queue, al_get_keyboard_event_source());/*El teclado puede dar eventos*/


    /* Crear bitmap para el fondo del tablero */
    ALLEGRO_BITMAP *board_bitmap = al_create_bitmap(col * lado, fil * lado);
    draw_board(board_bitmap, roboto, color_black, color_blue, color_white);

    al_draw_text(roboto, color_white, (col*lado)/2, (fil*lado)/2, ALLEGRO_ALIGN_CENTRE, "Ingrese cualquier tecla para empezar.");

    al_flip_display();

    ALLEGRO_EVENT ev; /*Creamos un evento que analizaremos*/

    al_wait_for_event(event_queue, &ev); /*Esperando a que el usuario reaccione*/

    while (!done) {
        /*Limpiamos el Backbuffer*/
        al_clear_to_color(al_map_rgb(0, 0, 0));

        al_wait_for_event(event_queue, &ev); /*Esperando a que ocurra un evento*/

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) /*Si es un cierre de la ventana*/
        {
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    moveTo(board, &player1, player1.position_fil-1, player1.position_col);
                    break;
                case ALLEGRO_KEY_DOWN:
                    moveTo(board, &player1, player1.position_fil+1, player1.position_col);
                    break;
                case ALLEGRO_KEY_LEFT:
                    moveTo(board, &player1, player1.position_fil, player1.position_col-1);
                    break;
                case ALLEGRO_KEY_RIGHT:
                    moveTo(board, &player1, player1.position_fil, player1.position_col+1);
                    break;
                case ALLEGRO_KEY_SPACE:
                    power(board, player1);
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    done = true;
                    break;
            }
        }

        /*Dibujamos el fondo en el backbuffer*/
        al_draw_bitmap(board_bitmap,0,0,0);

        /* Dibujar el tablero con colores en el backbuffer*/
        for(i=0; i<fil; i++)
        {
            for(j=0; j<col; j++)
            {
                switch (board[i][j])
                {
                    case 1:
                        draw_rectangle(i, j, color_purple1);
                        break;
                    case 2:
                        draw_rectangle(i, j, color_purple2);
                        break;
                }
            }
        }
        
        /* Actualizar pantalla */
        al_flip_display();
    }

    /* Cerrar recursos */
    al_destroy_display(ventana);
    al_destroy_event_queue(event_queue);
    al_destroy_font(roboto);
    /*Cerrando ventana*/
    al_destroy_display(ventana);

    return 0;
}
// funciones lógicas
int moveTo(int board[fil][col], personaje *pnj, int newfil, int newcol)
{
    if(newfil < 0 || newfil >= fil || newcol < 0 || newcol >= col) /*Si salgo del mapa*/
        return 1;
    else if(board[newfil][newcol]!=0)
        return 1;
    else
    {
        board[(*pnj).position_fil][(*pnj).position_col] = 0;
        board[newfil][newcol] = 1;

        //Actualizando posiciones del pj
        (*pnj).oldPosition_col = (*pnj).position_col;
        (*pnj).oldPosition_fil = (*pnj).position_fil;
        (*pnj).position_col = newcol;
        (*pnj).position_fil = newfil;

        //Definiendo direccion de personaje
        if((*pnj).oldPosition_fil < (*pnj).position_fil) //Mirando abajo
            (*pnj).direction='D';
        else if((*pnj).oldPosition_fil > (*pnj).position_fil) //Mirando arriba
            (*pnj).direction='U';
        else if((*pnj).oldPosition_col < (*pnj).position_col) //Mirando derecha
            (*pnj).direction='R';
        else if((*pnj).oldPosition_col > (*pnj).position_col) //Mirando izquierda
            (*pnj).direction='L';
    }

    return 0;
}

void power(int board[fil][col], personaje pnj)
{
    int i,j;

    if(pnj.direction == 'D')
    {
        j=pnj.position_col;
        for(i=pnj.position_fil+1; i<fil; i++)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }
    else if(pnj.direction == 'U')
    {
        j=pnj.position_col;
        for(i=pnj.position_fil-1; i>=0; i--)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }
    else if(pnj.direction == 'R')
    {
        i=pnj.position_fil;
        for(j=pnj.position_col+1; j<col; j++)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }
    else if(pnj.direction == 'L')
    {
        i=pnj.position_fil;
        for(j=pnj.position_col-1; j>=0; j--)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }

    return;
}

// FUnciones gráficas
void draw_rectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * lado)+1, (fila * lado)+1, ((columna + 1) * lado)-1, ((fila + 1) * lado)-1, color);
}

void draw_board(ALLEGRO_BITMAP *bitmap, ALLEGRO_FONT *font, ALLEGRO_COLOR backColor, ALLEGRO_COLOR lineColor, ALLEGRO_COLOR numberColor)
{
    int i,j, num;
    float x,y; //Valores de x, y usados en partes del código
    char str[4];

    al_set_target_bitmap(bitmap);
    
    /* Nos aseguramos que el color del fondo sea negro */
    al_clear_to_color(backColor);

    /* Dibujo de cuadrícula */
    for (i = 0; i <= col; i++)
        al_draw_line(i * lado, 0, i * lado, fil * lado, lineColor, 2);

    for (j = 0; j <= fil; j++)
        al_draw_line(0, j * lado, col * lado, j * lado, lineColor, 2);

    /* Dibujo de texto */
    num = 1;
    for (i = 0; i < fil; i++) {
        for (j = 0; j < col; j++) {
            sprintf(str, "%d", num); // Genera el número en formato cadena de texto
            x = (j * lado) + (lado / 2);
            y = (i * lado) + (lado / 2) - (font_size / 2);
            al_draw_text(font, numberColor, x, y, ALLEGRO_ALIGN_CENTRE, str);
            num++;
        }
    }

    // Seleccionamos el Display como el Backbuffer, ya no el birmap del fondo
    al_set_target_backbuffer(al_get_current_display());
    return;
}