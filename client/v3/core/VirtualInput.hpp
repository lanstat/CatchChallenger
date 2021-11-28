// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_CORE_VIRTUALINPUT_HPP_
#define CLIENT_V3_CORE_VIRTUALINPUT_HPP_

#include <QGraphicsProxyWidget>
#include <QLineEdit>

class VirtualInput : public QGraphicsProxyWidget {
 public:
  enum InputMode { kPassword, kNumber, kDefault, kReadOnly };

  static VirtualInput *GetInstance();
  ~VirtualInput();

  void SetMaxLength(int size);
  void SetMode(InputMode mode);
  QString Value() const;
  void SetValue(const QString &value);
  void Clear();
  void Show(const QString &value,
            std::function<void(std::string)> callback = nullptr);
  void Hide();
  bool IsVisible();
  void SetOnTextChange(std::function<void(std::string)> callback);
  void KeyPressEvent(QKeyEvent *event, bool &event_trigger);
  void KeyReleaseEvent(QKeyEvent *event, bool &event_trigger);

 private:
  std::function<void(std::string)> on_text_change_;
  QLineEdit *control_;
  static VirtualInput *instance_;
  bool visible_;

  explicit VirtualInput(QGraphicsItem *parent);
  void OnTextComplete();
};
#endif  // CLIENT_V3_CORE_VIRTUALINPUT_HPP_
