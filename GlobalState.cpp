#include "GlobalState.h"
#include "Tube.h"
#include <sgg/graphics.h>
#include <cmath>

// ----4. HELPER FUNCTIONS (LOGIC)----

void GlobalState::colorToRGB(Color c, float& r, float& g, float& b) const
{
    switch (c)
    {
    case Color::Red:    r = 1.0f; g = 0.2f; b = 0.2f; break;
    case Color::Green:  r = 0.2f; g = 1.0f; b = 0.2f; break;
    case Color::Blue:   r = 0.2f; g = 0.4f; b = 1.0f; break;
    case Color::Yellow: r = 1.0f; g = 1.0f; b = 0.2f; break;
    case Color::Purple: r = 0.7f; g = 0.2f; b = 1.0f; break;
    case Color::Orange: r = 1.0f; g = 0.6f; b = 0.2f; break;
    default:            r = 0.5f; g = 0.5f; b = 0.5f; break;
    }
}

bool GlobalState::canPour(int from, int to) const
{
    const Tube& fromTube = tubes[from];
    const Tube& toTube = tubes[to];

    if (fromTube.empty()) return false;
    if (toTube.full())   return false;

    // if destination is empty then you can move
    if (toTube.empty()) return true;

    // or you can move if the color matches
    return fromTube.top() == toTube.top();
}

int GlobalState::pourAmount(int from, int to) const
{
    if (!canPour(from, to)) return 0;

    const Tube& fromTube = tubes[from];
    const Tube& toTube = tubes[to];

    int block = fromTube.topBlockCount(); // same color at the top of "from"
    int space = Tube::CAP - toTube.size();       // free space of "to"

    return (block < space) ? block : space;
}

bool GlobalState::pour(int fromIdx, int toIdx)
{
    if (fromIdx == toIdx) return false;
    if (fromIdx < 0 || fromIdx >= (int)tubes.size()) return false;
    if (toIdx < 0 || toIdx >= (int)tubes.size()) return false;

    Tube& from = tubes[fromIdx];
    Tube& to = tubes[toIdx];

    int amount = pourAmount(fromIdx, toIdx);   
    if (amount == 0) return false;

    Color c = from.top();
    for (int i = 0; i < amount; i++) {
        from.layers.pop_back();
        to.layers.push_back(c);
    }
    return true;
}


void GlobalState::loadLevel(int level)
{
    prevLeftDown = false;
    tubes.clear();
    selected.reset();
    state = GameState::PLAYING;

    if (level == 0)
    {
        Tube t0; t0.layers = { Color::Red, Color::Red, Color::Green, Color::Green };
        Tube t1; t1.layers = { Color::Green, Color::Red, Color::Green, Color::Red };
        Tube t2;
        Tube t3;
        tubes = { t0, t1, t2, t3 };
        this->level = 0;              // σωστό αν έχεις member level
    }
    else if (level == 1)
    {
        Tube t0; t0.layers = { Color::Red, Color::Green, Color::Red, Color::Green };
        Tube t1; t1.layers = { Color::Green, Color::Red, Color::Green, Color::Red };
        Tube t2;
        tubes = { t0, t1, t2 };
        this->level = 1;              // idem
    }
    else
    {
        // αν τελειώσουν τα levels, γύρνα στο 0
        this->level = 0;
        loadLevel(0);
    }
}


// ΕΛΕΓΧΕΙ ΑΝ ΕΝΑΣ ΣΩΛΗΝΑΣ ΕΙΝΑΙ "ΛΥΜΕΝΟΣ"
bool GlobalState::isTubeSolved(const Tube& t) const
{
    // άδειο tube -> οκ
    if (t.empty()) return true;

    // ΓΕΜΑΤΟ ΚΑΙ ίδιο χρώμα -> οκ 
    if (!t.full()) return false;

    // όλα τα layers πρέπει να είναι ίδια
    Color c = t.layers[0];
    for (int i = 1; i < t.size(); ++i)
    {
        if (t.layers[i] != c) return false;
    }
    return true;
}


// ΕΛΕΓΧΕΙ ΑΝ ΤΟ ΠΑΙΧΝΙΔΙ ΕΧΕΙ ΤΕΛΕΙΩΣΕΙ (WIN)
bool GlobalState::isWin() const
{
    for (const auto& t : tubes)
        if (!isTubeSolved(t))
            return false;

    return true;
}

void GlobalState::handleResetUI(bool click, float mx, float my)
{
    if (!click) return;

    if (pointInRect(mx, my, resetBtn))
    {
        // RESET τρέχον level
        loadLevel(level);

        selected.reset();
        prevLeftDown = false;
    }
}

void GlobalState::handleExitUI(bool click, float mx, float my)
{
    if (!click) return;

    if (pointInRect(mx, my, exitBtn))
    {
        graphics::stopMessageLoop();
    }
}


void GlobalState::drawTube(const Tube& t, float x, float y) const
{
    const float w = 70.0f;   // width tube
    const float h = 220.0f;  // hight tube
    const float pad = 6.0f;  // “πάχος” γυαλιού/περιθώριο
    const float r = w / 2.0f; // radius για στρογγυλή βάση
    const float topY = y - h / 2.0f;
    const float bottomY = y + h / 2.0f;

    const float leftX = x - w / 2.0f;
    const float rightX = x + w / 2.0f;

    // outlines tube
    graphics::Brush outline;
    outline.fill_opacity = 0.0f;           // only outline
    outline.outline_opacity = 1.0f;
    outline.outline_width = 2.0f;
    outline.outline_color[0] = 1.0f;
    outline.outline_color[1] = 1.0f;
    outline.outline_color[2] = 1.0f;

    // 1) δύο πλευρές
    graphics::drawLine(leftX, topY, leftX, bottomY - r, outline);
    graphics::drawLine(rightX, topY, rightX, bottomY - r, outline);

    // 2) rounded bottom
    const int SEG = 18; 
    const float cx = x;
    const float cy = bottomY - r;

    for (int i = 0; i < SEG; i++)
    {
        float t0 = 3.1415926f * (float)i / (float)SEG;      
        float t1 = 3.1415926f * (float)(i + 1) / (float)SEG;

        float x0 = cx + r * cosf(t0);
        float y0 = cy + r * sinf(t0);
        float x1 = cx + r * cosf(t1);
        float y1 = cy + r * sinf(t1);

        graphics::drawLine(x0, y0, x1, y1, outline);
    }


    // layers ( bottom-> up)
    const float innerW = w - 2 * pad;
    const float innerH = h - 2 * pad;
    const float layerH = innerH / Tube::CAP;

    const float waterPad = 2.0f;
    const float waterW = innerW - 2.0f * waterPad;
    const float waterR = (w / 2.0f) - pad - waterPad;
    const float innerBottom = y + h / 2.0f - pad;

    const float bottomCy = (y + h / 2.0f) - (w / 2.0f); // ίδιο κέντρο με την καμπύλη του outline
   
    for (int i = 0; i < t.size(); i++)
    {
        Color c = t.layers[i]; // i=0 bottom
        float r, g, b;
        colorToRGB(c, r, g, b);

        graphics::Brush layer;
        layer.fill_opacity = 1.0f;
        layer.outline_opacity = 0.0f;
        layer.fill_color[0] = r;
        layer.fill_color[1] = g;
        layer.fill_color[2] = b;

        // y for every layer: bottom starts low
        float ly = (y + h / 2.0f - pad) - layerH / 2.0f - i * layerH;

        if (i == 0) // bottom layer
        {
            const float topY = innerBottom - layerH;
            const float rectH = bottomCy - topY;
            const float rectCy = (bottomCy + topY) / 2.0f;

            graphics::drawDisk(x, bottomCy, waterR, layer);
            graphics::drawRect(x, rectCy, waterW, rectH, layer);
        }
        else
        {
            graphics::drawRect(x, ly, waterW, layerH, layer);
        }
    }
}

bool GlobalState::pointInRect(float px, float py, const Rect& r) const
{
    return px >= r.x && px <= r.x + r.w && py >= r.y && py <= r.y + r.h;
}

void GlobalState::handleWinUI(bool click, float mx, float my)
{
    if (!click) return;

    if (pointInRect(mx, my, nextBtn))
    {
        level++;
        loadLevel(level);
        state = GameState::PLAYING;
        selected.reset();
        prevLeftDown = false;
    }
}

void GlobalState::handleStartUI(bool click, float mx, float my)
{
    if (!click) return;

    if (pointInRect(mx, my, startBtn))
    {
        state = GameState::PLAYING;
        loadLevel(level);
        selected.reset();
        prevLeftDown = false;
    }
}

int GlobalState::tubeAt(float mx, float my) const
{
    float startX = 120.0f;
    float y = 360.0f;
    float stepX = 95.0f;
    float w = 70.0f;
    float h = 220.0f;

    for (int i = 0; i < (int)tubes.size(); i++)
    {
        float x = startX + i * stepX;
        if (mx >= x - w / 2 && mx <= x + w / 2 &&
            my >= y - h / 2 && my <= y + h / 2)
        {
            return i;
        }
    }
    return -1;
}

void GlobalState::init()
{
    state = GameState::START;
    level = 0;
    initialized = true;

    graphics::setFont("assets/GILLUBCD.TTF");
    selected.reset();
}

void GlobalState::update(float dt)
{
    if (!initialized) init();

    graphics::MouseState ms;
    graphics::getMouseState(ms);

    float mx = ms.cur_pos_x;
    float my = ms.cur_pos_y;

    // click = "μόνο στο πάτημα" (edge)
    bool mouseDown = ms.button_left_down;
    bool click = (mouseDown && !prevLeftDown);
    prevLeftDown = mouseDown;

    if (state == GameState::START)
    {
        handleStartUI(click, mx, my);
        return;
    }

    if (state == GameState::WIN)
    {
        // εδώ θα χειριστούμε μόνο το κουμπί "Next"
        handleWinUI(click, mx, my);
        return;
    }

    //  RESET BUTTON
    handleResetUI(click, mx, my);
    handleExitUI(click, mx, my);
    handleMouse(click, mx, my);

    // win check στο τέλος (όχι μέσα στο pour)
    if (isWin()) {
        state = GameState::WIN;
    }

}

void GlobalState::draw()
{
    // background
    graphics::Brush bg;
    bg.fill_opacity = 1.0f;
    bg.fill_color[0] = 0.12f;
    bg.fill_color[1] = 0.12f;
    bg.fill_color[2] = 0.14f;
    graphics::drawRect(400, 300, 800, 600, bg);

    // text brush (σκούρο) 
    // κοινο για ολα τα μαυρα μηνυματα 
    graphics::Brush txt;
    txt.fill_opacity = 1.0f;
    txt.fill_color[0] = 0.0f;
    txt.fill_color[1] = 0.0f;
    txt.fill_color[2] = 0.0f;

    // text
    //κοινα αλλα για λευκα μηνυματα
    graphics::Brush text;
    text.fill_opacity = 1.0f;
    text.fill_color[0] = 1.0f;
    text.fill_color[1] = 1.0f;
    text.fill_color[2] = 1.0f;
    text.fill_opacity = 1.0f;


    //first row of tubes
    float startX = 120.0f;
    float y = 360.0f;   // senter
    float stepX = 95.0f;    // distance between tubes

    for (int i = 0; i < (int)tubes.size(); i++)
    {
        float x = startX + i * stepX;
        if (selected.has_value() && selected.value() == i)
        {
            graphics::Brush sel;
            sel.fill_opacity = 0.0f;
           sel.outline_opacity = 1.0f;
            sel.outline_width = 4.0f;
            sel.outline_color[0] = 1.0f;
            sel.outline_color[1] = 1.0f;
            sel.outline_color[2] = 0.0f;

            graphics::drawRect(x, y, 80.0f, 230.0f, sel);
        }

        drawTube(tubes[i], x, y);
    }

    if (state == GameState::WIN)
    {
        graphics::Brush overlay;
        overlay.fill_opacity = 0.6f;
        graphics::drawRect(400, 300, 800, 600, overlay);

        // κείμενα
        graphics::drawText(400 - 95, 230, 48, "YOU WIN!", text);

        // κουμπί
        graphics::Brush btn;
        btn.fill_opacity = 1.0f;
        graphics::drawRect(400, 340, nextBtn.w, nextBtn.h, btn);

        graphics::drawText(400 - 60, 348, 24, "Next Level", txt);
    }

    if (state == GameState::START)
    {
        graphics::Brush overlay;
        overlay.fill_opacity = 0.6f;
        graphics::drawRect(400, 300, 800, 600, overlay);

 
        graphics::drawText(400 - 120 + 12, 210, 48, "WELCOME", text);
        graphics::drawText(400 - 170 + 18, 255, 22, "Sort the water and have fun!", text);

        // START BUTTON
        graphics::drawRect( 400, 340, startBtn.w, startBtn.h, text);

        graphics::drawText(400 - 40, 348, 24, "START", txt);
    }

    if (state == GameState::PLAYING)
    {

        graphics::drawText(100, 100, 24, "Sort The Water", text);

        graphics::Brush b;
        b.fill_opacity = 1.0f;

        // κύκλος κουμπιού
        graphics::drawDisk(
            resetBtn.x + resetBtn.w / 2,
            resetBtn.y + resetBtn.h / 2,
            22,
            b
        );

        // "R"
        graphics::drawText(resetBtn.x + 19, resetBtn.y + 34, 24, "R", txt);

        // κύκλος κουμπιού (EXIT)
        graphics::drawDisk(
            exitBtn.x + exitBtn.w / 2,
            exitBtn.y + exitBtn.h / 2,
            22,
            b
        );

        graphics::drawText(exitBtn.x + exitBtn.w / 2 - 7, exitBtn.y + exitBtn.h / 2 + 8, 24, "X", txt);
    }

}


void GlobalState::resetLevel()
{
    // TODO: θα καλεί loadLevel(level) και θα μηδενίζει selection
    selected = -1;
    loadLevel(level);
}

void GlobalState::handleMouse(bool click, float mx, float my)
{
    if (!click) return;

    int idx = tubeAt(mx, my);
    if (idx == -1) return;

    if (!selected.has_value())
    {
        selected = idx;
        return;
    }

    int from = selected.value();
    int to = idx;

    if (from != to)
    {
        pour(from, to);
    }

    selected.reset();
}
