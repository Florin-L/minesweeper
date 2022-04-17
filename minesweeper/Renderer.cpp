// clang-format off
#include "pch.h"
#include "Renderer.hpp"
// clang-format on

void Renderer::clear() {
  if (_renderer) {
    SDL_RenderClear(_renderer.get());
  }
}

void Renderer::present() {
  if (_renderer) {
    SDL_RenderPresent(_renderer.get());
  }
}

bool Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  if (_renderer) {
    auto ret = SDL_SetRenderDrawColor(_renderer.get(), r, g, b, a);
    if (ret != 0) {
      return false;
    }
    return true;
  }
  return false;
}
