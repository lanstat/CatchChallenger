// Copyright 2021 CatchChallenger
#include "GraphicRenderer.hpp"

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>

#include "Scene.hpp"

GraphicRenderer::GraphicRenderer() : QGraphicsItem() { scene_ = nullptr; }

GraphicRenderer::~GraphicRenderer() {}

void GraphicRenderer::SetScene(Scene *scene) { scene_ = scene; }

void GraphicRenderer::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget) {
  if (scene_ != nullptr) {
    scene_->paint(painter, option, widget);
  }
}

void GraphicRenderer::MousePressEventXY(const QPointF &point,
                                        bool &press_validated) {
  if (scene_) scene_->MousePressEventXY(point, press_validated);
}

void GraphicRenderer::MouseReleaseEventXY(const QPointF &point,
                                          bool &prev_validated) {
  if (scene_) scene_->MouseReleaseEventXY(point, prev_validated);
}

void GraphicRenderer::MouseMoveEventXY(const QPointF &point) {
  if (scene_) scene_->MouseMoveEventXY(point);
}

void GraphicRenderer::KeyPressEvent(QKeyEvent *event, bool &event_trigger) {
  if (scene_) scene_->KeyPressEvent(event, event_trigger);
}

void GraphicRenderer::KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) {
  if (scene_) scene_->KeyReleaseEvent(event, event_trigger);
}

QRectF GraphicRenderer::boundingRect() const { return QRectF(); }
