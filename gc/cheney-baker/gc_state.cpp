#include <bindings/gc.hpp>

#include <gc_state.hpp>
#include <abi_errors.hpp>

namespace hw::stella::gc {

namespace {

GcState g_gc_state{};

}

void require_running() {
    if (g_gc_state.State() != Lifecycle::Running) {
        stella_abi_violation(errors::gc_not_runnint_msg);
    }
}

GcState& get_gc_state() {
    return g_gc_state;
}

} // namespace hw::stella::gc