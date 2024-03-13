#include <SDL.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 320;
const int screen_height = 480;
const int screen_bpp = 32;
const int x_space = 28;
const int y_space = 31;

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

// objetos ponteiros
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

// essa fun��o desenha o background na tela do jogo
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

// essa classe representa um ponto
class Point
{
    public:
    int x;
    int y;
};

Point a[4]; // array de objetos pra mover e desenhar as pe�as do jogo
Point b[4]; // objetos auxiliares pra copiar as posi��es dos objetos a[0],a[1],a[2],a[3]

// na nossa grade a gente tem
// 20 c�lulas na linha
// 10 c�lulas na coluna
const int M = 20;
const int N = 10;
// matriz do campo
int field[M][N] = {0};
// vari�veis
int color = (1+rand() % 7); // cor aleat�ria
int CurrentPiece = rand() % 7; // pe�a aleat�ria
float timer = 0;
float delay = 10;
int dx = 0;
bool IcanRotate = false;

// fun��o de colis�o
bool check()
{
    for(int i = 0; i < 4; i++)
    {
        if(a[i].x < 0 || a[i].x >= N || a[i].y >= M)
        {
            return false;
        }

        else if(field[a[i].y][a[i].x])
        {
            return false;
        }
    }

    return true;
}

// essa fun��o controla a f�sica do jogo
void MovePiece()
{
    Uint8 *tecla = SDL_GetKeyState(NULL);

    if(tecla[SDLK_RIGHT])
    {
        dx = 1;
    }

    else if(tecla[SDLK_LEFT])
    {
        dx = -1;
    }

    else if(tecla[SDLK_DOWN])
    {
        delay = 0.05;
    }

    // movimento
    for(int i = 0; i < 4; i++)
    {
        b[i] = a[i];
        a[i].x = a[i].x + dx;
    }

    // colis�o
    if(!check())
    {
        for(int i = 0; i < 4; i++)
        {
            a[i] = b[i];
        }
    }
}

// essa fun��o rotaciona/gira a pe�a
void RotatePiece()
{
    if(IcanRotate == true)
    {
        Point p = a[1]; // centro de rota��o

        for(int i = 0; i < 4; i++)
        {
            int x = a[i].y  - p.y;
            int y = a[i].x  - p.x;

            a[i].x = p.x - x;
            a[i].y = p.y + y;
        }

        if(!check())
        {
            for(int i = 0; i < 4; i++)
            {
                a[i] = b[i];
            }
        }
    }
}

// essa fun��o deleta as linhas horizontais do jogo quando elas s�o preenchidas
void CheckLine()
{
    int k = M - 1;
    for(int i = M - 1; i > 0; i--)
    {
        int count = 0;
        for(int j = 0; j < N; j++)
        {
            if(field[i][j])
            {
                count++;
            }

            field[k][j] = field[i][j];
        }
        if(count < N)
        {
            k--;
        }
    }

    dx = 0;
    IcanRotate = false;
}

// essa fun��o controla o tempo e a aleatoriedade das cores e pe�as do jogo
void Ticks()
{
    timer++;
    if(timer > delay)
    {
        for(int i = 0; i < 4; i++)
        {
            b[i] = a[i];
            a[i].y = a[i].y + 1;
        }

        if(!check())
        {
            for(int i = 0; i < 4; i++)
            {
                field[b[i].y][b[i].x] = color;
            }
            color = 1 + rand() % 7; // cor aleat�ria
            CurrentPiece = rand() % 7; // pe�a aleat�ria

            for(int i = 0; i < 4; i++)
            {
                a[i].x = figures[CurrentPiece][i] % 2;
                a[i].y = figures[CurrentPiece][i] / 2;
            }
        }

        timer = 0;
        delay = 10;
    }
}

// essa fun��o desenha as pe�as do jogo
void DrawPieces()
{
    for(int i = 0; i < 4; i++)
    {
        DrawCutImage(x_space+a[i].x*18,y_space+a[i].y*18,color*18,0,18,18,TilesImage);
    }
}

// o campo � quando as pe�as colidem em baixo do jogo
// essa fun��o desenha o campo/field se tiver o valor na matriz field
void DrawField()
{
    for(int i = 0 ; i < M ; i++)
    {
        for (int j = 0; j < N ; j++)
        {
            if (field[i][j]==0)
            {
                 continue;
            }

            DrawCutImage(x_space+j*18,y_space+i*18,field[i][j]*18,0,18,18,TilesImage);
        }
    }
}

// essa fun��o inicia a primeira pe�a do jogo
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

        if(evento.type == SDL_KEYDOWN)
        {
            switch(evento.key.keysym.sym)
            {
                case SDLK_UP: IcanRotate = true; break;
            }
        }
    }

    SDL_FillRect(tela, 0, 0xffffff); // desenha um ret�ngulo branco do tamanho da tela

    MovePiece();
    RotatePiece();
    Ticks();
    CheckLine();
    DrawBackground();
    DrawField();
    DrawPieces();

    SDL_Flip(tela); // atualiza a tela
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}

CloseFiles();
SDL_Quit();
return 0;
}

