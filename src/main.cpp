#include <cstring>

#include "Engine.hpp"
#include "CosmosConfig.hpp"
#include "CosmosGame.hpp"

int main(int argc, char* argv[]) {
    // Parse CLI arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--gl-debug") == 0) {
            CosmosConfig::get().set_gl_debug(true);
        }
    }

    Engine engine(960, 600, "Cosmos");
    CosmosGame game;
    engine.run(game);
    return 0;
}
