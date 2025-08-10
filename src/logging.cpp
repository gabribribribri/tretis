#include "logging.hpp"

void Log::SetLevel(int level) {
    assert(level >= ERROR_LOG_LEVEL and level <= DEBUG_LOG_LEVEL);
    LOG_LEVEL = level;
}
