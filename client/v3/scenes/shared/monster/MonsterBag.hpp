// Copyright 2021 <CatchChallenger>
#ifndef CLIENT_V3_SCENES_SHARED_MONSTER_MONSTERBAG_HPP_
#define CLIENT_V3_SCENES_SHARED_MONSTER_MONSTERBAG_HPP_

#include "../../../base/ConnectionManager.hpp"
#include "../../../ui/Button.hpp"
#include "../../../ui/Dialog.hpp"
#include "../../../ui/ListView.hpp"
#include "../../../ui/MessageDialog.hpp"

namespace Scenes {
class MonsterBag : public UI::Dialog {
 public:
  ~MonsterBag();
  static MonsterBag *Create();

  void OnScreenResize() override;
  void OnEnter() override;
  void OnExit() override;

  void OnCloseClicked();
  void OnAcceptClicked();
  void SetOnSelectMonster(std::function<void(uint8_t)> callback);
  void OnSelectMonster(uint8_t monsterPosition);
  void LoadMonster();

 protected:
  void OnActionClick(Node *node);

 private:
  UI::Button *quit_;
  UI::ListView *monster_list_;
  UI::MessageDialog *message_dialog_;

  ConnectionManager *connection_manager_;
  CatchChallenger::Api_protocol_Qt *client_;

  MonsterBag();
  void newLanguage();
  void OnSelectWidget();
  void ShowMessageDialog(const QString &title, const QString &message);
  void ClearSelection();
};
}  // namespace Scenes
#endif  // CLIENT_V3_SCENES_SHARED_MONSTER_MONSTERBAG_HPP_
