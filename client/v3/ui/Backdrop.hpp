// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_UI_BACKDROP_HPP_
#define CLIENT_V3_UI_BACKDROP_HPP_

#include <vector>

#include "../core/Node.hpp"

namespace UI {
class Backdrop : public Node {
 public:
  static Backdrop *Create(QColor color, Node *parent = nullptr);
  ~Backdrop();

  void Draw(QPainter *painter) override;

 protected:

  explicit Backdrop(QColor color, Node *parent = nullptr);

 private:
  QColor color_;
};
}  // namespace UI
#endif  // CLIENT_V3_UI_BACKDROP_HPP_
