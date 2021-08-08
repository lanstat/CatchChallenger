// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_UI_DIALOG_HPP_
#define CLIENT_QTOPENGL_UI_DIALOG_HPP_

#include <vector>

#include "../core/Scene.hpp"
#include "../core/Sprite.hpp"
#include "Button.hpp"
#include "Label.hpp"

namespace UI {
class Dialog : public Scene {
 public:
  ~Dialog();

  void SetOnClose(std::function<void()> callback);
  void AddActionButton(Button *action);
  void RemoveActionButton(Button *action);
  void SetTitle(const QString &text);
  QString Title() const;
  void Close();

  void OnEnter() override;
  void OnExit() override;

 protected:
  Dialog();

  int x_;
  int y_;
  Sprite *background_;

  void OnScreenSD() override;
  void OnScreenHD() override;
  void OnScreenHDR() override;
  void OnScreenResize() override;
  void Draw(QPainter *painter) override;
  void RecalculateActionButtons();
  QRectF ContentBoundary() const;
  virtual void OnActionClick(Node *node) = 0;

 private:
  std::vector<Button *> actions_;
  Label *title_;
  Sprite *title_background_;
  QRectF content_boundary_;

  std::function<void()> on_close_;

  void OnActionButton(Node *node);
};
}  // namespace UI
#endif  // CLIENT_QTOPENGL_UI_DIALOG_HPP_
