// Copyright 2021 <CatchChallenger>
#include "MonsterBag.hpp"

#include "../../../Language.hpp"
#include "../../../base/ConnectionManager.hpp"
#include "../../../cc/QtDatapackClientLoader.hpp"
#include "../../../ui/MessageDialog.hpp"

using CatchChallenger::PlayerMonster;
using Scenes::MonsterBag;

MonsterBag::MonsterBag() {
  quit_ = UI::Button::Create(":/CC/images/interface/cancel.png", this);
  accept_ = UI::Button::Create(":/CC/images/interface/validate.png", this);

  proxy_ = new QGraphicsProxyWidget(this);
  monster_list_ = new QListWidget();
  monster_list_->setViewMode(QListWidget::ListMode);
  monster_list_->setMovement(QListWidget::Static);
  if (!connect(monster_list_, &QListWidget::itemSelectionChanged, this,
               &MonsterBag::OnSelectWidget))
    abort();
  proxy_->setWidget(monster_list_);

  QPixmap p =
      *GameLoader::gameLoader->getImage(":/CC/images/interface/inputText.png");
  p = p.scaled(p.width(), 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  if (!connect(&Language::language, &Language::newLanguage, this,
               &MonsterBag::newLanguage, Qt::QueuedConnection))
    abort();
  if (!connect(quit_, &CustomButton::clicked, this, &MonsterBag::OnCloseClicked,
               Qt::QueuedConnection))
    abort();
  if (!connect(accept_, &CustomButton::clicked, this,
               &MonsterBag::OnAcceptClicked, Qt::QueuedConnection))
    abort();

  parent_ = nullptr;
  message_dialog_ = nullptr;

  newLanguage();
}

MonsterBag::~MonsterBag() {
  delete wdialog_;
  delete quit_;
  delete accept_;
  delete title_;
}

void MonsterBag::SetParent(ScreenInput *parent) { parent_ = parent; }

QRectF MonsterBag::boundingRect() const { return QRectF(); }

void MonsterBag::paint(QPainter *p, const QStyleOptionGraphicsItem *,
                       QWidget *widget) {
  p->fillRect(0, 0, widget->width(), widget->height(), QColor(0, 0, 0, 120));
  if (screen_width_ != widget->width() || screen_height_ != widget->height()) {
    RecalculateInnerElements(widget);
    screen_width_ = widget->width();
    screen_height_ = widget->height();
  }
}

void MonsterBag::OnScreenResize() {
  if (widget->width() < 600 || widget->height() < 480) {
    label_.setScale(0.5);
    quit_->setSize(83 / 2, 94 / 2);
    accept_->setSize(83 / 2, 94 / 2);
    label_.setPos(x_ + (idealW - label_.pixmap().width() / 2) / 2, y_ - 36 / 2);
    title_->setPixelSize(30 / 2);
  } else {
    label_.setScale(1);
    quit_->setSize(83, 94);
    accept_->setSize(83, 94);
    label_.setPos(x_ + (idealW - label_.pixmap().width()) / 2, y_ - 36);
    title_->setPixelSize(30);
  }
  monster_list_->SetPos(x_, y_);
  monster_list_->SetSize(background_->Width(), background_->Height());
}

void MonsterBag::mousePressEventXY(const QPointF &p, bool &pressValidated,
                                   bool &callParentClass) {
  quit_->mousePressEventXY(p, pressValidated);
  accept_->mousePressEventXY(p, pressValidated);
  pressValidated = true;
  callParentClass = true;
}

void MonsterBag::mouseReleaseEventXY(const QPointF &p, bool &pressValidated,
                                     bool &callParentClass) {
  quit_->mouseReleaseEventXY(p, pressValidated);
  accept_->mouseReleaseEventXY(p, pressValidated);
  pressValidated = true;
  callParentClass = true;
}

void MonsterBag::SetVar(ConnexionManager *connexionManager) {
  connection_manager_ = connexionManager;
  client_ = connection_manager_->client;
  LoadMonster();
}

void MonsterBag::newLanguage() { title_->setText(tr("Monsters")); }

void MonsterBag::OnCloseClicked() {
  ClearSelection();
  if (parent_ == nullptr) {
    emit setAbove(nullptr);
  } else {
    emit parent_->setAbove(nullptr);
  }
}

void MonsterBag::LoadMonster() {
  uint8_t targetSize = 50;
  const std::vector<PlayerMonster> &monsters = client_->getPlayerMonster();

#ifdef DEBUG_BASEWINDOWS
  qDebug() << "MonsterBag::LoadMonster()";
#endif
  monster_list_->clear();
  monster_list_->setIconSize(QSize(targetSize, targetSize));
  auto i = monsters.begin();
  while (i != monsters.cend()) {
    const uint16_t id = i->monster;
    const DatapackClientLoader::MonsterExtra monster_extra =
        QtDatapackClientLoader::datapackLoader->monsterExtra.at(id);
    QListWidgetItem *item = new QListWidgetItem();
    if (QtDatapackClientLoader::datapackLoader->monsterExtra.find(id) !=
        QtDatapackClientLoader::datapackLoader->monsterExtra.cend()) {
      QPixmap p =
          QtDatapackClientLoader::datapackLoader->getMonsterExtra(id).thumb;
      p = p.scaledToHeight(targetSize);
      item->setIcon(p);
      item->setText(QString::fromStdString(monster_extra.name));
      item->setToolTip(QString::fromStdString(monster_extra.name));
    } else {
      item->setIcon(
          QtDatapackClientLoader::datapackLoader->defaultInventoryImage());
      item->setText(QStringLiteral("id: %1").arg(id));
    }
    item->setData(99, id);
    monster_list_->addItem(item);
    if (id == last_item_selected_) item->setSelected(true);
    ++i;
  }
}

void MonsterBag::OnSelectWidget() {
  last_item_selected_ =
      monster_list_->selectedItems().first()->data(99).toUInt();
}

void MonsterBag::OnAcceptClicked() {
  if (last_item_selected_ == -1) {
    ShowMessageDialog(tr("Warning"), tr("Select a monster"));
    return;
  }
  if (last_item_selected_ == client_->getCurrentMonster()->monster) {
    ShowMessageDialog(tr("Warning"),
                      tr("The monster selected is already on battle"));
    return;
  }
  emit OnSelectMonster(
      monster_list_->selectionModel()->selectedIndexes().first().row());
  OnCloseClicked();
}

void MonsterBag::ClearSelection() {
  last_item_selected_ = -1;
  // monster_list_->selectionModel()->clearSelection();
}

void MonsterBag::ShowMessageDialog(const QString &title,
                                   const QString &message) {
  ClearSelection();
  if (message_dialog_ == nullptr) {
    message_dialog_ = UI::MessageDialog::Create();
  }

  message_dialog_->SetTitle(title);
  message_dialog_->SetMessage(message);
  AddChild(message_dialog_);
}

void MonsterBag::OnEnter() {
  UI::Dialog::OnEnter();
  monster_list_->RegisterEvents();
}

void MonsterBag::OnExit() {
  monster_list_->UnRegisterEvents();
  UI::Dialog::OnExit();
}
