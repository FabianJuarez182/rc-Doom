#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <print.h>

#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;

// Declarar variables para los efectos de sonido
Mix_Chunk* walkForwardSound = nullptr;
Mix_Chunk* walkBackwardSound = nullptr;

void clear() {
  SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
  SDL_RenderClear(renderer);
}

void draw_floor() {
  // floor color
  SDL_SetRenderDrawColor(renderer, 112, 122, 122, 255);
  SDL_Rect rect = {
    SCREEN_WIDTH, 
    SCREEN_HEIGHT / 2,
    SCREEN_WIDTH,
    SCREEN_HEIGHT / 2
  };
  SDL_RenderFillRect(renderer, &rect);
}

int main() {
  print("hello world");

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  ImageLoader::init();

  window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("+", "assets/wall3.png");
  ImageLoader::loadImage("-", "assets/wall1.png");
  ImageLoader::loadImage("|", "assets/wall2.png");
  ImageLoader::loadImage("*", "assets/wall4.png");
  ImageLoader::loadImage("g", "assets/wall5.png");

  Raycaster r = { renderer };
  r.load_map("assets/map.txt");

  // Load and play background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("assets/8-bit-halloween-story-166454.mp3");
  // Cargar efectos de sonido
    walkForwardSound = Mix_LoadWAV("assets/walk_forward.mp3");
    walkBackwardSound = Mix_LoadWAV("assets/walk_forward.mp3");

    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1); // -1 means loop indefinitely
    } else {
        printf("Failed to load background music: %s\n", Mix_GetError());
    }
    // Verificar si se cargaron los efectos de sonido correctamente
    if (!walkForwardSound || !walkBackwardSound) {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
    }
  bool running = true;
  int speed = 10;
  while(running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym ){
          case SDLK_LEFT:
            r.player.a += 3.14/24;
            break;
          case SDLK_RIGHT:
            r.player.a -= 3.14/24;
            break;
          case SDLK_UP:
            r.player.x += speed * cos(r.player.a);
            r.player.y += speed * sin(r.player.a);
            Mix_PlayChannel(-1, walkForwardSound, 0); // Reproduce una vez
            break;
          case SDLK_DOWN:
            r.player.x -= speed * cos(r.player.a);
            r.player.y -= speed * sin(r.player.a);
            Mix_PlayChannel(-1, walkBackwardSound, 0); // Reproduce una vez
            break;
           default:
            break;
        }
      }
    }

    clear();
    draw_floor();

    r.render();

    // render

    SDL_RenderPresent(renderer);
  }
    if (walkForwardSound) {
        Mix_FreeChunk(walkForwardSound);
        walkForwardSound = nullptr;
    }

    if (walkBackwardSound) {
        Mix_FreeChunk(walkBackwardSound);
        walkBackwardSound = nullptr;
    }
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
