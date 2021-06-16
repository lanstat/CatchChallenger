// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_CORE_SPRITE_HPP_
#define CLIENT_QTOPENGL_CORE_SPRITE_HPP_

#include <QGraphicsWidget>

#include "Node.hpp"
#include "Stateful.hpp"

class Sprite : public Node, public Stateful {
 public:
  static Sprite *Create(QGraphicsItem *parent = nullptr);
  ~Sprite();

  struct State {
    int x;
    int y;
  };

  void SetPixmap(const QString &file_name);
  void SetPixmap(const QPixmap pixmap);
  void SetSize(int width, int height);

  void MousePressEventXY(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEventXY(const QPointF &point) override;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;

 protected:
  virtual void *StoreInternal();
  virtual void UseInternal(void *data);

 private:
  QPixmap cache_;
  explicit Sprite(QGraphicsItem *parent = nullptr);
};
#endif  // CLIENT_QTOPENGL_CORE_SPRITE_HPP_
