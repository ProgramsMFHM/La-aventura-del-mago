#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h>
    #include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <math.h>

#define lado 64
#define windowNfil 10
#define windowNcol 15
#define windowWidth 960
#define windowheight 640
#define MAXFILS 30
#define MAXCOLS 30
#define font_size 52
#define FPS 24
#define NORMAL_OBJECTS_TYPE 10
#define ENEMY_TYPES 3
#define objectTimerCount 4

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
    int spritecol;
    int alcance;
    int powerType;
} player1;
typedef struct _personaje personaje;

struct _enemigo {
    square position;
    square boardPlace;
    square colisionSquare;
    char direction; // U, D, L, R
    char Olddirection; // U, D, L, R
    hitBox box;
    int velocity;
    int type;
    int state;
    bool movement;
    ALLEGRO_BITMAP *sprite;
    char spriteName[40];
    int spritecol;
    int numSpriteFrames; //Cambian por estados
    int alcance;
    int powerType;
    int powerCount;
};
typedef struct _enemigo enemigo;

struct _object{
    square position;
    int type;
    int state;
    int cont;
};
typedef struct _object object;

struct _gameInfo
{
    int gameRows;
    int gameCols;
    square startSquare;
    square endSquare;
    int mapColStart;
    int mapRowStart;
    int score;

    object *normalObjects;
    int numNormalObjects[NORMAL_OBJECTS_TYPE];
    int totalNormalObjects;
    int playingNormalObjectType;
    int MAXNormalObjectType;

    enemigo *enemies;
    int numEnemies[ENEMY_TYPES];
    int totalEnemies;

} Game;
typedef struct _gameInfo gameInfo;


/*Colores*/
ALLEGRO_COLOR color_black;
ALLEGRO_COLOR color_white;
ALLEGRO_COLOR color_gray;
ALLEGRO_COLOR color_blue;
ALLEGRO_COLOR color_purple1;
ALLEGRO_COLOR color_green1;
ALLEGRO_COLOR color_green2;
ALLEGRO_COLOR color_green3;
ALLEGRO_COLOR color_green4;

//Funciones lógicas
int movePlayer(int board[MAXFILS][MAXCOLS], personaje *pnj, char direction);
int moveEnemy(int board[MAXFILS][MAXCOLS], enemigo *enemy);
square defineSquare(int filPixel, int colPixel);
hielo power(int board[MAXFILS][MAXCOLS], personaje pnj);
void manageIce(int board[MAXFILS][MAXCOLS], hielo *ice);
int manageEnemy(int board[MAXFILS][MAXCOLS], enemigo *enemy);
int getBoard(int board[MAXFILS][MAXCOLS], char numero[3]);
int objectCollision(int board[MAXFILS][MAXCOLS], square colisionsquare);
bool enemyCollision(enemigo enemy, personaje pnj);
bool manageObjects(int board[MAXFILS][MAXCOLS]);
char doomieMovement(enemigo enemy);
char toPnjMovement(int board[MAXFILS][MAXCOLS], enemigo enemy , personaje pnj);
char bestToPnjMovement(int board[MAXFILS][MAXCOLS], enemigo enemy ,personaje pnj);
int count_files_in_directory(const char *path);

//Funciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_pnj(personaje *pnj, ALLEGRO_BITMAP *image);
void draw_enemy(enemigo *enemy);
void draw_background(ALLEGRO_BITMAP *bitmap);
void draw_board(int board[MAXFILS][MAXCOLS]);

/*Partes*/
int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int game_Over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int win_mwnu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int level_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int game(int board[MAXFILS][MAXCOLS], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer, int level);

/*Bitmaps*/
ALLEGRO_DISPLAY *ventana;
ALLEGRO_BITMAP *board_bitmap;
ALLEGRO_BITMAP *player_bitmap;
ALLEGRO_BITMAP *rockBitmap;
ALLEGRO_BITMAP *flowerBitmap;

ALLEGRO_FONT *font; /*Fuente*/
ALLEGRO_TIMER *timer; /*Timer*/

/*Other variables*/
bool keys[ALLEGRO_KEY_MAX] = { false }; /* Inicializa todas las teclas como no presionadas. */

int main()
{
    int i, j, num=1, level;

    // Inicializción colores
    color_black = al_map_rgb(0,0,0);
    color_white = al_map_rgb(255,255,255);
    color_gray = al_map_rgb(150,150,150);
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
    player1.velocity=7;
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
    font = al_load_ttf_font("./src/fonts/spooky_magic/SpookyMagic.ttf", font_size, 0);

    // Inicializando ventana
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
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
                level = level_menu(event_queue,&ev,timer);
                if(level>0)
                {
                    if(game(board, event_queue, &ev, timer, level) == -1)
                        done =true;
                    free(Game.normalObjects);
                    free(Game.enemies);
                }
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
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_bitmap(player_bitmap);
    al_destroy_bitmap(board_bitmap);
    return 0;
}

// funciones lógicas
int movePlayer(int board[MAXFILS][MAXCOLS], personaje *pnj, char direction)
{
    square colisionSquare;
    int mewrow, newcol, topEdge, bottomEdge, leftEdge, rightEdge;

    //Definiendo direccion de personaje
    pnj->direction = direction;

    // Definiendo nuevas posiciones
    switch (direction)
    {
    case 'U':
        mewrow = pnj->position.row - pnj->velocity;
        newcol = pnj->position.col;
        break;
    case 'D':
        mewrow = pnj->position.row + pnj->velocity;
        newcol = pnj->position.col;
        break;
    case 'L':
        mewrow = pnj->position.row;
        newcol = pnj->position.col - pnj->velocity;
        break;
    case 'R':
        mewrow = pnj->position.row;
        newcol = pnj->position.col + pnj->velocity;
        break;
    }

    //Definimos los nuevos bordes de la hitbox
    topEdge = mewrow - pnj->box.upBox;
    bottomEdge = mewrow + pnj->box.bottomBox;
    leftEdge = newcol - pnj->box.leftBox;
    rightEdge = newcol + pnj->box.rightBox;

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
            pnj->position.row = mewrow;
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
            pnj->position.row = mewrow;
    }
    else if(pnj->direction == 'L')
    {
        pnj->position.row=mewrow;
        if(leftEdge < 0)
            pnj->position.col = 1 + pnj->box.leftBox;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes izquierdos del personaje para generar colision.
            (board[pnj->boardPlace.row][defineSquare(mewrow , leftEdge).col]>40)
            || (board[defineSquare(mewrow-pnj->box.upBox , leftEdge).row][defineSquare(mewrow , leftEdge).col]>40)
            || (board[defineSquare(mewrow+pnj->box.bottomBox , leftEdge).row][defineSquare(mewrow , leftEdge).col]>40)
            )
        {
            pnj->position.col = (lado * (defineSquare(mewrow , leftEdge).col+1)) + pnj->box.leftBox; //Se agrega un +1 al defineSquare porque necesitamos el borde derecho de la casilla de colision
        }
        else
            pnj->position.col = newcol;
    }
    else if(pnj->direction == 'R')
    {
        pnj->position.row=mewrow;
        if(rightEdge >= (lado*Game.gameCols))
            pnj->position.col = (lado*Game.gameCols) - pnj->box.rightBox - 1;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes derechos del personaje para generar colision.
            (board[pnj->boardPlace.row][defineSquare(mewrow , rightEdge).col]>40)
            || (board[defineSquare(mewrow-pnj->box.upBox , rightEdge).row][defineSquare(mewrow , rightEdge).col]>40)
            || (board[defineSquare(mewrow+pnj->box.bottomBox , rightEdge).row][defineSquare(mewrow , rightEdge).col]>40)
            )
        {
            pnj->position.col = (lado * (defineSquare(mewrow , rightEdge).col)) - pnj->box.rightBox - 1;
        }
        else
            pnj->position.col = newcol;
    }


    // Alterando la posición del jugador en la matriz
    if((pnj->position.col / lado)!=pnj->boardPlace.col)
    {
        if(board[pnj->position.row / lado][pnj->position.col / lado]!=0)
        {
            colisionSquare.row = pnj->position.row / lado;
            colisionSquare.col = pnj->position.col / lado;
            if(objectCollision(board,colisionSquare) == 1)
                return 1;
        }
        board[pnj->boardPlace.row][pnj->boardPlace.col] = 0;
        board[pnj->position.row / lado][pnj->position.col / lado] = 1;
        pnj->boardPlace.row = pnj->position.row / lado;
        pnj->boardPlace.col = pnj->position.col / lado;
    }
    if((pnj->position.row / lado)!=pnj->boardPlace.row)
    {
        if(board[pnj->position.row / lado][pnj->position.col / lado]!=0)
        {
            colisionSquare.row = pnj->position.row / lado;
            colisionSquare.col = pnj->position.col / lado;
            if(objectCollision(board,colisionSquare) == 1)
                return 1;
        }
        board[pnj->boardPlace.row][pnj->boardPlace.col] = 0;
        board[pnj->position.row / lado][pnj->position.col / lado] = 1;
        pnj->boardPlace.row = pnj->position.row / lado;
        pnj->boardPlace.col = pnj->position.col / lado;
    }

    return 0;
}

int moveEnemy(int board[MAXFILS][MAXCOLS], enemigo *enemy)
{
    int mewrow, newcol, topEdge, bottomEdge, leftEdge, rightEdge;
    square colisionSquare;

    // Definiendo nuevas posiciones
    switch (enemy->direction)
    {
    case 'U':
        mewrow = enemy->position.row - enemy->velocity;
        newcol = enemy->position.col;
        break;
    case 'D':
        mewrow = enemy->position.row + enemy->velocity;
        newcol = enemy->position.col;
        break;
    case 'L':
        mewrow = enemy->position.row;
        newcol = enemy->position.col - enemy->velocity;
        break;
    case 'R':
        mewrow = enemy->position.row;
        newcol = enemy->position.col + enemy->velocity;
        break;
    }

    //Definimos los nuevos bordes de la hitbox
    topEdge = mewrow - enemy->box.upBox;
    bottomEdge = mewrow + enemy->box.bottomBox;
    leftEdge = newcol - enemy->box.leftBox;
    rightEdge = newcol + enemy->box.rightBox;

    if(enemy->direction == 'U')
    {
        enemy->position.col=newcol;
        if(topEdge<0)
        {
            enemy->position.row = 1 + enemy->box.upBox;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes superiors del personaje para generar colision.
        else if(board[defineSquare(topEdge , newcol).row][enemy->boardPlace.col]>40)
        {
            enemy->position.row = (lado * (defineSquare(topEdge,newcol).row+1)) + enemy->box.upBox; //Se agrega un +1 al defineSquare porque necesitamos el borde inferior de la casilla de colision
            enemy->colisionSquare.row = defineSquare(topEdge , newcol).row;
            enemy->colisionSquare.col = enemy->boardPlace.col;
            return 1;
        }
        else if(board[defineSquare(topEdge , newcol).row][defineSquare(topEdge , newcol-enemy->box.leftBox).col]>40)
        {
            enemy->position.row = (lado * (defineSquare(topEdge,newcol).row+1)) + enemy->box.upBox; //Se agrega un +1 al defineSquare porque necesitamos el borde inferior de la casilla de colision
            enemy->colisionSquare.row = defineSquare(topEdge , newcol).row;
            enemy->colisionSquare.col = defineSquare(topEdge , newcol-enemy->box.leftBox).col;
            return 1;
        }
        else if(board[defineSquare(topEdge , newcol).row][defineSquare(topEdge , newcol+enemy->box.rightBox).col]>40)
        {
            enemy->position.row = (lado * (defineSquare(topEdge,newcol).row+1)) + enemy->box.upBox; //Se agrega un +1 al defineSquare porque necesitamos el borde inferior de la casilla de colision
            enemy->colisionSquare.row = defineSquare(topEdge , newcol).row;
            enemy->colisionSquare.col = defineSquare(topEdge , newcol+enemy->box.rightBox).col;
            return 1;
        }
        else
            enemy->position.row = mewrow;
    }
    else if(enemy->direction == 'D')
    {
        enemy->position.col=newcol;
        if(bottomEdge >= (lado*Game.gameRows))
        {
            enemy->position.row = (lado*Game.gameRows) - enemy->box.bottomBox-1;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
        else if(board[defineSquare(bottomEdge , newcol).row][enemy->boardPlace.col]>40)
        {
            enemy->position.row = (lado * (defineSquare(bottomEdge,newcol).row)) - enemy->box.bottomBox - 1;
            enemy->colisionSquare.row = defineSquare(bottomEdge , newcol).row;
            enemy->colisionSquare.col = enemy->boardPlace.col;
            return 1;
        }
        else if(board[defineSquare(bottomEdge , newcol).row][defineSquare(bottomEdge , newcol-enemy->box.leftBox).col]>40)
        {
            enemy->position.row = (lado * (defineSquare(bottomEdge,newcol).row)) - enemy->box.bottomBox - 1;
            enemy->colisionSquare.row = defineSquare(bottomEdge , newcol).row;
            enemy->colisionSquare.col = defineSquare(bottomEdge , newcol-enemy->box.leftBox).col;
            return 1;
        }
        else if(board[defineSquare(bottomEdge , newcol).row][defineSquare(bottomEdge , newcol+enemy->box.rightBox).col]>40)
        {
            enemy->position.row = (lado * (defineSquare(bottomEdge,newcol).row)) - enemy->box.bottomBox - 1;
            enemy->colisionSquare.row = defineSquare(bottomEdge , newcol).row;
            enemy->colisionSquare.col = defineSquare(bottomEdge , newcol+enemy->box.rightBox).col;
            return 1;
        }
        else
            enemy->position.row = mewrow;
    }
    else if(enemy->direction == 'L')
    {
        enemy->position.row=mewrow;
        if(leftEdge < 0)
        {
            enemy->position.col = 1 + enemy->box.leftBox;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
        else if(board[enemy->boardPlace.row][defineSquare(mewrow , leftEdge).col]>40)
        {
            enemy->position.col = (lado * (defineSquare(mewrow , leftEdge).col+1)) + enemy->box.leftBox; //Se agrega un +1 al defineSquare porque necesitamos el borde derecho de la casilla de colision
            enemy->colisionSquare.row = enemy->boardPlace.row;
            enemy->colisionSquare.col = defineSquare(mewrow , leftEdge).col;
            return 1;
        }
        else if(board[defineSquare(mewrow-enemy->box.upBox , leftEdge).row][defineSquare(mewrow , leftEdge).col]>40)
        {
            enemy->position.col = (lado * (defineSquare(mewrow , leftEdge).col+1)) + enemy->box.leftBox; //Se agrega un +1 al defineSquare porque necesitamos el borde derecho de la casilla de colision
            enemy->colisionSquare.row = defineSquare(mewrow-enemy->box.upBox , leftEdge).row;
            enemy->colisionSquare.col = defineSquare(mewrow , leftEdge).col;
            return 1;
        }
        else if(board[defineSquare(mewrow+enemy->box.bottomBox , leftEdge).row][defineSquare(mewrow , leftEdge).col]>40)
        {
            enemy->position.col = (lado * (defineSquare(mewrow , leftEdge).col+1)) + enemy->box.leftBox; //Se agrega un +1 al defineSquare porque necesitamos el borde derecho de la casilla de colision
            enemy->colisionSquare.row = defineSquare(mewrow+enemy->box.bottomBox , leftEdge).row;
            enemy->colisionSquare.col = defineSquare(mewrow , leftEdge).col;
            return 1;
        }
        else
            enemy->position.col = newcol;
    }
    else if(enemy->direction == 'R')
    {
        enemy->position.row=mewrow;
        if(rightEdge >= (lado*Game.gameCols))
        {
            enemy->position.col = (lado*Game.gameCols) - enemy->box.rightBox - 1;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
        else if(board[enemy->boardPlace.row][defineSquare(mewrow , rightEdge).col]>40)
        {
            enemy->position.col = (lado * (defineSquare(mewrow , rightEdge).col)) - enemy->box.rightBox - 1;
            enemy->colisionSquare.row = enemy->boardPlace.row;
            enemy->colisionSquare.col = defineSquare(mewrow , rightEdge).col;
            return 1;
        }
        else if(board[defineSquare(mewrow-enemy->box.upBox , rightEdge).row][defineSquare(mewrow , rightEdge).col]>40)
        {
            enemy->position.col = (lado * (defineSquare(mewrow , rightEdge).col)) - enemy->box.rightBox - 1;
            enemy->colisionSquare.row = defineSquare(mewrow-enemy->box.upBox , rightEdge).row;
            enemy->colisionSquare.col = defineSquare(mewrow , rightEdge).col;
            return 1;
        }
        else if(board[defineSquare(mewrow+enemy->box.bottomBox , rightEdge).row][defineSquare(mewrow , rightEdge).col]>40)
        {
            enemy->position.col = (lado * (defineSquare(mewrow , rightEdge).col)) - enemy->box.rightBox - 1;
            enemy->colisionSquare.row = defineSquare(mewrow+enemy->box.bottomBox , rightEdge).row;
            enemy->colisionSquare.col = defineSquare(mewrow , rightEdge).col;
            return 1;
        }
        else
            enemy->position.col = newcol;
    }

    // Alterando la posición del enemigo en la matriz
    if((enemy->position.col / lado)!=enemy->boardPlace.col)
    {
        if(board[enemy->position.row / lado][enemy->position.col / lado]!=0)
        {
            colisionSquare.row = enemy->position.row / lado;
            colisionSquare.col = enemy->position.col / lado;
        }
        board[enemy->boardPlace.row][enemy->boardPlace.col] = 0;
        board[enemy->position.row / lado][enemy->position.col / lado] = enemy->type+2;
        enemy->boardPlace.row = enemy->position.row / lado;
        enemy->boardPlace.col = enemy->position.col / lado;
    }
    if((enemy->position.row / lado)!=enemy->boardPlace.row)
    {
        if(board[enemy->position.row / lado][enemy->position.col / lado]!=0)
        {
            colisionSquare.row = enemy->position.row / lado;
            colisionSquare.col = enemy->position.col / lado;
        }
        board[enemy->boardPlace.row][enemy->boardPlace.col] = 0;
        board[enemy->position.row / lado][enemy->position.col / lado] = enemy->type+2;
        enemy->boardPlace.row = enemy->position.row / lado;
        enemy->boardPlace.col = enemy->position.col / lado;
    }

    // Comprobando colisiones con el jugador
    if(enemyCollision(*enemy, player1))
    {
        return 3;
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

int manageEnemy(int board[MAXFILS][MAXCOLS], enemigo *enemy)
{
    char answerDirection;

    switch (enemy->type)
    {
    case 0:
        //Movemos al enemigo en la direccion que tiene y vemos que ocurre.
        switch (moveEnemy(board, enemy))
        {
        case 1:
            enemy->direction = doomieMovement(*enemy);
            break;
        case 3: //Choque con el jugador
            return 1;
            break;
        }
        break;
    case 1:
        //Decidimos la dirección del movimiento del personaje
        if((al_get_timer_count(timer)%5) == 0)
        {
            answerDirection = bestToPnjMovement(board, *enemy, player1);

            if(answerDirection != enemy->direction)
            {
                enemy->Olddirection = enemy->direction;
                enemy->direction = answerDirection;
            }
        }

        //Movemos al enemigo en la direccion que tiene y vemos que ocurre.
        switch (moveEnemy(board, enemy))
        {
        case 1: //Choque con bloque
            enemy->direction = enemy->Olddirection;
            moveEnemy(board, enemy);
            break;
        case 3: //Choque con el jugador
            return 1;
            break;
        }
        break;
    case 2:
        if(enemy->state == 0)
        {
            //Decidimos la dirección del movimiento del personaje
            enemy->direction = toPnjMovement(board, *enemy, player1);

            //Movemos al enemigo en la direccion que tiene y vemos que ocurre.
            switch (moveEnemy(board, enemy))
            {
            case 1: //Choque con bloque
                enemy->state = 1;
                enemy->numSpriteFrames = 4;
                enemy->spritecol = 0;
                enemy->powerCount = 4;
                break;
            case 3: //Choque con el jugador
                return 1;
                break;
            }
        }
        else if(enemy->state == 1)
        {
            if(enemy->powerCount!=1 && (board[enemy->colisionSquare.row][enemy->colisionSquare.col] != 0))
            {
                if((al_get_timer_count(timer)%20) == 0)
                {
                    board[enemy->colisionSquare.row][enemy->colisionSquare.col]--;
                    enemy->powerCount--;
                }
            }
            else
            {
                enemy->state = 0;
                enemy->numSpriteFrames = 9;
                enemy->spritecol = 0;
                board[enemy->colisionSquare.row][enemy->colisionSquare.col] = 0;
            }
        }
        break;
    }

    return 0;
}

int getBoard(int board[MAXFILS][MAXCOLS], char numero[3])
{
    int i,j, normalObjectsCont=0, enemiesCont=0;
    Game.totalNormalObjects=0;
    Game.totalEnemies = 0;
    int foundedObjectType;
    char filename[21] = "levels/level";
    strcat(filename, numero);
    strcat(filename, ".txt");

    //Inicializamos objetos en 0
    for(int i=0; i<NORMAL_OBJECTS_TYPE; i++)
        Game.numNormalObjects[i]=0;

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
    {
        fscanf(game,"%d", &board[i][j]);

        if(board[i][j]>=2 && board[i][j]<=9)
        {
            Game.numEnemies[board[i][j]-2]++;
            Game.totalEnemies++;
        }

        if(board[i][j]>=21 && board[i][j]<=30) //Si es objeto cuenta ese objeto dentro de los tipos
        {
            Game.numNormalObjects[board[i][j]-21]++;
            Game.totalNormalObjects++;
        }
    }
    fclose(game);

    Game.normalObjects = malloc(sizeof(object)*Game.totalNormalObjects);
    Game.enemies = malloc(sizeof(enemigo)*Game.totalEnemies);

    for(i=0; i<Game.gameRows; i++)
    for(j=0; j<Game.gameCols; j++)
    {
        if(board[i][j]>=21 && board[i][j]<=30) //Si es objeto sacamos su informacion
        {
            Game.normalObjects[normalObjectsCont].position.row = i;
            Game.normalObjects[normalObjectsCont].position.col = j;
            Game.normalObjects[normalObjectsCont].type = board[i][j]-21;
            Game.normalObjects[normalObjectsCont].state = -3;
            normalObjectsCont++;
        }
        if(board[i][j]>=2 && board[i][j]<=9) //Si es enemigo sacamos su informacion
        {
            Game.enemies[enemiesCont].boardPlace.row=i;
            Game.enemies[enemiesCont].boardPlace.col=j;
            Game.enemies[enemiesCont].position.row= i*lado + lado/2;
            Game.enemies[enemiesCont].position.col= j*lado + lado/2;
            Game.enemies[enemiesCont].movement=1;
            Game.enemies[enemiesCont].type = board[i][j]-2;
            Game.enemies[enemiesCont].spritecol = 0;
            enemiesCont++;
        }
    }


    // Ordenando objetos
    object auxObject;
    for(int i=0; i<Game.totalNormalObjects-1; i++)
    for(int j=0; j<Game.totalNormalObjects-i-1; j++)
    if(Game.normalObjects[j].type>Game.normalObjects[j+1].type)
    {
        auxObject = Game.normalObjects[j];
        Game.normalObjects[j] = Game.normalObjects[j+1];
        Game.normalObjects[j+1] = auxObject;
    }

    // Ordenando enemigos
    enemigo auxEnemy;
    for(int i=0; i<Game.totalEnemies-1; i++)
    for(int j=0; j<Game.totalEnemies-i-1; j++)
    if(Game.enemies[j].type>Game.enemies[j+1].type)
    {
        auxEnemy = Game.enemies[j];
        Game.enemies[j] = Game.enemies[j+1];
        Game.enemies[j+1] = auxEnemy;
    }

    // Instanciando enemigos iniciales.
    srand(time(0));
    for(i=0; i<Game.totalEnemies; i++)
    {
        int directionNumber = rand()%4;
        switch (directionNumber)
        {
        case 0:
            Game.enemies[i].direction='U';
            break;
        case 1:
            Game.enemies[i].direction='D';
            break;
        case 2:
            Game.enemies[i].direction='L';
            break;
        case 3:
            Game.enemies[i].direction='R';
            break;
        default:
            printf("Numero al azar: %d",directionNumber);
        }


        switch (Game.enemies[i].type)
        {
        case 0: //Enemigo tonto
            Game.enemies[i].velocity=3;
            Game.enemies[i].box.upBox=20;
            Game.enemies[i].box.bottomBox=20;
            Game.enemies[i].box.leftBox=20;
            Game.enemies[i].box.rightBox=20;
            Game.enemies[i].state=0;
            Game.enemies[i].numSpriteFrames=9;
            sprintf(Game.enemies[i].spriteName, "./src/sprites/enemies/enemy%d.png", Game.enemies[i].type);
            Game.enemies[i].sprite = al_load_bitmap(Game.enemies[i].spriteName);
            break;
        case 1: // Enemigo que te persigue inteligentemente
            Game.enemies[i].velocity=3;
            Game.enemies[i].box.upBox=20;
            Game.enemies[i].box.bottomBox=20;
            Game.enemies[i].box.leftBox=20;
            Game.enemies[i].box.rightBox=20;
            Game.enemies[i].state=0;
            Game.enemies[i].numSpriteFrames=9;
            sprintf(Game.enemies[i].spriteName, "./src/sprites/enemies/enemy%d.png", Game.enemies[i].type);
            Game.enemies[i].sprite = al_load_bitmap(Game.enemies[i].spriteName);
            break;
        case 2: // Enemigo que te persigue, si encuentra un obstáculo rompe el bloque de destino
            Game.enemies[i].velocity=3;
            Game.enemies[i].box.upBox=20;
            Game.enemies[i].box.bottomBox=20;
            Game.enemies[i].box.leftBox=20;
            Game.enemies[i].box.rightBox=20;
            Game.enemies[i].state=0;
            Game.enemies[i].numSpriteFrames=9;
            Game.enemies[i].direction = toPnjMovement(board, Game.enemies[i], player1);
            sprintf(Game.enemies[i].spriteName, "./src/sprites/enemies/enemy%d.png", Game.enemies[i].type);
            Game.enemies[i].sprite = al_load_bitmap(Game.enemies[i].spriteName);
            break;
        }
    }


    // Configuraciones para mostrar o no los objetos normales iniciales
    for(i=0; i<NORMAL_OBJECTS_TYPE; i++)
    {
        if(Game.numNormalObjects[i]!=0)
        {
            for(j=0; j<Game.numNormalObjects[i]; j++)
            {
                Game.normalObjects[j].state=0;
            }
            foundedObjectType=i;
            Game.playingNormalObjectType = foundedObjectType;
            break;
        }
    }
    for(i=0; i<Game.totalNormalObjects; i++)
    {
        if(Game.normalObjects[i].state!=0)
            board[Game.normalObjects[i].position.row][Game.normalObjects[i].position.col]=0;
    }
    Game.MAXNormalObjectType = Game.normalObjects[Game.totalNormalObjects -1].type;

    /* for(i=0; i<Game.totalNormalObjects; i++)
    {
        printf("Objeto %d, fila %d, columna %d, tipo %d, estado %d\n\n", i, Game.normalObjects[i].position.row, Game.normalObjects[i].position.col, Game.normalObjects[i].type, Game.normalObjects[i].state);
    } */

    /* for(i=0; i<Game.totalEnemies; i++)
    {
        printf("Enemigo %d, fila %d, columna %d, tipo %d, Dirección %c, [%d][%d][%d][%d]\n\n", i, Game.enemies[i].boardPlace.row, Game.enemies[i].boardPlace.col, Game.enemies[i].type, Game.enemies[i].direction, Game.enemies[i].box.upBox, Game.enemies[i].box.bottomBox, Game.enemies[i].box.leftBox, Game.enemies[i].box.rightBox);
    } */

    return 0;
}

int objectCollision(int board[MAXFILS][MAXCOLS], square colisionsquare)
{
    int i;

    for(i=0; i<Game.totalNormalObjects; i++)
    {
        if((Game.normalObjects[i].position.row == colisionsquare.row) && (Game.normalObjects[i].position.col == colisionsquare.col))
        {
            Game.normalObjects[i].state = 1;
            Game.normalObjects[i].cont = Game.normalObjects[i].state!=0;
        }
    }
    return 0;
}

bool enemyCollision(enemigo enemy, personaje pnj) {
    int pnjTop = pnj.position.row - pnj.box.upBox;
    int pnjBottom = pnj.position.row + pnj.box.bottomBox;
    int pnjLeft = pnj.position.col - pnj.box.leftBox;
    int pnjRight = pnj.position.col + pnj.box.rightBox;
    int enemyTop = enemy.position.row - enemy.box.upBox;
    int enemyBottom = enemy.position.row + enemy.box.bottomBox;
    int enemyLeft = enemy.position.col - enemy.box.leftBox;
    int enemyRight = enemy.position.col + enemy.box.rightBox;

    // Revisando colisiones con el método AABB
    if (pnjRight < enemyLeft || pnjLeft > enemyRight || pnjBottom < enemyTop || pnjTop > enemyBottom) {
        return false;
    }
    return true;
}

bool manageObjects(int board[MAXFILS][MAXCOLS])
{
    int i, pasedTypesObjects=0;
    bool nextNormalObjectType=1;

    for(i=0; i<Game.playingNormalObjectType; i++)
    {
        pasedTypesObjects += Game.numNormalObjects[i];
    }

    //Revisando si fueron recogidos los objetos actuales
    for(i=pasedTypesObjects; i<(pasedTypesObjects + Game.numNormalObjects[Game.playingNormalObjectType]); i++)
    {
        if(Game.normalObjects[i].state !=3)
            nextNormalObjectType=0;
        if(Game.normalObjects[i].state == 0 && board[Game.normalObjects[i].position.row][Game.normalObjects[i].position.col] == 0) //En caso de que por alguna razon pase un enemigo u objeto encima de este y quede el espacio vacio entonceas se regresa al estado original.
        {
            board[Game.normalObjects[i].position.row][Game.normalObjects[i].position.col] = Game.normalObjects[i].type + 21;
        }
    }

    if(nextNormalObjectType)
    {
        pasedTypesObjects += Game.numNormalObjects[Game.playingNormalObjectType];
        Game.playingNormalObjectType++;

        if(Game.playingNormalObjectType<=Game.MAXNormalObjectType)
        {
            for(i=pasedTypesObjects; i<(pasedTypesObjects + Game.numNormalObjects[Game.playingNormalObjectType]); i++)
            {
                Game.normalObjects[i].state = -2;
                Game.normalObjects[i].cont = objectTimerCount;
                board[Game.normalObjects[i].position.row][Game.normalObjects[i].position.col] = Game.normalObjects[i].type+21;
            }
        }
        else
            return 1;
    }

    //Manejando estados pasajeros de objetos normales
    for(i=0; i<Game.totalNormalObjects; i++)
    {
        if(Game.normalObjects[i].state<3 && Game.normalObjects[i].state!=0)
        {
            if(Game.normalObjects[i].cont !=0)
                Game.normalObjects[i].cont--;
            else
                {switch (Game.normalObjects[i].state)
                {
                case -2:
                    Game.normalObjects[i].state++;
                    break;
                case -1:
                    Game.normalObjects[i].state++;
                    break;
                case 1:
                    Game.normalObjects[i].state++;
                    break;
                case 2:
                    Game.normalObjects[i].state++;
                    break;
                }
                Game.normalObjects[i].cont=objectTimerCount;
            }
        }
    }
    return 0;
}

char doomieMovement(enemigo enemy)
{
    int newDirection, oldDirection;
    if(enemy.direction == 'U')
        oldDirection = 0;
    else if(enemy.direction == 'D')
        oldDirection = 1;
    else if(enemy.direction == 'L')
        oldDirection = 2;
    else if(enemy.direction == 'R')
        oldDirection = 3;

    srand(al_get_timer_count(timer)+time(0));
    do
    {
        newDirection = rand()%4;
    }while(newDirection == oldDirection);

    if(newDirection == 0)
        return 'U';
    else if(newDirection == 1)
        return 'D';
    else if(newDirection == 2)
        return 'L';
    else if(newDirection == 3)
        return 'R';
}

char toPnjMovement(int board[MAXFILS][MAXCOLS], enemigo enemy, personaje pnj)
{
    char bestDirection = enemy.direction;
    int vertical, horizontal;

    // Posición actual del personaje y enemigo
    vertical = pnj.boardPlace.row - enemy.boardPlace.row;
    horizontal = pnj.boardPlace.col - enemy.boardPlace.col;

    if(fabs(vertical) > fabs(horizontal))
    {
        if(vertical > 0)
            return 'D';
        else
            return 'U';
    }
    else
    {
        if(horizontal > 0)
            return 'R';
        else
            return 'L';
    }

    return bestDirection;
}

char bestToPnjMovement(int board[MAXFILS][MAXCOLS], enemigo enemy , personaje pnj)
{
    char bestDirection = enemy.direction;
    double minDistance = 100;
    struct movementDir {
        int vertical;
        int horizontal;
        bool obstacle;
        double distanciaEuclidiana;
    } mDir[5];

    // Posición actual del personaje y enemigo
    mDir[0].vertical = pnj.boardPlace.row - enemy.boardPlace.row;
    mDir[0].horizontal = pnj.boardPlace.col - enemy.boardPlace.col;
    mDir[0].obstacle = false; // La posición actual nunca tiene un obstáculo

    // Arriba
    if (enemy.boardPlace.row - 1 >= 0) { // Verificar límites del mapa
        mDir[1].vertical = pnj.boardPlace.row - (enemy.boardPlace.row - 1);
        mDir[1].horizontal = pnj.boardPlace.col - enemy.boardPlace.col;
        mDir[1].obstacle = (board[enemy.boardPlace.row - 1][enemy.boardPlace.col] >= 40);
    } else {
        mDir[1].obstacle = true;
    }

    // Abajo
    if (enemy.boardPlace.row + 1 < Game.gameRows) { // Verificar límites del mapa
        mDir[2].vertical = pnj.boardPlace.row - (enemy.boardPlace.row + 1);
        mDir[2].horizontal = pnj.boardPlace.col - enemy.boardPlace.col;
        mDir[2].obstacle = (board[enemy.boardPlace.row + 1][enemy.boardPlace.col] >= 40);
    } else {
        mDir[2].obstacle = true;
    }

    // Izquierda
    if (enemy.boardPlace.col - 1 >= 0) { // Verificar límites del mapa
        mDir[3].vertical = pnj.boardPlace.row - enemy.boardPlace.row;
        mDir[3].horizontal = pnj.boardPlace.col - (enemy.boardPlace.col - 1);
        mDir[3].obstacle = (board[enemy.boardPlace.row][enemy.boardPlace.col - 1] >= 40);
    } else {
        mDir[3].obstacle = true;
    }

    // Derecha
    if (enemy.boardPlace.col + 1 < Game.gameCols) { // Verificar límites del mapa
        mDir[4].vertical = pnj.boardPlace.row - enemy.boardPlace.row;
        mDir[4].horizontal = pnj.boardPlace.col - (enemy.boardPlace.col + 1);
        mDir[4].obstacle = (board[enemy.boardPlace.row][enemy.boardPlace.col + 1] >= 40);
    } else {
        mDir[4].obstacle = true;
    }

    // Calculo de todas las distancias
    for (int i = 0; i < 5; i++) {
        mDir[i].distanciaEuclidiana = sqrt(pow(mDir[i].vertical, 2) + pow(mDir[i].horizontal, 2));
    }

    // Selección de la mejor dirección
    if (!mDir[1].obstacle && mDir[1].distanciaEuclidiana < minDistance && (enemy.direction != 'D')) {
        minDistance = mDir[1].distanciaEuclidiana;
        bestDirection = 'U'; // Arriba
    }
    if (!mDir[2].obstacle && mDir[2].distanciaEuclidiana < minDistance && (enemy.direction != 'U')) {
        minDistance = mDir[2].distanciaEuclidiana;
        bestDirection = 'D'; // Abajo
    }
    if (!mDir[3].obstacle && mDir[3].distanciaEuclidiana < minDistance && (enemy.direction != 'R')) {
        minDistance = mDir[3].distanciaEuclidiana;
        bestDirection = 'L'; // Izquierda
    }
    if (!mDir[4].obstacle && mDir[4].distanciaEuclidiana < minDistance && (enemy.direction != 'L')) {
        minDistance = mDir[4].distanciaEuclidiana;
        bestDirection = 'R'; // Derecha
    }

    return bestDirection;
}

int count_files_in_directory(const char *path) {
    int file_count = 0;
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (dir == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // DT_REG indica un archivo regular
            file_count++;
        }
    }

    closedir(dir);
    return file_count;
}

// FUnciones gráficas
void draw_boardRectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * lado), (fila * lado), ((columna + 1) * lado), ((fila + 1) * lado), color);
}

void draw_pnj(personaje *pnj, ALLEGRO_BITMAP *image){
    int spriteWidht = al_get_bitmap_width(image)/4;
    int spriteHeight = al_get_bitmap_height(image)/4;
    int spritefil;

    switch (pnj->direction)
    {
    case 'D':
        spritefil = 0;
        break;
    case 'L':
        spritefil = 1;
        break;
    case 'R':
        spritefil = 2;
        break;
    case 'U':
        spritefil = 3;
        break;
    }
    if(al_get_timer_count(timer)%5 == 0)
    {
        if(pnj->movement)
            pnj->spritecol = (pnj->spritecol + 1)%4;
        else
            pnj->spritecol = 0;
    }

    /* al_draw_filled_rectangle(pnj->position.col-pnj->box.leftBox - Game.mapColStart, pnj->position.row-pnj->box.upBox  - Game.mapRowStart, pnj->position.col+pnj->box.rightBox -Game.mapColStart, pnj->position.row+pnj->box.bottomBox - Game.mapRowStart, color_purple1); */

    al_draw_bitmap_region(image, (spriteWidht*pnj->spritecol),(spriteHeight*spritefil),spriteWidht,spriteHeight, pnj->position.col-(spriteWidht/2) - Game.mapColStart , pnj->position.row-(spriteHeight/2) - Game.mapRowStart, 0);

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
    ALLEGRO_BITMAP *sparcleBitmap = al_load_bitmap("./src/sprites/effects/sparcle.png");
    int mapSpriteWidht = 64, mapSpriteHeight = 64;

    //Definiendo comienzo en X
    if((player1.position.col + (windowWidth/2)) > (Game.gameCols*lado))
        Game.mapColStart = (Game.gameCols * lado) - windowWidth;
    else if ((player1.position.col - (windowWidth/2)) < 0)
        Game.mapColStart = 0;
    else
        Game.mapColStart = player1.position.col - (windowWidth/2);
    //Definiendo comienzo en Y
    if((player1.position.row + (windowheight/2)) > (Game.gameRows*lado))
        Game.mapRowStart = (Game.gameRows * lado) - windowheight;
    else if ((player1.position.row - (windowheight/2)) < 0)
        Game.mapRowStart = 0;
    else
        Game.mapRowStart = player1.position.row - (windowheight/2);
    al_draw_bitmap_region(board_bitmap,Game.mapColStart, Game.mapRowStart, windowWidth, windowheight, 0, 0, 0);

    Game.startSquare.col = Game.mapColStart / lado;
    Game.endSquare.col = (Game.mapColStart + windowWidth) / lado;
    Game.startSquare.row = Game.mapRowStart / lado;
    Game.endSquare.row = (Game.mapRowStart + windowheight) /lado;

    for(i=Game.startSquare.row; i<=Game.endSquare.row; i++)
    {
        for(j=Game.startSquare.col; j<=Game.endSquare.col; j++)
        {
            if(board[i][j]>=41 && board[i][j]<=44)
            {
                al_draw_bitmap_region(rockBitmap, (board[i][j]-42)*mapSpriteWidht, 0, mapSpriteWidht, mapSpriteHeight, j*lado - Game.mapColStart, i*lado - Game.mapRowStart, 0 );
            }
            /* if(board[i][j]==1)
            {
                draw_boardRectangle(i-Game.startSquare.row,j-Game.startSquare.col,color_blue);
            }*/
            /* if(board[i][j]==3)
            {
                draw_boardRectangle(i-Game.startSquare.row,j-Game.startSquare.col,color_green3);
            } */
        }
    }

    // Dibujando objetos
    for(i=0; i<Game.totalNormalObjects; i++)
    {
        switch (Game.normalObjects[i].state)
        {
        case -2:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*0, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*lado - Game.mapColStart, Game.normalObjects[i].position.row*lado - Game.mapRowStart, 0 );
            break;
        case -1:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*1, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*lado - Game.mapColStart, Game.normalObjects[i].position.row*lado - Game.mapRowStart, 0 );
            break;
        case 0:
            al_draw_bitmap_region(flowerBitmap, mapSpriteWidht*Game.normalObjects[i].type, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*lado - Game.mapColStart, Game.normalObjects[i].position.row*lado - Game.mapRowStart, 0 );
            break;
        case 1:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*0, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*lado - Game.mapColStart, Game.normalObjects[i].position.row*lado - Game.mapRowStart, 0 );
            break;
        case 2:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*1, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*lado - Game.mapColStart, Game.normalObjects[i].position.row*lado - Game.mapRowStart, 0 );
            break;
        }
    }

    //Dibujando enemigos
    for(i=0; i<Game.totalEnemies; i++)
        if(Game.enemies[i].boardPlace.row>=Game.startSquare.row && Game.enemies[i].boardPlace.row<=Game.endSquare.row && Game.enemies[i].boardPlace.col>=Game.startSquare.col && Game.enemies[i].boardPlace.col<=Game.endSquare.col)
            draw_enemy(&Game.enemies[i]);

    //Dibujando al jugador
    draw_pnj(&player1, player_bitmap);

    al_destroy_bitmap(rockBitmap);
    al_destroy_bitmap(flowerBitmap);
    al_destroy_bitmap(sparcleBitmap);
}

void draw_enemy(enemigo *enemy)
{
    int spriteWidht = 64;
    int spriteHeight = 64;
    int spritefil;

    switch (enemy->direction)
    {
    case 'U':
        spritefil = 0+4*(enemy->state); //Al cambiar el estado cambia la fila del spritesheet
        break;
    case 'L':
        spritefil = 1+4*(enemy->state);
        break;
    case 'D':
        spritefil = 2+4*(enemy->state);
        break;
    case 'R':
        spritefil = 3+4*(enemy->state);
        break;
    }
    if(al_get_timer_count(timer)%5 == 0)
    {
        if(enemy->movement)
            enemy->spritecol = (enemy->spritecol + 1)%enemy->numSpriteFrames;
        else
            enemy->spritecol = 0;
    }

    /* al_draw_filled_rectangle(enemy->position.col-enemy->box.leftBox - Game.mapColStart, enemy->position.row-enemy->box.upBox  - Game.mapRowStart, enemy->position.col+enemy->box.rightBox -Game.mapColStart, enemy->position.row+enemy->box.bottomBox - Game.mapRowStart, color_purple1); */

    al_draw_bitmap_region(enemy->sprite, (spriteWidht*enemy->spritecol),(spriteHeight*spritefil),spriteWidht,spriteHeight, enemy->position.col-(spriteWidht/2) - Game.mapColStart , enemy->position.row-(spriteHeight/2) - Game.mapRowStart, 0);

    return;
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

int game_Over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    int i;
    bool done = false;
    ALLEGRO_BITMAP *gameOver_bmp = al_load_bitmap("src/gameOver/960x640/gameOver.png");
    ALLEGRO_BITMAP *gameOver1_bmp = al_load_bitmap("src/gameOver/960x640/gameOver1.png");

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(windowNcol*lado, windowNfil*lado);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(ventana), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    al_draw_bitmap(screenshot,0,0,0);
    al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
    al_draw_bitmap(gameOver_bmp,0,0,0);
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
            for(i=0; i<=4; i++)
            {
                if(i%2 == 0)
                {
                    al_draw_bitmap(screenshot,0,0,0);
                    al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
                    al_draw_bitmap(gameOver1_bmp,0,0,0);
                    al_flip_display();
                    al_rest(0.1);
                }
                else
                {
                    al_draw_bitmap(screenshot,0,0,0);
                    al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
                    al_draw_bitmap(gameOver_bmp,0,0,0);
                    al_flip_display();
                    al_rest(0.1);
                }
            }
            al_rest(0.2);
            return 0;
        }
    }

    al_destroy_bitmap(gameOver_bmp);
    al_destroy_bitmap(gameOver1_bmp);
}

int win_mwnu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    int i;
    char scoreText[5];
    bool done = false;
    ALLEGRO_BITMAP *win_bmp = al_load_bitmap("src/win//960x640/win.png");
    ALLEGRO_BITMAP *win0_bmp = al_load_bitmap("src/win/960x640/win0.png");

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(windowNcol*lado, windowNfil*lado);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(ventana), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    for(i=0; i<=Game.score; i++)
    {
        sprintf(scoreText, "%04d", i);
        al_draw_bitmap(screenshot,0,0,0);
        al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
        al_draw_bitmap(win_bmp,0,0,0);
        al_draw_text(font,color_white, 275, 475,ALLEGRO_ALIGN_LEFT,scoreText);
        al_flip_display();
    }

    while (!done)
    {
        al_wait_for_event(queue, ev); /*Esperando a que ocurra un evento*/

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /*Si es un cierre de la ventana*/
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            for(i=0; i<=4; i++)
            {
                if(i%2 == 0)
                {
                    al_draw_bitmap(screenshot,0,0,0);
                    al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
                    al_draw_bitmap(win0_bmp,0,0,0);
                    al_draw_text(font,color_white, 275, 475,ALLEGRO_ALIGN_LEFT,scoreText);
                    al_flip_display();
                    al_rest(0.1);
                }
                else
                {
                    al_draw_bitmap(screenshot,0,0,0);
                    al_draw_filled_rectangle(0,0,al_get_display_width(ventana), al_get_display_height(ventana), al_map_rgba(0,0,0,200));
                    al_draw_bitmap(win_bmp,0,0,0);
                    al_draw_text(font,color_white, 275, 475,ALLEGRO_ALIGN_LEFT,scoreText);
                    al_flip_display();
                    al_rest(0.1);
                }
            }
            al_rest(0.2);
            return 0;
        }
    }

    al_destroy_bitmap(win_bmp);
    al_destroy_bitmap(win0_bmp);
}

int level_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    ALLEGRO_BITMAP *level_bmap = al_load_bitmap("src/LevelSelection/levelSelection.png");
    ALLEGRO_BITMAP *level0_bmap = al_load_bitmap("src/LevelSelection/levelSelection0.png");
    ALLEGRO_BITMAP *pergamino_bmap = al_load_bitmap("src/LevelSelection/pergamino.png");
    ALLEGRO_BITMAP *pergaminoSelected_bmap = al_load_bitmap("src/LevelSelection/pergaminoSelected.png");
    int i;
    bool done = false;
    int pergaminox = (windowWidth*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
    int pergaminoy = windowheight*3/8 + 20;
    int actualLevel = 1;
    int totalLevels = count_files_in_directory("./levels"); //MAX 8
    char auxLvl[3]; //Para dibujar números

    //Dibujo iniciañ
    al_draw_bitmap(level0_bmap,0,0,0);
    for(i=1; i<=totalLevels; i++)
    {
        sprintf(auxLvl, "%02d", i);
        if( (i%5) != 0)
        {
            if(i == actualLevel)
            {
                al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            else
            {
                al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            pergaminox += windowWidth/5;
        }
        else
        {
            pergaminox = (windowWidth*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
            pergaminoy += 20 + windowheight/4;
            if(i == actualLevel)
            {
                al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            else
            {
                al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            pergaminox += windowWidth/5;
        }
    }
    al_flip_display();
    al_rest(0.1); // Para evitar un pequeño glitch


    //Seleccion de nivel
    while (!done)
    {
        al_wait_for_event(queue, ev); /* Esperando a que ocurra un evento */

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /* Si es un cierre de la ventana */
        {
            return 2;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN) //En caso de algún movimiento
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_TAB || ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)
            {
                actualLevel = (actualLevel+1) % (totalLevels+1);
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_LEFT)
            {
                actualLevel = (actualLevel-1);
                if(actualLevel<0)
                    actualLevel=0;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_DOWN)
            {
                actualLevel = (actualLevel+4) % (totalLevels+1);
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_UP)
            {
                actualLevel = (actualLevel-4);
                if(actualLevel<0)
                    actualLevel=0;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                return actualLevel;
            }
        }

        // Dibujo
        if(actualLevel!=0)
            al_draw_bitmap(level0_bmap,0,0,0);
        else
            al_draw_bitmap(level_bmap,0,0,0);

        for(i=1; i<=totalLevels; i++)
        {
            char auxLvl[2];
            sprintf(auxLvl, "%02d", i);
            if(i%5 != 0)
            {
                if(i == actualLevel)
                {
                    al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                else
                {
                    al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                pergaminox += windowWidth/5;
            }
            else
            {
                pergaminox = (windowWidth*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
                pergaminoy += 20 + windowheight/4;
                if(i == actualLevel)
                {
                    al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                else
                {
                    al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(font_size), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                pergaminox += windowWidth/5;
            }
        }
        al_flip_display();
        //Corrección a ubicación inicial de pergaminos
        pergaminox = (windowWidth*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
        pergaminoy = windowheight*3/8 + 20;
    }

    al_destroy_bitmap(level_bmap);
    al_destroy_bitmap(level0_bmap);
    al_destroy_bitmap(pergamino_bmap);
    al_destroy_bitmap(pergaminoSelected_bmap);
}

int game(int board[MAXFILS][MAXCOLS], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer, int level)
{
    bool done = false;
    bool win = false;
    int i, j;
    char level_string[2];
    sprintf(level_string, "%02d", level);
    printf("%s\n",level_string);

    getBoard(board, level_string);
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
    al_set_timer_count(timer, 0);
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
            al_clear_to_color(color_black);

            if (keys[ALLEGRO_KEY_UP])
            {
                if(movePlayer(board, &player1, 'U') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }
            else if (keys[ALLEGRO_KEY_DOWN])
            {
                if(movePlayer(board, &player1, 'D') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }
            else if (keys[ALLEGRO_KEY_LEFT])
            {
                if(movePlayer(board, &player1, 'L') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }
            else if (keys[ALLEGRO_KEY_RIGHT])
            {
                if(movePlayer(board, &player1, 'R') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }

            if((!ice.possible) && ((al_get_timer_count(timer)%2) == 0)) //Este if se ejecuta cada 2 ticks sólo si NO es posible crear hielo, es decir, si hay hielo pendiente por generar
                manageIce(board,&ice);

            for(i=0; i<Game.totalEnemies; i++)
                if(manageEnemy(board, &Game.enemies[i]) == 1) //Si hay choque con un enemigo
                {
                    done = true;
                    win = false;
                }

            if(manageObjects(board) == 1) // Si no hay más objetos que recoger
            {
                al_stop_timer(timer);
                done = true;
                win = true;
            }

            /* Dibujar el tablero en el backbuffer*/
            draw_board(board);

            /* Actualizar pantalla */
            al_flip_display();
        }
    }

    for(i=0; i<ALLEGRO_KEY_MAX; i++) //Se "levantan" todas las teclas
        keys[i] = false;

    for(int i=0; i<Game.totalEnemies; i++) //Eliminando birmaps de enemigos
        al_destroy_bitmap(Game.enemies[i].sprite);

    if(win)
    {
        Game.score = (-pow((al_get_timer_count(timer) / (float) FPS),2) / 10) + 10000;
        win_mwnu(queue,ev, timer);
        return 0;
    }
    else
    {
        game_Over(queue, ev, timer);
        return 1;
    }
    return 0;
}