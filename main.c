#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h>
    #include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <string.h>

#define lado 60
#define Nfil 10
#define Ncol 15
#define font_size 20
#define FPS 30

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

struct _hielo{
    square begin;
    char direction; // U, D, L, R
    int restantes;
    int limit;
    bool create; // Create or Destroy
    bool possible; // Able or unable to create Ice
};
typedef struct _hielo hielo;

struct _personaje {
    square position;
    square boardPlace;
    char direction; // U, D, L, R
    hitBox box;
    int velocity;
} player1;
typedef struct _personaje personaje;

/*Colores*/
ALLEGRO_COLOR color_black;
ALLEGRO_COLOR color_white;
ALLEGRO_COLOR color_blue;
ALLEGRO_COLOR color_purple1;
ALLEGRO_COLOR color_green1;
ALLEGRO_COLOR color_green2;
ALLEGRO_COLOR color_green3;
ALLEGRO_COLOR color_green4;

//Funciones lógicas
int moveTo(int board[Nfil][Ncol], personaje *pnj, int newfil, int newcol);
square defineSquare(int filPixel, int colPixel);
hielo power(int board[Nfil][Ncol], personaje pnj);
void manageIce(int board[Nfil][Ncol], hielo *ice);
int getBoard(int board[Nfil][Ncol], char numero[3]);

//Funciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_pnj(personaje pbj);
void draw_background(ALLEGRO_BITMAP *bitmap);
void draw_board(int board[Nfil][Ncol]);
/*Partes*/
int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int game(int board[Nfil][Ncol], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);

/*Bitmaps*/
ALLEGRO_DISPLAY *ventana;
ALLEGRO_BITMAP *board_bitmap;

ALLEGRO_FONT *roboto; /*Fuente*/
ALLEGRO_TIMER *timer; /*Timer*/

/*Other variables*/
bool keys[ALLEGRO_KEY_MAX] = { false }; /* Inicializa todas las teclas como no presionadas. */

int main()
{
    int i, j, num=1;

    // Inicializción colores
    color_black = al_map_rgb(0,0,0);
    color_white = al_map_rgb(255,255,255);
    color_blue = al_map_rgb(100,100,255);
    color_purple1 = al_map_rgb(87, 35, 100);
    color_green1 = al_map_rgb(8, 28, 21);
    color_green2 = al_map_rgb(27, 67, 50);
    color_green3 = al_map_rgb(12, 106, 79);
    color_green4 = al_map_rgb(64, 145, 108);

    // Banderas
    bool done = false;
    int position[2];
    int board[Nfil][Ncol];

    //Inicializando al jugador 1
    player1.direction='D';
    player1.box.leftBox=25;
    player1.box.rightBox=25;
    player1.box.upBox=25;
    player1.box.bottomBox=25;
    player1.velocity=1;

    /*Inicialización allegro*/
    al_init();
    al_init_primitives_addon(); // Figuras
    al_init_image_addon(); // Imagenes
    al_init_font_addon(); // Texto
    al_init_ttf_addon();
    al_install_keyboard(); // Teclado

    /*Fuentes*/
    roboto = al_load_ttf_font("./src/fonts/Roboto/Roboto-Bold.ttf", font_size, 0);

    // Inicializando ventana
    ventana = al_create_display(Ncol*lado, Nfil*lado);
    al_set_target_backbuffer(ventana);

    //Temporizadores
    timer = al_create_timer(1.0 / FPS);

    //Inicializar cola de eventor
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue(); /*Creamos cola de eventos*/
    al_register_event_source(event_queue, al_get_display_event_source(ventana)); /*La ventana puede dar eventos*/
    al_register_event_source(event_queue, al_get_keyboard_event_source());/*El teclado puede dar eventos*/
    al_register_event_source(event_queue, al_get_timer_event_source(timer));/*El temporizador puede dar eventos*/
    ALLEGRO_EVENT ev; /*Creamos un evento que analizaremos*/

    while (!done)
    {
        switch (main_menu(event_queue, &ev, timer))
        {
            case 0: //jugar
                if(game(board, event_queue, &ev, timer) == -1)
                    done =true;
                break;
            case 1: //Score
                printf("SCORE\n");
                break;
            default:
                done = true;
                break;
        }
    }

    /* Cerrar recursos */
    al_destroy_display(ventana);
    al_destroy_event_queue(event_queue);
    al_destroy_font(roboto);
    al_destroy_timer(timer);

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
            pnj->position.fil = 1 + pnj->box.upBox;
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
            pnj->position.fil = (lado*Nfil) - pnj->box.bottomBox-1;
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
            pnj->position.col = 1 + pnj->box.leftBox;
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
            pnj->position.col = (lado*Ncol) - pnj->box.rightBox - 1;
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

hielo power(int board[Nfil][Ncol], personaje pnj)
{
    int i,j, startPlace=0, affectedSqares=0;
    int VOID_SQUARE = 0, START_SQUARE_COLOR = 2, END_SQUARE_COLOR = 5, PAST_COLOR = VOID_SQUARE, NEW_COLOR = START_SQUARE_COLOR;

    hielo ice;
    ice.create=1;

    if(pnj.direction == 'D')
    {
        j=pnj.boardPlace.col;

        startPlace = defineSquare(pnj.position.fil + pnj.box.bottomBox , pnj.position.col).fil+1;
        ice.begin.fil = startPlace;
        ice.begin.col = j;

        if(board[startPlace][j] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(i=startPlace; i<Nfil; i++)
            if(board[i][j] == PAST_COLOR)
            {
                board[i][j] = NEW_COLOR;
                affectedSqares++;
            }
            else
                break;
    }
    else if(pnj.direction == 'U')
    {
        j=pnj.boardPlace.col;

        startPlace = defineSquare(pnj.position.fil - pnj.box.upBox , pnj.position.col).fil-1;
        ice.begin.fil = startPlace;
        ice.begin.col = j;

        if(board[startPlace][j] == 5)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(i=startPlace; i>=0; i--)
            if(board[i][j] == PAST_COLOR)
            {
                board[i][j] = NEW_COLOR;
                affectedSqares++;
            }
            else
                break;
    }
    else if(pnj.direction == 'R')
    {
        i=pnj.boardPlace.fil;

        startPlace = defineSquare(pnj.position.fil , pnj.position.col + pnj.box.rightBox).col+1;
        ice.begin.fil = i;
        ice.begin.col = startPlace;

        if(board[i][startPlace] == 5)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(j=startPlace; j<Ncol; j++)
            if(board[i][j] == PAST_COLOR)
            {
                board[i][j] = NEW_COLOR;
                affectedSqares++;
            }
            else
                break;
    }
    else if(pnj.direction == 'L')
    {
        i=pnj.boardPlace.fil;

        startPlace = defineSquare(pnj.position.fil , pnj.position.col - pnj.box.leftBox).col-1;
        ice.begin.fil = i;
        ice.begin.col = startPlace;

        if(board[i][startPlace] == 5)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(j=startPlace; j>=0; j--)
            if(board[i][j] == PAST_COLOR)
            {
                board[i][j] = NEW_COLOR;
                affectedSqares++;
            }
            else
                break;
    }

    if(affectedSqares != 0)
        ice.possible = 0;

    ice.limit = affectedSqares;
    ice.direction = pnj.direction;
    ice.restantes = ice.limit;
    return ice;
}

void manageIce(int board[Nfil][Ncol], hielo *ice)
{
    if(ice->limit > 2)
    {
        if(ice->create)
        {
            if(ice->restantes>=3)
            {
                if(board[ice->begin.fil][ice->begin.col] == 2)
                    board[ice->begin.fil][ice->begin.col]=3;
                else if(board[ice->begin.fil][ice->begin.col] == 3)
                {
                    board[ice->begin.fil][ice->begin.col]=4;
                        switch (ice->direction)
                        {
                            case 'U':
                                board[ice->begin.fil-1][ice->begin.col]=3;
                                break;
                            case 'D':
                                board[ice->begin.fil+1][ice->begin.col]=3;
                                break;
                            case 'L':
                                board[ice->begin.fil][ice->begin.col-1]=3;
                                break;
                            case 'R':
                                board[ice->begin.fil][ice->begin.col+1]=3;
                                break;
                        }
                }
                else if(board[ice->begin.fil][ice->begin.col] == 4)
                {
                    board[ice->begin.fil][ice->begin.col]=5;
                    ice->restantes--;
                        switch (ice->direction)
                        {
                            case 'U':
                                board[ice->begin.fil-1][ice->begin.col]=4;
                                board[ice->begin.fil-2][ice->begin.col]=3;
                                ice->begin.fil--;
                                break;
                            case 'D':
                                board[ice->begin.fil+1][ice->begin.col]=4;
                                board[ice->begin.fil+2][ice->begin.col]=3;
                                ice->begin.fil++;
                                break;
                            case 'L':
                                board[ice->begin.fil][ice->begin.col-1]=4;
                                board[ice->begin.fil][ice->begin.col-2]=3;
                                ice->begin.col--;
                                break;
                            case 'R':
                                board[ice->begin.fil][ice->begin.col+1]=4;
                                board[ice->begin.fil][ice->begin.col+2]=3;
                                ice->begin.col++;
                                break;
                        }
                }
            }
            else if(ice->restantes==2)
            {
                board[ice->begin.fil][ice->begin.col]=5;
                switch (ice->direction)
                {
                    case 'U':
                        board[ice->begin.fil-1][ice->begin.col]=4;
                        ice->begin.fil--;
                        break;
                    case 'D':
                        board[ice->begin.fil+1][ice->begin.col]=4;
                        ice->begin.fil++;
                        break;
                    case 'L':
                        board[ice->begin.fil][ice->begin.col-1]=4;
                        ice->begin.col--;
                        break;
                    case 'R':
                        board[ice->begin.fil][ice->begin.col+1]=4;
                        ice->begin.col++;
                        break;
                }
                ice->restantes--;
            }
            else if(ice->restantes==1)
            {
                board[ice->begin.fil][ice->begin.col]=5;
                ice->restantes--;
            }
            else if(ice->restantes == 0)
            {
                ice->possible = 1;
            }
        }
        else if(!ice->create)
        {
            if(ice->restantes>=3)
            {
                if(board[ice->begin.fil][ice->begin.col] == 5)
                    board[ice->begin.fil][ice->begin.col]=4;
                else if(board[ice->begin.fil][ice->begin.col] == 4)
                {
                    board[ice->begin.fil][ice->begin.col]=3;
                        switch (ice->direction)
                        {
                            case 'U':
                                board[ice->begin.fil-1][ice->begin.col]=4;
                                break;
                            case 'D':
                                board[ice->begin.fil+1][ice->begin.col]=4;
                                break;
                            case 'L':
                                board[ice->begin.fil][ice->begin.col-1]=4;
                                break;
                            case 'R':
                                board[ice->begin.fil][ice->begin.col+1]=4;
                                break;
                        }
                }
                else if(board[ice->begin.fil][ice->begin.col] == 3)
                {
                    board[ice->begin.fil][ice->begin.col]=0;
                    ice->restantes--;
                        switch (ice->direction)
                        {
                            case 'U':
                                board[ice->begin.fil-1][ice->begin.col]=3;
                                board[ice->begin.fil-2][ice->begin.col]=4;
                                ice->begin.fil--;
                                break;
                            case 'D':
                                board[ice->begin.fil+1][ice->begin.col]=3;
                                board[ice->begin.fil+2][ice->begin.col]=4;
                                ice->begin.fil++;
                                break;
                            case 'L':
                                board[ice->begin.fil][ice->begin.col-1]=3;
                                board[ice->begin.fil][ice->begin.col-2]=4;
                                ice->begin.col--;
                                break;
                            case 'R':
                                board[ice->begin.fil][ice->begin.col+1]=3;
                                board[ice->begin.fil][ice->begin.col+2]=4;
                                ice->begin.col++;
                                break;
                        }
                }
            }
            else if(ice->restantes==2)
            {
                board[ice->begin.fil][ice->begin.col]=0;
                switch (ice->direction)
                {
                    case 'U':
                        board[ice->begin.fil-1][ice->begin.col]=3;
                        ice->begin.fil--;
                        break;
                    case 'D':
                        board[ice->begin.fil+1][ice->begin.col]=3;
                        ice->begin.fil++;
                        break;
                    case 'L':
                        board[ice->begin.fil][ice->begin.col-1]=3;
                        ice->begin.col--;
                        break;
                    case 'R':
                        board[ice->begin.fil][ice->begin.col+1]=3;
                        ice->begin.col++;
                        break;
                }
                ice->restantes--;
            }
            else if(ice->restantes==1)
            {
                board[ice->begin.fil][ice->begin.col]=0;
                ice->restantes--;
            }
            if(ice->restantes == 0)
            {
                ice->possible = 1;
            }
        }
    }
    else if(ice->limit == 2)
    {
        if(ice->create)
        {
            if(ice->restantes==2)
            {
                if(board[ice->begin.fil][ice->begin.col] == 2)
                    board[ice->begin.fil][ice->begin.col]=3;
                else if(board[ice->begin.fil][ice->begin.col] == 3)
                {
                    board[ice->begin.fil][ice->begin.col]=4;
                    switch (ice->direction)
                    {
                        case 'U':
                            board[ice->begin.fil-1][ice->begin.col]=3;
                            break;
                        case 'D':
                            board[ice->begin.fil+1][ice->begin.col]=3;
                            break;
                        case 'L':
                            board[ice->begin.fil][ice->begin.col-1]=3;
                            break;
                        case 'R':
                            board[ice->begin.fil][ice->begin.col+1]=3;
                            break;
                    }
                }
                else if(board[ice->begin.fil][ice->begin.col] == 4)
                {
                    board[ice->begin.fil][ice->begin.col]=5;
                    switch (ice->direction)
                    {
                        case 'U':
                            board[ice->begin.fil-1][ice->begin.col]=4;
                            ice->begin.fil--;
                            break;
                        case 'D':
                            board[ice->begin.fil+1][ice->begin.col]=4;
                            ice->begin.fil++;
                            break;
                        case 'L':
                            board[ice->begin.fil][ice->begin.col-1]=4;
                            ice->begin.col--;
                            break;
                        case 'R':
                            board[ice->begin.fil][ice->begin.col+1]=4;
                            ice->begin.col++;
                            break;
                    }
                    ice->restantes--;
                }
            }
            else if(ice->restantes==1)
            {
                board[ice->begin.fil][ice->begin.col]=5;
                ice->restantes--;
            }
            else if(ice->restantes == 0)
            {
                ice->possible = 1;
            }
        }
        else if(!ice->create)
        {
            if(ice->restantes==2)
            {
                if(board[ice->begin.fil][ice->begin.col] == 5)
                    board[ice->begin.fil][ice->begin.col]=4;
                else if(board[ice->begin.fil][ice->begin.col] == 4)
                {
                    board[ice->begin.fil][ice->begin.col]=3;
                    switch (ice->direction)
                    {
                        case 'U':
                            board[ice->begin.fil-1][ice->begin.col]=4;
                            break;
                        case 'D':
                            board[ice->begin.fil+1][ice->begin.col]=4;
                            break;
                        case 'L':
                            board[ice->begin.fil][ice->begin.col-1]=4;
                            break;
                        case 'R':
                            board[ice->begin.fil][ice->begin.col+1]=4;
                            break;
                    }
                }
                else if(board[ice->begin.fil][ice->begin.col] == 3)
                {
                    board[ice->begin.fil][ice->begin.col]=0;
                    switch (ice->direction)
                    {
                        case 'U':
                            board[ice->begin.fil-1][ice->begin.col]=3;
                            ice->begin.fil--;
                            break;
                        case 'D':
                            board[ice->begin.fil+1][ice->begin.col]=3;
                            ice->begin.fil++;
                            break;
                        case 'L':
                            board[ice->begin.fil][ice->begin.col-1]=3;
                            ice->begin.col--;
                            break;
                        case 'R':
                            board[ice->begin.fil][ice->begin.col+1]=3;
                            ice->begin.col++;
                            break;
                    }
                    ice->restantes--;
                }
            }
            else if(ice->restantes==1)
            {
                board[ice->begin.fil][ice->begin.col]=0;
                ice->restantes--;
            }
            else if(ice->restantes == 0)
            {
                ice->possible = 1;
            }
        }
    }
    else if(ice->limit == 1)
    {
        if(ice->create)
        {
            if(board[ice->begin.fil][ice->begin.col] == 2)
                board[ice->begin.fil][ice->begin.col]=3;
            else if(board[ice->begin.fil][ice->begin.col] == 3)
                board[ice->begin.fil][ice->begin.col]=4;
            else if(board[ice->begin.fil][ice->begin.col] == 4)
            {
                board[ice->begin.fil][ice->begin.col]=5;
                ice->restantes--;
            }
        }
        if(!ice->create)
        {
            if(board[ice->begin.fil][ice->begin.col] == 5)
                board[ice->begin.fil][ice->begin.col]=4;
            else if(board[ice->begin.fil][ice->begin.col] == 4)
                board[ice->begin.fil][ice->begin.col]=3;
            else if(board[ice->begin.fil][ice->begin.col] == 3)
            {
                board[ice->begin.fil][ice->begin.col]=0;
                ice->restantes--;
            }
        }
        if(ice->restantes == 0)
        {
            ice->possible = 1;
        }
    }
    else if(ice->limit == 0)
    {
        ice->possible = 1;
    }
    return;
}

int getBoard(int board[Nfil][Ncol], char numero[3])
{
    int i,j;
    char filename[12] = "level";
    strcat(filename, numero);
    strcat(filename, ".txt");

    printf("%s\n", filename);

    FILE *game;

    if ((game = fopen(filename, "r"))==NULL)
        {
            printf("Error al abrir archivo\n");
            return 1;
        }
    else
        printf("Archivo abierto\n");

    for(i=0; i<Nfil; i++)
    for(j=0; j<Ncol; j++)
        fscanf(game,"%d",&board[i][j]);

    fclose(game);


    return 0;
}

// FUnciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * lado)+1, (fila * lado)+1, ((columna + 1) * lado)-1, ((fila + 1) * lado)-1, color);
}

void draw_pnj(personaje pbj){
    al_draw_filled_rectangle(pbj.position.col-pbj.box.leftBox, pbj.position.fil-pbj.box.upBox, pbj.position.col+pbj.box.rightBox, pbj.position.fil+pbj.box.bottomBox, color_purple1);

    switch (pbj.direction)
    {
    case 'U':
        al_draw_text(roboto,color_white,pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "U");
        break;
    case 'D':
        al_draw_text(roboto,color_white,pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "D");
        break;
    case 'L':
        al_draw_text(roboto,color_white,pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "L");
        break;
    case 'R':
        al_draw_text(roboto,color_white,pbj.position.col, pbj.position.fil-15, ALLEGRO_ALIGN_CENTRE, "R");
        break;
    }

    return;
}

void draw_background(ALLEGRO_BITMAP *bitmap)
{
    int i,j, num;
    float x,y; //Valores de x, y usados en partes del código
    char str[4];

    al_set_target_bitmap(bitmap);

    /* Nos aseguramos que el color del fondo sea negro */
    al_clear_to_color(color_black);

    /* Dibujo de cuadrícula */
    for (i = 0; i <= Ncol; i++)
        al_draw_line(i * lado, 0, i * lado, Nfil * lado, color_blue, 2);

    for (j = 0; j <= Nfil; j++)
        al_draw_line(0, j * lado, Ncol * lado, j * lado, color_blue, 2);

    /* Dibujo de texto */
    num = 1;
    for (i = 0; i < Nfil; i++) {
        for (j = 0; j < Ncol; j++) {
            sprintf(str, "%d", num); // Genera el número en formato cadena de texto
            x = (j * lado) + (lado / 2);
            y = (i * lado) + (lado / 2) - (font_size / 2);
            al_draw_text(roboto, color_white, x, y, ALLEGRO_ALIGN_CENTRE, str);
            num++;
        }
    }

    // Seleccionamos el Display como el Backbuffer, ya no el birmap del fondo
    al_set_target_backbuffer(al_get_current_display());
    return;
}

void draw_board(int board[Nfil][Ncol]){
    int i,j;

    for(i=0; i<Nfil; i++)
    {
        for(j=0; j<Ncol; j++)
        {
            switch (board[i][j])
            {
                case 1:
                    draw_boardRectangle(i, j, color_blue);
                    break;
                //case 2:
                    //draw_boardRectangle(i, j, color_green1);
                    //break;
                case 3:
                    draw_boardRectangle(i, j, color_green2);
                    break;
                case 4:
                    draw_boardRectangle(i, j, color_green3);
                    break;
                case 5:
                    draw_boardRectangle(i, j, color_green4);
                    break;
            }
        }
    }
}

/*Partes*/
int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    bool done = false;
    ALLEGRO_BITMAP *jugar_bmap = al_load_bitmap("src/mainMenu/900x600/jugar.png");
    ALLEGRO_BITMAP *score_bmap = al_load_bitmap("src/mainMenu/900x600/score.png");
    ALLEGRO_BITMAP *salir_bmap = al_load_bitmap("src/mainMenu/900x600/salir.png");

    int actualImage = 0; //Jugar, Score, Salir
    al_draw_bitmap(jugar_bmap,0,0,0);
    al_flip_display();

    while (!done)
    {
        al_wait_for_event(queue, ev); /*Esperando a que ocurra un evento*/

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /*Si es un cierre de la ventana*/
        {
            return 2;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_TAB || ev->keyboard.keycode == ALLEGRO_KEY_DOWN || ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)
            {
                actualImage = (actualImage+1)%3;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_UP || ev->keyboard.keycode == ALLEGRO_KEY_LEFT)
            {
                actualImage = (actualImage-1)%3;
                if(actualImage == -1)
                    actualImage = 2;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                return actualImage;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                return 2;
            }
        }

        switch (actualImage)
        {
            case 0:
                al_draw_bitmap(jugar_bmap,0,0,0);
                break;
            case 1:
                al_draw_bitmap(score_bmap,0,0,0);
                break;
            case 2:
                al_draw_bitmap(salir_bmap,0,0,0);
                break;
        }
        al_flip_display();
    }



    al_destroy_bitmap(jugar_bmap);
    al_destroy_bitmap(score_bmap);
    al_destroy_bitmap(salir_bmap);
}

int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    bool done = false;
    int actualImage = 0; //Continuar, Salir
    ALLEGRO_BITMAP *continuar_bmp = al_load_bitmap("src/pauseMenu/900x600/continuar.png");
    ALLEGRO_BITMAP *salir_bmap = al_load_bitmap("src/pauseMenu/900x600/salir.png");

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(Ncol*lado, Nfil*lado);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(ventana), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    al_draw_bitmap(screenshot,0,0,0);
    al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
    al_draw_bitmap(continuar_bmp,0,0,0);
    al_flip_display();

    while (!done)
    {
        al_wait_for_event(queue, ev); /*Esperando a que ocurra un evento*/

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /*Si es un cierre de la ventana*/
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_TAB || ev->keyboard.keycode == ALLEGRO_KEY_DOWN || ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)
            {
                actualImage = (actualImage+1)%2;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_UP || ev->keyboard.keycode == ALLEGRO_KEY_LEFT)
            {
                actualImage = (actualImage-1)%2;
                if(actualImage == -1)
                    actualImage = 1;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                al_resume_timer(timer);
                return actualImage;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                al_resume_timer(timer);
                return 0;
            }
        }

        switch (actualImage)
        {
            case 0:
                al_draw_bitmap(screenshot,0,0,0);
                al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
                al_draw_bitmap(continuar_bmp,0,0,0);
                break;
            case 1:
                al_draw_bitmap(screenshot,0,0,0);
                al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
                al_draw_bitmap(salir_bmap,0,0,0);
                break;
        }
        al_flip_display();
    }



    al_destroy_bitmap(continuar_bmp);
    al_destroy_bitmap(salir_bmap);
}

int game(int board[Nfil][Ncol], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    bool done = false;
    int i, j;
    getBoard(board, "01");
    for(i=0; i<Nfil; i++)
    for(j=0; j<Ncol; j++)
    {
        if(board[i][j]==1)
            {
                player1.boardPlace.fil=i;
                player1.boardPlace.col=j;
            }
    }
    // Ubicando jugador
    player1.position.fil=(lado/2)+(player1.boardPlace.fil*lado);
    player1.position.col=(lado/2)+(player1.boardPlace.col*lado);

    /*Variables utiles*/
    hielo ice; //Casillas a cambiar de color por Power
    ice.possible=1;

    /* Crear bitmap para el fondo del tablero */
    board_bitmap = al_create_bitmap(Ncol * lado, Nfil * lado);
    draw_background(board_bitmap);

    /*Inicia temporizador*/
    al_start_timer(timer);

    while (!done)
    {
        al_wait_for_event(queue, ev); /*Esperando a que ocurra un evento*/

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /*Si es un cierre de la ventana*/
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                for(i=0; i<ALLEGRO_KEY_MAX; i++) //Se "levantan" todas las teclas
                    keys[i] = false;
                switch (pause_menu(queue, ev, timer))
                {
                    case -1:
                        return -1;
                        break;
                    case 1:
                        return 1;
                        break;
                }
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_SPACE)
            {
                if(ice.possible)
                    ice = power(board, player1);
            }
            else
            {
                keys[ev->keyboard.keycode] = true;
            }
        }
        else if (ev->type == ALLEGRO_EVENT_KEY_UP)
        {
            keys[ev->keyboard.keycode] = false;
        }
        else if (ev->type == ALLEGRO_EVENT_TIMER)
        {
            /*Limpiamos el Backbuffer*/
            al_clear_to_color(color_blue);

            if (keys[ALLEGRO_KEY_UP])
            {
                moveTo(board, &player1, player1.position.fil-(5*player1.velocity), player1.position.col);
            }
            else if (keys[ALLEGRO_KEY_DOWN])
            {
                moveTo(board, &player1, player1.position.fil+(5*player1.velocity), player1.position.col);
            }
            else if (keys[ALLEGRO_KEY_LEFT])
            {
                moveTo(board, &player1, player1.position.fil, player1.position.col-(5*player1.velocity));
            }
            else if (keys[ALLEGRO_KEY_RIGHT])
            {
                moveTo(board, &player1, player1.position.fil, player1.position.col+(5*player1.velocity));
            }

            if((!ice.possible) && ((al_get_timer_count(timer)%2) == 0)) //Este if se ejecuta cada 2 ticks sólo si NO es posible crear hielo, es decir, si hay hielo pendiente por generar
                manageIce(board,&ice);

            /*Dibujamos el fondo en el backbuffer*/
            al_draw_bitmap(board_bitmap,0,0,0);

            /* Dibujar el tablero con colores en el backbuffer*/
            draw_board(board);

            //Dibujando al jugador
            draw_pnj(player1);

            /* Actualizar pantalla */
            al_flip_display();
        }
    }
    return 0;
}