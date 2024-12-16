#pragma once

#include <karm-gfx/canvas.h>
#include <karm-logger/logger.h>

#include "font.h"

namespace Karm::Text {

enum struct TextAlign {
    LEFT,
    CENTER,
    RIGHT,
};

struct ProseStyle {
    Font font;
    TextAlign align = TextAlign::LEFT;
    Opt<Gfx::Color> color = NONE;
    bool wordwrap = true;
    bool multiline = false;

    ProseStyle withSize(f64 size) const {
        ProseStyle style = *this;
        style.font.fontsize = size;
        return style;
    }

    ProseStyle withLineHeight(f64 height) const {
        ProseStyle style = *this;
        style.font.lineheight = height;
        return style;
    }

    ProseStyle withAlign(TextAlign align) const {
        ProseStyle style = *this;
        style.align = align;
        return style;
    }

    ProseStyle withColor(Gfx::Color color) const {
        ProseStyle style = *this;
        style.color = color;
        return style;
    }

    ProseStyle withWordwrap(bool wordwrap) const {
        ProseStyle style = *this;
        style.wordwrap = wordwrap;
        return style;
    }

    ProseStyle withMultiline(bool multiline) const {
        ProseStyle style = *this;
        style.multiline = multiline;
        return style;
    }
};

struct Prose : public Meta::Static {
    struct Span {
        MutCursor<Span> parent = nullptr;
        Opt<Gfx::Color> color;
    };

    struct Cell {
        MutCursor<Prose> prose;
        MutCursor<Span> span;

        urange runeRange;
        Glyph glyph;
        f64 pos = 0; //< Position of the glyph within the block
        f64 adv = 0; //< Advance of the glyph

        MutSlice<Rune> runes() {
            return mutSub(prose->_runes, runeRange);
        }

        Slice<Rune> runes() const {
            return sub(prose->_runes, runeRange);
        }

        bool newline() const {
            auto r = runes();
            if (not r)
                return false;
            return last(r) == '\n';
        }

        bool space() const {
            auto r = runes();
            if (not r)
                return false;
            return last(r) == '\n' or isAsciiSpace(last(r));
        }
    };

    struct Block {
        MutCursor<Prose> prose;

        urange runeRange;
        urange cellRange;

        f64 pos = 0; // Position of the block within the line
        f64 width = 0;

        MutSlice<Cell> cells() {
            return mutSub(prose->_cells, cellRange);
        }

        Slice<Cell> cells() const {
            return sub(prose->_cells, cellRange);
        }

        bool empty() const {
            return cellRange.empty();
        }

        bool spaces() const {
            if (empty())
                return false;
            return last(cells()).space();
        }

        bool newline() const {
            if (empty())
                return false;
            return last(cells()).newline();
        }
    };

    struct Line {
        MutCursor<Prose> prose;

        urange runeRange;
        urange blockRange;
        f64 baseline = 0; // Baseline of the line within the text
        f64 width = 0;

        Slice<Block> blocks() const {
            return sub(prose->_blocks, blockRange);
        }

        MutSlice<Block> blocks() {
            return mutSub(prose->_blocks, blockRange);
        }
    };

    ProseStyle _style;

    Vec<Rune> _runes;
    Vec<Cell> _cells;
    Vec<Block> _blocks;
    Vec<Line> _lines;

    // Various cached values
    bool _blocksMeasured = false;
    f64 _spaceWidth{};
    f64 _lineHeight{};

    Math::Vec2f _size;

    Prose(ProseStyle style, Str str = "");

    Math::Vec2f size() const {
        return _size;
    }

    // MARK: Prose --------------------------------------------------------------

    void _beginBlock();

    void append(Rune rune);

    void clear();

    template <typename E>
    void append(_Str<E> str) {
        for (auto rune : iterRunes(str))
            append(rune);
    }

    void append(Slice<Rune> runes);

    // MARK: Span --------------------------------------------------------------

    Vec<Box<Span>> _spans;
    MutCursor<Span> _currentSpan = nullptr;

    void pushSpan() {
        _spans.pushBack(makeBox<Span>(_currentSpan));
        _currentSpan = &*last(_spans);
    }

    void spanColor(Gfx::Color color) {
        if (not _currentSpan)
            return;

        _currentSpan->color = color;
    }

    void popSpan() {
        if (_currentSpan)
            _currentSpan = _currentSpan->parent;
    }

    // MARK: Layout ------------------------------------------------------------

    void _measureBlocks();

    void _wrapLines(f64 width);

    f64 _layoutVerticaly();

    f64 _layoutHorizontaly(f64 width);

    Math::Vec2f layout(f64 width);

    // MARK: Paint -------------------------------------------------------------

    void paint(Gfx::Canvas &g);

    void paintCaret(Gfx::Canvas &g, usize runeIndex, Gfx::Color color) const {
        auto m = _style.font.metrics();
        auto baseline = queryPosition(runeIndex);
        auto cs = baseline - Math::Vec2f{0, m.ascend};
        auto ce = baseline + Math::Vec2f{0, m.descend};

        g.beginPath();
        g.moveTo(cs);
        g.lineTo(ce);
        g.strokeStyle(Gfx::stroke(color).withAlign(Gfx::CENTER_ALIGN).withWidth(1));
        g.stroke();
    }

    struct Lbc {
        usize li, bi, ci;
    };

    Lbc lbcAt(usize runeIndex) const {
        auto maybeLi = searchLowerBound(
            _lines, [&](Line const &l) {
                return l.runeRange.start <=> runeIndex;
            }
        );

        if (not maybeLi)
            return {0, 0, 0};

        auto li = *maybeLi;

        auto &line = _lines[li];

        auto maybeBi = searchLowerBound(
            line.blocks(), [&](Block const &b) {
                return b.runeRange.start <=> runeIndex;
            }
        );

        if (not maybeBi)
            return {li, 0, 0};

        auto bi = *maybeBi;

        auto &block = line.blocks()[bi];

        auto maybeCi = searchLowerBound(
            block.cells(), [&](Cell const &c) {
                return c.runeRange.start <=> runeIndex;
            }
        );

        if (not maybeCi)
            return {li, bi, 0};

        auto ci = *maybeCi;

        auto cell = block.cells()[ci];

        if (cell.runeRange.end() == runeIndex) {
            // Handle the case where the rune is the last of the text
            ci++;
        }

        return {li, bi, ci};
    }

    Math::Vec2f queryPosition(usize runeIndex) const {
        auto [li, bi, ci] = lbcAt(runeIndex);

        if (isEmpty(_lines))
            return {};

        auto &line = _lines[li];

        if (isEmpty(line.blocks()))
            return {0, line.baseline};

        auto &block = line.blocks()[bi];

        if (isEmpty(block.cells()))
            return {block.pos, line.baseline};

        if (ci >= block.cells().len()) {
            // Handle the case where the rune is the last of the text
            auto &cell = last(block.cells());
            return {block.pos + cell.pos + cell.adv, line.baseline};
        }

        auto &cell = block.cells()[ci];

        return {block.pos + cell.pos, line.baseline};
    }
};

} // namespace Karm::Text
