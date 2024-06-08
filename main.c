#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h>
    #include <allegro5/allegro_ttf.h>
#include <stdio.h>

#define lado 101
#define Nfil 10
#define Ncol 15
#define font_size 30

struct _hitBox{
    int leftBox;
    int rightBox;
    int upBox;
    int bottomBox;
};
typedef struct _hitBox hitBox;

struct _square{
    int fil;
    int col;
};
typedef struct _square square;

struct _personaje {
    square position;
    square boardPlace;
    char direction; // U, D, L, R
    hitBox box;
    int velocity;
} player1;
typedef struct _personaje personaje;

//Funciones lógicasM
int moveTo(int board[Nfil][Ncol], personaje *pnj, int newfil, int newcol);
square defineSquare(int filPixel, int colPixel);
void power(int board[Nfil][Ncol], personaje pnj);

//Funciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_pnj(personaje pbj, ALLEGRO_COLOR color, ALLEGRO_FONT *font);
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
    int board[Nfil][Ncol];

    for(i=0; i<Nfil; i++)
        for(j=0; j<Ncol; j++)
            board[i][j]=0;

    board[0][0]=1;

    //Inicializando al jugador 1
    player1.position.fil=51;
    player1.position.col=51;
    player1.boardPlace.fil=0;
    player1.boardPlace.col=0;
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
    ALLEGRO_DISPLAY *ventana = al_create_display(Ncol*lado, Nfil*lado);
    al_set_target_backbuffer(ventana);

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue(); /*Creamos cola de eventos*/
    al_register_event_source(event_queue, al_get_display_event_source(ventana)); /*La ventana puede dar eventos*/
    al_register_event_source(event_queue, al_get_keyboard_event_source());/*El teclado puede dar eventos*/


    /* Crear bitmap para el fondo del tablero */
    ALLEGRO_BITMAP *board_bitmap = al_create_bitmap(Ncol * lado, Nfil * lado);
    draw_board(board_bitmap, roboto, color_black, color_blue, color_white);

    al_draw_text(roboto, color_white, (Ncol*lado)/2, (Nfil*lado)/2, ALLEGRO_ALIGN_CENTRE, "Ingrese cualquier tecla para empezar.");

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
                    moveTo(board, &player1, player1.position.fil-(5*player1.velocity), player1.position.col);
                    break;
                case ALLEGRO_KEY_DOWN:
                    moveTo(board, &player1, player1.position.fil+(5*player1.velocity), player1.position.col);
                    break;
                case ALLEGRO_KEY_LEFT:
                    moveTo(board, &player1, player1.position.fil, player1.position.col-(5*player1.velocity));
                    break;
                case ALLEGRO_KEY_RIGHT:
                    moveTo(board, &player1, player1.position.fil, player1.position.col+(5*player1.velocity));
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
        for(i=0; i<Nfil; i++)
        {
            for(j=0; j<Ncol; j++)
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
        draw_pnj(player1, color_purple1, roboto);

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
int moveTo(int board[Nfil][Ncol], personaje *pnj, int newfil, int newcol)
{
    //Definiendo direccion de personaje
    if((pnj->position.fil) < newfil) //Mirando abajo
    {
        if(pnj->direction != 'D')
        {
            pnj->direction='D';
            return 0;
        }
    }
    else if((pnj->position.fil) > newfil) //Mirando arriba
    {
        if(pnj->direction != 'U')
        {
            pnj->direction='U';
            return 0;
        }
    }
    else if((pnj->position.col) < newcol) //Mirando derecha
    {
        if(pnj->direction != 'R')
        {
            pnj->direction='R';
            return 0;
        }
    }
    else if((pnj->position.col) > newcol) //Mirando izquierda
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
        pnj->position.col=newcol;
        if(topEdge<0)
            pnj->position.fil = 0 + pnj->box.upBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes superiors del personaje para generar colision.
            (board[defineSquare(topEdge , newcol).fil][pnj->boardPlace.col]>1)
            || (board[defineSquare(topEdge , newcol).fil][defineSquare(topEdge , newcol-pnj->box.leftBox).col]>1) 
            || (board[defineSquare(topEdge , newcol).fil][defineSquare(topEdge , newcol+pnj->box.rightBox).col]>1) 
            )
        {
            pnj->position.fil = (lado * (defineSquare(topEdge,newcol).fil+1)) + pnj->box.upBox; //Se agrega un +1 al defineSquare porque necesitamos el borde inferior de la casilla de colision
        }
        else
            pnj->position.fil = newfil;
    }
    else if(pnj->direction == 'D')
    {
        pnj->position.col=newcol;
        if(bottomEdge >= (lado*Nfil))
            pnj->position.fil = (lado*Nfil) - pnj->box.bottomBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
            (board[defineSquare(bottomEdge , newcol).fil][pnj->boardPlace.col]>1)
            || (board[defineSquare(bottomEdge , newcol).fil][defineSquare(bottomEdge , newcol-pnj->box.leftBox).col]>1) 
            || (board[defineSquare(bottomEdge , newcol).fil][defineSquare(bottomEdge , newcol+pnj->box.rightBox).col]>1) 
            )
        {
            pnj->position.fil = (lado * (defineSquare(bottomEdge,newcol).fil)) - pnj->box.bottomBox - 1;
        }
        else
            pnj->position.fil = newfil;
    }
    else if(pnj->direction == 'L')
    {
        pnj->position.fil=newfil;
        if(leftEdge < 0)
            pnj->position.col = 0 + pnj->box.leftBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes izquierdos del personaje para generar colision.
            (board[pnj->boardPlace.fil][defineSquare(newfil , leftEdge).col]>1)
            || (board[defineSquare(newfil-pnj->box.upBox , leftEdge).fil][defineSquare(newfil , leftEdge).col]>1)
            || (board[defineSquare(newfil+pnj->box.bottomBox , leftEdge).fil][defineSquare(newfil , leftEdge).col]>1)
            )
        {
            pnj->position.col = (lado * (defineSquare(newfil , leftEdge).col+1)) + pnj->box.leftBox; //Se agrega un +1 al defineSquare porque necesitamos el borde derecho de la casilla de colision
        }
        else
            pnj->position.col = newcol;
    }
    else if(pnj->direction == 'R')
    {
        pnj->position.fil=newfil;
        if(rightEdge >= (lado*Ncol))
            pnj->position.col = (lado*Ncol) - pnj->box.rightBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes derechos del personaje para generar colision.
            (board[pnj->boardPlace.fil][defineSquare(newfil , rightEdge).col]>1)
            || (board[defineSquare(newfil-pnj->box.upBox , rightEdge).fil][defineSquare(newfil , rightEdge).col]>1)
            || (board[defineSquare(newfil+pnj->box.bottomBox , rightEdge).fil][defineSquare(newfil , rightEdge).col]>1)
            )
        {
            pnj->position.col = (lado * (defineSquare(newfil , rightEdge).col)) - pnj->box.rightBox - 1;
        }
        else
            pnj->position.col = newcol;
    }


    // Alterando la posición del jugador en la matriz
    if((pnj->position.col / lado)!=pnj->boardPlace.col)
    {
        board[pnj->boardPlace.fil][pnj->boardPlace.col] = 0;
        board[pnj->position.fil / lado][pnj->position.col / lado] = 1;
        pnj->boardPlace.fil = pnj->position.fil / lado;
        pnj->boardPlace.col = pnj->position.col / lado;
    }
    if((pnj->position.fil / lado)!=pnj->boardPlace.fil)
    {
        board[pnj->boardPlace.fil][pnj->boardPlace.col] = 0;
        board[pnj->position.fil / lado][pnj->position.col / lado] = 1;
        pnj->boardPlace.fil = pnj->position.fil / lado;
        pnj->boardPlace.col = pnj->position.col / lado;
    }

    return 0;
}

square defineSquare(int filPixel, int colPixel)
{
    square position;

    position.fil = filPixel / lado;
    position.col = colPixel / lado;

    return position;
}   

void power(int board[Nfil][Ncol], personaje pnj)
{
    int i,j, startPlace=0, replacedColor=2, toReplaceColor=0;

    if(pnj.direction == 'D')
    {
        j=pnj.boardPlace.col;

        startPlace = defineSquare(pnj.position.fil + pnj.box.bottomBox , pnj.position.col).fil+1;

        if(board[startPlace][j] == 2)
        {
            replacedColor = 0;
            toReplaceColor = 2;
        }

        for(i=startPlace; i<Nfil; i++)
            if(board[i][j] == toReplaceColor)
                board[i][j] = replacedColor;
            else
                break;
    }
    else if(pnj.direction == 'U')
    {
        j=pnj.boardPlace.col;

        startPlace = defineSquare(pnj.position.fil - pnj.box.upBox , pnj.position.col).fil-1;

        if(board[startPlace][j] == 2)
        {
            replacedColor = 0;
            toReplaceColor = 2;
        }

        for(i=startPlace; i>=0; i--)
            if(board[i][j] == toReplaceColor)
                board[i][j] = replacedColor;
            else
                break;
    }
    else if(pnj.direction == 'R')
    {
        i=pnj.boardPlace.fil;

        startPlace = defineSquare(pnj.position.fil , pnj.position.col + pnj.box.rightBox).col+1;

        if(board[i][startPlace] == 2)
        {
            replacedColor = 0;
            toReplaceColor = 2;
        }

        for(j=startPlace; j<Ncol; j++)
            if(board[i][j] == toReplaceColor)
                board[i][j] = replacedColor;
            else
                break;
    }
    else if(pnj.direction == 'L')
    {
        i=pnj.boardPlace.fil;

        startPlace = defineSquare(pnj.position.fil , pnj.position.col - pnj.box.leftBox).col-1;

        if(board[i][startPlace] == 2)
        {
            replacedColor = 0;
            toReplaceColor = 2;
        }

        for(j=startPlace; j>=0; j--)
            if(board[i][j] == toReplaceColor)
                board[i][j] = replacedColor;
            else
                break;
    }

    return;
}

// FUnciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * lado)+1, (fila * lado)+1, ((columna + 1) * lado)-1, ((fila + 1) * lado)-1, color);
}

void draw_pnj(personaje pbj, ALLEGRO_COLOR color, ALLEGRO_FONT *font){
    al_draw_filled_rectangle(pbj.position.col-pbj.box.leftBox, pbj.position.fil-pbj.box.upBox, pbj.position.col+pbj.box.rightBox, pbj.position.fil+pbj.box.bottomBox, color);

    switch (pbj.direction)
    {
    case 'U':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "U");
        break;
    case 'D':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "D");
        break;
    case 'L':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "L");
        break;
    case 'R':
        al_draw_text(font,al_map_rgb(255,255,255),pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "R");
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
    for (i = 0; i <= Ncol; i++)
        al_draw_line(i * lado, 0, i * lado, Nfil * lado, lineColor, 2);

    for (j = 0; j <= Nfil; j++)
        al_draw_line(0, j * lado, Ncol * lado, j * lado, lineColor, 2);

    /* Dibujo de texto */
    num = 1;
    for (i = 0; i < Nfil; i++) {
        for (j = 0; j < Ncol; j++) {
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
