#ifndef MessageDialog_H
#define MessageDialog_H

#include <QComboBox>
#include <QObject>

#include "../CCGraphicsTextItem.hpp"
#include "../CCSliderH.hpp"
#include "../ComboBox.hpp"
#include "../CustomButton.hpp"
#include "../CustomText.hpp"
#include "../ImagesStrechMiddle.hpp"
#include "../LineEdit.hpp"
#include "../ScreenInput.hpp"
#include "../SpinBox.hpp"

class MessageDialog : public ScreenInput {
  Q_OBJECT

 public:
  explicit MessageDialog();
  ~MessageDialog();
  void resizeEvent(QResizeEvent *e);
  QRectF boundingRect() const override;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *,
             QWidget *widget = nullptr) override;
  void mousePressEventXY(const QPointF &p, bool &pressValidated,
                         bool &callParentClass) override;
  void mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                           bool &callParentClass) override;

  void OnCloseClicked();
  void RecalculateInnerElements(QWidget *widget);
  void SetMessage(const QString &text);
 private slots:
  void newLanguage();

 private:
  ImagesStrechMiddle *wdialog_;
  CustomButton *quit_;
  CustomText *title_;
  QGraphicsPixmapItem label_;

  QGraphicsTextItem *message_;

  int x_;
  int y_;

  int screen_width_;
  int screen_height_;

 signals:
  void removeAbove();
};

#endif  // OPTIONSDIALOG_H
