#include <ucontext.h>
#include <list>
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
    void emplace(Routine* p_rout) {
        std::lock_guard<std::mutex> lock(mutex_);
        finished.emplace(p_rout);
    }
    size_t size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return routines.size();
    }

    friend Routine* http::cor::create(const RoutineFunction& function);

 private:
    std::list<Routine> routines;
    std::queue<Routine*> finished;
    std::mutex mutex_;
} main_ordinator;

thread_local struct Ordinator {
    Routine*   current = nullptr;
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

Routine* create(const RoutineFunction& function) {
    std::lock_guard<std::mutex> lock(main_ordinator.mutex_);
    if (main_ordinator.finished.empty()) {
        main_ordinator.routines.emplace_back(function);
        Routine* p_rout = &main_ordinator.routines.back();
        return p_rout;
    } else {
        Routine* p_rout = main_ordinator.finished.front();
        main_ordinator.finished.pop();
        p_rout->reset(function);
        return p_rout;
    }
}

bool resume(Routine* p_rout) {
    auto& o = ordinator;

    auto& routine = *p_rout;
    if (routine.finished) {
        return false;
    }

    o.current = p_rout;
    if (swapcontext(&o.ctx, &routine.ctx) < 0) {
        o.current = nullptr;
        return false;
    }

    if (routine.exception)
        std::rethrow_exception(routine.exception);

    return true;
}

void yield() {
    auto& o = ordinator;
    Routine* p_rout = o.current;

    o.current = nullptr;
    swapcontext(&p_rout->ctx, &o.ctx);
}

Routine* current() {
    return ordinator.current;
}

namespace {

void entry() {
    auto& o = ordinator;
    Routine* p_rout = o.current;

    if (p_rout->func) {
    try {
      p_rout->func();
    } catch (...) {
      p_rout->exception = std::current_exception();
    }
  }

    p_rout->finished = true;
    o.current = nullptr;
    main_ordinator.emplace(p_rout);
}

}  // namespace

}  // namespace cor
}  // namespace http
