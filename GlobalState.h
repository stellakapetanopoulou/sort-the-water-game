#pragma once
#include <vector>
#include <optional>
#include "Tube.h"

enum class GameState { START, PLAYING, WIN };

struct Rect {
    float x, y, w, h;
};

class GlobalState {
public:
    // "Καθολικά" δεδομένα παιχνιδιού (παλιά globals)
    GameState state = GameState::START;
    int level = 0;

    std::vector<Tube> tubes;
    std::optional<int> selected;   

    // Κεντρικές μέθοδοι (ακριβώς όπως ζητάει η εκφώνηση)
    void init();
    void update(float dt);
    void draw();

    bool initialized = false;
private:
    void loadLevel(int lvl);
    void resetLevel();

    void handleMouse(bool click, float mx, float my);
    bool isWin() const;  // win condition
    void drawTube(const Tube& t, float x, float y) const;
    void handleStartUI(bool click, float mx, float my);
    void handleWinUI(bool click, float mx, float my);
    bool prevLeftDown = false;
    bool pointInRect(float mx, float my, const Rect& r) const;
    void colorToRGB(Color c, float& r, float& g, float& b) const;
    bool canPour(int from, int to) const;
    int pourAmount(int from, int to) const;
    bool pour(int fromIdx, int toIdx);
    bool isTubeSolved(const Tube& t) const;
    void handleResetUI(bool click, float mx, float my);
    void handleExitUI(bool click, float mx, float my);
    int tubeAt(float mx, float my) const;

    //butons 
    Rect startBtn = { 260, 320, 280, 60 };
    Rect nextBtn = { 260, 320, 280, 60 };
    Rect exitBtn = { 720, 60, 50, 50 };
    Rect resetBtn = { 660, 60, 50, 50 };


};
