#pragma once

#include <skift/Time.h>

#include <libutils/Vector.h>

#include "kernel/node/Handle.h"
#include "kernel/system/System.h"

struct Task;

enum BlockerResult
{
    BLOCKER_UNBLOCKED,
    BLOCKER_TIMEOUT,
};

struct Blocker
{
    BlockerResult _result;
    TimeStamp _timeout;

    virtual ~Blocker() {}

    virtual bool can_unblock(struct Task *task)
    {
        __unused(task);
        return true;
    }

    virtual void on_unblock(struct Task *task)
    {
        __unused(task);
    }

    virtual void on_timeout(struct Task *task)
    {
        __unused(task);
    }
};

class BlockerAccept : public Blocker
{
private:
    RefPtr<FsNode> _node;

public:
    BlockerAccept(RefPtr<FsNode> node) : _node(node)
    {
    }

    bool can_unblock(struct Task *task);

    void on_unblock(struct Task *task);
};

class BlockerConnect : public Blocker
{
private:
    RefPtr<FsNode> _connection;

public:
    BlockerConnect(RefPtr<FsNode> connection)
        : _connection(connection)
    {
    }

    bool can_unblock(struct Task *task);
};

class BlockerRead : public Blocker
{
private:
    FsHandle &_handle;

public:
    BlockerRead(FsHandle &handle)
        : _handle{handle}
    {
    }

    bool can_unblock(Task *task);

    void on_unblock(Task *task);
};

struct Selected
{
    int handle_index;
    RefPtr<FsHandle> handle;
    PollEvent events;
};

class BlockerSelect : public Blocker
{
private:
    Vector<Selected> &_handles;
    Optional<Selected> _selected;

public:
    Optional<Selected> selected() { return _selected; }

    BlockerSelect(Vector<Selected> &handles)
        : _handles{handles}
    {
    }

    bool can_unblock(Task *task);

    void on_unblock(Task *task);
};

class BlockerTime : public Blocker
{
private:
    Tick _wakeup_tick;

public:
    BlockerTime(Tick wakeup_tick)
        : _wakeup_tick(wakeup_tick)
    {
    }

    bool can_unblock(Task *task);
};

class BlockerWait : public Blocker
{
private:
    Task *_task;
    int *_exit_value;

public:
    BlockerWait(Task *task, int *exit_value)
        : _task(task), _exit_value(exit_value)
    {
    }

    bool can_unblock(Task *task);

    void on_unblock(Task *task);
};

class BlockerWrite : public Blocker
{
private:
    FsHandle &_handle;

public:
    BlockerWrite(FsHandle &handle)
        : _handle{handle}
    {
    }

    bool can_unblock(Task *task);

    void on_unblock(Task *task);
};
