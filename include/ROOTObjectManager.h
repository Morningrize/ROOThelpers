#ifndef ROOTOBJECTMANAGER_H
#define ROOTOBJECTMANAGER_H

#include "TObject.h"
#include <concepts>
#include <memory>
#include <stdexcept>

#include <TDirectory.h>

namespace ROOThelpers {

template <typename T>
concept TObjectTDirectoryOwned =
    std::derived_from<T, TObject> &&requires(T *obj, TDirectory *dir) {
  { obj->SetDirectory(dir) }
  ->std::same_as<void>;
};

template <TObjectTDirectoryOwned T> class ROOTObjectManager {
private:
  std::unique_ptr<T> obj_;

public:
  template <typename... Args>
  explicit ROOTObjectManager(Args &&...args)
      : obj_(std::make_unique<T>(std::forward<Args>(args)...)) {
    obj_->SetDirectory(nullptr);
  }
  ROOTObjectManager() = delete;
  ROOTObjectManager(const ROOTObjectManager &) = delete;
  ROOTObjectManager &operator=(const ROOTObjectManager &) = delete;
  ROOTObjectManager(ROOTObjectManager &&) = default;
  ROOTObjectManager &operator=(ROOTObjectManager &&) = default;
  ~ROOTObjectManager() = default;

  T *operator->() { return obj_.get(); }
  const T *operator->() const { return obj_.get(); }

  T *get() { return obj_.get(); }

  [[nodiscard]] T *releaseToDirectory(TDirectory *dir = gDirectory) {
    if (!obj_) {
      throw std::invalid_argument("ROOTObjectManager: already transferred");
    }
    if (dir == nullptr) {
      throw std::invalid_argument("ROOTObjectManager: directory is null");
    }
    T *raw = obj_.get();
    obj_->SetDirectory(dir);
    obj_.release();
    return raw;
  }
};

} // namespace ROOThelpers
#endif