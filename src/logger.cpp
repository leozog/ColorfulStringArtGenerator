#include "logger.h"

std::mutex Logger::mtx;
auto Logger::start_time = std::chrono::steady_clock::now();
