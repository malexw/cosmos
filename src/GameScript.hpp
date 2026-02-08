#ifndef COSMOS_GAMESCRIPT_HPP_
#define COSMOS_GAMESCRIPT_HPP_

class Engine;

class GameScript {
public:
    virtual ~GameScript() = default;
    virtual void init(Engine& engine) = 0;
    virtual bool update(float dt) = 0;  // returns false to quit
    virtual void shutdown() {}
};

#endif
