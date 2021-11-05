// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_UI_LINKEDDIALOG_HPP_
#define CLIENT_V3_UI_LINKEDDIALOG_HPP_

#include "Button.hpp"
#include "Dialog.hpp"
#include "Label.hpp"

namespace UI {
class LinkedDialog : public Dialog {
 public:
  static LinkedDialog *Create();
  ~LinkedDialog();

  void AddItem(Node *item);
  void RemoveItem(Node *item);
  void SetCurrentItem(int index);
  void OnScreenResize() override;
  void SetOnNext(std::function<void()> callback);
  void SetOnBack(std::function<void()> callback);
  void SetOnUseItem(std::function<void(uint16_t)> callback);
  void OnEnter() override;
  void OnExit() override;
  Node* CurrentItem();

 private:
  Button *quit_;
  Button *next_;
  Button *back_;
  std::vector<Node *> items_;
  Node* current_item_;
  int current_index_;

  std::function<void()> on_next_;
  std::function<void()> on_back_;
  std::function<void(uint16_t)> on_use_item_;

  LinkedDialog();

 protected:
  void OnActionClick(Node *node) override;
};
}  // namespace UI
#endif  // CLIENT_V3_UI_LINKEDDIALOG_HPP_
