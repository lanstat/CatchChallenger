// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_LISTVIEW_HPP_
#define CLIENT_QTOPENGL_UI_LISTVIEW_HPP_

#include <QGraphicsWidget>
#include <vector>

#include "../core/Node.hpp"

namespace UI {
class ListView : public Node {
 public:
  static ListView *Create(Node *parent = nullptr);
  ~ListView();

  /** Add an item to the listview */
  void AddItem(Node *node);
  Node *GetItem(uint8_t index);
  void RemoveItem(uint8_t index);
  uint8_t Count();
  Node *GetSelectedItem();
  void SetSize(int width, int height);
  std::vector<Node *> Items();
  /** Release all items in the list */
  void Clear();
  void SetHorizontal(bool horizontal);

  void MousePressEvent(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEvent(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEvent(const QPointF &point) override;
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger) override;
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger) override;

  void Draw(QPainter *painter) override;
  void RegisterEvents() override;
  void UnRegisterEvents() override;

 private:
  QPixmap slider_rail_;
  QPixmap slider_;
  QPixmap content_;
  QRectF content_rect_;
  QRectF slider_rect_;
  bool show_slider_;
  bool is_slider_pressed_;
  qreal content_scale_;

  std::vector<Node *> items_;
  Node *selected_item_;
  bool is_horizontal_;
  bool is_drawable_;

  explicit ListView(Node *parent = nullptr);

  void ReCalculateSize();
};
}  // namespace UI
#endif  // CLIENT_QTOPENGL_UI_LISTVIEW_HPP_
