#include "Fluid.h"
#include <iostream>
#include <SDL2/SDL.h>

bool running = true, mousePressed = false;
int xMouse = 0, yMouse = 0;
bool densityMode = true;

void PollEvents(SDL_Event &event)
{
    std::cout << "events entered\n";
    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            running = false;
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
            {
                SDL_GetMouseState(&xMouse, &yMouse);
                mousePressed = true;
                break;
            }
            case SDL_BUTTON_RIGHT:
            {
                //do sth
                break;
            }
            }
        }
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                running = false;
                break;
            }
            break;
        }
        case SDL_MOUSEMOTION:
            break;
        }
    }
}

int main(int argc, char **argv)
{

    int n = 64;
    int xRes = 640, yRes = 640;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Event event;

    window = SDL_CreateWindow("Fluid Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Fluid fluid = Fluid(n);
    std::cout << "start\n";
    while (running)
    {

        PollEvents(event);

        if (mousePressed)
        {
            if (densityMode)
                fluid.userInputSourceDensity(xMouse, yMouse);
            else
                fluid.userInputSourceVelocity(xMouse, yMouse);
            std::cout << "x: " << xMouse << " y: " << yMouse << std::endl;
            mousePressed = false;
        }

        fluid.calcDensity(n);
        fluid.calcVelocity(n);
        std::cout << fluid.getDensContent(5) << " " << fluid.getDensContent(26) << " " << fluid.getDensContent(41) << " " << fluid.getDensContent(60) << "\n";

        fluid.draw(renderer, n);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}