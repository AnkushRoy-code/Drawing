#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL.h>
#include <vector>

enum DrawMode { DRAW_CIRCLE, DRAW_RECTANGLE, DRAW_SQUARE };

struct Shape {
  DrawMode mode;
  SDL_Point start;
  SDL_Point end;
  ImVec4 color;
};

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

void DrawShape(SDL_Renderer *renderer, const Shape &shape) {
  SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(shape.color.x * 255),
                         static_cast<Uint8>(shape.color.y * 255),
                         static_cast<Uint8>(shape.color.z * 255), 255);

  int w = shape.end.x - shape.start.x;
  int h = shape.end.y - shape.start.y;

  switch (shape.mode) {
  case DRAW_CIRCLE: {
    int radius = (int)sqrt(w * w + h * h) / 2;
    int centerX = shape.start.x + w / 2;
    int centerY = shape.start.y + h / 2;
    for (int w = 0; w < radius * 2; w++) {
      for (int h = 0; h < radius * 2; h++) {
        int dx = radius - w; // horizontal offset
        int dy = radius - h; // vertical offset
        if ((dx * dx + dy * dy) <= (radius * radius)) {
          SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
        }
      }
    }
    break;
  }
  case DRAW_RECTANGLE:
    SDL_RenderFillRect(renderer,
                       new SDL_Rect{shape.start.x, shape.start.y, w, h});
    break;
  case DRAW_SQUARE:
    int size = std::min(abs(w), abs(h));
    SDL_RenderFillRect(renderer,
                       new SDL_Rect{shape.start.x, shape.start.y, size, size});
    break;
  }
}

int main(int, char **) {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (!InitSDL(window, renderer))
    return -1;

  bool running = true;
  SDL_Event event;
  std::vector<Shape> shapes;
  ImVec4 drawColor = ImVec4(0.45f, 0.78f, 0.93f, 1.00f);
  DrawMode currentMode = DRAW_CIRCLE;
  bool drawing = false;
  SDL_Point startPoint;

  while (running) {
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        running = false;
      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        startPoint = {event.button.x, event.button.y};
        drawing = true;
      }
      if (event.type == SDL_MOUSEBUTTONUP &&
          event.button.button == SDL_BUTTON_LEFT) {
        if (drawing) {
          shapes.push_back({currentMode,
                            startPoint,
                            {event.button.x, event.button.y},
                            drawColor});
          drawing = false;
        }
      }
    }

    // Start ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // ImGui interface
    ImGui::Begin("Controls");
    if (ImGui::Button("Quit?")) {
      running = false;
    }
    ImGui::ColorEdit3("Draw Color", (float *)&drawColor);
    const char *items[] = {"Circle", "Rectangle", "Square"};
    int currentItem = (int)currentMode;
    ImGui::Combo("Mode", &currentItem, items, IM_ARRAYSIZE(items));
    currentMode = (DrawMode)currentItem;
    ImGui::End();

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 30, 30, 46, 255);
    SDL_RenderClear(renderer);

    // Draw shapes
    for (const auto &shape : shapes) {
      DrawShape(renderer, shape);
    }

    // Render ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // Present renderer
    SDL_RenderPresent(renderer);
  }

  CleanUp(window, renderer);
  return 0;
}
