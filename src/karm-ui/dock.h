#pragma once

#include "flow.h"

namespace Karm::Ui {

struct Dock {
    enum _Dock {
        NONE,
        FILL,
        START,
        TOP,
        END,
        BOTTOM,
    };

    _Dock _dock;

    Dock(_Dock dock) : _dock(dock) {}

    Flow flow() {
        switch (_dock) {
        default:
        case NONE:
        case FILL:
            return Flow::LEFT_TO_RIGHT;

        case START:
            return Flow::LEFT_TO_RIGHT;

        case TOP:
            return Flow::TOP_TO_BOTTOM;

        case END:
            return Flow::RIGHT_TO_LEFT;

        case BOTTOM:
            return Flow::BOTTOM_TO_TOP;
        }
    }

    template <typename T>
    Math::Rect<T> apply(Math::Rect<T> inner, Math::Rect<T> &outer) {
        switch (_dock) {
        case NONE:
            return inner;

        case FILL:
            return outer;

        default:
            inner = flow().setOrigin(inner, flow().getOrigin(outer));
            inner = flow().setHeight(inner, flow().getHeight(outer));
            outer = flow().setStart(outer, flow().getEnd(inner));
            return inner;
        }
    }
};

} // namespace Karm::Ui
