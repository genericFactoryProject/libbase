#pragma once

#include <linux/compiler.h>
#include <linux/printk.h>
#include <linux/param.h>

/* generic data direction definitions */
#define READ			0
#define WRITE			1

/*
 * request flags */
typedef __u32 __bitwise req_flags_t;
/* drive already may have started this one */
#define RQF_STARTED		((__force req_flags_t)(1 << 1))
/* may not be passed by ioscheduler */
#define RQF_SOFTBARRIER		((__force req_flags_t)(1 << 3))
/* request for flush sequence */
#define RQF_FLUSH_SEQ		((__force req_flags_t)(1 << 4))
/* merge of different types, fail separately */
#define RQF_MIXED_MERGE		((__force req_flags_t)(1 << 5))
/* track inflight for MQ */
#define RQF_MQ_INFLIGHT		((__force req_flags_t)(1 << 6))
/* don't call prep for this one */
#define RQF_DONTPREP		((__force req_flags_t)(1 << 7))
/* vaguely specified driver internal error.  Ignored by the block layer */
#define RQF_FAILED		((__force req_flags_t)(1 << 10))
/* don't warn about errors */
#define RQF_QUIET		((__force req_flags_t)(1 << 11))
/* elevator private data attached */
#define RQF_ELVPRIV		((__force req_flags_t)(1 << 12))
/* account into disk and partition IO statistics */
#define RQF_IO_STAT		((__force req_flags_t)(1 << 13))
/* runtime pm request */
#define RQF_PM			((__force req_flags_t)(1 << 15))
/* on IO scheduler merge hash */
#define RQF_HASHED		((__force req_flags_t)(1 << 16))
/* track IO completion time */
#define RQF_STATS		((__force req_flags_t)(1 << 17))
/* Look at ->special_vec for the actual data payload instead of the
   bio chain. */
#define RQF_SPECIAL_PAYLOAD	((__force req_flags_t)(1 << 18))
/* The per-zone write lock is held for this request */
#define RQF_ZONE_WRITE_LOCKED	((__force req_flags_t)(1 << 19))
/* already slept for hybrid poll */
#define RQF_MQ_POLL_SLEPT	((__force req_flags_t)(1 << 20))
/* ->timeout has been called, don't expire again */
#define RQF_TIMED_OUT		((__force req_flags_t)(1 << 21))
/* queue has elevator attached */
#define RQF_ELV			((__force req_flags_t)(1 << 22))

/* flags that prevent us from merging requests: */
#define RQF_NOMERGE_FLAGS \
	(RQF_STARTED | RQF_SOFTBARRIER | RQF_FLUSH_SEQ | RQF_SPECIAL_PAYLOAD)

enum mq_rq_state {
	MQ_RQ_IDLE		= 0,
	MQ_RQ_IN_FLIGHT		= 1,
	MQ_RQ_COMPLETE		= 2,
};

/*
 * The basic unit of block I/O is a sector. It is used in a number of contexts
 * in Linux (blk, bio, genhd). The size of one sector is 512 = 2**9
 * bytes. Variables of type sector_t represent an offset or size that is a
 * multiple of 512 bytes. Hence these two constants.
 */
#ifndef SECTOR_SHIFT
#define SECTOR_SHIFT 9
#endif
#ifndef SECTOR_SIZE
#define SECTOR_SIZE (1 << SECTOR_SHIFT)
#endif

typedef __u32 __bitwise blk_mq_req_flags_t;

/*
 * Block error status values.  See block/blk-core:blk_errors for the details.
 * Alpha cannot write a byte atomically, so we need to use 32-bit value.
 */
#if defined(CONFIG_ALPHA) && !defined(__alpha_bwx__)
typedef u32 __bitwise blk_status_t;
typedef u32 blk_short_t;
#else
typedef u8 __bitwise blk_status_t;
typedef u16 blk_short_t;
#endif
#define	BLK_STS_OK 0
#define BLK_STS_NOTSUPP		((__force blk_status_t)1)
#define BLK_STS_TIMEOUT		((__force blk_status_t)2)
#define BLK_STS_NOSPC		((__force blk_status_t)3)
#define BLK_STS_TRANSPORT	((__force blk_status_t)4)
#define BLK_STS_TARGET		((__force blk_status_t)5)
#define BLK_STS_NEXUS		((__force blk_status_t)6)
#define BLK_STS_MEDIUM		((__force blk_status_t)7)
#define BLK_STS_PROTECTION	((__force blk_status_t)8)
#define BLK_STS_RESOURCE	((__force blk_status_t)9)
#define BLK_STS_IOERR		((__force blk_status_t)10)

/* hack for device mapper, don't use elsewhere: */
#define BLK_STS_DM_REQUEUE    ((__force blk_status_t)11)

#define BLK_STS_AGAIN		((__force blk_status_t)12)

/*
 * BLK_STS_DEV_RESOURCE is returned from the driver to the block layer if
 * device related resources are unavailable, but the driver can guarantee
 * that the queue will be rerun in the future once resources become
 * available again. This is typically the case for device specific
 * resources that are consumed for IO. If the driver fails allocating these
 * resources, we know that inflight (or pending) IO will free these
 * resource upon completion.
 *
 * This is different from BLK_STS_RESOURCE in that it explicitly references
 * a device specific resource. For resources of wider scope, allocation
 * failure can happen without having pending IO. This means that we can't
 * rely on request completions freeing these resources, as IO may not be in
 * flight. Examples of that are kernel memory allocations, DMA mappings, or
 * any other system wide resources.
 */
#define BLK_STS_DEV_RESOURCE	((__force blk_status_t)13)

/*
 * BLK_STS_ZONE_RESOURCE is returned from the driver to the block layer if zone
 * related resources are unavailable, but the driver can guarantee the queue
 * will be rerun in the future once the resources become available again.
 *
 * This is different from BLK_STS_DEV_RESOURCE in that it explicitly references
 * a zone specific resource and IO to a different zone on the same device could
 * still be served. Examples of that are zones that are write-locked, but a read
 * to the same zone could be served.
 */
#define BLK_STS_ZONE_RESOURCE	((__force blk_status_t)14)

/*
 * BLK_STS_ZONE_OPEN_RESOURCE is returned from the driver in the completion
 * path if the device returns a status indicating that too many zone resources
 * are currently open. The same command should be successful if resubmitted
 * after the number of open zones decreases below the device's limits, which is
 * reported in the request_queue's max_open_zones.
 */
#define BLK_STS_ZONE_OPEN_RESOURCE	((__force blk_status_t)15)

/*
 * BLK_STS_ZONE_ACTIVE_RESOURCE is returned from the driver in the completion
 * path if the device returns a status indicating that too many zone resources
 * are currently active. The same command should be successful if resubmitted
 * after the number of active zones decreases below the device's limits, which
 * is reported in the request_queue's max_active_zones.
 */
#define BLK_STS_ZONE_ACTIVE_RESOURCE	((__force blk_status_t)16)

/*
 * BLK_STS_OFFLINE is returned from the driver when the target device is offline
 * or is being taken offline. This could help differentiate the case where a
 * device is intentionally being shut down from a real I/O error.
 */
#define BLK_STS_OFFLINE		((__force blk_status_t)17)

#define BLK_TAG_ALLOC_FIFO 0 /* allocate starting from 0 */
#define BLK_TAG_ALLOC_RR 1 /* allocate starting from last allocated tag */


#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x);
#define MODULE_LICENSE(x);
#define MODULE_DEVICE_TABLE(x, y);
#define MODULE_VERSION(x);



static inline void __might_resched(const char *file, int line,
				     unsigned int offsets) { }
static inline void __might_sleep(const char *file, int line) { }
# define might_resched() do { } while (0)
# define might_sleep() do { might_resched(); } while (0)
# define cant_sleep() do { } while (0)
# define cant_migrate()		do { } while (0)
# define sched_annotate_sleep() do { } while (0)
# define non_block_start() do { } while (0)
# define non_block_end() do { } while (0)


#define might_sleep_if(cond) do { if (cond) might_sleep(); } while (0)

static inline void might_fault(void) { }

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
#define kvfree(x) (void)(x)
#define free_pages(x, y) (void)x;(void)y
#define __get_free_pages(x, y) (unsigned long)0
#define kstrndup(x, y, z) (void *)0
#define kmemdup(x, y, z) (void *)0
#define kstrdup_const(x, y) (void *)0
#define kfree_const(x) (void)x
#define kstrdup(x, y) (void *)0
#define kvasprintf_const(x, y, z) (void *)0
#define kasprintf(x,y,z) (void *)0

typedef unsigned int gfp_t;
typedef unsigned int pgprot_t;

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
#define raw_spin_lock(x)
#define raw_spin_unlock(x)
#define __SPIN_LOCK_UNLOCKED(x) (x)
#define DEFINE_SPINLOCK(x) spinlock_t x
#define DEFINE_RAW_SPINLOCK(x) raw_spinlock_t x
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



