// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_UI_LINKEDDIALOG_HPP_
#define CLIENT_V3_UI_LINKEDDIALOG_HPP_

#include "Button.hpp"
#include "Dialog.hpp"
#include "Label.hpp"

namespace UI {
class LinkedDialog : public Dialog {
 public:
  static LinkedDialog *Create(bool show_navigation = true);
  ~LinkedDialog();

  void AddItem(Node *item);
  void RemoveItem(Node *item);
  void SetCurrentItem(int index);
  void OnScreenResize() override;
  void ShowNavigation(bool show);
  void SetOnNext(std::function<void(int)> callback);
  void SetOnBack(std::function<void(int)> callback);
  void SetOnUseItem(std::function<void(uint16_t)> callback);
  void OnEnter() override;
  void OnExit() override;
  Node *CurrentItem();
  Node *Item(int index);

 private:
  Button *next_;
  Button *back_;
  std::vector<Node *> items_;
  Node *current_item_;
  int current_index_;
  bool show_navigation_;

  std::function<void(int)> on_next_;
  std::function<void(int)> on_back_;
  std::function<void(uint16_t)> on_use_item_;

  LinkedDialog(bool show_navigation);

 protected:
  void OnActionClick(Node *node);
};
}  // namespace UI
#endif  // CLIENT_V3_UI_LINKEDDIALOG_HPP_
