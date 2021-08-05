// Copyright 2021 <CatchChallenger>
#ifndef CLIENT_QTOPENGL_ABOVE_MONSTER_MONSTERBAG_HPP_
#define CLIENT_QTOPENGL_ABOVE_MONSTER_MONSTERBAG_HPP_

#include <QListWidget>
#include <QObject>

#include "../../CCGraphicsTextItem.hpp"
#include "../../CCSliderH.hpp"
#include "../../ComboBox.hpp"
#include "../../ConnexionManager.hpp"
#include "../../CustomButton.hpp"
#include "../../CustomText.hpp"
#include "../../ImagesStrechMiddle.hpp"
#include "../../LineEdit.hpp"
#include "../../ScreenInput.hpp"
#include "../../SpinBox.hpp"
#include "../MessageDialog.hpp"

class MonsterBag : public ScreenInput {
  Q_OBJECT

 public:
  MonsterBag();
  ~MonsterBag();
  void resizeEvent(QResizeEvent *e);
  QRectF boundingRect() const override;
  void paint(QPainter *p, const QStyleOptionGraphicsItem *,
             QWidget *widget = nullptr) override;
  void mousePressEventXY(const QPointF &p, bool &pressValidated,
                         bool &callParentClass) override;
  void mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                           bool &callParentClass) override;

  void OnCloseClicked();
  void OnAcceptClicked();
  void RecalculateInnerElements(QWidget *widget);
  void SetParent(ScreenInput *parent);
  void SetVar(ConnexionManager *connexionManager);
 signals:
  void OnSelectMonster(uint8_t monsterPosition);
 private slots:
  void newLanguage();

 private:
  ImagesStrechMiddle *wdialog_;
  CustomButton *quit_;
  CustomButton *accept_;
  CustomText *title_;
  QGraphicsPixmapItem label_;
  QGraphicsProxyWidget *proxy_;
  QListWidget *monster_list_;
  MessageDialog *message_dialog_;

  ScreenInput *parent_;

  int x_;
  int y_;
  int screen_width_;
  int screen_height_;
  int last_item_selected_ = -1;

  ConnexionManager *connection_manager_;
  CatchChallenger::Api_protocol_Qt *client_;

  void OnSelectWidget();
  void LoadMonster();
  void ShowMessageDialog(const QString &title, const QString &message);
  void ClearSelection();
};

#endif  // CLIENT_QTOPENGL_ABOVE_MONSTER_MONSTERBAG_HPP_
