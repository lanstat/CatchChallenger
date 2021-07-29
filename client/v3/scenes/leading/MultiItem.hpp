// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_SCENES_LEADING_MULTIITEM_HPP_
#define CLIENT_QTOPENGL_SCENES_LEADING_MULTIITEM_HPP_

#include "../../base/ConnectionInfo.hpp"
#include "../../core/Node.hpp"

namespace Scenes {
class MultiItem : public Node {
 public:
  static MultiItem *Create(const ConnectionInfo &connexionInfo);
  ~MultiItem();

  void Draw(QPainter *painter) override;
  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;
  ConnectionInfo Connection() const;

 private:
  explicit MultiItem(const ConnectionInfo &connexionInfo);
  QPixmap *cache_;
  ConnectionInfo m_connexionInfo;
  QString name_;

  void FetchName();
};
}  // namespace Scenes
#endif  // CLIENT_QTOPENGL_SCENES_LEADING_MULTIITEM_HPP_
