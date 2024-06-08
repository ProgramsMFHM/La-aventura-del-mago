#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h> 
    #include <allegro5/allegro_ttf.h> 
#include <stdio.h>

#define lado 101
#define fil 10
#define col 15
#define font_size 30

struct _hitBox{
    int leftBox;
    int rightBox;
    int upBox;
    int bottomBox;
};
typedef struct _hitBox hitBox;

struct _personaje {
    int position_fil;
    int position_col;
    int oldPosition_fil;
    int oldPosition_col;
    int boardfil;
    int boardcol;
    char direction; // U, D, L, R
    hitBox box;
    int velocity;
} player1;
typedef struct _personaje personaje;

//Funciones lógicasM
int moveTo(int board[fil][col], personaje *pnj, int newfil, int newcol);
void power(int board[fil][col], personaje pnj);

//Funciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_pbj(personaje pbj, ALLEGRO_COLOR color, ALLEGRO_FONT *font);
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

    //Inicializando al jugador 1
    player1.position_fil=51;
    player1.position_col=51;
    player1.oldPosition_fil=51;
    player1.oldPosition_col=51;
    player1.boardfil=1;
    player1.boardcol=1;
    player1.direction='D';
    player1.box.leftBox=50;
    player1.box.rightBox=50;
    player1.box.upBox=50;
    player1.box.bottomBox=50;
    player1.velocity=1;

    /*Inicialización allegro*/
    al_init();
    al_init_primitives_addon(); // Figuras
    al_init_image_addon(); // Imagenes
    al_init_font_addon(); // Texto
    al_init_ttf_addon();
    al_install_keyboard(); // Teclado

    /*Fuentes*/
    ALLEGRO_FONT *roboto = al_load_ttf_font("./src/fonts/Roboto/Roboto-Bold.ttf", font_size, 0);

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
                    moveTo(board, &player1, player1.position_fil-(5*player1.velocity), player1.position_col);                    
                    break;
                case ALLEGRO_KEY_DOWN:
                    moveTo(board, &player1, player1.position_fil+(5*player1.velocity), player1.position_col);
                    break;
                case ALLEGRO_KEY_LEFT:
                    moveTo(board, &player1, player1.position_fil, player1.position_col-(5*player1.velocity));
                    break;
                case ALLEGRO_KEY_RIGHT:
                    moveTo(board, &player1, player1.position_fil, player1.position_col+(5*player1.velocity));
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
                        draw_boardRectangle(i, j, color_blue);
                        break;
                    case 2:
                        draw_boardRectangle(i, j, color_purple2);
                        break;
                }
            }
        }

        //Dibujando al jugador
        draw_pbj(player1, color_purple1, roboto);
        
        /* Actualizar pantalla */
        al_flip_display();
    }

    /* Cerrar recursos */
    al_destroy_display(ventana);
    al_destroy_event_queue(event_queue);
    al_destroy_font(roboto);

    return 0;
}
// funciones lógicas
int moveTo(int board[fil][col], personaje *pnj, int newfil, int newcol)
{
    //Definiendo la posición anterior antes del movimiento
    pnj->oldPosition_col=pnj->position_col;
    pnj->oldPosition_fil=pnj->position_fil;

    //Definiendo direccion de personaje
    if((pnj->oldPosition_fil) < newfil) //Mirando abajo
    {   
        if(pnj->direction != 'D')
        {
            pnj->direction='D';
            return 0;
        }
    }
    else if((pnj->oldPosition_fil) > newfil) //Mirando arriba
    {
        if(pnj->direction != 'U')
        {
            pnj->direction='U';
            return 0;
        }
    }
    else if((pnj->oldPosition_col) < newcol) //Mirando derecha
    {
        if(pnj->direction != 'R')
        {
            pnj->direction='R';
            return 0;
        }
    }
    else if((pnj->oldPosition_col) > newcol) //Mirando izquierda
    {
        if(pnj->direction != 'L')
        {
            pnj->direction='L';
            return 0;
        }
    }

    //Definimos los nuevos bordes de la hitbox
    int topEdge = newfil - pnj->box.upBox;
    int bottomEdge = newfil + pnj->box.bottomBox;
    int leftEdge = newcol - pnj->box.leftBox;
    int rightEdge = newcol + pnj->box.rightBox;

    if(pnj->direction == 'U')
    {
        pnj->position_col=newcol;
        if(topEdge<0)
            pnj->position_fil = 0 + pnj->box.upBox;
        else
            pnj->position_fil = newfil;
    }
    else if(pnj->direction == 'D')
    {
        pnj->position_col=newcol;
        if(bottomEdge >= (lado*fil))
            pnj->position_fil = (lado*fil) - pnj->box.bottomBox;
        else
            pnj->position_fil = newfil;
    }
    else if(pnj->direction == 'L')
    {
        pnj->position_fil=newfil;
        if(leftEdge < 0)
            pnj->position_col = 0 + pnj->box.leftBox;
        else
            pnj->position_col = newcol;
    }
    else if(pnj->direction == 'R')
    {
        pnj->position_fil=newfil;
        if(rightEdge >= (lado*col))
            pnj->position_col = (lado*col) - pnj->box.rightBox;
        else
            pnj->position_col = newcol;
    }


    // Alterando la posición del jugador en la matriz
    if((pnj->position_col / lado)!=pnj->boardcol)
    {
        board[pnj->boardfil][pnj->boardcol] = 0;
        board[pnj->position_fil / lado][pnj->position_col / lado] = 1;
        pnj->boardfil = pnj->position_fil / lado;
        pnj->boardcol = pnj->position_col / lado;
    }
    if((pnj->position_fil / lado)!=pnj->boardfil)
    {
        board[pnj->boardfil][pnj->boardcol] = 0;
        board[pnj->position_fil / lado][pnj->position_col / lado] = 1;
        pnj->boardfil = pnj->position_fil / lado;
        pnj->boardcol = pnj->position_col / lado;
    }

    return 0;
}

void power(int board[fil][col], personaje pnj)
{
    int i,j;

    if(pnj.direction == 'D')
    {
        j=pnj.boardcol;
        for(i=pnj.boardfil+1; i<fil; i++)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }
    else if(pnj.direction == 'U')
    {
        j=pnj.boardcol;
        for(i=pnj.boardfil-1; i>=0; i--)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }
    else if(pnj.direction == 'R')
    {
        i=pnj.boardfil;
        for(j=pnj.boardcol+1; j<col; j++)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }
    else if(pnj.direction == 'L')
    {
        i=pnj.boardfil;
        for(j=pnj.boardcol-1; j>=0; j--)
            if(board[i][j]== 0)
                board[i][j] = 2;
            else
                break;
    }

    return;
}

// FUnciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * lado)+1, (fila * lado)+1, ((columna + 1) * lado)-1, ((fila + 1) * lado)-1, color);
}

void draw_pbj(personaje pbj, ALLEGRO_COLOR color, ALLEGRO_FONT *font){
    al_draw_filled_rectangle(pbj.position_col-pbj.box.leftBox, pbj.position_fil-pbj.box.upBox, pbj.position_col+pbj.box.rightBox, pbj.position_fil+pbj.box.bottomBox, color);

    switch (pbj.direction)
    {
    case 'U':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position_col, pbj.position_fil-15, ALLEGRO_ALIGN_CENTRE, "U");
        break;
    case 'D':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position_col, pbj.position_fil-15, ALLEGRO_ALIGN_CENTRE, "D");
        break;
    case 'L':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position_col, pbj.position_fil-15, ALLEGRO_ALIGN_CENTRE, "L");
        break;
    case 'R':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position_col, pbj.position_fil-15, ALLEGRO_ALIGN_CENTRE, "R");
        break;
    }

    return;
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
