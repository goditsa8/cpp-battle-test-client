#pragma once

#include <cstddef>
#include <vector>
#include <cassert>

#include "Core/GameObject.hpp"


struct Square {
    uint32_t x = 0;
    uint32_t y = 0;
    GameObject* unit = nullptr;

    bool IsEmpty() const { return !unit; }
};


class Map : public Component {
public:
    Map(uint32_t width, u_int32_t height) {
        squares.resize(width);
        for (int i = 0; i < width; i++) {
            squares[i].resize(height);
        }

        for (uint32_t i = 0; i < width; i++) {
            for (uint32_t j = 0; j < height; j++) {
                squares[i][j] = {i, j};
            }
        }
    }

    Square& GetSquare(uint32_t x, uint32_t y) {
        assert(x < GetWidth() && y < GetHeight());
        return squares[x][y];
    }

    bool IsValidSquare(uint32_t x, uint32_t y) const { return x < GetWidth() && y < GetHeight(); }

    int GetWidth() const { return squares.size(); }
    int GetHeight() const { return squares[0].size(); }

private:
    std::vector<std::vector<Square>> squares;
};


static inline GameObject* map;

inline GameObject* CreateMap(uint32_t w, uint32_t h) {
    map = new GameObject(0);
    map->AddComponent(new Map(w, h));

    return map;
}

inline GameObject* GetMap() {
    return map;
}