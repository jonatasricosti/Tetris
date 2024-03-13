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

// use essa função pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}

// use essa função pra desenhar uma imagem cortada na tela
// nota: os valores de corte você pode pegar no gimp
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

// use essa função pra carregar arquivos
// nota: essa função só deve ser chamada no começo do programa
void LoadFiles()
{
    backgroundImage = SDL_LoadBMP("background.bmp");
    TilesImage = SDL_LoadBMP("tiles.bmp");
}

// use essa função pra fechar arquivos
// nota: essa função só deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(iconImage);
    SDL_FreeSurface(backgroundImage);
    SDL_FreeSurface(TilesImage);
}

// essa função desenha o background na tela do jogo
void DrawBackground()
{
    DrawImage(0,0,backgroundImage);
}

// essa estrutura representa uma matriz
// 7 peças cada peça é formada por 4 números
// veja o arquivo explicação.txt
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

Point a[4]; // array de objetos pra mover e desenhar as peças do jogo
Point b[4]; // objetos auxiliares pra copiar as posições dos objetos a[0],a[1],a[2],a[3]

// na nossa grade a gente tem
// 20 células na linha
// 10 células na coluna
const int M = 20;
const int N = 10;
// matriz do campo
int field[M][N] = {0};
// variáveis
int color = (1+rand() % 7); // cor aleatória
int CurrentPiece = rand() % 7; // peça aleatória
float timer = 0;
float delay = 10;
int dx = 0;
bool IcanRotate = false;

// função de colisão
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

// essa função controla a física do jogo
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

    // colisão
    if(!check())
    {
        for(int i = 0; i < 4; i++)
        {
            a[i] = b[i];
        }
    }
}

// essa função rotaciona/gira a peça
void RotatePiece()
{
    if(IcanRotate == true)
    {
        Point p = a[1]; // centro de rotação

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

// essa função deleta as linhas horizontais do jogo quando elas são preenchidas
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

// essa função controla o tempo e a aleatoriedade das cores e peças do jogo
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
            color = 1 + rand() % 7; // cor aleatória
            CurrentPiece = rand() % 7; // peça aleatória

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

// essa função desenha as peças do jogo
void DrawPieces()
{
    for(int i = 0; i < 4; i++)
    {
        DrawCutImage(x_space+a[i].x*18,y_space+a[i].y*18,color*18,0,18,18,TilesImage);
    }
}

// o campo é quando as peças colidem em baixo do jogo
// essa função desenha o campo/field se tiver o valor na matriz field
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

// essa função inicia a primeira peça do jogo
void Init()
{
    // primeira peça
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

    SDL_FillRect(tela, 0, 0xffffff); // desenha um retângulo branco do tamanho da tela

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

