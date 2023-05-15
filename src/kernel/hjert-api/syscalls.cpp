#include "syscalls.h"

namespace Hj {

#ifdef __osdk_arch_x86_64__

Res<> _syscall(Syscall s, Arg a0, Arg a1, Arg a2, Arg a3, Arg a4, Arg a5) {
    // s : rax, arg1 : rdi, arg2 : rsi, arg3 : rdx, arg4 : r10, arg5 : r8, arg6 : r9
    Error::Code c = {};
    register Arg r3 asm("r10") = a3;
    register Arg r4 asm("r8") = a4;
    register Arg r5 asm("r9") = a5;
    asm volatile("syscall"
                 : "=a"(c)
                 : "a"(s), "D"(a0), "S"(a1), "d"(a2),
                   "r"(r3), "r"(r4), "r"(r5)
                 : "rcx", "r11", "memory");

    if (c != Error::Code::_OK)
        return Error(c);

    return Ok();
}

#else
#    error "Unsupported architecture"
#endif

Res<> _log(char const *msg, usize len) {
    return _syscall(Syscall::LOG, (Arg)msg, len);
}

Res<> _createDomain(Cap dest, Cap *cap) {
    return _syscall(Syscall::CREATE_DOMAIN, dest.raw(), (Arg)cap);
}

Res<> _createTask(Cap dest, Cap *cap, Cap node, Cap space) {
    return _syscall(Syscall::CREATE_TASK, dest.raw(), (Arg)cap, node.raw(), space.raw());
}

Res<> _createSpace(Cap dest, Cap *cap) {
    return _syscall(Syscall::CREATE_SPACE, dest.raw(), (Arg)cap);
}

Res<> _createVmo(Cap dest, Cap *cap, usize phys, usize len, VmoFlags flags) {
    return _syscall(Syscall::CREATE_VMO, dest.raw(), (Arg)cap, phys, len, (Arg)flags);
}

Res<> _createIo(Cap dest, Cap *cap, usize base, usize len) {
    return _syscall(Syscall::CREATE_IO, dest.raw(), (Arg)cap, base, len);
}

Res<> _createChannel(Cap dest, Cap *cap, usize len) {
    return _syscall(Syscall::CREATE_CHANNEL, dest.raw(), (Arg)cap, len);
}

Res<> _createIrq(Cap dest, Cap *cap, usize irq) {
    return _syscall(Syscall::CREATE_IRQ, dest.raw(), (Arg)cap, irq);
}

Res<> _label(Cap cap, char const *label, usize len) {
    return _syscall(Syscall::LABEL, cap.raw(), (Arg)label, len);
}

Res<> _drop(Cap cap) {
    return _syscall(Syscall::DROP, cap.raw());
}

Res<> _dup(Cap node, Cap *dst, Cap src) {
    return _syscall(Syscall::DUP, node.raw(), (Arg)dst, src.raw());
}

Res<> _start(Cap cap, usize ip, usize sp, Args const *args) {
    return _syscall(Syscall::START, cap.raw(), ip, sp, (Arg)args);
}

using MapFlags = Hal::VmmFlags;

Res<> _map(Cap cap, usize *virt, Cap mem, usize off, usize len, MapFlags flags) {
    return _syscall(Syscall::MAP, cap.raw(), (Arg)virt, mem.raw(), off, len, (Arg)flags);
}

Res<> _unmap(Cap cap, usize virt, usize len) {
    return _syscall(Syscall::UNMAP, cap.raw(), virt, len);
}

Res<> _in(Cap cap, IoLen len, usize port, Arg *val) {
    return _syscall(Syscall::IN, cap.raw(), (Arg)len, port, (Arg)val);
}

Res<> _out(Cap cap, IoLen len, usize port, Arg val) {
    return _syscall(Syscall::OUT, cap.raw(), (Arg)len, port, val);
}

Res<> _send(Cap cap, Msg const *msg, Cap from) {
    return _syscall(Syscall::SEND, cap.raw(), (Arg)msg, from.raw());
}

Res<> _recv(Cap cap, Msg *msg, Cap to) {
    return _syscall(Syscall::RECV, cap.raw(), (Arg)msg, to.raw());
}

Res<> _close(Cap cap) {
    return _syscall(Syscall::CLOSE, cap.raw());
}

Res<> _signal(Cap cap, Signals set, Signals unset, Arg value) {
    return _syscall(Syscall::SIGNAL, cap.raw(), (Arg)set, (Arg)unset, value);
}

Res<> _wait(Cond const *conds, usize condLen, Event *ev, usize *evLen, Karm::TimeStamp deadline) {
    return _syscall(Syscall::WAIT, (Arg)conds, condLen, (Arg)ev, (Arg)evLen, deadline.val());
}

} //  namespace Hj