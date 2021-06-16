// Copyright 2021 CatchChallenger
#include "Sequence.hpp"

#include <iostream>
#include <vector>

#include "../core/Node.hpp"
#include "Action.hpp"

using std::vector;

Sequence* Sequence::Create(const vector<Action*>& actions) {
  Sequence* instance = new (std::nothrow) Sequence();

  instance->actions_ = actions;

  return instance;
}

Sequence::Sequence() : Action() {
  done_ = true;
  current_ = 0;
}

Sequence::~Sequence() { Action::~Action(); }

void Sequence::Start() {
  done_ = false;
  current_ = 0;
  actions_.at(0)->Start();
}

void Sequence::Stop() {
  done_ = true;
  actions_.at(current_)->Stop();
}

void Sequence::Step() {
  if (done_ || node_ == nullptr) return;

  if (actions_.at(current_)->IsDone()) {
    current_++;
    if (current_ >= actions_.size()) {
      OnFinish();
    } else {
      actions_.at(current_)->Start();
    }
  }
}

bool Sequence::IsDone() { return done_; }

void Sequence::OnFinish() {
  done_ = true;
  auto callback = node_->GetOnActionDone();
  if (callback) {
    callback(Action::Tag());
  }
}
