// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_PROGRESSBAR_HPP_
#define CLIENT_QTOPENGL_UI_PROGRESSBAR_HPP_

#include <QGraphicsWidget>

#include "../core/Node.hpp"

namespace UI {
class Progressbar : public Node {
 public:
  static Progressbar *Create(QGraphicsItem *parent = nullptr);
  virtual ~Progressbar();

  void SetSize(int width, int height);
  void SetMaximum(const int &value);
  void SetMinimum(const int &value);
  void SetValue(const int &value);
  int Maximum();
  int Minimum();
  int Value();

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
  QRectF boundingRect() const override;

  void MousePressEventXY(const QPointF &point,
                                 bool &press_validated) override;
  void MouseReleaseEventXY(const QPointF &point,
                                   bool &prev_validated) override;
  void MouseMoveEventXY(const QPointF &point) override;

 private:
  QPixmap backgroundLeft, backgroundMiddle, backgroundRight;
  QPixmap barLeft, barMiddle, barRight;
  QPainterPath *textPath;
  QFont *font;
  QString oldText;
  QPixmap *cache;

  int m_value, m_min, m_max;

  explicit Progressbar(QGraphicsItem *parent = nullptr);
};
}  // namespace UI

#endif  // CLIENT_QTOPENGL_UI_PROGRESSBAR_HPP_
