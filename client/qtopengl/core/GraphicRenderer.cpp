// Copyright 2021 CatchChallenger
#include "GraphicRenderer.hpp"

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <iostream>

#include "Scene.hpp"

GraphicRenderer::GraphicRenderer() : QGraphicsItem() { scene_ = nullptr; }

GraphicRenderer::~GraphicRenderer() {}

void GraphicRenderer::SetScene(Scene *scene) { scene_ = scene; }

void GraphicRenderer::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget) {
  if (scene_ != nullptr) {
    scene_->Paint(painter, option, widget);
  }
}

QRectF GraphicRenderer::boundingRect() const { return QRectF(); }
