#pragma once

#define rcu_dereference_check(x, y) (x)
#define rcu_dereference_protected(x, y) (x)
#define rcu_dereference(x) (x)
#define rcu_dereference_raw(x) (x)
#define rcu_assign_pointer(x, y) (x) = (y)
#define call_rcu(x, f) f((x))
#define RCU_INIT_POINTER(x, y) (x) = (y)
#define rcu_read_lock()
#define rcu_read_unlock()

