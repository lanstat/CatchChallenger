// Copyright 2021 CatchChallenger
#include "Scene.hpp"

#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <cstdarg>
#include <iostream>
#include <string>

#include "../ScreenInput.hpp"
#include "ActionManager.hpp"
#include "Node.hpp"

Scene::Scene(Node *parent) : Node(parent) {
  action_manager_ = ActionManager::GetInstance();
}

Scene::~Scene() {}

void Scene::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget) {
  action_manager_->Update();
  if (bounding_rect_.width() != widget->width() ||
      bounding_rect_.height() != widget->height()) {
    bounding_rect_ = QRectF(0, 0, widget->width(), widget->height());
    if (bounding_rect_.width() < 512) {
      OnScreenSD();
    } else if (bounding_rect_.width() < 1024) {
      OnScreenHD();
    } else {
      OnScreenHDR();
    }
  }
  Paint(painter, option, widget);
}

QRectF Scene::boundingRect() const { return bounding_rect_; }

void Scene::RegisterMouseListener(Node *node) {
  mouse_listener_.push_back(node);
}

void Scene::RegisterKeyboardListerner(Node *node) {
  keyboard_listener_.push_back(node);
}

void Scene::mousePressEventXY(const QPointF &p, bool &press,
                              bool &call_parent_class) {
  (void)call_parent_class;
  for (auto it = begin(mouse_listener_); it != end(mouse_listener_); ++it) {
    (*it)->MousePressEventXY(p, press);
  }
}

void Scene::mouseReleaseEventXY(const QPointF &p, bool &press,
                                bool &call_parent_class) {
  (void)call_parent_class;
  for (auto it = begin(mouse_listener_); it != end(mouse_listener_); ++it) {
    (*it)->MouseReleaseEventXY(p, press);
  }
}

void Scene::mouseMoveEventXY(const QPointF &p, bool &press,
                             bool &call_parent_class) {
  (void)call_parent_class;
  for (auto it = begin(mouse_listener_); it != end(mouse_listener_); ++it) {
    (*it)->MouseMoveEventXY(p);
  }
}

void Scene::Draw(QPainter *painter) { (void)painter; }

void Scene::MousePressEventXY(const QPointF &point, bool &press_validated) {
  (void)point;
  (void)press_validated;
}

void Scene::MouseReleaseEventXY(const QPointF &point, bool &prev_validated) {
  (void)point;
  (void)prev_validated;
}

void Scene::MouseMoveEventXY(const QPointF &point) { (void)point; }

void Scene::PrintError(const char* file, int line, const QString &message) {
  qDebug() << "[" << file << ":" << line <<"]" << message;
}
