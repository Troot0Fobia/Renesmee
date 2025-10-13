#pragma once

#include <functional>

namespace adapters::controllers::signal_handler {

static std::function<void()> handler_ = nullptr;
bool setHandler(std::function<void()> handler);

}  // namespace adapters::controllers::signal_handler
