// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_PROGRESSBAR_HPP_
#define CLIENT_QTOPENGL_UI_PROGRESSBAR_HPP_

#include <QGraphicsWidget>

#include "../core/Node.hpp"
#include "../action/Tick.hpp"

namespace UI {
class Progressbar : public Node {
 public:
  static Progressbar *Create(Node *parent = nullptr);
  virtual ~Progressbar();

  void SetSize(int width, int height);
  void SetMaximum(const int &value);
  void SetMinimum(const int &value);
  void SetValue(const int &value);
  void IncrementValue(const int &delta, const bool &animate);
  int Maximum();
  int Minimum();
  int Value();
  void SetOnIncrementDone(const std::function<void()> &callback);

  void Draw(QPainter *painter) override;

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;

 private:
  QPixmap background_left_;
  QPixmap background_middle_;
  QPixmap background_right_;
  QPixmap bar_left_;
  QPixmap bar_middle_;
  QPixmap bar_right_;
  QPainterPath *text_path_;
  QFont *font_;
  QString oldText;
  QPixmap *cache_;

  int value_;
  int min_;
  int max_;
  float delta_value_;
  float internal_value_;
  unsigned int increment_times_;
  std::function<void()> on_increment_done_;

  Tick *tick_;

  explicit Progressbar(Node *parent = nullptr);

  bool OnTick();
};
}  // namespace UI

#endif  // CLIENT_QTOPENGL_UI_PROGRESSBAR_HPP_
