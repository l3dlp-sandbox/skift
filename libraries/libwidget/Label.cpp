#include <libgraphic/Painter.h>
#include <libwidget/Label.h>
#include <libwidget/Window.h>
#include <string.h>

Label::Label(Component *parent, String text)
    : Label(parent, text, Anchor::LEFT)
{
}

Label::Label(Component *parent, String text, Anchor anchor)
    : Component(parent)
{
    _text = text;
    _anchor = anchor;
}

void Label::paint(Graphic::Painter &painter, const Recti &)
{
    painter.draw_string_within(
        *font(),
        _text.cstring(),
        content(),
        _anchor,
        color(THEME_FOREGROUND));
}

Vec2i Label::size()
{
    return {font()->mesure_with_fulllineheight(_text.cstring()).size()};
}
