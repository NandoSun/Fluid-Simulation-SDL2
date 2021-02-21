#include <iostream>
#include <SDL2/SDL.h>
#include "Fluid.h"

bool running = true;

inline double map(double value, double min_in, double max_in, double min_out, double max_out) {
	return (value - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
}

void PollEvents (SDL_Event& event) {

    if (SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_QUIT: {
                running = false;
                std::cout << "Simulation was terminated via running variable\n";
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                switch (event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        //do sth
                        break;
                    }
                    case SDL_BUTTON_RIGHT: {
                        //do sth
                        break;
                    }
                }
            }
            case SDL_KEYDOWN: {
                
            }
		}
    }
}

int main(int argc, char** argv){

    int xRes = 640, yRes = 640;
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Event event;


    window = SDL_CreateWindow("Fluid Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    //background
    //SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    //SDL_RenderClear(renderer);
    int n = 64;
    //set up fuid
    Fluid fluid = Fluid(n);
    //draw fluid
    while (running){
        PollEvents(event);
        //process input
        //make caculations
        //convert values to color
        //RenderDrawColor & RenderClear
        fluid.draw(renderer, n);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}