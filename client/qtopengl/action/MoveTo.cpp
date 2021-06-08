// Copyright 2021 CatchChallenger
#include "MoveTo.hpp"

#include <cmath>
#include <iostream>

#include "../core/Node.hpp"
#include "Action.hpp"

MoveTo* MoveTo::Create(int milliseconds, int x, int y) {
  MoveTo* instance = new MoveTo();

  instance->milliseconds_ = milliseconds;
  instance->end_x_ = x;
  instance->end_y_ = y;

  return instance;
}

MoveTo::MoveTo() : Action() {
  delta_x_ = 0;
  delta_y_ = 0;
  done_ = false;
}

MoveTo::~MoveTo() { Action::~Action(); }

void MoveTo::Start(bool animate, bool trigger_done) {
  CalculateDelta(animate);
  done_ = true;
  trigger_done_ = trigger_done;
}

void MoveTo::Stop() { done_ = false; }

void MoveTo::Step() {
  if (!done_ || node_ == nullptr) return;

  if (std::abs(node_->X() - end_x_) > std::abs(delta_x_) ||
      std::abs(node_->Y() - end_y_) > std::abs(delta_y_)) {
    if (delta_x_ != 0) node_->SetX(node_->X() + delta_x_);
    if (delta_y_ != 0) node_->SetY(node_->Y() + delta_y_);
  } else {
    node_->SetPos(end_x_, end_y_);
    OnFinish();
  }
}

bool MoveTo::IsDone() { return done_; }

void MoveTo::OnFinish() {
  done_ = false;
  delta_x_ = 0;
  delta_y_ = 0;
  if (trigger_done_) {
    auto callback = node_->GetOnActionDone();
    if (callback) {
      callback(Action::Tag());
    }
  }
}

void MoveTo::CalculateDelta(bool animate) {
  if (animate) {
    double frames = milliseconds_ / 1000 * 60;
    delta_x_ = (end_x_ - node_->X()) / frames;
    delta_y_ = (end_y_ - node_->Y()) / frames;
  } else {
    delta_x_ = INT_MAX;
    delta_y_ = INT_MAX;
  }
}
