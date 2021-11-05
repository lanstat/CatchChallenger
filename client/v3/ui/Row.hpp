// Copyright 2021 CatchChallenger
#ifndef CLIENT_V3_UI_ROW_HPP_
#define CLIENT_V3_UI_ROW_HPP_

#include <vector>

#include "../core/Node.hpp"

namespace UI {
class Row : public Node {
 public:
  static Row *Create(Node *parent = nullptr);
  ~Row();

  void SetItemSpacing(int value);
  void ReCalculateSize();
  void AddChild(Node *node);
  /** Remove a child from the node */
  void RemoveChild(Node *node);

  void Draw(QPainter *painter) override;
  void OnChildResize(Node *node) override;

 protected:
  int spacing_;

  explicit Row(Node *parent = nullptr);

 private:
  bool is_processing_resize_;
};
}  // namespace UI
#endif  // CLIENT_V3_UI_ROW_HPP_
