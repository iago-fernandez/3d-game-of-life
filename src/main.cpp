#include "app/app.h"

/**
 * @brief Program entry point.
 * @return 0 on success, non-zero on failure.
 */
int main() {
    app::AppConfig cfg;
    app::App app(cfg);

    if (!app.init()) {
        // Initialization failed
        return 1;
    }
    app.run();

    app.shutdown();
    return 0;
}
