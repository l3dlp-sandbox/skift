#pragma once

#include <karm-text/font.h>
#include <karm-text/run.h>
#include <vaev-paint/text.h>

#include "frag.h"

namespace Vaev::Layout {

struct Run : public Frag {
    static constexpr auto TYPE = RUN;
    Strong<Text::Run> _run;

    Run(Strong<Style::Computed> style, Strong<Text::Run> run)
        : Frag(style), _run(run) {
    }

    Type type() const override {
        return TYPE;
    }

    Px computeIntrinsicSize(Context &, Axis axis, IntrinsicSize, Px) override {
        return Px{_run->layout()[axis.index()]};
    }

    void makePaintables(Paint::Stack &s) override {
        s.add(makeStrong<Paint::Text>(_box.borderBox.topStart().cast<f64>(), _run));
    }
};

} // namespace Vaev::Layout
