#pragma once

#include "align.h"
#include "box.h"
#include "funcs.h"
#include "proxy.h"
#include "text.h"

namespace Karm::Ui {

struct Button : public Proxy<Button> {
    enum State {
        IDLE,
        OVER,
        PRESS,
    };

    enum Style {
        DEFAULT,
        PRIMARY,
        OUTLINE,
        SUBTLE,
    };

    Func<void()> _onPress;
    Style _style = DEFAULT;
    State _state = IDLE;
    static constexpr int RADIUS = 4;

    Button(Func<void()> onPress, Style style, Child child)
        : Proxy(child), _onPress(std::move(onPress)), _style(style) {}

    void paint(Gfx::Context &g) override {
        if (_style == PRIMARY) {
            if (_state == OVER) {
                g.fillStyle(Gfx::BLUE600);
                g.fill(bound(), RADIUS);
            } else {
                g.fillStyle(Gfx::BLUE700);
                g.fill(bound(), RADIUS);
            }
        } else if (_style == DEFAULT) {
            if (_state == OVER) {
                g.fillStyle(Gfx::ZINC600);
                g.fill(bound(), RADIUS);
            } else {
                g.fillStyle(Gfx::ZINC700);
                g.fill(bound(), RADIUS);
            }
        } else if (_style == OUTLINE) {
            if (_state == OVER) {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC600)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), RADIUS);
            } else {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC700)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), RADIUS);
            }
        } else if (_style == SUBTLE) {
            if (_state == OVER) {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC600)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), RADIUS);
            } else if (_state == PRESS) {
                g.strokeStyle(Gfx::stroke(Gfx::ZINC700)
                                  .withWidth(1)
                                  .withAlign(Gfx::INSIDE_ALIGN));
                g.stroke(bound(), RADIUS);
            }
        }

        g.save();
        g.fillStyle(Gfx::fill(Gfx::WHITE));
        child().paint(g);
        g.restore();
    }

    void event(Events::Event &e) override {
        State state = _state;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (!bound().contains(m.pos)) {
                state = IDLE;
                return false;
            }

            if (state != PRESS)
                state = OVER;

            if (m.type == Events::MouseEvent::PRESS) {
                state = PRESS;
            } else if (m.type == Events::MouseEvent::RELEASE) {
                state = OVER;
                _onPress();
            }

            return true;
        });

        if (state != _state) {
            _state = state;
            Ui::shouldRepaint(*this);
        }
    };
};

static inline Child button(Func<void()> onPress, Button::Style style, Child child) {
    return makeStrong<Button>(std::move(onPress), style, child);
}

static inline Child button(Func<void()> onPress, Button::Style style, Str t) {
    return button(
        std::move(onPress),
        style,
        pinSize({Sizing::UNCONSTRAINED, 36},
                center(
                    spacing(
                        {16, 0},
                        text(t)))));
}

static inline Child button(Func<void()> onPress, Child child) {
    return button(std::move(onPress), Button::DEFAULT, child);
}

static inline Child button(Func<void()> onPress, Str t) {
    return button(std::move(onPress), Button::DEFAULT, t);
}

static inline Child primaryButton(Func<void()> onPress, Child child) {
    return button(std::move(onPress), Button::PRIMARY, child);
}

static inline Child primaryButton(Func<void()> onPress, Str t) {
    return button(std::move(onPress), Button::PRIMARY, t);
}

static inline Child outlineButton(Func<void()> onPress, Child child) {
    return button(std::move(onPress), Button::OUTLINE, child);
}

static inline Child outlineButton(Func<void()> onPress, Str t) {
    return button(std::move(onPress), Button::OUTLINE, t);
}

static inline Child subtleButton(Func<void()> onPress, Child child) {
    return button(std::move(onPress), Button::SUBTLE, child);
}

static inline Child subtleButton(Func<void()> onPress, Str t) {
    return button(std::move(onPress), Button::SUBTLE, t);
}

} // namespace Karm::Ui