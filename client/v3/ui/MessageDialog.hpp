// Copyright 2021 <CatchChallenger>
#ifndef CLIENT_V3_UI_MESSAGEDIALOG_HPP_
#define CLIENT_V3_UI_MESSAGEDIALOG_HPP_

#include "Button.hpp"
#include "Dialog.hpp"
#include "Label.hpp"

namespace UI {
class MessageDialog : public Dialog {
 public:
  ~MessageDialog();
  static MessageDialog *Create();

  void OnScreenResize() override;
  void SetMessage(const QString &text);
  void newLanguage();

 protected:
  void OnActionClick(Node *node);

 private:
  MessageDialog();
  UI::Button *quit_;
  UI::Label *message_;
};
}  // namespace UI
#endif  // CLIENT_V3_UI_MESSAGEDIALOG_HPP_
