#include <SDL.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 320;
const int screen_height = 480;
const int screen_bpp = 32;

// para o framerate
Uint32 start = 0;
const int fps = 30;
const int framerate =  1000/fps;

// use essa fun��o pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}


// use essa fun��o pra desenhar uma imagem cortada na tela
// nota: os valores de corte voc� pode pegar no gimp
void DrawCutImage(int x, int y, int cx, int cy, int cw, int ch, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_Rect corte;
    corte.x = cx;
    corte.y = cy;
    corte.w = cw;
    corte.h = ch;

    SDL_BlitSurface(image, &corte, tela, &mover);
}


SDL_Surface *iconImage = NULL;
SDL_Surface *backgroundImage = NULL;
SDL_Surface *TilesImage = NULL;


// use essa fun��o pra carregar arquivos
// nota: essa fun��o s� deve ser chamada no come�o do programa
void LoadFiles()
{
    backgroundImage = SDL_LoadBMP("background.bmp");
    TilesImage = SDL_LoadBMP("tiles.bmp");
}

// use essa fun��o pra fechar arquivos
// nota: essa fun��o s� deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(iconImage);
    SDL_FreeSurface(backgroundImage);
    SDL_FreeSurface(TilesImage);
}

// desenha o background na tela do jogo
void DrawBackground()
{
    DrawImage(0,0,backgroundImage);
}

// essa estrutura representa uma matriz
// 7 pe�as cada pe�a � formada por 4 n�meros
// veja o arquivo explica��o.txt
int figures[7][4]
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};

class Point
{
    public:
    int x;
    int y;
};

Point a[4]; // array de objetos pra mover e desenhar as pe�as do jogo
Point b[4]; // objetos auxiliares pra copiar as posis�es dos objetos a[0],a[1],a[2],a[3]


// na nossa grade a gente tem
// 20 c�lulas na linha
// 10 c�lulas na coluna
const int M = 20;
const int N = 10;
// matriz do campo
int field[M][N] = {0};


// essa fun��o move a pe�a pra baixo
// nota: esse c�digo � provis�rio
void MoveDown()
{
    static int timer = 0;
    static int delay = 10;

    timer++;
    if(timer > delay)
    {
        for(int i = 0; i < 4; i++)
        {
            a[i].y = a[i].y + 1;
        }
        timer = 0;
    }
}

// desenha a pe�a do jogo de modo aleat�rio
void DrawPieces()
{
    static int CurrentPiece = rand() % 7;   // pe�a aleat�ria
    static int color = (1+rand() % 7)*18; // cor aleat�ria

    const int x_space = 28;
    const int y_space = 31;

    static int check = 1;

    if(check == 1)
    {
        for(int i = 0; i < 4; i++)
        {
            a[i].x = figures[CurrentPiece][i] % 2;
            a[i].y = figures[CurrentPiece][i] / 2;
            check = 2;
        }
    }

    for(int i = 0; i < 4; i++)
    {
        DrawCutImage(x_space+a[i].x*18,y_space+a[i].y*18,color,0,18,18,TilesImage);
    }
}


// o campo � quando as pe�as colidem em baixo do jogo
// desenha o campo/field se tiver o valor na matriz field
void DrawField()
{
    for(int i=0; i < M ; i++)
    {
        for (int j=0; j < N ; j++)
        {
            if (field[i][j]==0)
            {
                 continue;
            }

            DrawCutImage(28+j*18,31+i*18,field[i][j]*18,0,18,18,TilesImage);
        }
    }
}

// inicia a primeira pe�a do jogo
void Init()
{
    // primeira pe�a
    const int firstFigure = rand() % 7;
    for (int i = 0; i < 4; i++)
    {
        a[i].x = figures[firstFigure][i] % 2;
        a[i].y = figures[firstFigure][i] / 2;
    }
}

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);

iconImage = SDL_LoadBMP("icon.bmp");
SDL_WM_SetIcon(iconImage, NULL);
SDL_putenv("SDL_VIDEO_WINDOW_POS=center");
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);
SDL_WM_SetCaption("Tetris", NULL);

Init();
LoadFiles();


// game loop
while(executando)
{
    start = SDL_GetTicks();
    while(SDL_PollEvent(&evento))
    {
        // clicou no x da janela
        if(evento.type == SDL_QUIT)
        {
            executando = false; // fecha o programa
        }
    }

    SDL_FillRect(tela, 0, 0xffffff);

    MoveDown();

    DrawBackground();
    DrawPieces();
    DrawField();

    SDL_Flip(tela);
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}



CloseFiles();
SDL_Quit();
return 0;
}
