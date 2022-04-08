
// clang-format off
#include "pch.h"
#include "assets.hpp"

// clang-format on

namespace fs = std::filesystem;

namespace {
SDL_Texture* createTextureFromImage(SDL_Renderer* renderer,
                                    const std::string& name,
                                    const fs::path& assetsDirPath,
                                    const std::string& fileName) {
  fs::path imagePath = assetsDirPath;
  imagePath /= fileName;

  SDL_Surface* surface = IMG_Load(imagePath.string().c_str());
  if (surface == nullptr) {
    return nullptr;
  }

  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return tex;
}
}  // namespace

bool GraphicsAssets::load() {
  fs::path imagePath;

  SDL_Texture* unexplored =
      createTextureFromImage(_renderer, "unexplored", _assetsDir,
                             "Minesweeper_LAZARUS_21x21_unexplored.png");
  if (!unexplored) {
    return false;
  }
  _graphics.emplace(kUnexplored,
                    std::make_shared<Texture>(unexplored, SDL_DestroyTexture));

  SDL_Texture* mine = createTextureFromImage(
      _renderer, "mine", _assetsDir, "Minesweeper_LAZARUS_21x21_mine.png");
  if (!mine) {
    return false;
  }
  _graphics.emplace(kMine, std::make_shared<Texture>(mine, SDL_DestroyTexture));

  SDL_Texture* mineHit =
      createTextureFromImage(_renderer, "mine_hit", _assetsDir,
                             "Minesweeper_LAZARUS_21x21_mine_hit.png");
  if (!mineHit) {
    return false;
  }
  _graphics.emplace(kMineHit,
                    std::make_shared<Texture>(mineHit, SDL_DestroyTexture));

  for (int i = 0; i <= 8; i++) {
    std::string fileName =
        "Minesweeper_LAZARUS_21x21_" + std::to_string(i) + ".png";

    std::string name = fmt::format("{}", i);
    SDL_Texture* tex =
        createTextureFromImage(_renderer, name, _assetsDir, fileName);
    if (!tex) {
      return false;
    }
    _graphics.emplace(name, std::make_shared<Texture>(tex, SDL_DestroyTexture));
  }

  return true;
}