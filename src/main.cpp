#include "../libs/imgui/imgui.h"
#include "../libs/imgui/imgui_impl_sdl2.h"
#include "../libs/imgui/imgui_impl_sdlrenderer2.h"
#include <SDL.h>
#include <vector>

// Initialization of SDL and ImGui
bool InitSDL(SDL_Window *&window, SDL_Renderer *&renderer) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    return false;
  window = SDL_CreateWindow("Drawing App", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
  if (!window)
    return false;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
    return false;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);
  return true;
}

void CleanUp(SDL_Window *window, SDL_Renderer *renderer) {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

// Drawing function
void Draw(SDL_Renderer *renderer, std::vector<SDL_Point> &points,
          SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
  for (const auto &point : points) {
    SDL_RenderDrawPoint(renderer, point.x, point.y);
  }
}

int main(int, char **) {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (!InitSDL(window, renderer))
    return -1;

  bool running = true;
  SDL_Event event;
  std::vector<SDL_Point> points;
  SDL_Color drawColor = {255, 0, 0, 255};

  while (running) {
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        running = false;
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        points.push_back({x, y});
      }
    }

    // Start ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // ImGui interface
    ImGui::Begin("Controls");
    ImGui::ColorEdit3("Draw Color", (float *)&drawColor);
    ImGui::End();

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw points
    Draw(renderer, points, drawColor);

    // Render ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // Present renderer
    SDL_RenderPresent(renderer);
  }

  CleanUp(window, renderer);
  return 0;
}
