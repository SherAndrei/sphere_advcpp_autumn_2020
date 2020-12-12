#ifndef HTTP_COR_COROUTINE_H
#define HTTP_COR_COROUTINE_H
#include <utility>
#include <functional>

namespace http {
namespace cor {

using routine_t = size_t;
using RoutineFunction = std::function<void()>;
struct Routine;

Routine* create(const RoutineFunction& function);
bool resume(Routine* p_rout);
void yield();
Routine* current();

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
