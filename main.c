#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h>
    #include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>

#define lado 64
#define windowNfil 10
#define windowNcol 15
#define windowWidth lado*windowNcol
#define windowheight lado*windowNfil
#define MAXFILS 30
#define MAXCOLS 30
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
    int row;
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
    bool movement;
    int spriteFil;
    int alcance;
    int powerType;
} player1;
typedef struct _personaje personaje;

struct _gameInfo
{
    int gameRows;
    int gameCols;
    square startSquare;
    square endSquare;
    int mapColStart;
    int mapFilStart;
} Game;
typedef struct _gameInfo gameInfo;

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
int moveTo(int board[MAXFILS][MAXCOLS], personaje *pnj, int newfil, int newcol);
square defineSquare(int filPixel, int colPixel);
hielo power(int board[MAXFILS][MAXCOLS], personaje pnj);
void manageIce(int board[MAXFILS][MAXCOLS], hielo *ice);
int getBoard(int board[MAXFILS][MAXCOLS], char numero[3]);

//Funciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_pnj(personaje *pnj, ALLEGRO_BITMAP *image);
void draw_background(ALLEGRO_BITMAP *bitmap);
void draw_board(int board[MAXFILS][MAXCOLS]);
/*Partes*/
int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int game(int board[MAXFILS][MAXCOLS], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);

/*Bitmaps*/
ALLEGRO_DISPLAY *ventana;
ALLEGRO_BITMAP *board_bitmap;
ALLEGRO_BITMAP *player_bitmap;
ALLEGRO_BITMAP *rockBitmap;
ALLEGRO_BITMAP *flowerBitmap;

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
    int board[MAXFILS][MAXCOLS];
    for(i=0; i<MAXFILS; i++)
    for(j=0; j<MAXCOLS; j++)
        board[i][j]=0;

    //Inicializando al jugador 1
    player1.direction='D';
    player1.box.leftBox=25;
    player1.box.rightBox=25;
    player1.box.upBox=25;
    player1.box.bottomBox=25;
    player1.velocity=1;
    player1.alcance=5;
    player1.powerType = 0;

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
    ventana = al_create_display(windowWidth , windowheight);
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
    al_destroy_bitmap(player_bitmap);
    al_destroy_bitmap(board_bitmap);
    return 0;
}

// funciones lógicas
int moveTo(int board[MAXFILS][MAXCOLS], personaje *pnj, int newfil, int newcol)
{
    //Definiendo direccion de personaje
    if((pnj->position.row) < newfil) //Mirando abajo
    {
        if(pnj->direction != 'D')
        {
            pnj->direction='D';
            return 0;
        }
    }
    else if((pnj->position.row) > newfil) //Mirando arriba
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
            pnj->position.row = 1 + pnj->box.upBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes superiors del personaje para generar colision.
            (board[defineSquare(topEdge , newcol).row][pnj->boardPlace.col]>40)
            || (board[defineSquare(topEdge , newcol).row][defineSquare(topEdge , newcol-pnj->box.leftBox).col]>40)
            || (board[defineSquare(topEdge , newcol).row][defineSquare(topEdge , newcol+pnj->box.rightBox).col]>40)
            )
        {
            pnj->position.row = (lado * (defineSquare(topEdge,newcol).row+1)) + pnj->box.upBox; //Se agrega un +1 al defineSquare porque necesitamos el borde inferior de la casilla de colision
        }
        else
            pnj->position.row = newfil;
    }
    else if(pnj->direction == 'D')
    {
        pnj->position.col=newcol;
        if(bottomEdge >= (lado*Game.gameRows))
            pnj->position.row = (lado*Game.gameRows) - pnj->box.bottomBox-1;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
            (board[defineSquare(bottomEdge , newcol).row][pnj->boardPlace.col]>40)
            || (board[defineSquare(bottomEdge , newcol).row][defineSquare(bottomEdge , newcol-pnj->box.leftBox).col]>40)
            || (board[defineSquare(bottomEdge , newcol).row][defineSquare(bottomEdge , newcol+pnj->box.rightBox).col]>40)
            )
        {
            pnj->position.row = (lado * (defineSquare(bottomEdge,newcol).row)) - pnj->box.bottomBox - 1;
        }
        else
            pnj->position.row = newfil;
    }
    else if(pnj->direction == 'L')
    {
        pnj->position.row=newfil;
        if(leftEdge < 0)
            pnj->position.col = 1 + pnj->box.leftBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes izquierdos del personaje para generar colision.
            (board[pnj->boardPlace.row][defineSquare(newfil , leftEdge).col]>40)
            || (board[defineSquare(newfil-pnj->box.upBox , leftEdge).row][defineSquare(newfil , leftEdge).col]>40)
            || (board[defineSquare(newfil+pnj->box.bottomBox , leftEdge).row][defineSquare(newfil , leftEdge).col]>40)
            )
        {
            pnj->position.col = (lado * (defineSquare(newfil , leftEdge).col+1)) + pnj->box.leftBox; //Se agrega un +1 al defineSquare porque necesitamos el borde derecho de la casilla de colision
        }
        else
            pnj->position.col = newcol;
    }
    else if(pnj->direction == 'R')
    {
        pnj->position.row=newfil;
        if(rightEdge >= (lado*Game.gameCols))
            pnj->position.col = (lado*Game.gameCols) - pnj->box.rightBox - 1;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes derechos del personaje para generar colision.
            (board[pnj->boardPlace.row][defineSquare(newfil , rightEdge).col]>40)
            || (board[defineSquare(newfil-pnj->box.upBox , rightEdge).row][defineSquare(newfil , rightEdge).col]>40)
            || (board[defineSquare(newfil+pnj->box.bottomBox , rightEdge).row][defineSquare(newfil , rightEdge).col]>40)
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
        board[pnj->boardPlace.row][pnj->boardPlace.col] = 0;
        board[pnj->position.row / lado][pnj->position.col / lado] = 1;
        pnj->boardPlace.row = pnj->position.row / lado;
        pnj->boardPlace.col = pnj->position.col / lado;
    }
    if((pnj->position.row / lado)!=pnj->boardPlace.row)
    {
        board[pnj->boardPlace.row][pnj->boardPlace.col] = 0;
        board[pnj->position.row / lado][pnj->position.col / lado] = 1;
        pnj->boardPlace.row = pnj->position.row / lado;
        pnj->boardPlace.col = pnj->position.col / lado;
    }

    return 0;
}

square defineSquare(int filPixel, int colPixel)
{
    square position;

    position.row = filPixel / lado;
    position.col = colPixel / lado;

    return position;
}

hielo power(int board[MAXFILS][MAXCOLS], personaje pnj)
{
    int i,j, startPlace=0, affectedSqares=0;
    int VOID_SQUARE, START_SQUARE_COLOR, END_SQUARE_COLOR, PAST_COLOR, NEW_COLOR;

    VOID_SQUARE = 0;
    START_SQUARE_COLOR = (pnj.powerType * 4) + 41;
    END_SQUARE_COLOR = START_SQUARE_COLOR + 3;
    PAST_COLOR = VOID_SQUARE;
    NEW_COLOR = START_SQUARE_COLOR;

    hielo ice;
    ice.create=1;

    if(pnj.direction == 'D')
    {
        j=pnj.boardPlace.col;

        startPlace = defineSquare(pnj.position.row + pnj.box.bottomBox , pnj.position.col).row+1;
        ice.begin.row = startPlace;
        ice.begin.col = j;

        if(board[startPlace][j] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(i=startPlace; i<Game.gameRows; i++)
            if(affectedSqares==pnj.alcance)
                break;
            else if(board[i][j] == PAST_COLOR)
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

        startPlace = defineSquare(pnj.position.row - pnj.box.upBox , pnj.position.col).row-1;
        ice.begin.row = startPlace;
        ice.begin.col = j;

        if(board[startPlace][j] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(i=startPlace; i>=0; i--)
        {
            if(affectedSqares==pnj.alcance)
                break;
            else if(board[i][j] == PAST_COLOR)
            {
                board[i][j] = NEW_COLOR;
                affectedSqares++;
            }
            else
                break;
        }
    }
    else if(pnj.direction == 'R')
    {
        i=pnj.boardPlace.row;

        startPlace = defineSquare(pnj.position.row , pnj.position.col + pnj.box.rightBox).col+1;
        ice.begin.row = i;
        ice.begin.col = startPlace;

        if(board[i][startPlace] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(j=startPlace; j<Game.gameCols; j++)
            if(affectedSqares==pnj.alcance)
                break;
            else if(board[i][j] == PAST_COLOR)
            {
                board[i][j] = NEW_COLOR;
                affectedSqares++;
            }
            else
                break;
    }
    else if(pnj.direction == 'L')
    {
        i=pnj.boardPlace.row;

        startPlace = defineSquare(pnj.position.row , pnj.position.col - pnj.box.leftBox).col-1;
        ice.begin.row = i;
        ice.begin.col = startPlace;

        if(board[i][startPlace] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(j=startPlace; j>=0; j--)
            if(affectedSqares==pnj.alcance)
                break;
            else if(board[i][j] == PAST_COLOR)
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

void manageIce(int board[MAXFILS][MAXCOLS], hielo *ice)
{
    int NUM1=41, NUM2=42, NUM3=43, NUM4=44;

    if(ice->create)
    {
        NUM1=(player1.powerType * 4) + 41;
        NUM2=NUM1+1;
        NUM3=NUM2+1;
        NUM4=NUM3+1;
    }
    else if(!ice->create)
    {
        NUM1=(player1.powerType * 4) + 44;
        NUM2=NUM1-1;
        NUM3=NUM2-1;
        NUM4=0;
    }

    if(ice->limit > 2)
    {
        if(ice->restantes>=3)
        {
            if(board[ice->begin.row][ice->begin.col] == NUM1)
                board[ice->begin.row][ice->begin.col]=NUM2;
            else if(board[ice->begin.row][ice->begin.col] == NUM2)
            {
                board[ice->begin.row][ice->begin.col]=NUM3;
                    switch (ice->direction)
                    {
                        case 'U':
                            board[ice->begin.row-1][ice->begin.col]=NUM2;
                            break;
                        case 'D':
                            board[ice->begin.row+1][ice->begin.col]=NUM2;
                            break;
                        case 'L':
                            board[ice->begin.row][ice->begin.col-1]=NUM2;
                            break;
                        case 'R':
                            board[ice->begin.row][ice->begin.col+1]=NUM2;
                            break;
                    }
            }
            else if(board[ice->begin.row][ice->begin.col] == NUM3)
            {
                board[ice->begin.row][ice->begin.col]=NUM4;
                ice->restantes--;
                    switch (ice->direction)
                    {
                        case 'U':
                            board[ice->begin.row-1][ice->begin.col]=NUM3;
                            board[ice->begin.row-2][ice->begin.col]=NUM2;
                            ice->begin.row--;
                            break;
                        case 'D':
                            board[ice->begin.row+1][ice->begin.col]=NUM3;
                            board[ice->begin.row+2][ice->begin.col]=NUM2;
                            ice->begin.row++;
                            break;
                        case 'L':
                            board[ice->begin.row][ice->begin.col-1]=NUM3;
                            board[ice->begin.row][ice->begin.col-2]=NUM2;
                            ice->begin.col--;
                            break;
                        case 'R':
                            board[ice->begin.row][ice->begin.col+1]=NUM3;
                            board[ice->begin.row][ice->begin.col+2]=NUM2;
                            ice->begin.col++;
                            break;
                    }
            }
        }
        else if(ice->restantes==2)
        {
            board[ice->begin.row][ice->begin.col]=NUM4;
            switch (ice->direction)
            {
                case 'U':
                    board[ice->begin.row-1][ice->begin.col]=NUM3;
                    ice->begin.row--;
                    break;
                case 'D':
                    board[ice->begin.row+1][ice->begin.col]=NUM3;
                    ice->begin.row++;
                    break;
                case 'L':
                    board[ice->begin.row][ice->begin.col-1]=NUM3;
                    ice->begin.col--;
                    break;
                case 'R':
                    board[ice->begin.row][ice->begin.col+1]=NUM3;
                    ice->begin.col++;
                    break;
            }
            ice->restantes--;
        }
        else if(ice->restantes==1)
        {
            board[ice->begin.row][ice->begin.col]=NUM4;
            ice->restantes--;
        }
        else if(ice->restantes == 0)
        {
            ice->possible = 1;
        }
    }
    else if(ice->limit == 2)
    {
        if(ice->restantes==2)
        {
            if(board[ice->begin.row][ice->begin.col] == NUM1)
                board[ice->begin.row][ice->begin.col]=NUM2;
            else if(board[ice->begin.row][ice->begin.col] == NUM2)
            {
                board[ice->begin.row][ice->begin.col]=NUM3;
                switch (ice->direction)
                {
                    case 'U':
                        board[ice->begin.row-1][ice->begin.col]=NUM2;
                        break;
                    case 'D':
                        board[ice->begin.row+1][ice->begin.col]=NUM2;
                        break;
                    case 'L':
                        board[ice->begin.row][ice->begin.col-1]=NUM2;
                        break;
                    case 'R':
                        board[ice->begin.row][ice->begin.col+1]=NUM2;
                        break;
                }
            }
            else if(board[ice->begin.row][ice->begin.col] == NUM3)
            {
                board[ice->begin.row][ice->begin.col]=NUM4;
                switch (ice->direction)
                {
                    case 'U':
                        board[ice->begin.row-1][ice->begin.col]=NUM3;
                        ice->begin.row--;
                        break;
                    case 'D':
                        board[ice->begin.row+1][ice->begin.col]=NUM3;
                        ice->begin.row++;
                        break;
                    case 'L':
                        board[ice->begin.row][ice->begin.col-1]=NUM3;
                        ice->begin.col--;
                        break;
                    case 'R':
                        board[ice->begin.row][ice->begin.col+1]=NUM3;
                        ice->begin.col++;
                        break;
                }
                ice->restantes--;
            }
        }
        else if(ice->restantes==1)
        {
            board[ice->begin.row][ice->begin.col]=NUM4;
            ice->restantes--;
        }
        else if(ice->restantes == 0)
            {
                ice->possible = 1;
            }
    }
    else if(ice->limit == 1)
    {
        if(board[ice->begin.row][ice->begin.col] == NUM1)
            board[ice->begin.row][ice->begin.col]=NUM2;
        else if(board[ice->begin.row][ice->begin.col] == NUM2)
            board[ice->begin.row][ice->begin.col]=NUM3;
        else if(board[ice->begin.row][ice->begin.col] == NUM3)
        {
            board[ice->begin.row][ice->begin.col]=NUM4;
            ice->restantes--;
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

int getBoard(int board[MAXFILS][MAXCOLS], char numero[3])
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

    fscanf(game, "%d", &Game.gameRows); //Leyendo columnas
    fscanf(game, "%d", &Game.gameCols); //Leyendo filas

    for(i=0; i<Game.gameRows; i++)
    for(j=0; j<Game.gameCols; j++)
        fscanf(game,"%d", &board[i][j]);

    fclose(game);


    return 0;
}

// FUnciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * lado), (fila * lado), ((columna + 1) * lado), ((fila + 1) * lado), color);
}

void draw_pnj(personaje *pnj, ALLEGRO_BITMAP *image){
    int spriteWidht = al_get_bitmap_width(image)/4;
    int spriteHeight = al_get_bitmap_height(image)/4;
    int spritecol;

    switch (pnj->direction)
    {
    case 'D':
        spritecol = 0;
        break;
    case 'L':
        spritecol = 1;
        break;
    case 'R':
        spritecol = 2;
        break;
    case 'U':
        spritecol = 3;
        break;
    }
    if(al_get_timer_count(timer)%5 == 0)
    {
        if(pnj->movement)
            pnj->spriteFil = (pnj->spriteFil + 1)%4;
        else
            pnj->spriteFil = 0;
    }

    //al_draw_filled_rectangle(pnj->position.col-pnj->box.leftBox - Game.mapColStart, pnj->position.row-pnj->box.upBox  - Game.mapFilStart, pnj->position.col+pnj->box.rightBox -Game.mapColStart, pnj->position.row+pnj->box.bottomBox - Game.mapFilStart, color_purple1);

    al_draw_bitmap_region(image, (spriteWidht*pnj->spriteFil),(spriteHeight*spritecol),spriteWidht,spriteHeight, pnj->position.col-(spriteWidht/2) - Game.mapColStart , pnj->position.row-(spriteHeight/2) - Game.mapFilStart, 0);
    return;
}

void draw_background(ALLEGRO_BITMAP *bitmap)
{
    int i,j, frameCount;
    float x,y; //Valores de x, y usados en partes del código
    char str[4];
    ALLEGRO_BITMAP *grass = al_load_bitmap("./src/sprites/board/grass.png");

    frameCount = 0;

    al_set_target_bitmap(bitmap);

    /* Nos aseguramos que el color del fondo sea negro */
    al_clear_to_color(color_black);

    srand(time(0));
    for (i = 0; i < Game.gameRows; i++) {
        for (j = 0; j < Game.gameCols; j++) {
            al_draw_bitmap_region(grass, (frameCount*64), 0, 64, 64 , j*lado, i*lado, 0);
            frameCount = rand() % 4;
        }
    }

    // Seleccionamos el Display como el Backbuffer, ya no el birmap del fondo
    al_set_target_backbuffer(al_get_current_display());
    al_destroy_bitmap(grass);
    return;
}

void draw_board(int board[MAXFILS][MAXCOLS]){
    int i,j;
    rockBitmap = al_load_bitmap("./src/sprites/board/rock.png");
    flowerBitmap = al_load_bitmap("./src/sprites/board/flowers.png");
    int rockwidht = 64, rockheight = 64;

    //Definiendo comienzo en X
    if((player1.position.col + (windowWidth/2)) > (Game.gameCols*lado))
        Game.mapColStart = (Game.gameCols * lado) - windowWidth;
    else if ((player1.position.col - (windowWidth/2)) < 0)
        Game.mapColStart = 0;
    else
        Game.mapColStart = player1.position.col - (windowWidth/2);
    //Definiendo comienzo en Y
    if((player1.position.row + (windowheight/2)) > (Game.gameRows*lado))
        Game.mapFilStart = (Game.gameRows * lado) - windowheight;
    else if ((player1.position.row - (windowheight/2)) < 0)
        Game.mapFilStart = 0;
    else
        Game.mapFilStart = player1.position.row - (windowheight/2);
    al_draw_bitmap_region(board_bitmap,Game.mapColStart, Game.mapFilStart, windowWidth, windowheight, 0, 0, 0);

    Game.startSquare.col = Game.mapColStart / lado;
    Game.endSquare.col = (Game.mapColStart + windowWidth) / lado;
    Game.startSquare.row = Game.mapFilStart / lado;
    Game.endSquare.row = (Game.mapFilStart + windowheight) /lado;

    for(i=Game.startSquare.row; i<=(Game.startSquare.row + windowNfil); i++)
    {
        for(j=Game.startSquare.col; j<=(Game.startSquare.col + windowNcol); j++)
        {
            if(board[i][j]>=41 && board[i][j]<=44)
            {
                al_draw_bitmap_region(rockBitmap, (board[i][j]-42)*rockwidht, 0, rockwidht, rockheight, j*lado - Game.mapColStart, i*lado - Game.mapFilStart, 0 );
            }
            else if(board[i][j]==21)
            {
                al_draw_bitmap_region(flowerBitmap, 0, 0, rockwidht, rockheight, j*lado - Game.mapColStart, i*lado - Game.mapFilStart, 0 );
            }
        }
    }
    al_destroy_bitmap(rockBitmap);
    al_destroy_bitmap(flowerBitmap);
}

/*Partes*/
int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    bool done = false;
    ALLEGRO_BITMAP *jugar_bmap = al_load_bitmap("src/mainMenu/960x640/jugar.png");
    ALLEGRO_BITMAP *score_bmap = al_load_bitmap("src/mainMenu/960x640/score.png");
    ALLEGRO_BITMAP *salir_bmap = al_load_bitmap("src/mainMenu/960x640/salir.png");

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
    ALLEGRO_BITMAP *continuar_bmp = al_load_bitmap("src/pauseMenu/960x640/continuar.png");
    ALLEGRO_BITMAP *salir_bmap = al_load_bitmap("src/pauseMenu/960x640/salir.png");

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(windowNcol*lado, windowNfil*lado);
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

int game(int board[MAXFILS][MAXCOLS], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    bool done = false;
    int i, j;
    getBoard(board, "01");
    for(i=0; i<Game.gameRows; i++)
    for(j=0; j<Game.gameCols; j++)
    {
        if(board[i][j]==1)
            {
                player1.boardPlace.row=i;
                player1.boardPlace.col=j;
            }
    }
    // Ubicando jugador
    player1.position.row=(lado/2)+(player1.boardPlace.row*lado);
    player1.position.col=(lado/2)+(player1.boardPlace.col*lado);

    /*Variables utiles*/
    hielo ice; //Casillas a cambiar de color por Power
    ice.possible=1;

    /*Bitmap personaje*/
    player_bitmap = al_load_bitmap("./src/sprites/pnj/spritesheet.png");

    /* Crear bitmap para el fondo del tablero */
    board_bitmap = al_create_bitmap(Game.gameCols * lado, Game.gameRows * lado);
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
                player1.movement = true;
            }
        }
        else if (ev->type == ALLEGRO_EVENT_KEY_UP)
        {
            keys[ev->keyboard.keycode] = false;
            if(!(keys[ALLEGRO_KEY_UP] || keys[ALLEGRO_KEY_DOWN] || keys[ALLEGRO_KEY_LEFT]|| keys[ALLEGRO_KEY_RIGHT]))
                player1.movement = false;
        }
        else if (ev->type == ALLEGRO_EVENT_TIMER)
        {
            /*Limpiamos el Backbuffer*/
            al_clear_to_color(color_blue);

            if (keys[ALLEGRO_KEY_UP])
            {
                moveTo(board, &player1, player1.position.row-(5*player1.velocity), player1.position.col);
            }
            else if (keys[ALLEGRO_KEY_DOWN])
            {
                moveTo(board, &player1, player1.position.row+(5*player1.velocity), player1.position.col);
            }
            else if (keys[ALLEGRO_KEY_LEFT])
            {
                moveTo(board, &player1, player1.position.row, player1.position.col-(5*player1.velocity));
            }
            else if (keys[ALLEGRO_KEY_RIGHT])
            {
                moveTo(board, &player1, player1.position.row, player1.position.col+(5*player1.velocity));
            }

            if((!ice.possible) && ((al_get_timer_count(timer)%2) == 0)) //Este if se ejecuta cada 2 ticks sólo si NO es posible crear hielo, es decir, si hay hielo pendiente por generar
                manageIce(board,&ice);

            /* Dibujar el tablero con colores en el backbuffer*/
            draw_board(board);

            //Dibujando al jugador
            draw_pnj(&player1, player_bitmap);

            /* Actualizar pantalla */
            al_flip_display();
        }
    }
    return 0;
}