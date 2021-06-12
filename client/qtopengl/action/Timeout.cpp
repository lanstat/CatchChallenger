// Copyright 2021 CatchChallenger
#include "Timeout.hpp"

#include <chrono>
#include <cmath>

#include "../core/Node.hpp"
#include "Action.hpp"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

Timeout* Timeout::Create(int milliseconds) {
  Timeout* instance = new Timeout();

  instance->timeout_ = milliseconds;

  return instance;
}

Timeout::Timeout() : Action() { done_ = false; }

Timeout::~Timeout() { Action::~Action(); }

void Timeout::Start() {
  uint64_t now =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  ellapsed_ = now + timeout_;
  done_ = true;
}

void Timeout::Stop() { done_ = false; }

void Timeout::Step() {
  if (!done_ || node_ == nullptr) return;

  uint64_t now =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();

  if (now > timeout_) {
    OnFinish();
  }
}

bool Timeout::IsDone() { return done_; }

void Timeout::OnFinish() {
  done_ = false;
  ellapsed_ = 0;
  auto callback = node_->GetOnActionDone();
  if (callback) {
    callback(Action::Tag());
  }
}
