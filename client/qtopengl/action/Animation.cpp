// Copyright 2021 CatchChallenger
#include "Animation.hpp"

#include <QPainter>
#include <chrono>

#include "../core/Node.hpp"
#include "../core/Sprite.hpp"
#include "Action.hpp"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

Animation* Animation::Create(std::vector<QPixmap> frames, int delay,
                             unsigned int loop) {
  Animation* instance = new Animation();

  instance->frames_ = frames;
  instance->delay_ = delay;
  instance->loop_ = loop;

  return instance;
}

Animation::Animation() : Action() { done_ = false; }

Animation::~Animation() { Action::~Action(); }

void Animation::Start() {
  ellapsed_ =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  index_ = 0;
  done_ = true;
}

void Animation::Stop() { done_ = false; }

void Animation::Step() {
  if (!done_ || node_ == nullptr) return;
  uint64_t now =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();

  if (now < ellapsed_ + delay_) return;

  ellapsed_ = now;

  (static_cast<Sprite*>(node_))->SetPixmap(frames_.at(index_));
  index_++;
  if (frames_.size() == index_) {
    OnFinish();
  }
}

bool Animation::IsDone() { return done_; }

void Animation::OnFinish() {
  done_ = false;
  auto callback = node_->GetOnActionDone();
  if (callback) {
    callback(Action::Tag());
  }
}

void Animation::SetFrames(std::vector<QPixmap> frames) {
  frames_ = frames;
}
