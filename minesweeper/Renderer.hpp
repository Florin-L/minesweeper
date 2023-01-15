#pragma once
class Renderer {
 public:
  explicit Renderer(SDL_Renderer* renderer) : _renderer{renderer} {}
  ~Renderer() = default;

  Renderer(Renderer&& other) noexcept {
    _renderer = std::move(other._renderer);
  }

  Renderer& operator=(const Renderer&&) = delete;

  SDL_Renderer* raw_ptr() const { return _renderer.get(); }

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  void clear();
  void present();
  bool setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

 private:
  struct Deleter {
    void operator()(SDL_Renderer* p) {
      if (p != nullptr) {
        SDL_DestroyRenderer(p);
      }
    }
  };

  std::unique_ptr<SDL_Renderer, Deleter> _renderer{nullptr};
};
