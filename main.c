#include <allegro5/allegro.h> //Biblioteca Allegro
    #include <allegro5/allegro_primitives.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_font.h>
    #include <allegro5/allegro_ttf.h>
    #include <allegro5/allegro_audio.h>
    #include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <math.h>

#define SQUARE_SIDE 64
#define WINDOW_ROWS 10
#define WINDOW_COLS 15
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 640
#define MAX_ROWS 30
#define MAX_COLS 30
#define BIG_FONT_SIZE 60
#define FONT_SIZE 52
#define TINY_FONT_SIZE 28
#define FPS 24
#define NORMAL_OBJECTS_TYPES 10
#define MAX_POWER_TYPES 2
#define ENEMY_TYPES 3
#define OBJECT_TIMER_COUNT 4
#define MAX_LEVELS 8
#define MAX_LEVEL_INPUT 10
#define HIT_COOLDOWN FPS // 1 segundo de cooldown
#define MAX_CONFIG_ITEMS 8

struct _hitBox{
    int left;
    int right;
    int top;
    int bottom;
};
typedef struct _hitBox hitBox;

struct _square{
    int row;
    int col;
};
typedef struct _square square;

struct _obstacle{
    square begin;
    char direction; // U, D, L, R
    int remaining;
    int limit;
    bool create; // Create or Destroy
    bool possible; // Able or unable to create Ice
};
typedef struct _obstacle obstacle;

struct _character {
    square position;
    square boardPlace;
    char direction; // U, D, L, R
    hitBox box;
    float velocity;
    bool movement;
    int spritecol;
    int powerScope;
    int powerType;
    bool power_hability[MAX_POWER_TYPES];
    unsigned int hits;
    int hitCooldown;
    bool hited; //Golpeado, booleano para animacion
} player1;
typedef struct _character character;

struct _enemy {
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
    int powerScope;
    int powerType;
    int powerCount;
};
typedef struct _enemy enemy;

struct _object{
    square position;
    ALLEGRO_BITMAP *bmp;
    int type;
    int state;
    int cont;
    int powerTimer;
    int MAXpowerTimer;
    bool active;
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
    bool minimap;
    int score;
    char userName[11];
    int levelNumber;

    object *normalObjects;
    int numNormalObjects[NORMAL_OBJECTS_TYPES];
    int totalNormalObjects;
    int playingNormalObjectType;
    int MAXNormalObjectType;

    object *specialObjects;
    int totalSpecialObjects;

    enemy *enemies;
    int numEnemies[ENEMY_TYPES];
    int totalEnemies;
} Game;
typedef struct _gameInfo gameInfo;

struct _scoreInput
{
    char name[11];
    int score;
};
typedef struct _scoreInput scoreInput;

struct _levelScore
{
    scoreInput inputs[MAX_LEVEL_INPUT];
    int quantity;
}levelScores[MAX_LEVELS];
typedef struct _levelScore levelScore;

struct _music
{
    ALLEGRO_AUDIO_STREAM *strem;
    int code;
    bool paused;
} gameMusic;
typedef struct _music music;

struct _config
{
    int UP;
    int DOWN;
    int LEFT;
    int RIGHT;
    int POWER;
    int MINIMAP;
    float MUSICVOLUME;
    float SFXVOLUME;
} gameConfig;
typedef struct _config config;

/*Colores*/
ALLEGRO_COLOR color_black;
ALLEGRO_COLOR color_white;
ALLEGRO_COLOR color_gray;
ALLEGRO_COLOR color_blue;
ALLEGRO_COLOR color_purple;

//Funciones lógicas
int move_player(int board[MAX_ROWS][MAX_COLS], character *pnj, char direction);
int move_enemy(int board[MAX_ROWS][MAX_COLS], enemy *enm);
square define_square(int filPixel, int colPixel);
obstacle power(int board[MAX_ROWS][MAX_COLS], character pnj);
void manage_obstacles(int board[MAX_ROWS][MAX_COLS], obstacle *ice);
int manage_enemy(int board[MAX_ROWS][MAX_COLS], enemy *enm);
int get_board(int board[MAX_ROWS][MAX_COLS], char numero[3]);
int object_collision(int board[MAX_ROWS][MAX_COLS], square colisionsquare);
bool enemy_collision(enemy enemy, character pnj);
bool manage_objects(int board[MAX_ROWS][MAX_COLS]);
char doomie_movement(enemy enemy);
char to_pnj_movement(int board[MAX_ROWS][MAX_COLS], enemy enemy , character pnj);
char best_to_pnj_movement(int board[MAX_ROWS][MAX_COLS], enemy enemy ,character pnj);
int count_files_in_directory(const char *path);
void read_score();
void update_score(int level, char name[11], int score);
void whrite_score();
void select_music(int code);
void set_music_volume(float volume);
void get_config();
void whrite_config();
void get_key_name();

//Funciones gráficas
void draw_board_rectangle(int fila, int columna, ALLEGRO_COLOR color);
void draw_minimap(int board[MAX_ROWS][MAX_COLS]);
void draw_pnj(character *pnj, ALLEGRO_BITMAP *image);
void draw_enemy(enemy *enm);
void draw_background(ALLEGRO_BITMAP *bitmap);
void draw_board(int board[MAX_ROWS][MAX_COLS]);
void draw_HUD();

/*Partes*/
int select_key(char *purpose, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int select_number(int min, int max, int def, char *purpose, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int name_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int game_over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int win_mwnu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int level_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int config_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int score_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer);
int game(int board[MAX_ROWS][MAX_COLS], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer, int level);

/*Bitmaps*/
ALLEGRO_DISPLAY *window;
ALLEGRO_BITMAP *board_bitmap;
ALLEGRO_BITMAP *player_bitmap;
ALLEGRO_BITMAP *bushBitmap;
ALLEGRO_BITMAP *rootBitmap;
ALLEGRO_BITMAP *rockBitmap;
ALLEGRO_BITMAP *flowerBitmap;

ALLEGRO_FONT *bigfont; /*Fuente grande*/
ALLEGRO_FONT *font; /*Fuente*/
ALLEGRO_FONT *tinyFont; /*Fuente pequenia*/
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
    color_purple = al_map_rgb(87, 35, 100);

    // Banderas
    bool done = false;
    int position[2];
    int board[MAX_ROWS][MAX_COLS];
    for(i=0; i<MAX_ROWS; i++)
    for(j=0; j<MAX_COLS; j++)
        board[i][j]=0;

    /*Inicialización allegro*/
    al_init();
    al_init_primitives_addon(); // Figuras
    al_init_image_addon(); // Imagenes
    al_init_font_addon(); // Texto
    al_init_ttf_addon();
    al_install_keyboard(); // Teclado
    al_install_audio(); // sistema de a udio
    al_init_acodec_addon(); // codecs de audio
    al_reserve_samples(10);

    /*Fuentes*/
    font = al_load_ttf_font("./src/fonts/spooky_magic/SpookyMagic.ttf", FONT_SIZE, 0);
    tinyFont = al_load_ttf_font("./src/fonts/spooky_magic/SpookyMagic.ttf", TINY_FONT_SIZE, 0);
    bigfont = al_load_ttf_font("./src/fonts/spooky_magic/SpookyMagic.ttf", BIG_FONT_SIZE, 0);

    // Inicializando ventana
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    window = al_create_display(WINDOW_WIDTH , WINDOW_HEIGHT);
    al_set_target_backbuffer(window);

    //Temporizadores
    timer = al_create_timer(1.0 / FPS);

    //Inicializar cola de eventor
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue(); /*Creamos cola de eventos*/
    al_register_event_source(event_queue, al_get_display_event_source(window)); /*La ventana puede dar eventos*/
    al_register_event_source(event_queue, al_get_keyboard_event_source());/*El teclado puede dar eventos*/
    al_register_event_source(event_queue, al_get_timer_event_source(timer));/*El temporizador puede dar eventos*/
    ALLEGRO_EVENT ev; /*Creamos un evento que analizaremos*/

    /*Inicializamos musica*/
    gameMusic.code=-1; // No hay cancion seleccionada
    gameMusic.paused=false; // No hay cancion seleccionada

    get_config(); // Configuracion
    read_score(); //Lee el Archivo de score para guardarlo en RAM

    name_menu(event_queue, &ev, timer);
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
            case 1: // Configuracion
                if(config_menu(event_queue,&ev,timer) == -1)
                    done = true;
                break;
            case 2: // Score
                if(score_menu(event_queue,&ev,timer) == -1)
                    done = true;
                break;
            default:
                done = true;
                break;
        }
    }
    whrite_score(); //Guarda el score en archivo

    /* Cerrar recursos */
    al_destroy_display(window);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font);
    al_destroy_font(tinyFont);
    al_destroy_font(bigfont);
    al_destroy_timer(timer);
    al_destroy_bitmap(player_bitmap);
    al_destroy_bitmap(board_bitmap);
    if (gameMusic.strem) {
        al_destroy_audio_stream(gameMusic.strem);
    }
    return 0;
}

// funciones lógicas
int move_player(int board[MAX_ROWS][MAX_COLS], character *pnj, char direction)
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
    topEdge = mewrow - pnj->box.top;
    bottomEdge = mewrow + pnj->box.bottom;
    leftEdge = newcol - pnj->box.left;
    rightEdge = newcol + pnj->box.right;

    if(pnj->direction == 'U')
    {
        pnj->position.col=newcol;
        if(topEdge<0)
            pnj->position.row = 1 + pnj->box.top;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes superiors del personaje para generar colision.
            (board[define_square(topEdge , newcol).row][pnj->boardPlace.col]>40)
            || (board[define_square(topEdge , newcol).row][define_square(topEdge , newcol-pnj->box.left).col]>40)
            || (board[define_square(topEdge , newcol).row][define_square(topEdge , newcol+pnj->box.right).col]>40)
            )
        {
            pnj->position.row = (SQUARE_SIDE * (define_square(topEdge,newcol).row+1)) + pnj->box.top; //Se agrega un +1 al define_square porque necesitamos el borde inferior de la casilla de colision
        }
        else
            pnj->position.row = mewrow;
    }
    else if(pnj->direction == 'D')
    {
        pnj->position.col=newcol;
        if(bottomEdge >= (SQUARE_SIDE*Game.gameRows))
            pnj->position.row = (SQUARE_SIDE*Game.gameRows) - pnj->box.bottom-1;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
            (board[define_square(bottomEdge , newcol).row][pnj->boardPlace.col]>40)
            || (board[define_square(bottomEdge , newcol).row][define_square(bottomEdge , newcol-pnj->box.left).col]>40)
            || (board[define_square(bottomEdge , newcol).row][define_square(bottomEdge , newcol+pnj->box.right).col]>40)
            )
        {
            pnj->position.row = (SQUARE_SIDE * (define_square(bottomEdge,newcol).row)) - pnj->box.bottom - 1;
        }
        else
            pnj->position.row = mewrow;
    }
    else if(pnj->direction == 'L')
    {
        pnj->position.row=mewrow;
        if(leftEdge < 0)
            pnj->position.col = 1 + pnj->box.left;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes izquierdos del personaje para generar colision.
            (board[pnj->boardPlace.row][define_square(mewrow , leftEdge).col]>40)
            || (board[define_square(mewrow-pnj->box.top , leftEdge).row][define_square(mewrow , leftEdge).col]>40)
            || (board[define_square(mewrow+pnj->box.bottom , leftEdge).row][define_square(mewrow , leftEdge).col]>40)
            )
        {
            pnj->position.col = (SQUARE_SIDE * (define_square(mewrow , leftEdge).col+1)) + pnj->box.left; //Se agrega un +1 al define_square porque necesitamos el borde derecho de la casilla de colision
        }
        else
            pnj->position.col = newcol;
    }
    else if(pnj->direction == 'R')
    {
        pnj->position.row=mewrow;
        if(rightEdge >= (SQUARE_SIDE*Game.gameCols))
            pnj->position.col = (SQUARE_SIDE*Game.gameCols) - pnj->box.right - 1;
        else if(
            //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes derechos del personaje para generar colision.
            (board[pnj->boardPlace.row][define_square(mewrow , rightEdge).col]>40)
            || (board[define_square(mewrow-pnj->box.top , rightEdge).row][define_square(mewrow , rightEdge).col]>40)
            || (board[define_square(mewrow+pnj->box.bottom , rightEdge).row][define_square(mewrow , rightEdge).col]>40)
            )
        {
            pnj->position.col = (SQUARE_SIDE * (define_square(mewrow , rightEdge).col)) - pnj->box.right - 1;
        }
        else
            pnj->position.col = newcol;
    }


    // Alterando la posición del jugador en la matriz
    if((pnj->position.col / SQUARE_SIDE)!=pnj->boardPlace.col)
    {
        if(board[pnj->position.row / SQUARE_SIDE][pnj->position.col / SQUARE_SIDE]!=0)
        {
            colisionSquare.row = pnj->position.row / SQUARE_SIDE;
            colisionSquare.col = pnj->position.col / SQUARE_SIDE;
            if(object_collision(board,colisionSquare) == 1)
                return 1;
        }
        board[pnj->boardPlace.row][pnj->boardPlace.col] = 0;
        board[pnj->position.row / SQUARE_SIDE][pnj->position.col / SQUARE_SIDE] = 1;
        pnj->boardPlace.row = pnj->position.row / SQUARE_SIDE;
        pnj->boardPlace.col = pnj->position.col / SQUARE_SIDE;
    }
    if((pnj->position.row / SQUARE_SIDE)!=pnj->boardPlace.row)
    {
        if(board[pnj->position.row / SQUARE_SIDE][pnj->position.col / SQUARE_SIDE]!=0)
        {
            colisionSquare.row = pnj->position.row / SQUARE_SIDE;
            colisionSquare.col = pnj->position.col / SQUARE_SIDE;
            if(object_collision(board,colisionSquare) == 1)
                return 1;
        }
        board[pnj->boardPlace.row][pnj->boardPlace.col] = 0;
        board[pnj->position.row / SQUARE_SIDE][pnj->position.col / SQUARE_SIDE] = 1;
        pnj->boardPlace.row = pnj->position.row / SQUARE_SIDE;
        pnj->boardPlace.col = pnj->position.col / SQUARE_SIDE;
    }

    return 0;
}

int move_enemy(int board[MAX_ROWS][MAX_COLS], enemy *enm)
{
    int mewrow, newcol, topEdge, bottomEdge, leftEdge, rightEdge;
    square colisionSquare;

    // Definiendo nuevas posiciones
    switch (enm->direction)
    {
    case 'U':
        mewrow = enm->position.row - enm->velocity;
        newcol = enm->position.col;
        break;
    case 'D':
        mewrow = enm->position.row + enm->velocity;
        newcol = enm->position.col;
        break;
    case 'L':
        mewrow = enm->position.row;
        newcol = enm->position.col - enm->velocity;
        break;
    case 'R':
        mewrow = enm->position.row;
        newcol = enm->position.col + enm->velocity;
        break;
    }

    //Definimos los nuevos bordes de la hitbox
    topEdge = mewrow - enm->box.top;
    bottomEdge = mewrow + enm->box.bottom;
    leftEdge = newcol - enm->box.left;
    rightEdge = newcol + enm->box.right;

    if(enm->direction == 'U')
    {
        enm->position.col=newcol;
        if(topEdge<0)
        {
            enm->position.row = 1 + enm->box.top;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes superiors del personaje para generar colision.
        else if(board[define_square(topEdge , newcol).row][enm->boardPlace.col]>40)
        {
            enm->position.row = (SQUARE_SIDE * (define_square(topEdge,newcol).row+1)) + enm->box.top; //Se agrega un +1 al define_square porque necesitamos el borde inferior de la casilla de colision
            enm->colisionSquare.row = define_square(topEdge , newcol).row;
            enm->colisionSquare.col = enm->boardPlace.col;
            return 1;
        }
        else if(board[define_square(topEdge , newcol).row][define_square(topEdge , newcol-enm->box.left).col]>40)
        {
            enm->position.row = (SQUARE_SIDE * (define_square(topEdge,newcol).row+1)) + enm->box.top; //Se agrega un +1 al define_square porque necesitamos el borde inferior de la casilla de colision
            enm->colisionSquare.row = define_square(topEdge , newcol).row;
            enm->colisionSquare.col = define_square(topEdge , newcol-enm->box.left).col;
            return 1;
        }
        else if(board[define_square(topEdge , newcol).row][define_square(topEdge , newcol+enm->box.right).col]>40)
        {
            enm->position.row = (SQUARE_SIDE * (define_square(topEdge,newcol).row+1)) + enm->box.top; //Se agrega un +1 al define_square porque necesitamos el borde inferior de la casilla de colision
            enm->colisionSquare.row = define_square(topEdge , newcol).row;
            enm->colisionSquare.col = define_square(topEdge , newcol+enm->box.right).col;
            return 1;
        }
        else
            enm->position.row = mewrow;
    }
    else if(enm->direction == 'D')
    {
        enm->position.col=newcol;
        if(bottomEdge >= (SQUARE_SIDE*Game.gameRows))
        {
            enm->position.row = (SQUARE_SIDE*Game.gameRows) - enm->box.bottom-1;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
        else if(board[define_square(bottomEdge , newcol).row][enm->boardPlace.col]>40)
        {
            enm->position.row = (SQUARE_SIDE * (define_square(bottomEdge,newcol).row)) - enm->box.bottom - 1;
            enm->colisionSquare.row = define_square(bottomEdge , newcol).row;
            enm->colisionSquare.col = enm->boardPlace.col;
            return 1;
        }
        else if(board[define_square(bottomEdge , newcol).row][define_square(bottomEdge , newcol-enm->box.left).col]>40)
        {
            enm->position.row = (SQUARE_SIDE * (define_square(bottomEdge,newcol).row)) - enm->box.bottom - 1;
            enm->colisionSquare.row = define_square(bottomEdge , newcol).row;
            enm->colisionSquare.col = define_square(bottomEdge , newcol-enm->box.left).col;
            return 1;
        }
        else if(board[define_square(bottomEdge , newcol).row][define_square(bottomEdge , newcol+enm->box.right).col]>40)
        {
            enm->position.row = (SQUARE_SIDE * (define_square(bottomEdge,newcol).row)) - enm->box.bottom - 1;
            enm->colisionSquare.row = define_square(bottomEdge , newcol).row;
            enm->colisionSquare.col = define_square(bottomEdge , newcol+enm->box.right).col;
            return 1;
        }
        else
            enm->position.row = mewrow;
    }
    else if(enm->direction == 'L')
    {
        enm->position.row=mewrow;
        if(leftEdge < 0)
        {
            enm->position.col = 1 + enm->box.left;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
        else if(board[enm->boardPlace.row][define_square(mewrow , leftEdge).col]>40)
        {
            enm->position.col = (SQUARE_SIDE * (define_square(mewrow , leftEdge).col+1)) + enm->box.left; //Se agrega un +1 al define_square porque necesitamos el borde derecho de la casilla de colision
            enm->colisionSquare.row = enm->boardPlace.row;
            enm->colisionSquare.col = define_square(mewrow , leftEdge).col;
            return 1;
        }
        else if(board[define_square(mewrow-enm->box.top , leftEdge).row][define_square(mewrow , leftEdge).col]>40)
        {
            enm->position.col = (SQUARE_SIDE * (define_square(mewrow , leftEdge).col+1)) + enm->box.left; //Se agrega un +1 al define_square porque necesitamos el borde derecho de la casilla de colision
            enm->colisionSquare.row = define_square(mewrow-enm->box.top , leftEdge).row;
            enm->colisionSquare.col = define_square(mewrow , leftEdge).col;
            return 1;
        }
        else if(board[define_square(mewrow+enm->box.bottom , leftEdge).row][define_square(mewrow , leftEdge).col]>40)
        {
            enm->position.col = (SQUARE_SIDE * (define_square(mewrow , leftEdge).col+1)) + enm->box.left; //Se agrega un +1 al define_square porque necesitamos el borde derecho de la casilla de colision
            enm->colisionSquare.row = define_square(mewrow+enm->box.bottom , leftEdge).row;
            enm->colisionSquare.col = define_square(mewrow , leftEdge).col;
            return 1;
        }
        else
            enm->position.col = newcol;
    }
    else if(enm->direction == 'R')
    {
        enm->position.row=mewrow;
        if(rightEdge >= (SQUARE_SIDE*Game.gameCols))
        {
            enm->position.col = (SQUARE_SIDE*Game.gameCols) - enm->box.right - 1;
            return 1;
        }
        //Evaluamos si la casilla de destino es un obstaculo, evaluando los bordes inferiores del personaje para generar colision.
        else if(board[enm->boardPlace.row][define_square(mewrow , rightEdge).col]>40)
        {
            enm->position.col = (SQUARE_SIDE * (define_square(mewrow , rightEdge).col)) - enm->box.right - 1;
            enm->colisionSquare.row = enm->boardPlace.row;
            enm->colisionSquare.col = define_square(mewrow , rightEdge).col;
            return 1;
        }
        else if(board[define_square(mewrow-enm->box.top , rightEdge).row][define_square(mewrow , rightEdge).col]>40)
        {
            enm->position.col = (SQUARE_SIDE * (define_square(mewrow , rightEdge).col)) - enm->box.right - 1;
            enm->colisionSquare.row = define_square(mewrow-enm->box.top , rightEdge).row;
            enm->colisionSquare.col = define_square(mewrow , rightEdge).col;
            return 1;
        }
        else if(board[define_square(mewrow+enm->box.bottom , rightEdge).row][define_square(mewrow , rightEdge).col]>40)
        {
            enm->position.col = (SQUARE_SIDE * (define_square(mewrow , rightEdge).col)) - enm->box.right - 1;
            enm->colisionSquare.row = define_square(mewrow+enm->box.bottom , rightEdge).row;
            enm->colisionSquare.col = define_square(mewrow , rightEdge).col;
            return 1;
        }
        else
            enm->position.col = newcol;
    }

    // Alterando la posición del enemigo en la matriz
    if((enm->position.col / SQUARE_SIDE)!=enm->boardPlace.col)
    {
        if(board[enm->position.row / SQUARE_SIDE][enm->position.col / SQUARE_SIDE]!=0)
        {
            colisionSquare.row = enm->position.row / SQUARE_SIDE;
            colisionSquare.col = enm->position.col / SQUARE_SIDE;
        }
        board[enm->boardPlace.row][enm->boardPlace.col] = 0;
        board[enm->position.row / SQUARE_SIDE][enm->position.col / SQUARE_SIDE] = enm->type+2;
        enm->boardPlace.row = enm->position.row / SQUARE_SIDE;
        enm->boardPlace.col = enm->position.col / SQUARE_SIDE;
    }
    if((enm->position.row / SQUARE_SIDE)!=enm->boardPlace.row)
    {
        if(board[enm->position.row / SQUARE_SIDE][enm->position.col / SQUARE_SIDE]!=0)
        {
            colisionSquare.row = enm->position.row / SQUARE_SIDE;
            colisionSquare.col = enm->position.col / SQUARE_SIDE;
        }
        board[enm->boardPlace.row][enm->boardPlace.col] = 0;
        board[enm->position.row / SQUARE_SIDE][enm->position.col / SQUARE_SIDE] = enm->type+2;
        enm->boardPlace.row = enm->position.row / SQUARE_SIDE;
        enm->boardPlace.col = enm->position.col / SQUARE_SIDE;
    }

    // Comprobando colisiones con el jugador
    if(enemy_collision(*enm, player1))
    {
        return 3;
    }

    return 0;
}

square define_square(int filPixel, int colPixel)
{
    square position;

    position.row = filPixel / SQUARE_SIDE;
    position.col = colPixel / SQUARE_SIDE;

    return position;
}

obstacle power(int board[MAX_ROWS][MAX_COLS], character pnj)
{
    int i,j, startPlace=0, affectedSqares=0;
    int VOID_SQUARE, START_SQUARE_COLOR, END_SQUARE_COLOR, PAST_COLOR, NEW_COLOR;

    VOID_SQUARE = 0;
    START_SQUARE_COLOR = (pnj.powerType * 4) + 41;
    END_SQUARE_COLOR = START_SQUARE_COLOR + 3;
    PAST_COLOR = VOID_SQUARE;
    NEW_COLOR = START_SQUARE_COLOR;

    obstacle ice;
    ice.create=1;

    if(pnj.direction == 'D')
    {
        j=pnj.boardPlace.col;

        startPlace = define_square(pnj.position.row + pnj.box.bottom , pnj.position.col).row+1;
        ice.begin.row = startPlace;
        ice.begin.col = j;

        if(board[startPlace][j] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(i=startPlace; i<Game.gameRows; i++)
            if(affectedSqares==pnj.powerScope)
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

        startPlace = define_square(pnj.position.row - pnj.box.top , pnj.position.col).row-1;
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
            if(affectedSqares==pnj.powerScope)
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

        startPlace = define_square(pnj.position.row , pnj.position.col + pnj.box.right).col+1;
        ice.begin.row = i;
        ice.begin.col = startPlace;

        if(board[i][startPlace] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(j=startPlace; j<Game.gameCols; j++)
            if(affectedSqares==pnj.powerScope)
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

        startPlace = define_square(pnj.position.row , pnj.position.col - pnj.box.left).col-1;
        ice.begin.row = i;
        ice.begin.col = startPlace;

        if(board[i][startPlace] == END_SQUARE_COLOR)
        {
            NEW_COLOR = END_SQUARE_COLOR;
            PAST_COLOR = END_SQUARE_COLOR;
            ice.create=false;
        }

        for(j=startPlace; j>=0; j--)
            if(affectedSqares==pnj.powerScope)
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
    ice.remaining = ice.limit;
    return ice;
}

void manage_obstacles(int board[MAX_ROWS][MAX_COLS], obstacle *ice)
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
        if(ice->remaining>=3)
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
                ice->remaining--;
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
        else if(ice->remaining==2)
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
            ice->remaining--;
        }
        else if(ice->remaining==1)
        {
            board[ice->begin.row][ice->begin.col]=NUM4;
            ice->remaining--;
        }
        else if(ice->remaining == 0)
        {
            ice->possible = 1;
        }
    }
    else if(ice->limit == 2)
    {
        if(ice->remaining==2)
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
                ice->remaining--;
            }
        }
        else if(ice->remaining==1)
        {
            board[ice->begin.row][ice->begin.col]=NUM4;
            ice->remaining--;
        }
        else if(ice->remaining == 0)
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
            ice->remaining--;
        }
        if(ice->remaining == 0)
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

int manage_enemy(int board[MAX_ROWS][MAX_COLS], enemy *enm)
{
    char answerDirection;

    switch (enm->type)
    {
    case 0:
        //Movemos al enemigo en la direccion que tiene y vemos que ocurre.
        switch (move_enemy(board, enm))
        {
        case 1:
            enm->direction = doomie_movement(*enm);
            break;
        case 3: //Choque con el jugador
            if(player1.hits==0)
                return 1;
            break;
        }
        break;
    case 1:
        //Decidimos la dirección del movimiento del personaje
        if((al_get_timer_count(timer)%5) == 0)
        {
            answerDirection = best_to_pnj_movement(board, *enm, player1);

            if(answerDirection != enm->direction)
            {
                enm->Olddirection = enm->direction;
                enm->direction = answerDirection;
            }
        }

        //Movemos al enemigo en la direccion que tiene y vemos que ocurre.
        switch (move_enemy(board, enm))
        {
        case 1: //Choque con bloque
            enm->direction = enm->Olddirection;
            move_enemy(board, enm);
            break;
        case 3: //Choque con el jugador
            if(player1.hits==0)
                return 1;
            break;
        }
        break;
    case 2:
        if(enm->state == 0)
        {
            //Decidimos la dirección del movimiento del personaje
            enm->direction = to_pnj_movement(board, *enm, player1);

            //Movemos al enemigo en la direccion que tiene y vemos que ocurre.
            switch (move_enemy(board, enm))
            {
            case 1: //Choque con bloque
                enm->state = 1;
                enm->numSpriteFrames = 4;
                enm->spritecol = 0;
                enm->powerCount = 4;
                break;
            case 3: //Choque con el jugador
                if(player1.hits==0)
                    return 1;
                break;
            }
        }
        else if(enm->state == 1)
        {
            if(enm->powerCount!=1 && (board[enm->colisionSquare.row][enm->colisionSquare.col] != 0))
            {
                if((al_get_timer_count(timer)%20) == 0)
                {
                    board[enm->colisionSquare.row][enm->colisionSquare.col]--;
                    enm->powerCount--;
                }
            }
            else
            {
                enm->state = 0;
                enm->numSpriteFrames = 9;
                enm->spritecol = 0;
                board[enm->colisionSquare.row][enm->colisionSquare.col] = 0;
            }
        }
        break;
    }

    return 0;
}

int get_board(int board[MAX_ROWS][MAX_COLS], char numero[3])
{
    int i,j, normalObjectsCont=0, specialObjectsCont=0, enemiesCont=0;
    Game.totalNormalObjects=0;
    Game.totalSpecialObjects=0;
    Game.totalEnemies = 0;
    int foundedObjectType;
    char filename[21] = "levels/level";
    strcat(filename, numero);
    strcat(filename, ".txt");

    //Inicializamos objetos en 0
    for(int i=0; i<NORMAL_OBJECTS_TYPES; i++)
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
        if(board[i][j]>=31 && board[i][j]<=40) //Si es objeto especial lo cuenta
        {
            Game.totalSpecialObjects++;
        }
    }
    fclose(game);

    Game.normalObjects = malloc(sizeof(object)*Game.totalNormalObjects);
    Game.specialObjects = malloc(sizeof(object)*Game.totalSpecialObjects);
    Game.enemies = malloc(sizeof(enemy)*Game.totalEnemies);

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
        if(board[i][j]>=31 && board[i][j]<=40) //Si es objeto especial sacamos su informacion
        {
            Game.specialObjects[specialObjectsCont].position.row = i;
            Game.specialObjects[specialObjectsCont].position.col = j;
            Game.specialObjects[specialObjectsCont].type = board[i][j]-31;
            Game.specialObjects[specialObjectsCont].state = 0;
            Game.specialObjects[specialObjectsCont].active = false;
            switch (Game.specialObjects[specialObjectsCont].type)
            {
                case 0: // Botas
                    Game.specialObjects[specialObjectsCont].bmp = al_load_bitmap("./src/sprites/board/specialObjects/boots.png");
                    Game.specialObjects[specialObjectsCont].MAXpowerTimer = 5*FPS;
                    break;
                case 1: // Arco
                    Game.specialObjects[specialObjectsCont].bmp = al_load_bitmap("./src/sprites/board/specialObjects/bow.png");
                    Game.specialObjects[specialObjectsCont].MAXpowerTimer = 10*FPS;
                    break;
                case 2: // Pocion
                    Game.specialObjects[specialObjectsCont].bmp = al_load_bitmap("./src/sprites/board/specialObjects/pocion.png");
                    Game.specialObjects[specialObjectsCont].MAXpowerTimer = 0;
                    break;
                case 3: // Varita de roca
                    Game.specialObjects[specialObjectsCont].bmp = al_load_bitmap("./src/sprites/board/specialObjects/rock_wound.png");
                    Game.specialObjects[specialObjectsCont].MAXpowerTimer = 0;
                    break;
            }
            specialObjectsCont++;
        }
        if(board[i][j]>=2 && board[i][j]<=9) //Si es enemigo sacamos su informacion
        {
            Game.enemies[enemiesCont].boardPlace.row=i;
            Game.enemies[enemiesCont].boardPlace.col=j;
            Game.enemies[enemiesCont].position.row= i*SQUARE_SIDE + SQUARE_SIDE/2;
            Game.enemies[enemiesCont].position.col= j*SQUARE_SIDE + SQUARE_SIDE/2;
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
    enemy auxEnemy;
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
            Game.enemies[i].box.top=20;
            Game.enemies[i].box.bottom=20;
            Game.enemies[i].box.left=20;
            Game.enemies[i].box.right=20;
            Game.enemies[i].state=0;
            Game.enemies[i].numSpriteFrames=9;
            sprintf(Game.enemies[i].spriteName, "./src/sprites/enemies/enemy%d.png", Game.enemies[i].type);
            Game.enemies[i].sprite = al_load_bitmap(Game.enemies[i].spriteName);
            break;
        case 1: // Enemigo que te persigue inteligentemente
            Game.enemies[i].velocity=3;
            Game.enemies[i].box.top=20;
            Game.enemies[i].box.bottom=20;
            Game.enemies[i].box.left=20;
            Game.enemies[i].box.right=20;
            Game.enemies[i].state=0;
            Game.enemies[i].numSpriteFrames=9;
            sprintf(Game.enemies[i].spriteName, "./src/sprites/enemies/enemy%d.png", Game.enemies[i].type);
            Game.enemies[i].sprite = al_load_bitmap(Game.enemies[i].spriteName);
            break;
        case 2: // Enemigo que te persigue, si encuentra un obstáculo rompe el bloque de destino
            Game.enemies[i].velocity=3;
            Game.enemies[i].box.top=20;
            Game.enemies[i].box.bottom=20;
            Game.enemies[i].box.left=20;
            Game.enemies[i].box.right=20;
            Game.enemies[i].state=0;
            Game.enemies[i].numSpriteFrames=9;
            Game.enemies[i].direction = to_pnj_movement(board, Game.enemies[i], player1);
            sprintf(Game.enemies[i].spriteName, "./src/sprites/enemies/enemy%d.png", Game.enemies[i].type);
            Game.enemies[i].sprite = al_load_bitmap(Game.enemies[i].spriteName);
            break;
        }
    }


    // Configuraciones para mostrar o no los objetos normales iniciales
    for(i=0; i<NORMAL_OBJECTS_TYPES; i++)
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

    /* for(i=0; i<Game.totalSpecialObjects; i++)
    {
        printf("Objeto %d, fila %d, columna %d, tipo %d, estado %d\n\n", i, Game.specialObjects[i].position.row, Game.specialObjects[i].position.col, Game.specialObjects[i].type, Game.specialObjects[i].state);
    } */

    /* for(i=0; i<Game.totalEnemies; i++)
    {
        printf("Enemigo %d, fila %d, columna %d, tipo %d, Dirección %c, [%d][%d][%d][%d]\n\n", i, Game.enemies[i].boardPlace.row, Game.enemies[i].boardPlace.col, Game.enemies[i].type, Game.enemies[i].direction, Game.enemies[i].box.top, Game.enemies[i].box.bottom, Game.enemies[i].box.left, Game.enemies[i].box.right);
    } */

    return 0;
}

int object_collision(int board[MAX_ROWS][MAX_COLS], square colisionsquare)
{
    int i;

    // Objetons normales
    for(i=0; i<Game.totalNormalObjects; i++)
    {
        if((Game.normalObjects[i].position.row == colisionsquare.row) && (Game.normalObjects[i].position.col == colisionsquare.col))
        {
            Game.normalObjects[i].state = 1;
            Game.normalObjects[i].cont = OBJECT_TIMER_COUNT;
        }
    }

    // Objetos especiales
    for(i=0; i<Game.totalSpecialObjects; i++)
    {
        if((Game.specialObjects[i].position.row == colisionsquare.row) && (Game.specialObjects[i].position.col == colisionsquare.col))
        {
            Game.specialObjects[i].state = 1;
            Game.specialObjects[i].cont = OBJECT_TIMER_COUNT;
            Game.specialObjects[i].active = true;
            switch (Game.specialObjects[i].type) // Iniciamos contadores de objetos especiales
            {
            case 0: // Botas
                Game.specialObjects[i].powerTimer = Game.specialObjects[i].MAXpowerTimer;
                player1.velocity += 3;
                break;
            case 1: // Arco
                Game.specialObjects[i].powerTimer = Game.specialObjects[i].MAXpowerTimer;
                player1.powerScope += 4;
                break;
            case 2: // Pocion
                player1.hits++;
                break;
            case 3: // Varita de Roca
                player1.power_hability[1] = true;
                break;
            }
        }
    }
    return 0;
}

bool enemy_collision(enemy enemy, character pnj) {
    int pnjTop = pnj.position.row - pnj.box.top;
    int pnjBottom = pnj.position.row + pnj.box.bottom;
    int pnjLeft = pnj.position.col - pnj.box.left;
    int pnjRight = pnj.position.col + pnj.box.right;
    int enemyTop = enemy.position.row - enemy.box.top;
    int enemyBottom = enemy.position.row + enemy.box.bottom;
    int enemyLeft = enemy.position.col - enemy.box.left;
    int enemyRight = enemy.position.col + enemy.box.right;

    // Revisando colisiones con el método AABB
    if (pnjRight < enemyLeft || pnjLeft > enemyRight || pnjBottom < enemyTop || pnjTop > enemyBottom) {
        return false;
    }

    // Si se llega aqui es porque hubo colision
    if(player1.hitCooldown == 0)
    {
        player1.hits--;
        player1.hitCooldown = HIT_COOLDOWN;
    }
    return true;
}

bool manage_objects(int board[MAX_ROWS][MAX_COLS])
{
    int i, pasedTypesObjects=0;
    bool nextNormalObjectType=1;

    // Objetos normales
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
                Game.normalObjects[i].cont = OBJECT_TIMER_COUNT;
                board[Game.normalObjects[i].position.row][Game.normalObjects[i].position.col] = Game.normalObjects[i].type+21;
            }
        }
        else
            return 1;
    }

    //Manejando estados pasajeros
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
                Game.normalObjects[i].cont=OBJECT_TIMER_COUNT;
            }
        }
    }

    // Objetos especiales
    for(i=0; i<Game.totalSpecialObjects; i++)
    {
        //Manejamos estados pasajeros
        if(Game.specialObjects[i].state<3 && Game.specialObjects[i].state!=0)
        {
            if(Game.specialObjects[i].cont !=0)
                Game.specialObjects[i].cont--;
            else
                {switch (Game.specialObjects[i].state)
                {
                case 1:
                    Game.specialObjects[i].state++;
                    break;
                case 2:
                    Game.specialObjects[i].state++;
                    break;
                }
                Game.specialObjects[i].cont=OBJECT_TIMER_COUNT;
            }
        }
        //Manejando habilodades de los objetos especiales
        if(Game.specialObjects[i].active)
        {
            switch (Game.specialObjects[i].type) // Controlamos cada objeto segun su tipo
            {
            case 0:  // Botas
                if(Game.specialObjects[i].powerTimer > 0)
                    Game.specialObjects[i].powerTimer--;
                else if(Game.specialObjects[i].powerTimer == 0)
                {
                    player1.velocity -= 3;
                    Game.specialObjects[i].active = false;
                }
                break;
            case 1:  // Arco
                if(Game.specialObjects[i].powerTimer > 0)
                    Game.specialObjects[i].powerTimer--;
                else if(Game.specialObjects[i].powerTimer == 0)
                {
                    player1.powerScope -= 4;
                    Game.specialObjects[i].active = false;
                }
                break;
            }
        }
    }

    //Revisando si fueron recogidos, por accidente los objetos especiales
    for(i=0; i<Game.totalSpecialObjects; i++)
    {
        if(Game.specialObjects[i].state == 0 && board[Game.specialObjects[i].position.row][Game.specialObjects[i].position.col] == 0) //En caso de que por alguna razon pase un enemigo u objeto encima de este y quede el espacio vacio entonceas se regresa al estado original.
        {
            board[Game.specialObjects[i].position.row][Game.specialObjects[i].position.col] = Game.specialObjects[i].type + 31;
        }
    }

    return 0;
}

char doomie_movement(enemy enemy)
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

char to_pnj_movement(int board[MAX_ROWS][MAX_COLS], enemy enemy, character pnj)
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

char best_to_pnj_movement(int board[MAX_ROWS][MAX_COLS], enemy enemy , character pnj)
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

void read_score()
{
    int i, j;
    char aux[7];
    FILE *scoreFile;

    //Abriendo archivo
    if ((scoreFile = fopen("./src/score/score.txt", "r"))==NULL)
        {
            printf("Error al abrir el SCORE\n");
        }
    else
        printf("SCORE abierto\n");

    //Leyendo archivo
    for(i=0; i<MAX_LEVELS; i++)
    {
        fscanf(scoreFile, "%s", aux);
        fscanf(scoreFile, "%d", &levelScores[i].quantity);
        for(j=0; j<10; j++)
        {
            fscanf(scoreFile, "%s", levelScores[i].inputs[j].name);
            fscanf(scoreFile, "%d", &levelScores[i].inputs[j].score);
        }
    }


    //Imprimiendo leido
    /* for(i=0; i<MAX_LEVELS; i++)
    {
        printf("Nivel%d %d\n",i+1,  levelScores[i].quantity);
        for(j=0; j<10; j++)
        {
            printf("%s  %d\n", levelScores[i].inputs[j].name, levelScores[i].inputs[j].score);
        }
    } */

   fclose(scoreFile);
    return;
}

void update_score(int level, char name[11], int score)
{
    bool done = false;
    int i,j;

    //Guardamos el valor la estructura de niveles
    if(levelScores[level-1].quantity < MAX_LEVEL_INPUT)
    {
        strcpy(levelScores[level-1].inputs[levelScores[level-1].quantity].name, name);
        levelScores[level-1].inputs[levelScores[level-1].quantity].score = score;
        levelScores[level-1].quantity++;
    }
    else
    {
        for(i=0; i<10; i++)
            if(!done)
            {
                if(levelScores[level-1].inputs[i].score < score)
                {
                    strcpy(levelScores[level-1].inputs[i].name, name);
                    levelScores[level-1].inputs[i].score = score;
                    done = true;
                }
            }
    }

    // Ordenando inputs
    scoreInput auxInput;
    for(int i=0; i<MAX_LEVEL_INPUT-1; i++)
    for(int j=0; j<MAX_LEVEL_INPUT-i-1; j++)
        if(levelScores[level-1].inputs[j].score<levelScores[level-1].inputs[j+1].score)
        {
            auxInput = levelScores[level-1].inputs[j];
            levelScores[level-1].inputs[j] = levelScores[level-1].inputs[j+1];
            levelScores[level-1].inputs[j+1] = auxInput;
        }


    return;
}

void whrite_score()
{
    int i, j;
    char aux[7];
    FILE *scoreFile;

    //Abriendo archivo
    if ((scoreFile = fopen("./src/score/score.txt", "w"))==NULL)
        {
            printf("Error al abrir el SCORE\n");
        }
    else
        printf("SCORE abierto\n");

    //Escribiendo archivo
     for(i=0; i<MAX_LEVELS; i++)
    {
        fprintf(scoreFile,"Nivel%d %d\n",i+1,  levelScores[i].quantity);
        for(j=0; j<10; j++)
        {
           fprintf(scoreFile,"%s  %d\n", levelScores[i].inputs[j].name, levelScores[i].inputs[j].score);
        }
    }

    fclose(scoreFile);
    return;
}

void select_music(int code)
{
    if(gameMusic.code != code)
    {
        if (gameMusic.strem)
           al_destroy_audio_stream(gameMusic.strem);
        switch (code)
        {
            case 0:
                gameMusic.strem = al_load_audio_stream("./src/music/menu.mp3", 4, 2048);
                if (!gameMusic.strem) {
                    printf("Cancion menu.mp3 no pudo ser cargada");
                    return;
                }
                al_set_audio_stream_playmode(gameMusic.strem, ALLEGRO_PLAYMODE_LOOP);
                break;
            case 1:
                gameMusic.strem = al_load_audio_stream("./src/music/level.mp3", 4, 2048);
                if (!gameMusic.strem) {
                    printf("Cancion level.mp3 no pudo ser cargada");
                    return;
                }
                al_set_audio_stream_playmode(gameMusic.strem, ALLEGRO_PLAYMODE_LOOP);
                break;
            case 2:
                gameMusic.strem = al_load_audio_stream("./src/music/win.mp3", 4, 2048);
                if (!gameMusic.strem) {
                    printf("Cancion win.mp3 no pudo ser cargada");
                    return;
                }
                al_set_audio_stream_playmode(gameMusic.strem, ALLEGRO_PLAYMODE_ONCE);
                break;
            case 3:
                gameMusic.strem = al_load_audio_stream("./src/music/lose.mp3", 4, 2048);
                if (!gameMusic.strem) {
                    printf("Cancion lose.mp3 no pudo ser cargada");
                    return;
                }
                al_set_audio_stream_playmode(gameMusic.strem, ALLEGRO_PLAYMODE_ONCE);
                break;
        }
        /*Reproducir la cancion*/
        set_music_volume(gameConfig.MUSICVOLUME);
        al_attach_audio_stream_to_mixer(gameMusic.strem, al_get_default_mixer());
        al_set_audio_stream_playing(gameMusic.strem, !gameMusic.paused);
        gameMusic.code = code;
    }

    return;
}

void set_music_volume(float volume)
{
    gameConfig.MUSICVOLUME = volume;
    if (gameMusic.strem) {
        al_set_audio_stream_gain(gameMusic.strem, volume);
    }
}

void get_config()
{
    int i;
    char aux[12];
    FILE *configFile;

    //Abriendo archivo
    if ((configFile = fopen("./src/config/config.txt", "r"))==NULL)
        {
            printf("Error al abrir el archivo de configuracion\n");
        }
    else
        printf("Configuracion abierto\n");

    //Leyendo archivo

    // Izquierda
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%d", &gameConfig.LEFT);

    // Derecha
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%d", &gameConfig.RIGHT);

    // Arriba
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%d", &gameConfig.UP);

    // ABAJO
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%d", &gameConfig.DOWN);

    // Poder
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%d", &gameConfig.POWER);

    // Minimapa
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%d", &gameConfig.MINIMAP);

    // Volumen de la musica
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%f", &gameConfig.MUSICVOLUME);

    // Volumen Efectos de sonido
    fscanf(configFile, "%s", aux);
    fscanf(configFile, "%f", &gameConfig.SFXVOLUME);


    /* Imprimiendo leido
    Printf("UP: %d\n", gameConfig.UP);
    printf("DOWN: %d\n", gameConfig.DOWN);
    printf("LEFT: %d\n", gameConfig.LEFT);
    printf("RIGHT: %d\n", gameConfig.RIGHT);
    printf("POWER: %d\n", gameConfig.POWER);
    printf("MINIMAP: %d\n", gameConfig.MINIMAP);
    printf("MUSICVOLUME: %f\n", gameConfig.MUSICVOLUME);
    printf("SFXVOLUME: %f\n", gameConfig.SFXVOLUME); */

   fclose(configFile);
    return;
}

void whrite_config()
{
int i, j;
    char aux[7];
    FILE *configFile;

    //Abriendo archivo
    if ((configFile = fopen("./src/config/config.txt", "w"))==NULL)
        {
            printf("Error al abrir el archivo de configuracion\n");
        }
    else
        printf("Configuracion abierta\n");

    //Escribiendo archivo
    fprintf(configFile, "LEFT %d\n", gameConfig.LEFT);
    fprintf(configFile, "RIGHT %d\n", gameConfig.RIGHT);
    fprintf(configFile, "UP %d\n", gameConfig.UP);
    fprintf(configFile, "DOWN %d\n", gameConfig.DOWN);
    fprintf(configFile, "POWER %d\n", gameConfig.POWER);
    fprintf(configFile, "MINIMAP %d\n", gameConfig.MINIMAP);
    fprintf(configFile, "MUSICVOLUME %f\n", gameConfig.MUSICVOLUME);
    fprintf(configFile, "SFXVOLUME %f\n", gameConfig.SFXVOLUME);

    fclose(configFile);
    return;
}

void get_key_name(int x, char *aux) {
    switch (x) {
        case 1: sprintf(aux, "A"); break;
        case 2: sprintf(aux, "B"); break;
        case 3: sprintf(aux, "C"); break;
        case 4: sprintf(aux, "D"); break;
        case 5: sprintf(aux, "E"); break;
        case 6: sprintf(aux, "F"); break;
        case 7: sprintf(aux, "G"); break;
        case 8: sprintf(aux, "H"); break;
        case 9: sprintf(aux, "I"); break;
        case 10: sprintf(aux, "J"); break;
        case 11: sprintf(aux, "K"); break;
        case 12: sprintf(aux, "L"); break;
        case 13: sprintf(aux, "M"); break;
        case 14: sprintf(aux, "N"); break;
        case 15: sprintf(aux, "O"); break;
        case 16: sprintf(aux, "P"); break;
        case 17: sprintf(aux, "Q"); break;
        case 18: sprintf(aux, "R"); break;
        case 19: sprintf(aux, "S"); break;
        case 20: sprintf(aux, "T"); break;
        case 21: sprintf(aux, "U"); break;
        case 22: sprintf(aux, "V"); break;
        case 23: sprintf(aux, "W"); break;
        case 24: sprintf(aux, "X"); break;
        case 25: sprintf(aux, "Y"); break;
        case 26: sprintf(aux, "Z"); break;

        case 27: sprintf(aux, "0"); break;
        case 28: sprintf(aux, "1"); break;
        case 29: sprintf(aux, "2"); break;
        case 30: sprintf(aux, "3"); break;
        case 31: sprintf(aux, "4"); break;
        case 32: sprintf(aux, "5"); break;
        case 33: sprintf(aux, "6"); break;
        case 34: sprintf(aux, "7"); break;
        case 35: sprintf(aux, "8"); break;
        case 36: sprintf(aux, "9"); break;

        case 37: sprintf(aux, "PAD 0"); break;
        case 38: sprintf(aux, "PAD 1"); break;
        case 39: sprintf(aux, "PAD 2"); break;
        case 40: sprintf(aux, "PAD 3"); break;
        case 41: sprintf(aux, "PAD 4"); break;
        case 42: sprintf(aux, "PAD 5"); break;
        case 43: sprintf(aux, "PAD 6"); break;
        case 44: sprintf(aux, "PAD 7"); break;
        case 45: sprintf(aux, "PAD 8"); break;
        case 46: sprintf(aux, "PAD 9"); break;

        case 47: sprintf(aux, "F1"); break;
        case 48: sprintf(aux, "F2"); break;
        case 49: sprintf(aux, "F3"); break;
        case 50: sprintf(aux, "F4"); break;
        case 51: sprintf(aux, "F5"); break;
        case 52: sprintf(aux, "F6"); break;
        case 53: sprintf(aux, "F7"); break;
        case 54: sprintf(aux, "F8"); break;
        case 55: sprintf(aux, "F9"); break;
        case 56: sprintf(aux, "F10"); break;
        case 57: sprintf(aux, "F11"); break;
        case 58: sprintf(aux, "F12"); break;

        case 59: sprintf(aux, "ESCAPE"); break;
        case 60: sprintf(aux, "TILDE"); break;
        case 61: sprintf(aux, "MINUS"); break;
        case 62: sprintf(aux, "EQUALS"); break;
        case 63: sprintf(aux, "BACKSPACE"); break;
        case 64: sprintf(aux, "TAB"); break;
        case 65: sprintf(aux, "OPENBRACE"); break;
        case 66: sprintf(aux, "CLOSEBRACE"); break;
        case 67: sprintf(aux, "ENTER"); break;
        case 68: sprintf(aux, "SEMICOLON"); break;
        case 69: sprintf(aux, "QUOTE"); break;
        case 70: sprintf(aux, "BACKSLASH"); break;
        case 71: sprintf(aux, "BACKSLASH2"); break; /* < > | on UK/Germany keyboards */
        case 72: sprintf(aux, "COMMA"); break;
        case 73: sprintf(aux, "FULLSTOP"); break;
        case 74: sprintf(aux, "SLASH"); break;
        case 75: sprintf(aux, "SPACE"); break;

        case 76: sprintf(aux, "INSERT"); break;
        case 77: sprintf(aux, "DELETE"); break;
        case 78: sprintf(aux, "HOME"); break;
        case 79: sprintf(aux, "END"); break;
        case 80: sprintf(aux, "PGUP"); break;
        case 81: sprintf(aux, "PGDN"); break;
        case 82: sprintf(aux, "LEFT"); break;
        case 83: sprintf(aux, "RIGHT"); break;
        case 84: sprintf(aux, "UP"); break;
        case 85: sprintf(aux, "DOWN"); break;

        case 86: sprintf(aux, "PAD SLASH"); break;
        case 87: sprintf(aux, "PAD ASTERISK"); break;
        case 88: sprintf(aux, "PAD MINUS"); break;
        case 89: sprintf(aux, "PAD PLUS"); break;
        case 90: sprintf(aux, "PAD DELETE"); break;
        case 91: sprintf(aux, "PAD ENTER"); break;

        case 92: sprintf(aux, "PRINTSCREEN"); break;
        case 93: sprintf(aux, "PAUSE"); break;

        case 94: sprintf(aux, "ABNT C1"); break;
        case 95: sprintf(aux, "YEN"); break;
        case 96: sprintf(aux, "KANA"); break;
        case 97: sprintf(aux, "CONVERT"); break;
        case 98: sprintf(aux, "NOCONVERT"); break;
        case 99: sprintf(aux, "AT"); break;
        case 100: sprintf(aux, "CIRCUMFLEX"); break;
        case 101: sprintf(aux, "COLON2"); break;
        case 102: sprintf(aux, "KANJI"); break;

        case 103: sprintf(aux, "PAD EQUALS"); break;
        case 104: sprintf(aux, "BACKQUOTE"); break;
        case 105: sprintf(aux, "SEMICOLON2"); break;
        case 106: sprintf(aux, "COMMAND"); break;

        case 107: sprintf(aux, "BACK"); break;        /* Android back key */
        case 108: sprintf(aux, "VOLUME UP"); break;
        case 109: sprintf(aux, "VOLUME DOWN"); break;

        case 110: sprintf(aux, "SEARCH"); break;
        case 111: sprintf(aux, "DPAD CENTER"); break;
        case 112: sprintf(aux, "BUTTON X"); break;
        case 113: sprintf(aux, "BUTTON Y"); break;
        case 114: sprintf(aux, "DPAD UP"); break;
        case 115: sprintf(aux, "DPAD DOWN"); break;
        case 116: sprintf(aux, "DPAD LEFT"); break;
        case 117: sprintf(aux, "DPAD RIGHT"); break;
        case 118: sprintf(aux, "SELECT"); break;
        case 119: sprintf(aux, "START"); break;
        case 120: sprintf(aux, "BUTTON L1"); break;
        case 121: sprintf(aux, "BUTTON R1"); break;
        case 122: sprintf(aux, "BUTTON L2"); break;
        case 123: sprintf(aux, "BUTTON R2"); break;
        case 124: sprintf(aux, "BUTTON A"); break;
        case 125: sprintf(aux, "BUTTON B"); break;
        case 126: sprintf(aux, "THUMBL"); break;
        case 127: sprintf(aux, "THUMBR"); break;

        case 128: sprintf(aux, "UNKNOWN"); break;

        case 215: sprintf(aux, "MODIFIERS"); break;
        case 216: sprintf(aux, "LSHIFT"); break;
        case 217: sprintf(aux, "RSHIFT"); break;
        case 218: sprintf(aux, "LCTRL"); break;
        case 219: sprintf(aux, "RCTRL"); break;
        case 220: sprintf(aux, "ALT"); break;
        case 221: sprintf(aux, "ALTGR"); break;
        case 222: sprintf(aux, "LWIN"); break;
        case 223: sprintf(aux, "RWIN"); break;
        case 224: sprintf(aux, "MENU"); break;
        case 225: sprintf(aux, "SCROLLLOCK"); break;
        case 226: sprintf(aux, "NUMLOCK"); break;
        case 227: sprintf(aux, "CAPSLOCK"); break;

        default: sprintf(aux, "UNKNOWN"); break;
    }
}

// FUnciones gráficas
void draw_board_rectangle(int fila, int columna, ALLEGRO_COLOR color){
    al_draw_filled_rectangle((columna * SQUARE_SIDE), (fila * SQUARE_SIDE), ((columna + 1) * SQUARE_SIDE), ((fila + 1) * SQUARE_SIDE), color);
}

void draw_minimap(int board[MAX_ROWS][MAX_COLS])
{
    int i,j, startx, starty, miniLado, padding;
    ALLEGRO_BITMAP *faces_bmp;
    ALLEGRO_BITMAP *pnjFace_bmp;
    ALLEGRO_COLOR map_fondo, map_green, map_green2, map_blue, map_red, map_yellow, map_orange, map_gray, map_brown;
    map_fondo = al_map_rgba(0,0,0,100);
    map_green = al_map_rgba(0,50,0,100);
    map_green2 = al_map_rgba(0,70,0,100);
    map_blue = al_map_rgba(0,0,255,100);
    map_red = al_map_rgba(255,10,10,100);
    map_yellow = al_map_rgba(255,255,0,100);
    map_orange = al_map_rgba(255,69,0,100);
    map_gray = al_map_rgba(60,60,60,100);
    map_brown = al_map_rgba(139,69,19,100);

    if(Game.minimap)
    {
        faces_bmp = al_load_bitmap("./src/sprites/enemies/miniFaces.png");
        pnjFace_bmp = al_load_bitmap("./src/sprites/pnj/face.png");
        miniLado=32;
        starty = (WINDOW_HEIGHT-(miniLado*Game.gameRows))/2;
        startx = (WINDOW_WIDTH-(miniLado*Game.gameCols))/2;

        al_draw_filled_rectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, map_fondo);

        for(i=0; i<Game.gameRows; i++)
        {
            for(j=0; j<Game.gameCols; j++)
            {
                if(board[i][j]==0)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                else if(board[i][j]==1)
                    al_draw_scaled_bitmap(pnjFace_bmp, 0, 0, 20, 20, startx, starty, miniLado, miniLado, 0);
                else if(board[i][j]>=2 && board[i][j]<=20)
                {
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                    al_draw_scaled_bitmap(faces_bmp, 20*(board[i][j]-2), 0, 20, 20, startx, starty, miniLado, miniLado, 0);
                }
                else if(board[i][j]>=21 && board[i][j]<=30)
                {
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                    al_draw_filled_circle(startx + (miniLado/2), starty + (miniLado/2), (miniLado/2)-1, map_yellow);
                }
                else if(board[i][j]>=31 && board[i][j]<40)
                {
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                    al_draw_filled_circle(startx + (miniLado/2), starty + (miniLado/2), (miniLado/2)-1, map_orange);
                }
                else if(board[i][j]>=40 && board[i][j]<=44)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green2);
                else if(board[i][j]>=45 && board[i][j]<=48)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_gray);
                else if(board[i][j]==81)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_brown);
                startx += miniLado;
            }
            startx = (WINDOW_WIDTH-(miniLado*Game.gameCols))/2;
            starty += miniLado;
        }
    }
    else
    {
        miniLado= 8;
        padding = 10;
        starty = padding;
        startx = WINDOW_WIDTH - padding - (Game.gameCols*miniLado);

        al_draw_filled_rectangle(startx-padding, 0, WINDOW_WIDTH, (padding*2)+(Game.gameRows*miniLado), map_fondo);

        for(i=0; i<Game.gameRows; i++)
        {
            for(j=0; j<Game.gameCols; j++)
            {
                if(board[i][j]==0)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                else if(board[i][j]==1)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_blue);
                else if(board[i][j]>=2 && board[i][j]<=20)
                {
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                    al_draw_filled_rectangle(startx+1, starty+1, startx+miniLado-2, starty+miniLado-2, map_red);
                }
                else if(board[i][j]>=21 && board[i][j]<=30)
                {
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                    al_draw_filled_circle(startx + (miniLado/2), starty + (miniLado/2), (miniLado/2)-1, map_yellow);
                }
                else if(board[i][j]>=31 && board[i][j]<40)
                {
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green);
                    al_draw_filled_circle(startx + (miniLado/2), starty + (miniLado/2), (miniLado/2)-1, map_orange);
                }
                else if(board[i][j]>=40 && board[i][j]<=44)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_green2);
                else if(board[i][j]>=45 && board[i][j]<=48)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_gray);
                else if(board[i][j]==81)
                    al_draw_filled_rectangle(startx, starty, startx+miniLado, starty+miniLado, map_brown);
                startx += miniLado;
            }
            startx = WINDOW_WIDTH - padding - (Game.gameCols*miniLado);
            starty += miniLado;
        }
    }


    return;
}

void draw_pnj(character *pnj, ALLEGRO_BITMAP *image){
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

    /* al_draw_filled_rectangle(pnj->position.col-pnj->box.left - Game.mapColStart, pnj->position.row-pnj->box.top  - Game.mapRowStart, pnj->position.col+pnj->box.right -Game.mapColStart, pnj->position.row+pnj->box.bottom - Game.mapRowStart, color_purple); */

    if(player1.hitCooldown!=0) // "Animacion de golpe"
    {
        if(player1.hited)
            player1.hited = false;
        else
        {
            al_draw_bitmap_region(image, (spriteWidht*pnj->spritecol),(spriteHeight*spritefil),spriteWidht,spriteHeight, pnj->position.col-(spriteWidht/2) - Game.mapColStart , pnj->position.row-(spriteHeight/2) - Game.mapRowStart, 0);
            player1.hited = true;
        }
    }
    else //En caso de no ser golpeado
    {
        al_draw_bitmap_region(image, (spriteWidht*pnj->spritecol),(spriteHeight*spritefil),spriteWidht,spriteHeight, pnj->position.col-(spriteWidht/2) - Game.mapColStart , pnj->position.row-(spriteHeight/2) - Game.mapRowStart, 0);
    }

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
            al_draw_bitmap_region(grass, (frameCount*64), 0, 64, 64 , j*SQUARE_SIDE, i*SQUARE_SIDE, 0);
            frameCount = rand() % 4;
        }
    }

    // Seleccionamos el Display como el Backbuffer, ya no el birmap del fondo
    al_set_target_backbuffer(al_get_current_display());
    al_destroy_bitmap(grass);
    return;
}

void draw_board(int board[MAX_ROWS][MAX_COLS]){
    int i,j;
    bushBitmap = al_load_bitmap("./src/sprites/board/bush.png");
    rootBitmap = al_load_bitmap("./src/sprites/board/root.png");
    rockBitmap = al_load_bitmap("./src/sprites/board/rock.png");
    flowerBitmap = al_load_bitmap("./src/sprites/board/flowers.png");
    ALLEGRO_BITMAP *sparcleBitmap = al_load_bitmap("./src/sprites/effects/sparcle.png");
    int mapSpriteWidht = 64, mapSpriteHeight = 64;

    //Definiendo comienzo en X
    if((player1.position.col + (WINDOW_WIDTH/2)) > (Game.gameCols*SQUARE_SIDE))
        Game.mapColStart = (Game.gameCols * SQUARE_SIDE) - WINDOW_WIDTH;
    else if ((player1.position.col - (WINDOW_WIDTH/2)) < 0)
        Game.mapColStart = 0;
    else
        Game.mapColStart = player1.position.col - (WINDOW_WIDTH/2);
    //Definiendo comienzo en Y
    if((player1.position.row + (WINDOW_HEIGHT/2)) > (Game.gameRows*SQUARE_SIDE))
        Game.mapRowStart = (Game.gameRows * SQUARE_SIDE) - WINDOW_HEIGHT;
    else if ((player1.position.row - (WINDOW_HEIGHT/2)) < 0)
        Game.mapRowStart = 0;
    else
        Game.mapRowStart = player1.position.row - (WINDOW_HEIGHT/2);
    al_draw_bitmap_region(board_bitmap,Game.mapColStart, Game.mapRowStart, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0);

    Game.startSquare.col = Game.mapColStart / SQUARE_SIDE;
    Game.endSquare.col = (Game.mapColStart + WINDOW_WIDTH) / SQUARE_SIDE;
    Game.startSquare.row = Game.mapRowStart / SQUARE_SIDE;
    Game.endSquare.row = (Game.mapRowStart + WINDOW_HEIGHT) /SQUARE_SIDE;

    for(i=Game.startSquare.row; i<=Game.endSquare.row; i++)
    {
        for(j=Game.startSquare.col; j<=Game.endSquare.col; j++)
        {
            if(board[i][j]>=41 && board[i][j]<=44)
            {
                al_draw_bitmap_region(bushBitmap, (board[i][j]-42)*mapSpriteWidht, 0, mapSpriteWidht, mapSpriteHeight, j*SQUARE_SIDE - Game.mapColStart, i*SQUARE_SIDE - Game.mapRowStart, 0 );
            }
            if(board[i][j]>=45 && board[i][j]<=48)
            {
                al_draw_bitmap_region(rockBitmap, (board[i][j]-46)*mapSpriteWidht, 0, mapSpriteWidht, mapSpriteHeight, j*SQUARE_SIDE - Game.mapColStart, i*SQUARE_SIDE - Game.mapRowStart, 0 );
            }
            if(board[i][j]==81)
            {
                al_draw_bitmap_region(rootBitmap, (board[i][j]-81)*mapSpriteWidht, 0, mapSpriteWidht, mapSpriteHeight, j*SQUARE_SIDE - Game.mapColStart, i*SQUARE_SIDE - Game.mapRowStart, 0 );
            }
            /* if(board[i][j]==1)
            {
                draw_board_rectangle(i-Game.startSquare.row,j-Game.startSquare.col,color_blue);
            }*/
            /* if(board[i][j]==3)
            {
                draw_board_rectangle(i-Game.startSquare.row,j-Game.startSquare.col,color_green3);
            } */
        }
    }

    // Dibujando objetos
    for(i=0; i<Game.totalNormalObjects; i++)
    {
        switch (Game.normalObjects[i].state)
        {
        case -2:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*0, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.normalObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        case -1:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*1, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.normalObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        case 0:
            al_draw_bitmap_region(flowerBitmap, mapSpriteWidht*Game.normalObjects[i].type, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.normalObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        case 1:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*0, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.normalObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        case 2:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*1, 0, mapSpriteWidht, mapSpriteHeight, Game.normalObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.normalObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        }
    }

    // Dibujando especiales
    for(i=0; i<Game.totalSpecialObjects; i++)
    {
        switch (Game.specialObjects[i].state)
        {
        case 0:
            al_draw_bitmap(Game.specialObjects[i].bmp, Game.specialObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.specialObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0);
            break;
        case 1:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*0, 0, mapSpriteWidht, mapSpriteHeight, Game.specialObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.specialObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        case 2:
            al_draw_bitmap_region(sparcleBitmap, mapSpriteWidht*1, 0, mapSpriteWidht, mapSpriteHeight, Game.specialObjects[i].position.col*SQUARE_SIDE - Game.mapColStart, Game.specialObjects[i].position.row*SQUARE_SIDE - Game.mapRowStart, 0 );
            break;
        }
    }

    //Dibujando enemigos
    for(i=0; i<Game.totalEnemies; i++)
        if(Game.enemies[i].boardPlace.row>=Game.startSquare.row && Game.enemies[i].boardPlace.row<=Game.endSquare.row && Game.enemies[i].boardPlace.col>=Game.startSquare.col && Game.enemies[i].boardPlace.col<=Game.endSquare.col)
            draw_enemy(&Game.enemies[i]);

    //Dibujando al jugador
    draw_pnj(&player1, player_bitmap);

    al_destroy_bitmap(bushBitmap);
    al_destroy_bitmap(rootBitmap);
    al_destroy_bitmap(rockBitmap);
    al_destroy_bitmap(flowerBitmap);
    al_destroy_bitmap(sparcleBitmap);
}

void draw_enemy(enemy *enm)
{
    int spriteWidht = 64;
    int spriteHeight = 64;
    int spritefil;

    switch (enm->direction)
    {
    case 'U':
        spritefil = 0+4*(enm->state); //Al cambiar el estado cambia la fila del spritesheet
        break;
    case 'L':
        spritefil = 1+4*(enm->state);
        break;
    case 'D':
        spritefil = 2+4*(enm->state);
        break;
    case 'R':
        spritefil = 3+4*(enm->state);
        break;
    }
    if(al_get_timer_count(timer)%5 == 0)
    {
        if(enm->movement)
            enm->spritecol = (enm->spritecol + 1)%enm->numSpriteFrames;
        else
            enm->spritecol = 0;
    }

    /* al_draw_filled_rectangle(enm->position.col-enm->box.left - Game.mapColStart, enm->position.row-enm->box.top  - Game.mapRowStart, enm->position.col+enm->box.right -Game.mapColStart, enm->position.row+enm->box.bottom - Game.mapRowStart, color_purple); */

    al_draw_bitmap_region(enm->sprite, (spriteWidht*enm->spritecol),(spriteHeight*spritefil),spriteWidht,spriteHeight, enm->position.col-(spriteWidht/2) - Game.mapColStart , enm->position.row-(spriteHeight/2) - Game.mapRowStart, 0);

    return;
}

void draw_HUD()
{
    ALLEGRO_BITMAP *heart_bmp = al_load_bitmap("./src/sprites/HUD/heart.png");
    ALLEGRO_BITMAP *pergamino_bmp = al_load_bitmap("./src/sprites/HUD/Pergamino.png");
    ALLEGRO_BITMAP *flowers_bmp = al_load_bitmap("./src/sprites/board/flowers.png");
    ALLEGRO_BITMAP *powers_bmp = al_load_bitmap("./src/sprites/HUD/power_icons.png");
    int i, specialObjectx, specialObjecty, powersx, powersy, specialPercent, cantidadTiposObjNormales=0, contTipo=0, pergaminox, pergaminoy;
    specialObjectx = 5+64;
    specialObjecty = 5;
    powersx = 5;
    powersy = 5+64;

    // Dibujando vidas
    al_draw_scaled_bitmap(heart_bmp, 0, 0, al_get_bitmap_width(heart_bmp), al_get_bitmap_height(heart_bmp), 5, WINDOW_HEIGHT-5-al_get_bitmap_height(heart_bmp), 52, 52, 0);
    al_draw_textf(font, color_gray, 57, WINDOW_HEIGHT-25-al_get_bitmap_height(heart_bmp), ALLEGRO_ALIGN_LEFT, "%d", player1.hits);

    //Dibujando objetos normales
    for(i=0; i<NORMAL_OBJECTS_TYPES; i++) // Contanto la cantidad de tipos de objetos normales
    if(Game.numNormalObjects[i] !=0)
        cantidadTiposObjNormales++;
    int tipos[cantidadTiposObjNormales];

    for(i=0; i<NORMAL_OBJECTS_TYPES; i++) // Guardando en arreglo para seleccionar la parte adecuada del bitmap de objetos normales
    if(Game.numNormalObjects[i] !=0)
    {
        tipos[contTipo] = i;
        contTipo++;
    }

    // Se inicializa el desplazamiento del pergamino
    pergaminox = WINDOW_WIDTH - 10 - 32*(cantidadTiposObjNormales+1);
    pergaminoy = WINDOW_HEIGHT - (al_get_bitmap_height(pergamino_bmp)/2) - 10 ;
    contTipo = 0;


    // Dibujamos todas las partes del pergamino
    for(i=0; i<=cantidadTiposObjNormales; i++)
    {
        if(i==0) // Parte inicial
            al_draw_scaled_bitmap(pergamino_bmp, 64*0, 0, 64, al_get_bitmap_height(pergamino_bmp), pergaminox, pergaminoy, 32, (al_get_bitmap_height(pergamino_bmp)/2), 0);
        else if(i!=cantidadTiposObjNormales) // Partes intermedias
        {
            al_draw_scaled_bitmap(pergamino_bmp, 64*1, 0, 64, al_get_bitmap_height(pergamino_bmp), pergaminox, pergaminoy, 32, (al_get_bitmap_height(pergamino_bmp)/2), 0);
            if(tipos[contTipo] == Game.playingNormalObjectType)
            {
                al_draw_scaled_bitmap(flowers_bmp, 64*tipos[contTipo], 0, 64, 64, pergaminox, pergaminoy+13, 32, 32, 0);
                al_draw_rounded_rectangle(pergaminox, pergaminoy+13, pergaminox+32, pergaminoy+13+34, 3, 1, color_black, 1);
            }
            else if(tipos[contTipo] > Game.playingNormalObjectType)
                al_draw_scaled_bitmap(flowers_bmp, 64*tipos[contTipo], 0, 64, 64, pergaminox, pergaminoy+13, 32, 32, 0);
            else
            {
                al_draw_tinted_scaled_bitmap(flowers_bmp, color_black, 64*tipos[contTipo], 0, 64, 64, pergaminox, pergaminoy+12, 32, 32, 0);
            }
            contTipo++;
        }
        else // Parte final del pergamino
        {
            al_draw_scaled_bitmap(pergamino_bmp, 64*2, 0, 64, al_get_bitmap_height(pergamino_bmp), pergaminox, pergaminoy, 32, (al_get_bitmap_height(pergamino_bmp)/2), 0);
            al_draw_scaled_bitmap(flowers_bmp, 64*tipos[contTipo], 0, 64, 64, pergaminox, pergaminoy+13, 32, 32, 0);
            if(tipos[contTipo] == Game.playingNormalObjectType)
                al_draw_rounded_rectangle(pergaminox, pergaminoy+13, pergaminox+32, pergaminoy+13+34, 3, 1, color_black, 1);
        }

        pergaminox+=32;
    } // El pegamino tiene espacios de 64x64, pero lo dibujamos a la mitad de escala, en espacios de 32x32 para que no sea grande



    //Dibujando objetos especiales activos
    for(i=0; i<Game.totalSpecialObjects; i++)
    {
        if(Game.specialObjects[i].active && Game.specialObjects[i].MAXpowerTimer!=0)
        {
            specialPercent = Game.specialObjects[i].powerTimer*100/Game.specialObjects[i].MAXpowerTimer;

            al_draw_rectangle(specialObjectx, specialObjecty, specialObjectx + al_get_bitmap_width(Game.specialObjects[i].bmp), specialObjecty + al_get_bitmap_height(Game.specialObjects[i].bmp),color_gray,2);
            al_draw_filled_rectangle(specialObjectx, specialObjecty, specialObjectx + al_get_bitmap_width(Game.specialObjects[i].bmp)*((float)specialPercent/100), specialObjecty + al_get_bitmap_height(Game.specialObjects[i].bmp), al_map_rgba(30,30,30,30));
            al_draw_bitmap(Game.specialObjects[i].bmp, specialObjectx, specialObjecty, 0);

            specialObjectx+=al_get_bitmap_width(Game.specialObjects[i].bmp);
        }
    }
    specialObjectx = 5;

    //Dibujando poderes activos
    for(i=0; i<MAX_POWER_TYPES; i++)
    {
        if(player1.power_hability[i])
        {
            if(player1.powerType==i) // Dibujamos fondo del recuadro si el poder esta actuvi
                al_draw_filled_rectangle(powersx, powersy, powersx + 64, powersy + 64, al_map_rgba(0,0,0,100));
            al_draw_rectangle(powersx, powersy, powersx + 64, powersy + 64, color_gray, 2); // Borde del cuadro
            al_draw_bitmap_region(powers_bmp, 64*i, 0, 64, 64, powersx, powersy, 0); // Imagen de poder
            al_draw_textf(tinyFont, color_gray, powersx+64, powersy, ALLEGRO_ALIGN_RIGHT, "%d", i+1);
            powersy+=64;
        }
    }
    powersy = 5 + 64;


    al_destroy_bitmap(heart_bmp);
    al_destroy_bitmap(pergamino_bmp);
    al_destroy_bitmap(flowers_bmp);
    al_destroy_bitmap(powers_bmp);
    return;
}

/*Partes*/
int select_key(char *purpose, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    bool done = false;

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(window), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    al_draw_bitmap(screenshot,0,0,0);
    al_draw_filled_rectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, al_map_rgba(0,0,0,200));
    al_draw_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, color_gray, 7);
    al_draw_filled_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, al_map_rgba(20,20,20,50));
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, "Ingrese una tecla");
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 20, ALLEGRO_ALIGN_CENTER, "para asignar a:");
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, purpose);

    al_flip_display();

    while (!done)
    {
        al_wait_for_event(queue, ev); // Esperando a que ocurra un evento

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) // Si es un cierre de la ventana
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            return ev->keyboard.keycode;
        }

        //Dibujo
        al_draw_bitmap(screenshot,0,0,0);
        al_draw_filled_rectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, al_map_rgba(0,0,0,200));
        al_draw_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, color_gray, 7);
        al_draw_filled_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, al_map_rgba(20,20,20,50));
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, "Ingrese una tecla");
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 20, ALLEGRO_ALIGN_CENTER, "para asignar a:");
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, purpose);
        al_flip_display();
    }

    al_destroy_bitmap(screenshot);
}

int select_number(int min, int max, int def, char *purpose, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    bool done = false;
    char aux[5];
    int offset = (WINDOW_WIDTH/3)+20;
    float percent;
    int indicatorPosition;
    int i;
    char purposeStrung[30];
    sprintf(purposeStrung, "Seleccion %s", purpose);

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(window), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    percent = ((float)def / (float)(max-min));
    indicatorPosition = (WINDOW_WIDTH-(WINDOW_WIDTH/3)-20 - offset)*percent + offset;
    sprintf(aux, "%03d", def);
    al_draw_bitmap(screenshot,0,0,0);
    al_draw_filled_rectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, al_map_rgba(0,0,0,200));
    al_draw_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, color_gray, 7);
    al_draw_filled_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, al_map_rgba(20,20,20,50));
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, purposeStrung);
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, aux);

    // Dibujando barra de volumen
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/3, WINDOW_HEIGHT/2 - 20, ALLEGRO_ALIGN_LEFT, "-"); // Menos
    al_draw_text(tinyFont, color_gray, WINDOW_WIDTH-(WINDOW_WIDTH/3), WINDOW_HEIGHT/2 - 20, ALLEGRO_ALIGN_LEFT, "+"); // Max
    al_draw_line((WINDOW_WIDTH/3)+20, WINDOW_HEIGHT/2 - 20 + 18, WINDOW_WIDTH-(WINDOW_WIDTH/3)-20, WINDOW_HEIGHT/2 - 20 + 18, color_gray, 3); // Barra horizontal
    al_draw_line(indicatorPosition, WINDOW_HEIGHT/2 - 20 + 8, indicatorPosition, WINDOW_HEIGHT/2 - 20 + 28, color_gray, 3); // Indicador volumen (Se usa el porcentaje)
    al_flip_display();


    al_start_timer(timer);
    while (!done)
    {
        al_wait_for_event(queue, ev); // Esperando a que ocurra un evento

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) // Si es un cierre de la ventana
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_ENTER || ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                return def;
            else
                keys[ev->keyboard.keycode] = true;
        }
        else if (ev->type == ALLEGRO_EVENT_KEY_UP)
        {
            keys[ev->keyboard.keycode] = false;
        }
        else if (ev->type == ALLEGRO_EVENT_TIMER)
        {
            if (keys[ALLEGRO_KEY_UP] || keys[ALLEGRO_KEY_RIGHT])
            {
                if(def<max)
                    def++;
            }
            else if (keys[ALLEGRO_KEY_DOWN] || keys[ALLEGRO_KEY_LEFT])
            {
                if(def>min)
                    def--;
            }
        }

        //Dibujo
        percent = ((float)def / (float)(max-min));
        indicatorPosition = (WINDOW_WIDTH-(WINDOW_WIDTH/3)-20 - offset)*percent + offset;
        sprintf(aux, "%03d", def);
        al_draw_bitmap(screenshot,0,0,0);
        al_draw_filled_rectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, al_map_rgba(0,0,0,200));
        al_draw_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, color_gray, 7);
        al_draw_filled_rounded_rectangle(WINDOW_WIDTH/4, WINDOW_HEIGHT/4, 3*WINDOW_WIDTH/4, 3*WINDOW_HEIGHT/4, 5, 5, al_map_rgba(20,20,20,50));
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, purposeStrung);
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + FONT_SIZE*1.5 - 20, ALLEGRO_ALIGN_CENTER, aux);

        // Dibujando barra de volumen
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH/3, WINDOW_HEIGHT/2 - 20, ALLEGRO_ALIGN_LEFT, "-"); // Menos
        al_draw_text(tinyFont, color_gray, WINDOW_WIDTH-(WINDOW_WIDTH/3), WINDOW_HEIGHT/2 - 20, ALLEGRO_ALIGN_LEFT, "+"); // Max
        al_draw_line((WINDOW_WIDTH/3)+20, WINDOW_HEIGHT/2 - 20 + 18, WINDOW_WIDTH-(WINDOW_WIDTH/3)-20, WINDOW_HEIGHT/2 - 20 + 18, color_gray, 3); // Barra horizontal
        al_draw_line(indicatorPosition, WINDOW_HEIGHT/2 - 20 + 8, indicatorPosition, WINDOW_HEIGHT/2 - 20 + 28, color_gray, 3); // Indicador volumen (Se usa el porcentaje)
        al_flip_display();
    }

    for(i=0; i<ALLEGRO_KEY_MAX; i++) //Se "levantan" todas las teclas
        keys[i] = false;

    al_stop_timer(timer);
    al_destroy_bitmap(screenshot);
}

int name_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    select_music(0); //Musica de Menu
    bool done = false;
    char auxName[11];
    int namePosition=-1;
    auxName[0]='\0';
    ALLEGRO_BITMAP *name_bmap = al_load_bitmap("src/name/nameSelection.png");

    al_draw_bitmap(name_bmap, 0,0,0);
    al_flip_display();

    while (!done)
    {
        al_wait_for_event(queue, ev); /* Esperando a que ocurra un evento */

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /* Si es un cierre de la ventana */
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_CHAR) //En caso de algún movimiento
        {
            if(ev->keyboard.unichar > 32 && ev->keyboard.unichar < 127 && namePosition<9)
            {
                namePosition++;
                auxName[namePosition] = ev->keyboard.unichar;
                auxName[namePosition+1]='\0';
            }
            else if((ev->keyboard.unichar == 127 || ev->keyboard.unichar == 8) && namePosition>=0)
            {
                auxName[namePosition]='\0';
                namePosition--;
            }
            else if(ev->keyboard.unichar == 13 && namePosition!=-1)
            {
                strcpy(Game.userName, auxName);
                printf("Nombre elegido: %s\n\n", Game.userName);
                return 0;
            }
        }

        al_draw_bitmap(name_bmap, 0,0,0);
        al_draw_text(font, color_black, 500, 450, ALLEGRO_ALIGN_CENTER, auxName);
        al_flip_display();
    }

    al_destroy_bitmap(name_bmap);
}

int main_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    select_music(0); //Musica de Menu
    al_stop_timer(timer);
    bool done = false;
    ALLEGRO_BITMAP *jugar_bmap = al_load_bitmap("src/mainMenu/960x640/jugar.png");
    ALLEGRO_BITMAP *score_bmap = al_load_bitmap("src/mainMenu/960x640/score.png");
    ALLEGRO_BITMAP *salir_bmap = al_load_bitmap("src/mainMenu/960x640/salir.png");
    ALLEGRO_BITMAP *config_bmap = al_load_bitmap("src/mainMenu/960x640/configuracion.png");

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
                actualImage = (actualImage+1)%4;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_UP || ev->keyboard.keycode == ALLEGRO_KEY_LEFT)
            {
                actualImage = (actualImage-1)%4;
                if(actualImage == -1)
                    actualImage = 3;
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
                al_draw_bitmap(config_bmap,0,0,0);
                break;
            case 2:
                al_draw_bitmap(score_bmap,0,0,0);
                break;
            case 3:
                al_draw_bitmap(salir_bmap,0,0,0);
                break;
        }
        al_flip_display();
    }



    al_destroy_bitmap(jugar_bmap);
    al_destroy_bitmap(score_bmap);
    al_destroy_bitmap(salir_bmap);
    al_destroy_bitmap(config_bmap);
}

int pause_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    bool done = false;
    int actualImage = 0; //Continuar, Salir, Configuracion
    ALLEGRO_BITMAP *continuar_bmp = al_load_bitmap("src/pauseMenu/960x640/continuar.png");
    ALLEGRO_BITMAP *salir_bmap = al_load_bitmap("src/pauseMenu/960x640/salir.png");
    ALLEGRO_BITMAP *configuracion_bmap = al_load_bitmap("src/pauseMenu/960x640/configuracion.png");

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(WINDOW_WIDTH, WINDOW_HEIGHT);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(window), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    al_draw_bitmap(screenshot,0,0,0);
    al_draw_filled_rectangle(0,0, WINDOW_WIDTH, WINDOW_HEIGHT, al_map_rgba(0,0,0,200));
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
                al_resume_timer(timer);
                return actualImage;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                al_resume_timer(timer);
                return 0;
            }
        }

        // Dibujo
        al_draw_bitmap(screenshot,0,0,0);
        al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
        switch (actualImage)
        {
            case 0:
                al_draw_bitmap(continuar_bmp,0,0,0);
                break;
            case 1:
                al_draw_bitmap(salir_bmap,0,0,0);
                break;
            case 2:
                al_draw_bitmap(configuracion_bmap,0,0,0);
                break;
        }
        al_flip_display();
    }


    al_destroy_bitmap(screenshot);
    al_destroy_bitmap(continuar_bmp);
    al_destroy_bitmap(salir_bmap);
    al_destroy_bitmap(configuracion_bmap);
}

int game_over(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    al_stop_timer(timer);
    int i;
    bool done = false;
    ALLEGRO_BITMAP *gameOver_bmp = al_load_bitmap("src/gameOver/960x640/gameOver.png");
    ALLEGRO_BITMAP *gameOver1_bmp = al_load_bitmap("src/gameOver/960x640/gameOver1.png");

    select_music(3); // Musica de Derrota

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(WINDOW_COLS*SQUARE_SIDE, WINDOW_ROWS*SQUARE_SIDE);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(window), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    al_draw_bitmap(screenshot,0,0,0);
    al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
    al_draw_bitmap(gameOver_bmp,0,0,0);
    al_flip_display();
    al_rest(8);

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
                    al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
                    al_draw_bitmap(gameOver1_bmp,0,0,0);
                    al_flip_display();
                    al_rest(0.1);
                }
                else
                {
                    al_draw_bitmap(screenshot,0,0,0);
                    al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
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

    select_music(2); // Musica de Victoria

    // Capturar la pantalla actual
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(WINDOW_COLS*SQUARE_SIDE, WINDOW_ROWS*SQUARE_SIDE);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(window), 0, 0, 0);
    al_set_target_backbuffer(al_get_current_display());

    //Dibujo inicial
    for(i=0; i<=Game.score; i++)
    {
        sprintf(scoreText, "%04d", i);
        al_draw_bitmap(screenshot,0,0,0);
        al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
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
                    al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
                    al_draw_bitmap(win0_bmp,0,0,0);
                    al_draw_text(font,color_white, 275, 475,ALLEGRO_ALIGN_LEFT,scoreText);
                    al_flip_display();
                    al_rest(0.1);
                }
                else
                {
                    al_draw_bitmap(screenshot,0,0,0);
                    al_draw_filled_rectangle(0,0,al_get_display_width(window), al_get_display_height(window), al_map_rgba(0,0,0,200));
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
    int pergaminox = (WINDOW_WIDTH*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
    int pergaminoy = WINDOW_HEIGHT*3/8 + 20;
    int actualLevel = 1;
    int totalLevels = count_files_in_directory("./levels"); //MAX 8
    char auxLvl[3]; //Para dibujar números

    //Dibujo inicial
    al_draw_bitmap(level0_bmap,0,0,0);
    for(i=1; i<=totalLevels; i++)
    {
        sprintf(auxLvl, "%02d", i);
        if( (i%5) != 0)
        {
            if(i == actualLevel)
            {
                al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            else
            {
                al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            pergaminox += WINDOW_WIDTH/5;
        }
        else
        {
            pergaminox = (WINDOW_WIDTH*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
            pergaminoy += 20 + WINDOW_HEIGHT/4;
            if(i == actualLevel)
            {
                al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            else
            {
                al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
            }
            pergaminox += WINDOW_WIDTH/5;
        }
    }
    al_flip_display();
    //Corrección a ubicación inicial de pergaminos
    pergaminox = (WINDOW_WIDTH*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
    pergaminoy = WINDOW_HEIGHT*3/8 + 20;


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
                Game.levelNumber = actualLevel;
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
                    al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                else
                {
                    al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                pergaminox += WINDOW_WIDTH/5;
            }
            else
            {
                pergaminox = (WINDOW_WIDTH*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
                pergaminoy += 20 + WINDOW_HEIGHT/4;
                if(i == actualLevel)
                {
                    al_draw_bitmap(pergaminoSelected_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_white, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                else
                {
                    al_draw_bitmap(pergamino_bmap, pergaminox, pergaminoy, 0);
                    al_draw_text(font, color_gray, pergaminox + (al_get_bitmap_width(pergamino_bmap)/2), pergaminoy + (al_get_bitmap_height(pergamino_bmap)/2)-(FONT_SIZE), ALLEGRO_ALIGN_CENTER, auxLvl);
                }
                pergaminox += WINDOW_WIDTH/5;
            }
        }
        al_flip_display();
        //Corrección a ubicación inicial de pergaminos
        pergaminox = (WINDOW_WIDTH*3/10)-(al_get_bitmap_width(pergamino_bmap)/2)-80;
        pergaminoy = WINDOW_HEIGHT*3/8 + 20;
    }

    al_destroy_bitmap(level_bmap);
    al_destroy_bitmap(level0_bmap);
    al_destroy_bitmap(pergamino_bmap);
    al_destroy_bitmap(pergaminoSelected_bmap);
}

int config_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    ALLEGRO_BITMAP *config_bmap = al_load_bitmap("src/config/config.png");
    int i;
    bool done = false;
    int textx, texty, firstconfig, actualconfig, lastconfig;

    textx=150;
    texty=250;
    actualconfig=0;
    firstconfig=0;
    lastconfig=7;

    struct _configItems
    {
        char item[MAX_CONFIG_ITEMS][20];
        char value[MAX_CONFIG_ITEMS][13];
    } configItems;

    /*Llenamos la informacion de configItems que nos servira para imprimir esta unformacion en pantalla*/
    strcpy(configItems.item[0], "Izquierda");
    get_key_name(gameConfig.LEFT, configItems.value[0]);

    strcpy(configItems.item[1], "Derecha");
    get_key_name(gameConfig.RIGHT, configItems.value[1]);

    strcpy(configItems.item[2], "Arriba");
    get_key_name(gameConfig.UP, configItems.value[2]);

    strcpy(configItems.item[3], "Abajo");
    get_key_name(gameConfig.DOWN, configItems.value[3]);

    strcpy(configItems.item[4], "Activar poder");
    get_key_name(gameConfig.POWER, configItems.value[4]);

    strcpy(configItems.item[5], "Minimpa");
    get_key_name(gameConfig.MINIMAP, configItems.value[5]);

    strcpy(configItems.item[6], "Volumen musica");
    sprintf(configItems.value[6], "%d%%", (int)(gameConfig.MUSICVOLUME*100));

    strcpy(configItems.item[7], "Volumen SFX");
    sprintf(configItems.value[7], "%d%%", (int)(gameConfig.SFXVOLUME*100));

    //Dibujo inicial
    al_draw_bitmap(config_bmap,0,0,0);

    for(i=firstconfig; i<=lastconfig; i++)
    {
        if(i==actualconfig)
        {
            al_draw_text(tinyFont, color_white, textx, texty, ALLEGRO_ALIGN_LEFT, configItems.item[i]);
            al_draw_text(tinyFont, color_white, WINDOW_WIDTH-textx, texty, ALLEGRO_ALIGN_RIGHT, configItems.value[i]);
        }
        else
        {
            al_draw_text(tinyFont, color_gray, textx, texty, ALLEGRO_ALIGN_LEFT, configItems.item[i]);
            al_draw_text(tinyFont, color_gray, WINDOW_WIDTH-textx, texty, ALLEGRO_ALIGN_RIGHT, configItems.value[i]);
        }
        texty += TINY_FONT_SIZE*1.5;
    }
    // Posisiones iniciales de ubicacion del texto
    textx=150;
    texty=250;
    al_flip_display();

    // Cambio de configuraciones
    while (!done)
    {
        al_wait_for_event(queue, ev); // Esperando a que ocurra un evento

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) // Si es un cierre de la ventana
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN) //En caso de algún movimiento
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_TAB || ev->keyboard.keycode == ALLEGRO_KEY_RIGHT || ev->keyboard.keycode == ALLEGRO_KEY_DOWN)
            {
                actualconfig = (actualconfig+1) % (MAX_CONFIG_ITEMS);
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_LEFT || ev->keyboard.keycode == ALLEGRO_KEY_UP)
            {
                actualconfig = (actualconfig-1);
                if(actualconfig<0)
                    actualconfig=lastconfig;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                switch (actualconfig)
                {
                case 0:
                    gameConfig.LEFT = select_key(configItems.item[actualconfig], queue, ev, timer);
                    get_key_name(gameConfig.LEFT, configItems.value[0]);
                    break;
                case 1:
                    gameConfig.RIGHT = select_key(configItems.item[actualconfig], queue, ev, timer);
                    get_key_name(gameConfig.RIGHT, configItems.value[1]);
                    break;
                case 2:
                    gameConfig.UP = select_key(configItems.item[actualconfig], queue, ev, timer);
                    get_key_name(gameConfig.UP, configItems.value[2]);
                    break;
                case 3:
                    gameConfig.DOWN = select_key(configItems.item[actualconfig], queue, ev, timer);
                    get_key_name(gameConfig.DOWN, configItems.value[3]);
                    break;
                case 4:
                    gameConfig.POWER = select_key(configItems.item[actualconfig], queue, ev, timer);
                    get_key_name(gameConfig.POWER, configItems.value[4]);
                    break;
                case 5:
                    gameConfig.MINIMAP = select_key(configItems.item[actualconfig], queue, ev, timer);
                    get_key_name(gameConfig.MINIMAP, configItems.value[5]);
                    break;
                case 6:
                    gameConfig.MUSICVOLUME = (float)select_number(0, 100, (int)(gameConfig.MUSICVOLUME*100), "Volumen Musica", queue, ev, timer) / 100;
                    sprintf(configItems.value[6], "%d%%", (int)(gameConfig.MUSICVOLUME*100));
                    set_music_volume(gameConfig.MUSICVOLUME);
                    break;
                case 7:
                    gameConfig.SFXVOLUME = (float)select_number(0, 100, (int)(gameConfig.SFXVOLUME*100), "volumen SFX", queue, ev, timer) / 100;
                    sprintf(configItems.value[7], "%d%%", (int)(gameConfig.MUSICVOLUME*100));
                    break;
                }
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                done = true;
            }
        }

        // Dibujo
        al_draw_bitmap(config_bmap,0,0,0);

        for(i=firstconfig; i<=lastconfig; i++)
        {
            if(i==actualconfig)
            {
                al_draw_text(tinyFont, color_white, textx, texty, ALLEGRO_ALIGN_LEFT, configItems.item[i]);
                al_draw_text(tinyFont, color_white, WINDOW_WIDTH-textx, texty, ALLEGRO_ALIGN_RIGHT, configItems.value[i]);
            }
            else
            {
                al_draw_text(tinyFont, color_gray, textx, texty, ALLEGRO_ALIGN_LEFT, configItems.item[i]);
                al_draw_text(tinyFont, color_gray, WINDOW_WIDTH-textx, texty, ALLEGRO_ALIGN_RIGHT, configItems.value[i]);
            }
            texty += TINY_FONT_SIZE*1.5;
        }
        //Posisiones iniciales de ubicacion del texto
        textx=150;
        texty=250;
        al_flip_display();
    }

    al_destroy_bitmap(config_bmap);
    whrite_config();
    return 1;
}

int score_menu(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer)
{
    ALLEGRO_BITMAP *next_bmap = al_load_bitmap("src/score/next.png");
    ALLEGRO_BITMAP *before_nmp = al_load_bitmap("src/score/before.png");
    ALLEGRO_BITMAP *both_bmap = al_load_bitmap("src/score/both.png");
    int i;
    bool done = false;
    int textx = 160;
    int texty = 245;
    int scorex = 380;
    int scorey = 245;
    int actualLevel = 0;
    int lastlevel = 0;
    int totalLevels = count_files_in_directory("./levels");
    char auxscore[5]; //Para dibujar números

    //Dibujo inicial
    al_draw_bitmap(next_bmap,0,0,0);
    al_draw_textf(bigfont, color_white, 780, 50, ALLEGRO_ALIGN_CENTER, "Nivel %d",actualLevel+1); //Nivel

    // Dibujo de scores
    for(i=0; i<levelScores[actualLevel].quantity; i++)
    {
        if(i!=6)
        {
            al_draw_text(tinyFont, color_white, textx, texty, ALLEGRO_ALIGN_CENTRE, levelScores[actualLevel].inputs[i].name);
            al_draw_textf(tinyFont, color_white, scorex, scorey, ALLEGRO_ALIGN_CENTRE, "%04d", levelScores[actualLevel].inputs[i].score);
            texty += TINY_FONT_SIZE*2;
            scorey += TINY_FONT_SIZE*2;
        }
        else
        {
            textx = 630;
            texty = 245;
            scorex = 850;
            scorey = 245;
            al_draw_text(tinyFont, color_white, textx, texty, ALLEGRO_ALIGN_CENTRE, levelScores[actualLevel].inputs[i].name);
            al_draw_textf(tinyFont, color_white, scorex, scorey, ALLEGRO_ALIGN_CENTRE, "%04d", levelScores[actualLevel].inputs[i].score);
            texty += TINY_FONT_SIZE*2;
            scorey += TINY_FONT_SIZE*2;
        }
    }
    al_flip_display();
    // Correccion a ubicacion inicial de textos
    textx = 160;
    texty = 245;
    scorex = 380;
    scorey = 245;


    //Seleccion de nivel
    while (!done)
    {
        al_wait_for_event(queue, ev); /* Esperando a que ocurra un evento */

        if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) /* Si es un cierre de la ventana */
        {
            return -1;
        }
        else if(ev->type == ALLEGRO_EVENT_KEY_DOWN) //En caso de algún movimiento
        {
            if(ev->keyboard.keycode == ALLEGRO_KEY_TAB || ev->keyboard.keycode == ALLEGRO_KEY_RIGHT || ev->keyboard.keycode == ALLEGRO_KEY_DOWN)
            {
                lastlevel = actualLevel;
                actualLevel++;
                if(actualLevel>totalLevels-1)
                    actualLevel=totalLevels-1;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_LEFT || ev->keyboard.keycode == ALLEGRO_KEY_UP)
            {
                lastlevel = actualLevel;
                actualLevel--;
                if(actualLevel<0)
                    actualLevel=0;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_SPACE || ev->keyboard.keycode == ALLEGRO_KEY_ENTER || ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                return 0;
            }
        }

        //Dibujo inicial
        if(actualLevel==-1)
        {
            return 0;
        }
        else if(actualLevel == 0)
            al_draw_bitmap(next_bmap,0,0,0);
        else if(actualLevel == totalLevels-1)
            al_draw_bitmap(before_nmp,0,0,0);
        else
            al_draw_bitmap(both_bmap,0,0,0);

        al_draw_textf(bigfont, color_white, 780, 50, ALLEGRO_ALIGN_CENTER, "Nivel %d",actualLevel+1); //Nivel

        // Dibujo de scores
        for(i=0; i<levelScores[actualLevel].quantity; i++)
        {
            if(i!=6)
            {
                al_draw_text(tinyFont, color_white, textx, texty, ALLEGRO_ALIGN_CENTRE, levelScores[actualLevel].inputs[i].name);
                al_draw_textf(tinyFont, color_white, scorex, scorey, ALLEGRO_ALIGN_CENTRE, "%04d", levelScores[actualLevel].inputs[i].score);
                texty += TINY_FONT_SIZE*2;
                scorey += TINY_FONT_SIZE*2;
            }
            else
            {
                textx = 630;
                texty = 245;
                scorex = 850;
                scorey = 245;
                al_draw_text(tinyFont, color_white, textx, texty, ALLEGRO_ALIGN_CENTRE, levelScores[actualLevel].inputs[i].name);
                al_draw_textf(tinyFont, color_white, scorex, scorey, ALLEGRO_ALIGN_CENTRE, "%04d", levelScores[actualLevel].inputs[i].score);
                texty += TINY_FONT_SIZE*2;
                scorey += TINY_FONT_SIZE*2;
            }
        }
        // Correccion a ubicacion inicial de textos
        textx = 160;
        texty = 245;
        scorex = 380;
        scorey = 245;
        al_flip_display();

    }
    al_destroy_bitmap(next_bmap);
    al_destroy_bitmap(before_nmp);
    al_destroy_bitmap(both_bmap);
}

int game(int board[MAX_ROWS][MAX_COLS], ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_EVENT *ev, ALLEGRO_TIMER *timer, int level)
{
    bool done = false;
    bool win = false;
    int i, j;
    char level_string[2];
    sprintf(level_string, "%02d", level);
    printf("%s\n",level_string);

    // Inicializamos al jugador
    player1.direction='D';
    player1.box.left=25;
    player1.box.right=25;
    player1.box.top=25;
    player1.box.bottom=25;
    player1.hited = false;
    player1.hits = 1; // Asignando el numero de vidas del jugador a 1
    player1.direction='D';
    player1.velocity=7;
    player1.powerScope=5;
    player1.powerType = 0;
    player1.hitCooldown = 0;
    player1.hited = false;

    /*Iniciamos las habilidades del personaje*/
    player1.power_hability[0]=true;
    for(i=1;i<MAX_POWER_TYPES; i++)
        player1.power_hability[i]=false;

    select_music(1); // Musica de juego
    get_board(board, level_string);
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
    player1.position.row=(SQUARE_SIDE/2)+(player1.boardPlace.row*SQUARE_SIDE);
    player1.position.col=(SQUARE_SIDE/2)+(player1.boardPlace.col*SQUARE_SIDE);

    /*Variables utiles*/
    obstacle ice; //Casillas a cambiar de color por Power
    ice.possible=1;

    /*Bitmap personaje*/
    player_bitmap = al_load_bitmap("./src/sprites/pnj/spritesheet.png");

    /* Crear bitmap para el fondo del tablero */
    board_bitmap = al_create_bitmap(Game.gameCols * SQUARE_SIDE, Game.gameRows * SQUARE_SIDE);
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
                case 2:
                    switch (config_menu(queue, ev, timer))
                    {
                    case -1:
                        return -1;
                        break;
                    }
                    break;
                }
            }
            else if(ev->keyboard.keycode == gameConfig.POWER)
            {
                if(ice.possible)
                    ice = power(board, player1);
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_1)
            {
                if(player1.power_hability[0] && ice.possible)
                    player1.powerType=0;
            }
            else if(ev->keyboard.keycode == ALLEGRO_KEY_2)
            {
                if(player1.power_hability[1] && ice.possible)
                    player1.powerType=1;
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
            //Cooldowns
            if(player1.hitCooldown !=0) //Cooldown de golpe en caso de ser necesario
                player1.hitCooldown--;

            /*Limpiamos el Backbuffer*/
            al_clear_to_color(color_black);

            if (keys[gameConfig.UP])
            {
                if(move_player(board, &player1, 'U') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }
            else if (keys[gameConfig.DOWN])
            {
                if(move_player(board, &player1, 'D') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }
            else if (keys[gameConfig.LEFT])
            {
                if(move_player(board, &player1, 'L') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }
            else if (keys[gameConfig.RIGHT])
            {
                if(move_player(board, &player1, 'R') ==  1)
                {
                    al_stop_timer(timer);
                    done = true;
                    win = false;
                }
            }

            // Minimapa
            if (keys[gameConfig.MINIMAP])
                Game.minimap = true;
            else
                Game.minimap = false;

            if((!ice.possible) && ((al_get_timer_count(timer)%2) == 0)) //Este if se ejecuta cada 2 ticks sólo si NO es posible crear los obstaculos, es decir, si hay obstaculos pendientes por generar
                manage_obstacles(board,&ice);

            for(i=0; i<Game.totalEnemies; i++)
                if(manage_enemy(board, &Game.enemies[i]) == 1) //Si hay choque con un enemigo
                {
                    done = true;
                    win = false;
                }

            if(manage_objects(board) == 1) // Si no hay más objetos que recoger
            {
                al_stop_timer(timer);
                done = true;
                win = true;
            }

            /* Dibujar el tablero en el backbuffer*/
            draw_board(board);

            /*Dibujar minimapa*/
            draw_minimap(board);

            /*Dibujar HUD*/
            draw_HUD();

            /* Actualizar pantalla */
            al_flip_display();
        }
    }

    for(i=0; i<ALLEGRO_KEY_MAX; i++) //Se "levantan" todas las teclas
        keys[i] = false;

    for(int i=0; i<Game.totalEnemies; i++) //Eliminando birmaps de enemigos
        al_destroy_bitmap(Game.enemies[i].sprite);

    for(int i=0; i<Game.totalSpecialObjects; i++) //Eliminando birmaps de objetos especiales
        al_destroy_bitmap(Game.specialObjects[i].bmp);

    if(win)
    {
        Game.score = (-pow((al_get_timer_count(timer) / (float) FPS),2) / 10) + 10000;
        update_score(Game.levelNumber, Game.userName, Game.score);
        win_mwnu(queue,ev, timer);
        return 0;
    }
    else
    {
        game_over(queue, ev, timer);
        return 1;
    }
    return 0;
}