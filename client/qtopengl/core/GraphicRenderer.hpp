// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_GRAPHICRENDERER_HPP_
#define CLIENT_QTOPENGL_GRAPHICRENDERER_HPP_

#include <QGraphicsItem>
#include <QObject>

#include "Scene.hpp"

class GraphicRenderer : public QObject, public QGraphicsItem {
  Q_OBJECT

 public:
  GraphicRenderer();
  ~GraphicRenderer();

  void SetScene(Scene *scene);
  void MousePressEventXY(const QPointF &point, bool &press_validated);
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated);
  void MouseMoveEventXY(const QPointF &point);
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger);
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger);

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
  QRectF boundingRect() const override;

 private:
  Scene *scene_;
};

#endif  // CLIENT_QTOPENGL_GRAPHICRENDERER_HPP_
