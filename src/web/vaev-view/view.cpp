#include <karm-ui/view.h>
#include <vaev-base/color.h>

#include "render.h"
#include "view.h"

namespace Vaev::View {

struct View : public Ui::View<View> {
    Strong<Dom::Document> _dom;
    Opt<Strong<Paint::Node>> _paintRoot;

    View(Strong<Dom::Document> dom) : _dom(dom) {}

    void paint(Gfx::Context &g, Math::Recti) override {
        if (not _paintRoot)
            _paintRoot = render(*_dom, bound().size().cast<Px>());

        g.save();

        g.origin(bound().xy);
        g.clip(bound().size());
        g.clear(bound().size(), WHITE);

        (*_paintRoot)->paint(g);
        g.restore();
    }

    void layout(Math::Recti bound) override {
        _paintRoot = NONE;
        Ui::View<View>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return {};
    }
};

Ui::Child view(Strong<Dom::Document> dom) {
    return makeStrong<View>(dom);
}

} // namespace Vaev::View
