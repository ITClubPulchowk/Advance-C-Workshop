#include "threading.h"

#include <stdlib.h>
#ifdef __linux__
#include <unistd.h>
#endif

//
//
//

typedef void (*thrd_pool_worker)(void *arg);

struct thrd_pool_work_node {
	thrd_pool_worker            proc;
	void *arg;
	struct thrd_pool_work_node *next;
};
typedef struct thrd_pool_work_node thrd_pool_work_node;

typedef struct {
	thrd_pool_work_node  head;
	thrd_pool_work_node *leaf;
	thrd_pool_work_node *free_list;

	mtx_t                work_mutex;
} thrd_pool_queue;

//
//
//

static thrd_pool_queue g_work_queue;

static volatile uint32_t g_thread_pool_run;
static volatile uint32_t g_thread_pool_work_queued;
static volatile uint32_t g_thread_pool_work_remaining;

//
//
//

static inline thrd_pool_work_node *thrd_pool_queue_alloc_node() {
	if (g_work_queue.free_list == NULL) {
		const int MAX_ALLOCATION = 8;
		g_work_queue.free_list = malloc(sizeof(thrd_pool_work_node) * MAX_ALLOCATION);

		thrd_pool_work_node *ptr = g_work_queue.free_list;
		thrd_pool_work_node *trav = ptr;

		for (int i = 1; i < MAX_ALLOCATION; ++i) {
			trav->next = &ptr[i];
			trav = trav->next;
		}

		trav->next = NULL;
	}

	thrd_pool_work_node *node = g_work_queue.free_list;
	g_work_queue.free_list = g_work_queue.free_list->next;
	return node;
}

static inline void thrd_pool_queue_free_node(thrd_pool_work_node *node) {
	node->next = g_work_queue.free_list;
	g_work_queue.free_list = node;
}

//
//
//

static inline void thrd_pool_add(thrd_pool_worker proc, void *arg) {
	mtx_lock(&g_work_queue.work_mutex);

	thrd_pool_work_node *work = thrd_pool_queue_alloc_node();
	work->proc = proc;
	work->arg  = arg;
	work->next = &g_work_queue.head;

	g_work_queue.leaf->next = work;
	g_work_queue.leaf       = work;

	g_thread_pool_work_queued += 1;
	interlocked_inc(&g_thread_pool_work_remaining);

	mtx_unlock(&g_work_queue.work_mutex);
}

static inline int thrd_pool_remove(thrd_pool_work_node *work_node) {
	int result = 0;

	mtx_lock(&g_work_queue.work_mutex);

	thrd_pool_work_node *node = g_work_queue.head.next;

	if (node != &g_work_queue.head) {
		result                 = 1;
		*work_node             = *node;
		g_work_queue.head.next = g_work_queue.head.next->next;

		if (node == g_work_queue.leaf) {
			g_work_queue.leaf = &g_work_queue.head;
		}

		g_thread_pool_work_queued -= 1;
		thrd_pool_queue_free_node(node);
	}

	mtx_unlock(&g_work_queue.work_mutex);

	return result;
}

static inline void thrd_pool_perform_work(thrd_pool_work_node work_node) {
	work_node.proc(work_node.arg);
	interlocked_dec(&g_thread_pool_work_remaining);
}

static inline int thrd_pool(void *arg) {
	thrd_pool_work_node work_node;

	while (g_thread_pool_run) {
		if (thrd_pool_remove(&work_node)) {
			thrd_pool_perform_work(work_node);
		}
		else {
			while (g_thread_pool_run) {
				thrd_sleep_millisecs(10);
				if (g_thread_pool_work_queued > 0)
					break;
			}
		}
	}

	return 0;
}

//
//
//

static inline void thrd_pool_create(uint32_t thread_count) {
	g_thread_pool_run = 0;

	g_work_queue.leaf       = &g_work_queue.head;
	g_work_queue.leaf->next = &g_work_queue.head;

	mtx_init(&g_work_queue.work_mutex, mtx_plain);

	if (thread_count == UINT32_MAX) {
#ifdef __linux__
		thread_count = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_WIN32)
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		thread_count = sysinfo.dwNumberOfProcessors;
#else
#error "Unimplemented"
#endif
	}

	interlocked_inc(&g_thread_pool_run);

	thrd_t thread;
	for (uint32_t index = 0; index < thread_count; ++index) {
		thrd_create(&thread, thrd_pool, NULL);
	}
}

static inline void thrd_pool_destroy() {
	interlocked_dec(&g_thread_pool_run);

	mtx_destroy(&g_work_queue.work_mutex);
}

static inline void thrd_pool_wait() {
	thrd_pool_work_node work_node;
	while (g_thread_pool_work_remaining != 0) {
		if (g_thread_pool_work_queued && thrd_pool_remove(&work_node)) {
			thrd_pool_perform_work(work_node);
		}
	}
}

//
//
//
