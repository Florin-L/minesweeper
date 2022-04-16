#pragma once

const std::string kUnexplored("unexplored");
const std::string kMine("mine");
const std::string kMineHit("mine_hit");
const std::string k0("0");
const std::string k1("1");
const std::string k2("2");
const std::string k3("3");
const std::string k4("4");
const std::string k5("5");
const std::string k6("6");
const std::string k7("7");
const std::string k8("8");

class Texture {
 public:
  Texture(SDL_Texture* texture) : _tex(texture) {}
  ~Texture() { SDL_DestroyTexture(_tex); }

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other) {
    SDL_DestroyTexture(_tex);
    _tex = other._tex;
    other._tex = nullptr;
  }

  SDL_Texture* raw_ptr() const { return _tex; }

 private:
  SDL_Texture* _tex{nullptr};
};

/// @brief
///
///
class GraphicsAssets {
 public:
  GraphicsAssets(const std::filesystem::path& assetsDir)
      : _renderer(nullptr), _assetsDir(assetsDir) {}
  ~GraphicsAssets() {}

  GraphicsAssets(const GraphicsAssets&) = delete;
  GraphicsAssets& operator=(const GraphicsAssets&) = delete;

  void setRenderer(SDL_Renderer* renderer) { _renderer = renderer; }

  bool load();

  std::shared_ptr<Texture> get(const std::string& name) {
    auto it = _graphics.find(name);
    if (it != std::end(_graphics)) {
      return it->second;
    }
    return nullptr;
  }

 private:
  SDL_Renderer* _renderer;
  std::filesystem::path _assetsDir;
  std::unordered_map<std::string, std::shared_ptr<Texture>> _graphics;
};