// Game Q Woter sort 
// γραφοσ: 
// Κάθε σωλήνας = κόμβος
// Η μεταφορά χρώματος(pour) =  ακμή μεταξύ κόμβων
//

// ----1. INCLUDES----
#include "sgg/graphics.h"
#include <vector>
#include <array>
#include <optional>

#include "Color.h"
#include "Tube.h"
#include "GlobalState.h"

GlobalState g_game;

//---- SGG CALLBACKS----
//    init() - update() - draw()

static void init()
{
    g_game.init();
}

void update(float dt)
{
    g_game.update(dt);
}

void draw()
{
    g_game.draw();
}


// ---6. MAIN---
int main()
{
    // window creasion (only once)
    graphics::createWindow(800, 600, "SGG Test");

    // 
    graphics::setCanvasSize(800, 600);

    // SGG funcions
    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);

    // begins game loop
    graphics::startMessageLoop();

    // 7) (Προαιρετικό) καθάρισμα – σε κάποιες εκδόσεις υπάρχει
    graphics::destroyWindow();
    return 0;
}