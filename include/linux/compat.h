#pragma once

#include <linux/compiler.h>
#include <linux/printk.h>

extern const char hex_asc[];
#define hex_asc_lo(x)	hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)	hex_asc[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack(char *buf, u8 byte)
{
	*buf++ = hex_asc_hi(byte);
	*buf++ = hex_asc_lo(byte);
	return buf;
}

extern const char hex_asc_upper[];
#define hex_asc_upper_lo(x)	hex_asc_upper[((x) & 0x0f)]
#define hex_asc_upper_hi(x)	hex_asc_upper[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack_upper(char *buf, u8 byte)
{
	*buf++ = hex_asc_upper_hi(byte);
	*buf++ = hex_asc_upper_lo(byte);
	return buf;
}
int hex_to_bin(unsigned char ch);

#define NR_CPUS 1
#define PAGE_SIZE 4096
#define PAGE_MASK (PAGE_SIZE-1)
#define PAGE_SHIFT 12
#define PFN_ALIGN(x)	(((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)
#define PFN_UP(x)	(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((phys_addr_t)(x) << PAGE_SHIFT)
#define PHYS_PFN(x)	((unsigned long)((x) >> PAGE_SHIFT))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

enum {
	REGION_INTERSECTS,
	REGION_DISJOINT,
	REGION_MIXED,
};

/*
 * ZERO_SIZE_PTR will be returned for zero sized kmalloc requests.
 *
 * Dereferencing ZERO_SIZE_PTR will lead to a distinct access fault.
 *
 * ZERO_SIZE_PTR can be passed to kfree though in the same way that NULL can.
 * Both make kfree a no-op.
 */
#define ZERO_SIZE_PTR ((void *)16)
#define ZERO_OR_NULL_PTR(x) ((unsigned long)(x) <= \
				(unsigned long)ZERO_SIZE_PTR)
#define ARCH_KMALLOC_MINALIGN __alignof__(unsigned long long)
#define kmalloc(x, y)   (void *)0
#define kzalloc(x, y)   (void *)0
#define kcalloc(x, y ,z) (void *)0
#define kfree(x) (void)(x)
#define ksize(x) 0
#define free_pages(x, y) (void)x;(void)y
#define __get_free_pages(x, y) (unsigned long)0
#define kstrndup(x, y, z) (void *)0
#define kmemdup(x, y, z) (void *)0
#define kstrdup_const(x, y) (void *)0
#define kfree_const(x) (void)x
#define kstrdup(x, y) (void *)0
#define kvasprintf_const(x, y, z) (void *)0

typedef unsigned int gfp_t;

#define __GFP_BITS_SHIFT (27)
#define __GFP_BITS_MASK ((__force gfp_t)((1 << __GFP_BITS_SHIFT) - 1))
typedef unsigned int spinlock_t;
typedef unsigned int raw_spinlock_t;
struct lock_class_key {

};

#define spin_lock_irqsave(x, y) (void)(x);(void)y
#define spin_unlock_irqrestore(x, y) (void)(x);(void)y
#define spin_lock_irq(x) (void)(x)
#define spin_unlock_irq(x) (void)(x)
#define spin_lock(x) (void)(x)
#define spin_unlock(x) (void)(x)
#define spin_lock_init(x) (void)(x)
#define raw_spin_lock_irqsave(x, y) (void)(x);(void)y
#define raw_spin_unlock_irqrestore(x, y) (void)(x);(void)y
#define __SPIN_LOCK_UNLOCKED(x) (x)
#define DEFINE_SPINLOCK(x) spinlock_t x

typedef unsigned int rwlock_t;

#define DEFINE_RWLOCK(x) rwlock_t x
#define write_lock(x)
#define write_unlock(x)
#define read_lock(x)
#define read_unlock(x)

struct rw_semaphore {

};
#define DECLARE_RWSEM(name) \
struct rw_semaphore name

#define __RWSEM_INITIALIZER(x) (x)
#define init_rwsem(x) (void)(x)
#define down_write(x) (void)(x)
#define up_write(x) (void)(x)
#define down_read(x) (void)(x)
#define up_read(x) (void)(x)

struct mutex {

};
#define DEFINE_MUTEX(mutexname) \
	struct mutex mutexname
#define mutex_init(x) (void)(x)
#define __mutex_init(x,y,z) (void)(x);(void)y;(void)(z)
#define mutex_lock(x) (void)(x)
#define mutex_lock_interruptible(x) 0
#define mutex_trylock(x) 0
#define mutex_unlock(x) (void)(x)
#define lockdep_assert_held(x) (void)(x)

#define rcu_dereference_check(x, y) (x)
#define rcu_dereference_protected(x, y) (x)
#define rcu_dereference(x) (x)
#define rcu_dereference_raw(x) (x)
#define rcu_assign_pointer(x, y) (x) = (y)
#define call_rcu(x, f) f((x))
#define RCU_INIT_POINTER(x, y) (x) = (y)
#define rcu_read_lock()
#define rcu_read_unlock()
#define synchronize_rcu()
#define rcu_access_pointer(x) 0

#define BUG() do {} while (1)
#define BUG_ON(condition) do { if (unlikely(condition)) BUG(); } while (0)
#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	unlikely(__ret_warn_on);					\
})
#define WARN(condition, format...) ({					\
	int __ret_warn_on = !!(condition);				\
	no_printk(format);						\
	unlikely(__ret_warn_on);					\
})
#define WARN_ON_ONCE(condition) WARN_ON(condition)
#define WARN_ONCE(condition, format...) WARN(condition, format)
#define WARN_TAINT(condition, taint, format...) WARN(condition, format)
#define WARN_TAINT_ONCE(condition, taint, format...) WARN(condition, format)
/*
 * WARN_ON_SMP() is for cases that the warning is either
 * meaningless for !SMP or may even cause failures.
 * It can also be used with values that are only defined
 * on SMP:
 *
 * struct foo {
 *  [...]
 * #ifdef CONFIG_SMP
 *	int bar;
 * #endif
 * };
 *
 * void func(struct foo *zoot)
 * {
 *	WARN_ON_SMP(!zoot->bar);
 *
 * For CONFIG_SMP, WARN_ON_SMP() should act the same as WARN_ON(),
 * and should be a nop and return false for uniprocessor.
 *
 * if (WARN_ON_SMP(x)) returns true only when CONFIG_SMP is set
 * and x is true.
 */
#ifdef CONFIG_SMP
# define WARN_ON_SMP(x)			WARN_ON(x)
#else
/*
 * Use of ({0;}) because WARN_ON_SMP(x) may be used either as
 * a stand alone line statement or as a condition in an if ()
 * statement.
 * A simple "0" would cause gcc to give a "statement has no effect"
 * warning.
 */
# define WARN_ON_SMP(x)			({0;})
#endif

/*
 * WARN_ON_FUNCTION_MISMATCH() warns if a value doesn't match a
 * function address, and can be useful for catching issues with
 * callback functions, for example.
 *
 * With CONFIG_CFI_CLANG, the warning is disabled because the
 * compiler replaces function addresses taken in C code with
 * local jump table addresses, which breaks cross-module function
 * address equality.
 */
#if defined(CONFIG_CFI_CLANG) && defined(CONFIG_MODULES)
# define WARN_ON_FUNCTION_MISMATCH(x, fn) ({ 0; })
#else
# define WARN_ON_FUNCTION_MISMATCH(x, fn) WARN_ON_ONCE((x) != (fn))
#endif



