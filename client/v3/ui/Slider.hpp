// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_UI_SLIDER_HPP_
#define CLIENT_V3_UI_SLIDER_HPP_

#include "../core/Node.hpp"

namespace UI {
class Slider : public Node {
 public:
  static Slider *Create(Node *parent = nullptr);
  ~Slider();

  void SetValue(qreal value);
  void SetMaximum(qreal value);
  qreal Value();
  qreal Maximum();

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;

  void Draw(QPainter *painter) override;
  void RegisterEvents() override;
  void UnRegisterEvents() override;
  void SetOnValueChange(std::function<void(qreal)> callback);

 private:
  bool is_slider_pressed_;
  QRectF slider_rect_;
  QPixmap slider_rail_;
  QPixmap slider_;
  bool is_horizontal_;
  std::function<void(qreal)> on_value_change_;
  qreal value_;
  qreal maximum_;

  explicit Slider(Node *parent = nullptr);
};
}  // namespace UI
#endif  // CLIENT_V3_UI_SLIDER_HPP_
