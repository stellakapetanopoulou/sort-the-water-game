#pragma once
#include <vector>
#include "Color.h"


struct Tube {
    static constexpr int CAP = 4;           // 4 layers
    std::vector<Color> layers;              //layers[0] bottom to layers.back() top

    int size()  const { return (int)layers.size(); }
    bool empty() const { return layers.empty(); }
    bool full()  const { return size() >= CAP; }

    Color top() const
    {
        return empty() ? Color::Empty : layers.back();
    }

    //same layers on top 
    int topBlockCount() const {
        if (empty()) return 0;
        Color c = top();
        int cnt = 0;
        for (int i = size() - 1; i >= 0; --i) {
            if (layers[i] == c) cnt++;
            else break;
        }
        return cnt;
    }


};
