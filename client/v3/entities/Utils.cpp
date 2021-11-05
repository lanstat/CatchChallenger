// Copyright 2021 CatchChallenger
#include "Utils.hpp"

#include <iostream>

#include "../ui/Label.hpp"
#include "../ui/ThemedButton.hpp"

Node *Utils::GetCurrentScene(Node *node) {
  Node *parent = node;
  while (parent != nullptr) {
    if (parent->ClassName() == "Scene") {
      break;
    }
    parent = parent->Parent();
  }
  return parent;
}

NodeParsed Utils::HTML2Node(const QString &html) {
  return Utils::HTML2Node(html, nullptr);
}

NodeParsed Utils::HTML2Node(const QString &html,
                            std::function<void(Node *)> on_click) {
  QRegExp new_line("<br(\\s*)(\\/|)>");
  QRegExp anchor("<a\\s+href=\"(.+)\">(.+)</a>");
  auto lines = html.split(new_line);
  auto records = std::vector<Node *>();
  NodeParsed response;
  for (auto iter = lines.begin(); iter != lines.end(); ++iter) {
    auto line = (*iter);
    line = line.trimmed();
    if (line.startsWith("<a")) {
      if (anchor.exactMatch(line)) {
        auto node = UI::GreenButton::Create();
        node->SetHeight(30);
        node->SetPixelSize(12);
        node->SetText(anchor.cap(2));
        node->SetData(99, anchor.cap(1).toStdString());
        if (on_click) {
          node->SetOnClick(on_click);
        }
        records.push_back(node);
      }
    } else {
      auto node = UI::Label::Create();
      node->SetText(line);
      node->SetPixelSize(12);
      records.push_back(node);
    }
  }
  response.nodes = records;
  return response;
}

QPixmap Utils::B64Png2Pixmap(const QString &b64) {
  QByteArray by = QByteArray::fromBase64(b64.toUtf8());
  QImage image = QImage::fromData(by, "PNG");
  return QPixmap::fromImage(image);
}
