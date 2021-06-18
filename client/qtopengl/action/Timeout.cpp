// Copyright 2021 CatchChallenger
#include "Timeout.hpp"

#include <iostream>

#include "../core/Node.hpp"
#include "Action.hpp"

Timeout* Timeout::Create(int milliseconds) {
  Timeout* instance = new Timeout();

  instance->timeout_ = milliseconds;

  return instance;
}

Timeout::Timeout() : Action() { done_ = true; }

Timeout::~Timeout() { Action::~Action(); }

void Timeout::Start() {
  ellapsed_ = 0;
  done_ = false;
}

void Timeout::Stop() { done_ = true; }

void Timeout::Step(unsigned int ellapsed) {
  if (done_ || node_ == nullptr) return;

  ellapsed_ += ellapsed;

  if (ellapsed_ >= timeout_) {
    OnFinish();
  }
}

bool Timeout::IsDone() { return done_; }

void Timeout::OnFinish() {
  done_ = true;
  ellapsed_ = 0;
  auto callback = node_->GetOnActionDone();
  if (callback) {
    callback(Action::Tag());
  }
}
