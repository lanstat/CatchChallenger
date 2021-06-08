// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_SPRITE_HPP_
#define CLIENT_QTOPENGL_CORE_SPRITE_HPP_

#include <QGraphicsWidget>
#include <vector>

#include "Node.hpp"
#include "Stateful.hpp"

class Sprite : public Node, public Stateful {
 public:
  static Sprite* Create(QGraphicsItem* parent = nullptr);
  ~Sprite();

  struct State {
    int x;
    int y;
  };

 protected:
  virtual void* StoreInternal();
  virtual void UseInternal(void* data);

 private:
  explicit Sprite(QGraphicsItem* parent = nullptr);
};
#endif  // CLIENT_QTOPENGL_CORE_SPRITE_HPP_
