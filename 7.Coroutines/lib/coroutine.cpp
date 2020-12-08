#include <ucontext.h>
#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include "coroutine.h"

namespace http {
namespace cor {

struct Routine;

namespace {

static constexpr size_t STACK_SIZE = 1 << 16;

class MainOrdinator {
 public:
    Routine& at(routine_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        return routines[id - 1];
    }

    void emplace(routine_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        finished.emplace(id);
    }
    size_t size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return routines.size();
    }

    friend routine_t http::cor::create(const RoutineFunction& function);

 private:
    std::vector<Routine> routines;
    std::queue<routine_t> finished;
    std::mutex mutex_;
} main_ordinator;

thread_local struct Ordinator {
    routine_t current = 0;
    ucontext_t ctx{};
} ordinator;

void entry();

}  // namespace

struct Routine {
    RoutineFunction func;
    std::unique_ptr<uint8_t[]> stack;
    bool finished = false;
    ucontext_t ctx;
    std::exception_ptr exception;

    void reset(const RoutineFunction& f) {
        func = f;
        finished = false;
        exception = {};

        ctx.uc_stack.ss_sp = stack.get();
        ctx.uc_stack.ss_size = STACK_SIZE;
        ctx.uc_link = &ordinator.ctx;
        getcontext(&ctx);
        makecontext(&ctx, entry, 0);
    }

    explicit Routine(const RoutineFunction& f)
            : func{f},
              stack{std::make_unique<uint8_t[]>(STACK_SIZE)} {
        ctx.uc_stack.ss_sp = stack.get();
        ctx.uc_stack.ss_size = STACK_SIZE;
        ctx.uc_link = &ordinator.ctx;
        getcontext(&ctx);
        makecontext(&ctx, entry, 0);
    }

    Routine(const Routine&) = delete;
    Routine(Routine&&) = default;
};

routine_t create(const RoutineFunction& function) {
    std::lock_guard<std::mutex> lock(main_ordinator.mutex_);
    if (main_ordinator.finished.empty()) {
        main_ordinator.routines.emplace_back(function);
        return main_ordinator.routines.size();
    } else {
        routine_t id = main_ordinator.finished.front();
        main_ordinator.finished.pop();
        auto& routine = main_ordinator.routines[id - 1];
        routine.reset(function);
        return id;
    }
}

bool resume(routine_t id) {
    auto& o = ordinator;
    if (id == 0 || id > main_ordinator.size())
        return false;

    auto& routine = main_ordinator.at(id);
    if (routine.finished) {
        routine.reset(routine.func);
    }

    o.current = id;
    if (swapcontext(&o.ctx, &routine.ctx) < 0) {
        o.current = 0;
        return false;
    }

    if (routine.exception)
        std::rethrow_exception(routine.exception);

    return true;
}

void yield() {
    auto& o = ordinator;
    routine_t id = o.current;
    auto& routine = main_ordinator.at(id);

    o.current = 0;
    swapcontext(&routine.ctx, &o.ctx);
}

routine_t current() {
    return ordinator.current;
}

namespace {

void entry() {
    auto& o = ordinator;
    routine_t id = o.current;
    auto &routine = main_ordinator.at(id);

    if (routine.func) {
    try {
      routine.func();
    } catch (...) {
      routine.exception = std::current_exception();
    }
  }

    routine.finished = true;
    o.current = 0;
    main_ordinator.emplace(id);
}

}  // namespace


}  // namespace cor
}  // namespace http
