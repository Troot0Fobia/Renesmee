#pragma once

#include <functional>

namespace adapters::controllers::signal_handler {

static std::function<bool()> handler_ = nullptr;
bool setHandler(std::function<bool()> handler);

} // namespace adapters::controllers::signal_handler
