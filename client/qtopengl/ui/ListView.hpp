// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_LISTVIEW_HPP_
#define CLIENT_QTOPENGL_UI_LISTVIEW_HPP_

#include <QGraphicsWidget>
#include <vector>

#include "../core/Node.hpp"

namespace UI {
class ListView : public Node {
 public:
  static ListView *Create(QGraphicsItem *parent = nullptr);
  ~ListView();

  /** Add an item to the listview */
  void AddItem(Node *node);
  Node *GetItem(uint8_t index);
  void RemoveItem(uint8_t index);
  Node *GetSelectedItem();
  void SetSize(int width, int height);
  /** Release all items in the list */
  void Clear();

  void MousePressEventXY(const QPointF &point, bool &press_validated) override;
  void MouseReleaseEventXY(const QPointF &point, bool &prev_validated) override;
  void MouseMoveEventXY(const QPointF &point) override;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *) override;

 private:
  QPixmap slider_rail_v_;
  QPixmap slider_v_;
  QPixmap slider_rail_h_;
  QPixmap slider_h_;
  QPixmap content_;
  QRectF content_rect_;
  QRectF slider_rect_;
  bool show_vert_slider_;
  bool show_hor_slider_;
  bool is_slider_pressed_;
  qreal content_scale_;

  std::vector<Node *> items_;
  Node *selected_item_;

  explicit ListView(QGraphicsItem *parent = nullptr);

  void ReCalculateSize();
};
}  // namespace UI
#endif  // CLIENT_QTOPENGL_UI_LISTVIEW_HPP_
