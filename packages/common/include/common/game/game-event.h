#pragma once

#include <typeindex>

class IGameEvent {
public:
  virtual ~IGameEvent() = default;
    virtual std::type_index GetType() const = 0;
};