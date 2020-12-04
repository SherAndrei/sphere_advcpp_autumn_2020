#ifndef HTTP_COR_COROUTINE_H
#define HTTP_COR_COROUTINE_H
#include <utility>
#include <functional>

namespace http {
namespace cor {

using routine_t = size_t;
using RoutineFunction = std::function<void()>;

routine_t create(const RoutineFunction& function);
bool resume(routine_t id);
void yield();
routine_t current();
bool is_done(routine_t id);

template <typename F, typename ...Args, typename = std::enable_if_t<!std::is_invocable_v<F>>>
routine_t create(F&& f, Args&&... args) {
    return create(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
}

template <typename F, typename ...Args>
bool create_and_run(F&& f, Args&&... args) {
    return resume(create(std::forward<F>(f), std::forward<Args>(args)...));
}

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_COROUTINE_H
