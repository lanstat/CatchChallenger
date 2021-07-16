// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_COMBO_HPP_
#define CLIENT_QTOPENGL_UI_COMBO_HPP_

#include <QGraphicsItem>
#include <QPointF>

#include "../core/Node.hpp"
#include "Label.hpp"

namespace UI {
class Combo : public Node {
 public:
  static Combo *Create(QString pix, Node *parent = nullptr);
  ~Combo();

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;
  void SetCurrentIndex(uint8_t index);
  void SetOnSelectChange(std::function<void(uint8_t)> callback);
  uint8_t CurrentIndex();
  void AddItem(const QString &item);
  void SetSize(int width, int height);
  void RegisterEvents() override;
  void UnRegisterEvents() override;

  void Draw(QPainter *painter) override;

 protected:
  explicit Combo(QString pix, Node *parent = nullptr);

 private:
  QString background_;
  uint8_t current_index_;
  std::vector<QString> items_;
  Label *label_;
  std::function<void(uint8_t)> on_select_change_;

  void OnClick();
};
}  // namespace UI

#endif  // CLIENT_QTOPENGL_UI_COMBO_HPP_
