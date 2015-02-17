#include <stdexcept>
#include "CTile.h"

CTile::CTile(const chtype c) {
    for (int i = 0; i < TILE_AREA; i++) {
        m_tiles[i] = c;
    }
}

CTile::CTile(const chtype c1, const chtype c2, const chtype c3, const chtype c4, const chtype c5, const chtype c6) {
    Set(c1, c2, c3, c4, c5, c6);
}

void CTile::Set(const chtype c1, const chtype c2, const chtype c3, const chtype c4, const chtype c5, const chtype c6) {
    m_tiles[0] = c1;
    m_tiles[1] = c2;
    m_tiles[2] = c3;
    m_tiles[3] = c4;
    m_tiles[4] = c5;
    m_tiles[5] = c6;
}

CTile::~CTile() {
}

chtype& CTile::operator [](int i) {
    if (i < 0 || i >= TILE_AREA)
        throw out_of_range("tile character index out of range");
    return m_tiles[i];
}

void CTile::Draw() const {
    int x = getcurx(stdscr);
    int y = getcury(stdscr);

    for (int i = 0; i < TILE_AREA; i++) {
        if (i != 0 && i % (TILE_AREA / HEIGHT) == 0)
            wmove(stdscr, y + 1, x);
        waddch(stdscr, m_tiles[i]);
    }

    wmove(stdscr, y, x);
}



