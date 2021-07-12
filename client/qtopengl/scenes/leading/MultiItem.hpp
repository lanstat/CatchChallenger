// Copyright 2021 CatchChallenger
#ifndef CLIENT_QTOPENGL_SCENES_LEADING_MULTIITEM_HPP_
#define CLIENT_QTOPENGL_SCENES_LEADING_MULTIITEM_HPP_

#include "../../ConnexionInfo.hpp"
#include "../../ui/Button.hpp"

namespace Scenes {
class MultiItem : public UI::Button {
 public:
  static MultiItem *Create(const ConnexionInfo &connexionInfo);
  ~MultiItem();

 private:
  MultiItem(const ConnexionInfo &connexionInfo);
  ConnexionInfo m_connexionInfo;

  void FetchName();
};
}  // namespace Scenes
#endif  // CLIENT_QTOPENGL_SCENES_LEADING_MULTIITEM_HPP_
