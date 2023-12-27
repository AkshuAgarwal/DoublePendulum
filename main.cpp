#include <SDL3/SDL.h>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#define SDL_INIT_FLAG SDL_INIT_VIDEO
#define WINDOW_TITLE "Double Pendulum"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define PI 3.141592653589793238

void DrawCircle(SDL_Renderer *renderer, float c_x, float c_y, float r) {
  float prec = 0.01;
  float th = 0;
  float s_x, s_y, e_x, e_y;

  while (th < PI) {
    s_x = c_x + (r * cos(PI + th));
    s_y = c_y + (r * sin(PI + th));
    e_x = c_x + (r * cos(th));
    e_y = c_y + (r * sin(th));

    SDL_RenderLine(renderer, s_x, s_y, e_x, e_y);
    th += prec;
  }
}

int main(int argc, char *argv[]) {
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  /***** SDL Initialization *****/
  if (SDL_Init(SDL_INIT_FLAG) < 0) {
    std::cerr << "SDL_Init Failed! Error: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  window = SDL_CreateWindow(WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (!window) {
    std::cerr << "SDL_CreateWindow failed! Error in window creation\n";
    return EXIT_FAILURE;
  }

  renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "SDL_CreateRenderer failed! Error in renderer creation\n";
    return EXIT_FAILURE;
  }

  /***** Object Initialization *****/
  float M1 = 10, L1 = 150;
  float M2 = 10, L2 = 150;
  float x1 = 0, y1 = 0, th1 = PI / 3, w1 = 0, al1 = 0;
  float x2 = 0, y2 = 0, th2 = PI / 2, w2 = 0, al2 = 0;
  float C_X = (float)SCREEN_WIDTH / 2, C_Y = (float)SCREEN_HEIGHT / 2;
  float dt = 0.001;
  const float g = 9.81;
  std::vector<std::array<float, 2>> trails;

  /***** Main Loop *****/
  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // (th)eta can reach infinity on continuous rotation and result in memory
    // overflow. To overcome this effect, theta is reset to 0 after every 2 * PI
    // radians rotation (periodicity).
    if (std::abs(th1) > 2 * PI) {
      th1 -= (int)(th1 / (2 * PI)) * 2 * PI;
    }
    if (std::abs(th2) > 2 * PI) {
      th2 -= (int)(th2 / (2 * PI)) * 2 * PI;
    }

    al1 = dt *
          ((-g * ((2 * M1) + M2) * sin(th1)) - (M2 * g * sin(th1 - (2 * th2))) -
           (2 * sin(th1 - th2) * M2 *
            ((pow(w2, 2) * L2) + (pow(w1, 2) * L1 * cos(th1 - th2))))) /
          (L1 * ((2 * M1) + M2 - (M2 * cos((2 * th1) - (2 * th2)))));
    al2 = dt *
          (2 * sin(th1 - th2) *
           ((pow(w1, 2) * L1 * (M1 + M2)) + (g * (M1 + M2) * cos(th1)) +
            (pow(w2, 2) * L2 * M2 * cos(th1 - th2)))) /
          (L1 * ((2 * M1) + M2 - (M2 * cos((2 * th1) - (2 * th2)))));

    w1 += al1;
    w2 += al2;

    th1 += w1;
    th2 += w2;

    x1 = C_X + (L1 * sin(th1));
    y1 = C_Y + (L1 * cos(th1));
    x2 = x1 + (L2 * sin(th2));
    y2 = y1 + (L2 * cos(th2));

    std::cout << "x1: " << x1 << "\ty1: " << y1 << "\tw1: " << w1
              << "\tal1: " << al1 << "\tth1: " << th1 << '\n';
    std::cout << "x2: " << x2 << "\ty2: " << y2 << "\tw2: " << w2
              << "\tal2: " << al2 << "\tth2: " << th2 << "\n\n";

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderLine(renderer, C_X, C_Y, x1, y1);
    DrawCircle(renderer, x1, y1, M1);
    SDL_RenderLine(renderer, x1, y1, x2, y2);
    DrawCircle(renderer, x2, y2, M2);

    // trails
    trails.push_back({x2, y2});

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (auto i = trails.begin(); i != trails.end(); i++) {
      SDL_RenderPoint(renderer, (*i)[0], (*i)[1]);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(2);
  }

  /***** SDL Quit *****/
  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
  return EXIT_SUCCESS;
}
