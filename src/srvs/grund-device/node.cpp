#include "node.h"

namespace Dev {

Node::Node() {
    static usize id = 0;
    _id = id++;
}

Node::~Node() {
    for (auto &child : _children) {
        child->_parent = nullptr;
    }
}

Res<> Node::init() {
    for (auto &child : _children) {
        try$(child->init());
    }
    return Ok();
}

Res<> Node::event(Events::Event &e) {
    if (e.accepted) {
        return Ok();
    }

    for (auto &child : _children) {
        try$(child->event(e));

        if (e.accepted) {
            return Ok();
        }
    }

    return Ok();
}

Res<> Node::bubble(Events::Event &e) {
    if (_parent and not e.accepted) {
        try$(_parent->bubble(e));
    }

    return Ok();
}

Res<> Node::attach(Strong<Node> child) {
    child->_parent = this;
    _children.pushBack(child);
    try$(child->init());
    return Ok();
}

void Node::detach(Strong<Node> child) {
    child->_parent = nullptr;
    _children.removeAll(child);
}

} // namespace Dev