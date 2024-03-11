#include "spec.h"

namespace Mdi {

char const *_names[] = {
#define ICON(id, name, code) name,
#include "defs/icons.inc"
#undef ICON
};

Rune _codepoints[] = {
#define ICON(id, name, code) code,
#include "defs/icons.inc"
#undef ICON
};

inline Slice<Rune> codepoints() {
    return Slice{
        _codepoints,
        sizeof(_codepoints) / sizeof(_codepoints[0]),
    };
}

inline Res<Icon> byName(Str query) {
    for (size_t i = 0; i < sizeof(_names) / sizeof(_names[0]); ++i) {
        if (Str{_names[i]} == query)
            return Res<Icon>{Icon(_codepoints[i])};
    }
    return Error::notFound("icon not found");
}

inline Str name(Icon icon) {
    for (size_t i = 0; i < sizeof(_codepoints) / sizeof(_codepoints[0]); ++i) {
        if (_codepoints[i] == (Rune)icon)
            return Str{_names[i]};
    }
    return "unknown";
}

} // namespace Mdi