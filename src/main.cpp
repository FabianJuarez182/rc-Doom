#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <print.h>

#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;
bool playerWon = false;

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

void renderVictoryScreen() {
  // Borra la ventana
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // Carga la imagen de victoria
  SDL_Surface* victoryImage = IMG_Load("assets/WinPicture.png");
  if (!victoryImage) {
    printf("Failed to load victory image: %s\n", IMG_GetError());
    return;
  }

  // Convierte la imagen en una textura
  SDL_Texture* victoryTexture = SDL_CreateTextureFromSurface(renderer, victoryImage);
  if (!victoryTexture) {
    printf("Failed to create texture from victory image: %s\n", SDL_GetError());
    SDL_FreeSurface(victoryImage);
    return;
  }

  // Renderiza la textura de la imagen
  SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_RenderCopy(renderer, victoryTexture, NULL, &destRect);

  // Refresca la ventana
  SDL_RenderPresent(renderer);

  // Limpia la memoria
  SDL_DestroyTexture(victoryTexture);
  SDL_FreeSurface(victoryImage);
}

// Define una estructura para representar un nivel
struct Level {
    std::string mapFile;
    std::string name;
};

// Crea una lista de niveles disponibles
std::vector<Level> levels = {
    { "assets/map_1.txt", "Level 1" },
    { "assets/map_2.txt", "Level 2" },
    // Agrega más niveles según sea necesario
};


// Función para mostrar el menú y permitir al jugador elegir un nivel
int showMainMenu(SDL_Renderer* renderer) {
    int selectedOption = 0;
    int quitOption = levels.size(); // Opción para salir
    bool quit = false;

    TTF_Font* font = TTF_OpenFont("assets/OpenSans-Bold.ttf", 24);

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_Color welcomeColor = { 255, 255, 255 };
    SDL_Surface* welcomeSurface = TTF_RenderText_Solid(font, "Welcome to DOOM!", welcomeColor);
    SDL_Texture* welcomeTexture = SDL_CreateTextureFromSurface(renderer, welcomeSurface);

    int welcomeX = (windowWidth - welcomeSurface->w) / 2;
    int welcomeY = (windowHeight - welcomeSurface->h) / 2 - 50;

    while (!quit) {
        SDL_RenderClear(renderer);

        SDL_Rect welcomeRect = { welcomeX, welcomeY, welcomeSurface->w, welcomeSurface->h };
        SDL_RenderCopy(renderer, welcomeTexture, nullptr, &welcomeRect);

        int menuY = windowHeight / 2;

        for (int i = 0; i < levels.size(); ++i) {
            SDL_Color textColor = { 255, 255, 255 };

            if (i == selectedOption) {
                textColor.r = 255;
                textColor.g = 0;
                textColor.b = 0;
            }

            SDL_Surface* textSurface = TTF_RenderText_Solid(font, levels[i].name.c_str(), textColor);
            if (textSurface) {
                int textX = (windowWidth - textSurface->w) / 2;
                int textY = menuY + i * 40;

                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = { textX, textY, textSurface->w, textSurface->h };

                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }

        // Renderizar la opción "Salir"
        SDL_Color exitColor = { 255, 255, 255 };
        if (selectedOption == quitOption) {
            exitColor.r = 255;
            exitColor.g = 0;
            exitColor.b = 0;
        }
        SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "Salir", exitColor);
        int exitX = (windowWidth - exitSurface->w) / 2;
        int exitY = menuY + (levels.size() * 40); // Debajo de las otras opciones
        SDL_Texture* exitTexture = SDL_CreateTextureFromSurface(renderer, exitSurface);
        SDL_Rect exitRect = { exitX, exitY, exitSurface->w, exitSurface->h };
        SDL_RenderCopy(renderer, exitTexture, nullptr, &exitRect);
        SDL_FreeSurface(exitSurface);
        SDL_DestroyTexture(exitTexture);

        // Maneja la entrada del usuario
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP) {
                    selectedOption = (selectedOption - 1 + (quitOption + 1)) % (quitOption + 1);
                } else if (event.key.keysym.sym == SDLK_DOWN) {
                    selectedOption = (selectedOption + 1) % (quitOption + 1);
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    if (selectedOption == quitOption) {
                        quit = true; // Salir si se selecciona "Salir"
                    } else {
                        quit = true; // También salir si se selecciona un nivel
                    }
                }
            }
        }

        // Renderiza la pantalla del menú
        SDL_RenderPresent(renderer);
    }

    // libera la fuente TTF
    TTF_CloseFont(font);
    SDL_FreeSurface(welcomeSurface);
    SDL_DestroyTexture(welcomeTexture);


    return (selectedOption == quitOption) ? -1 : selectedOption; // Devolver -1 si se selecciona "Salir"
}



int main() {
  print("hello world");
  // Al principio de tu función main
  Uint32 lastTime = SDL_GetTicks();
  int frameCount = 0;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  ImageLoader::init();

  window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("+", "assets/wall3.png");
  ImageLoader::loadImage("-", "assets/wall1.png");
  ImageLoader::loadImage("|", "assets/wall2.png");
  ImageLoader::loadImage("*", "assets/wall4.png");
  ImageLoader::loadImage("g", "assets/wall5.png");

    // Inicializa las fuentes TrueType (TTF) de SDL
    if (TTF_Init() == -1) {
        printf("Failed to initialize SDL_ttf: %s\n", TTF_GetError());
        return 1;  // o maneja el error de la manera que prefieras
    }

  int selectedLevel = showMainMenu(renderer);
  if (selectedLevel == -1) {
      // Cerrar la ventana
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 0;
  }
  // Continuar con el nivel seleccionado
  Raycaster r = { renderer };
  r.load_map(levels[selectedLevel].mapFile);

  // Load and play background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("assets/ZombiesTheme.mp3");
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
  bool mouseCaptured = false;
  int previousMouseX = 0;

  while(running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }

      if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
          mouseCaptured = true;
          previousMouseX = event.button.x;
      }

      if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
          mouseCaptured = false;
      }

      if (mouseCaptured && event.type == SDL_MOUSEMOTION) {
          int mouseX = event.motion.x;

          // Calcula la diferencia entre la posición actual del mouse y el centro
          int deltaX = mouseX - previousMouseX;

          // Ajusta la velocidad de rotación según tus preferencias
          float rotationSpeed = 0.00005f;

          // Limita la rotación para que no exceda el valor máximo
          if (deltaX < 0) {
              r.player.a -= 3.14/24;
          } else{
              r.player.a += 3.14/24;
          }
      }
      if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym ){
          case SDLK_LEFT:
            r.player.a += 3.14/24;
            break;
          case SDLK_RIGHT:
            r.player.a -= 3.14/24;
            break;
        case SDLK_UP: {
            int nextX = r.player.x + speed * cos(r.player.a);
            int nextY = r.player.y + speed * sin(r.player.a);
            if (r.checkPlayerWin()) {
                playerWon = true;
                print("You won!");
            }
            if (!r.checkCollision(nextX, nextY)) {
                Mix_PlayChannel(-1, walkForwardSound, 0);
                r.player.x = nextX;
                r.player.y = nextY;
            }

            break;
        }
        case SDLK_DOWN: {
            int nextX = r.player.x - speed * cos(r.player.a);
            int nextY = r.player.y - speed * sin(r.player.a);
            if (r.checkPlayerWin()) {
                playerWon = true;
                print("You won!");
            }
            if (!r.checkCollision(nextX, nextY)) {
                Mix_PlayChannel(-1, walkBackwardSound, 0);
                r.player.x = nextX;
                r.player.y = nextY;
            }
            break;
        }
            default:
            break;
        }
      }
    }

    clear();
    draw_floor();

    if (playerWon) {
        renderVictoryScreen();
    } else {
        r.render(); // Renderiza la escena del juego normal
    }

    r.render();

    // render

    SDL_RenderPresent(renderer);
    // Al final del bucle, después de SDL_RenderPresent
      frameCount++;
      Uint32 currentTime = SDL_GetTicks();
      Uint32 elapsedTime = currentTime - lastTime;
    if (elapsedTime >= 1000) {
        // Si ha transcurrido más de un segundo
        int fps = frameCount * 1000 / elapsedTime;
        std::string title = "DOOM - FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(window, title.c_str());

        // Reiniciar contador de fotogramas
        frameCount = 0;
        lastTime = currentTime;
    }
  }
    if (walkForwardSound) {
        Mix_FreeChunk(walkForwardSound);
        walkForwardSound = nullptr;
    }

    if (walkBackwardSound) {
        Mix_FreeChunk(walkBackwardSound);
        walkBackwardSound = nullptr;
    }
    TTF_Quit();
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
