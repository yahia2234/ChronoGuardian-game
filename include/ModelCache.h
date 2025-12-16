#ifndef MODEL_CACHE_H
#define MODEL_CACHE_H

#include "Model.h"
#include <memory>
#include <string>
#include <unordered_map>

// Singleton class to cache loaded models for reuse
class ModelCache {
public:
  static ModelCache& getInstance() {
    static ModelCache instance;
    return instance;
  }
  
  // Get or load a model - returns shared pointer for efficient reuse
  std::shared_ptr<Model> getModel(const std::string& path) {
    auto it = cache.find(path);
    if (it != cache.end()) {
      return it->second; // Return cached model
    }
    
    // Load new model and cache it
    try {
      auto model = std::make_shared<Model>(path.c_str());
      cache[path] = model;
      return model;
    } catch (...) {
      return nullptr;
    }
  }
  
  // Clear all cached models (useful for level transitions)
  void clear() {
    cache.clear();
  }
  
  // Check if model is already cached
  bool isCached(const std::string& path) const {
    return cache.find(path) != cache.end();
  }

private:
  ModelCache() = default;
  ~ModelCache() = default;
  ModelCache(const ModelCache&) = delete;
  ModelCache& operator=(const ModelCache&) = delete;
  
  std::unordered_map<std::string, std::shared_ptr<Model>> cache;
};

#endif
