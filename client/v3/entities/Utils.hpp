// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_ENTITIES_UTILS_HPP_
#define CLIENT_V3_ENTITIES_UTILS_HPP_

#include "../core/Scene.hpp"

struct NodeParsed {
  std::vector<Node*> nodes;
};

class Utils {
 public:
  static Node* GetCurrentScene(Node* node);
  static NodeParsed HTML2Node(const QString& html);
  static NodeParsed HTML2Node(const QString& html,
                              std::function<void(Node*)> on_click);
  static QPixmap B64Png2Pixmap(const QString& b64);
};
#endif  // CLIENT_V3_ENTITIES_UTILS_HPP_
