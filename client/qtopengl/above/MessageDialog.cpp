#include "MessageDialog.hpp"

#include <QDesktopServices>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QPainter>
#include <QTextDocument>

#include "../../qt/GameLoader.hpp"
#include "../../qt/Settings.hpp"
#include "../../qt/Ultimate.hpp"
#include "../Language.hpp"

MessageDialog::MessageDialog()
    : wdialog_(new ImagesStrechMiddle(46, ":/CC/images/interface/message.png",
                                      this)),
      label_(this) {
  x_ = -1;
  y_ = -1;
  screen_width_ = -1;
  screen_height_ = -1;
  label_.setPixmap(
      *GameLoader::gameLoader->getImage(":/CC/images/interface/label.png"));
  label_.setTransformationMode(Qt::SmoothTransformation);
  label_.setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  quit_ = new CustomButton(":/CC/images/interface/cancel.png", this);
  QLinearGradient gradient1(0, 0, 0, 100);
  gradient1.setColorAt(0.25, QColor(230, 153, 0));
  gradient1.setColorAt(0.75, QColor(255, 255, 255));
  QLinearGradient gradient2(0, 0, 0, 100);
  gradient2.setColorAt(0, QColor(64, 28, 2));
  gradient2.setColorAt(1, QColor(64, 28, 2));
  title_ = new CustomText(gradient1, gradient2, this);

  QPixmap p =
      *GameLoader::gameLoader->getImage(":/CC/images/interface/inputText.png");
  p = p.scaled(p.width(), 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  message_ = new QGraphicsTextItem(this);
  message_->setVisible(true);
  QFont f;
  f.setPixelSize(23);
  f.setBold(true);
  message_->setFont(f);
  message_->setDefaultTextColor(QColor(33, 33, 33));

  if (!connect(&Language::language, &Language::newLanguage, this,
               &MessageDialog::newLanguage, Qt::QueuedConnection))
    abort();
  if (!connect(quit_, &CustomButton::clicked, this,
               &MessageDialog::OnCloseClicked, Qt::QueuedConnection))
    abort();

  parent_ = nullptr;

  newLanguage();
}

MessageDialog::~MessageDialog() {
  delete wdialog_;
  delete quit_;
  delete title_;
  delete message_;
}

void MessageDialog::SetParent(ScreenInput *parent) {
  parent_ = parent;
}

void MessageDialog::SetMessage(const QString &text) {
  message_->setPlainText(text);
}

QRectF MessageDialog::boundingRect() const { return QRectF(); }

void MessageDialog::paint(QPainter *p, const QStyleOptionGraphicsItem *,
                          QWidget *widget) {
  p->fillRect(0, 0, widget->width(), widget->height(), QColor(0, 0, 0, 120));
  if (screen_width_ != widget->width() || screen_height_ != widget->height()) {
    RecalculateInnerElements(widget);
    screen_width_ = widget->width();
    screen_height_ = widget->height();
  }
}

void MessageDialog::RecalculateInnerElements(QWidget *widget) {
  bool forcedCenter = true;
  int idealW = 600;
  int idealH = message_->document()->pageSize().height() + 200;
  if (widget->width() < 600 || widget->height() < 480) {
    idealW /= 2;
    idealH /= 2;
  }
  if (idealW > widget->width()) idealW = widget->width();
  int top = 36;
  int bottom = 94 / 2;
  if (widget->width() < 600 || widget->height() < 480) {
    top /= 2;
    bottom /= 2;
  }
  if ((idealH + top + bottom) > widget->height())
    idealH = widget->height() - (top + bottom);

  if (x_ < 0 || y_ < 0 || forcedCenter) {
    x_ = widget->width() / 2 - idealW / 2;
    y_ = widget->height() / 2 - idealH / 2;
  } else {
    if ((x_ + idealW) > widget->width()) x_ = widget->width() - idealW;
    if ((y_ + idealH + bottom) > widget->height())
      y_ = widget->height() - (idealH + bottom);
    if (y_ < top) y_ = top;
  }

  wdialog_->setPos(x_, y_);
  wdialog_->setSize(idealW, idealH);

  if (widget->width() < 600 || widget->height() < 480) {
    label_.setScale(0.5);
    quit_->setSize(83 / 2, 94 / 2);
    label_.setPos(x_ + (idealW - label_.pixmap().width() / 2) / 2, y_ - 36 / 2);
    title_->setPixelSize(30 / 2);
  } else {
    label_.setScale(1);
    quit_->setSize(83, 94);
    label_.setPos(x_ + (idealW - label_.pixmap().width()) / 2, y_ - 36);
    title_->setPixelSize(30);
  }
  unsigned int space = 30;
  if (widget->width() < 600 || widget->height() < 480) {
    space = 10;
  }

  quit_->setPos(x_ + (idealW / 2 - quit_->width() / 2),
                y_ + idealH - quit_->height() / 2);
  const QRectF trect = title_->boundingRect();
  title_->setPos(x_ + (idealW - title_->boundingRect().width()) / 2,
                 y_ - trect.height() / 2);

  message_->setPos(x_ + idealW / 2 - message_->document()->size().width() / 2,
                   y_ + 60);
}

void MessageDialog::mousePressEventXY(const QPointF &p, bool &pressValidated,
                                      bool &callParentClass) {
  quit_->mousePressEventXY(p, pressValidated);
  pressValidated = true;
  callParentClass = true;
}

void MessageDialog::mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                                        bool &callParentClass) {
  quit_->mouseReleaseEventXY(p, pressValidated);
  pressValidated = true;
  callParentClass = true;
}

void MessageDialog::newLanguage() { title_->setText(tr("Alert")); }

void MessageDialog::OnCloseClicked() {
  if (parent_ == nullptr) {
    emit setAbove(nullptr);
  } else {
    emit parent_->setAbove(nullptr);
  }
}
