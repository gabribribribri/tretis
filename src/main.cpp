#include "logging.hpp"
#include "tretis.hpp"

int main() {
    Log::SetLevel(DEBUG_LOG_LEVEL);

    Tretis::Get().gameloop();
}
