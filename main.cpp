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

// use essa função pra desenhar uma imagem
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


SDL_Surface *iconImage = NULL;

SDL_Surface *backgroundImage = NULL;


// use essa função pra carregar arquivos
// nota: essa função só deve ser chamada no começo do programa
void LoadFiles()
{
    backgroundImage = SDL_LoadBMP("background.bmp");
}

// use essa função pra fechar arquivos
// nota: essa função só deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(iconImage);
    SDL_FreeSurface(backgroundImage);
}

// desenha o background na tela do jogo
void DrawBackground()
{
    DrawImage(0,0,backgroundImage);
}

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);

iconImage = SDL_LoadBMP("icon.bmp");
SDL_WM_SetIcon(iconImage, NULL);
SDL_putenv("SDL_VIDEO_WINDOW_POS=center");
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);
SDL_WM_SetCaption("Tetris", NULL);

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

    DrawBackground();

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
