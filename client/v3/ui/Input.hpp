// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_INPUT_HPP_
#define CLIENT_QTOPENGL_UI_INPUT_HPP_

#include <QGraphicsWidget>

#include "../core/Node.hpp"
#include "../core/Sprite.hpp"
#include "Label.hpp"

namespace UI {
class Input : public Node {
 public:
  enum InputMode {
    kPassword,
    kNumber,
    kDefault,
    kReadOnly
  };

  static Input *Create(Node *parent = nullptr);
  ~Input();

  void SetFocus(bool focus);
  bool HasFocus();
  void SetMaxLength(int size);

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;
  void SetSize(int width, int height);
  void SetMode(InputMode mode);
  QString Value() const;
  void SetValue(const QString &value);
  void Clear();
  void SetPlaceholder(const QString &value);
  void SetFont(const QFont &font);
  void RegisterEvents() override;
  void UnRegisterEvents() override;

  void Draw(QPainter *painter) override;

 private:
  Label *label_;
  bool pressed_;
  bool focus_;
  QString text_;
  QString placeholder_;
  Sprite *background_;
  InputMode mode_;
  QPointF inner_padding_;

  explicit Input(Node *parent = nullptr);
};
}  // namespace UI
#endif  // CLIENT_QTOPENGL_UI_INPUT_HPP_
