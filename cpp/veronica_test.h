#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <linux/memfd.h>

/* bits to get memfd_create to work */
#define MFD_SECRET 0x0008U
#define MFD_SECRET_IOCTL '-'
#define MFD_SECRET_EXCLUSIVE	_IOW(MFD_SECRET_IOCTL, 0x13, unsigned long)
#define MFD_SECRET_UNCACHED	_IOW(MFD_SECRET_IOCTL, 0x14, unsigned long)

/* secretmemfd defines */
#define SECRETMEM_EXCLUSIVE	0x1
#define SECRETMEM_UNCACHED	0x2

#define ASSERT(x) do { if (!(x)) { printf("ASSERTION failed at line %d\n", __LINE__); exit(1); } } while (0)

/* glibc should have defined this by now, sigh */
static inline int memfd_create(const char *name, unsigned int flags)
{
	return syscall(__NR_memfd_create, name, flags);
}

#ifndef __NR_secretmemfd
#define __NR_secretmemfd 440
#endif

static inline int secretmemfd(unsigned long flags)
{
	return syscall(__NR_secretmemfd, flags);
}

static int use_secret = 1;
//static int secret_option = MFD_SECRET_EXCLUSIVE;
static int secret_option = MFD_SECRET_UNCACHED;

static int secretmem_open(void)
{
	unsigned long secretmemfd_flags = SECRETMEM_UNCACHED;
	int fd, ret;

	if (!use_secret)
		return memfd_create("secure", MFD_CLOEXEC);

    printf("Pobu comment out\n");
	//if (secret_option == MFD_SECRET_UNCACHED)
		//secretmemfd_flags = SECRETMEM_EXCLUSIVE;

	/* try dedicated syscall first */
	fd = secretmemfd(secretmemfd_flags);
	if (fd >= 0 || errno != ENOSYS)
		return fd;

	/* secretmemfd is not implemented, maybe memfd_create(SECRET) is */
	fd = memfd_create("secure", MFD_CLOEXEC|MFD_SECRET);
	if (fd < 0)
		return fd;

	ret = ioctl(fd, secret_option);
	if (ret < 0)
		return ret;

	return fd;
}


/* segment size.  Matches hugepage size */
#define SEG_SIZE 2*1024*1024

static int debug = 0;

#define DEBUG(...) do { if (debug > 1) printf(__VA_ARGS__); } while(0)
#define INFO(...) do { if (debug) printf(__VA_ARGS__); } while(0)

#define PINUSE_BIT	0x01
#define CINUSE_BIT	0x02

#define FLAG_BITS	(CINUSE_BIT | PINUSE_BIT)

struct malloc_chunk_head {
	int	prev_foot;	/* size of previous if free */
	int	head;		/* entire size of this chunk */
};

struct malloc_chunk {
	struct malloc_chunk_head h;
	struct malloc_chunk *fd;
	struct malloc_chunk *bk;
};

#define CHUNK_SIZE (sizeof(struct malloc_chunk_head))
#define MIN_CHUNK 0x20
#define CHUNK_ALIGNMENT (MIN_CHUNK - 1)

struct segptr {
	char *base;
	/* no size because they're always SEG_SIZE */
	struct segptr *next;
};

struct malloc_state {
	struct segptr *seg;
	struct malloc_chunk free;
};

static 	struct malloc_state m;

static size_t pad_request(size_t s)
{
	return (s + CHUNK_SIZE + CHUNK_ALIGNMENT) & ~CHUNK_ALIGNMENT;
}

static int in_use(struct malloc_chunk *c)
{
	return c->h.head & CINUSE_BIT ? 1 : 0;
}

static int prev_in_use(struct malloc_chunk *c)
{
	return c->h.head & PINUSE_BIT ? 1 : 0;
}

static void check(int cond, const char *str)
{
	if (cond) {
		perror(str);
		exit(1);
	}
}

static struct segptr *chunk_to_segment(struct malloc_chunk *c)
{
	struct segptr *sp;

	for (sp = m.seg; sp != NULL; sp = sp->next)
		if (sp->base <= (char *)c &&
		    (char *)c < sp->base + SEG_SIZE)
			return sp;
	return NULL;
}

static void *chunk2mem(struct malloc_chunk *c)
{
	return (char *)c + CHUNK_SIZE;
}

static struct malloc_chunk *mem2chunk(void *p)
{
	return (struct malloc_chunk *)((char *)p - CHUNK_SIZE);
}

static size_t chunk_size(struct malloc_chunk *c)
{
	return c->h.head & ~FLAG_BITS;
}

static struct malloc_chunk *next_chunk(struct malloc_chunk *c)
{
	struct segptr *seg = chunk_to_segment(c);
	void *n = (char*)c + chunk_size(c);

	ASSERT(seg != NULL);

	ASSERT((char *)n > seg->base &&
	       (char *)n <= seg->base + SEG_SIZE);
	if (n == seg->base + SEG_SIZE)
		return NULL;

	return n;
}



static struct malloc_chunk *prev_chunk(struct malloc_chunk *c)
{
	return (struct malloc_chunk *)((char*)c - c->h.prev_foot);
}

static void link_free_chunk(struct malloc_chunk *c)
{
	struct malloc_chunk *f = &m.free;
	struct malloc_chunk *b = f->bk;

	c->fd = f;
	f->bk = c;

	b->fd = c;
	c->bk = b;
}

static void unlink_free_chunk(struct malloc_chunk *c)
{
	struct malloc_chunk *f = c->fd;
	struct malloc_chunk *b = c->bk;

	b->fd = f;
	f->bk = b;
}

static void show_segment(void)
{
	struct malloc_chunk *c;
	struct segptr *seg;
	int i;

	if (debug < 2)
		return;

	for (i = 0, seg = m.seg; seg != NULL; i++, seg = seg->next) {
		printf("SHOW SEGMENT %i\n", i);
		for (c = (struct malloc_chunk *)seg->base; c != NULL; c = next_chunk(c)) {
			printf("%p:%d:%d:%zu:%zu", c, in_use(c), prev_in_use(c),
			       c->h.prev_foot, chunk_size(c));
			if (!in_use(c))
				printf(":%p:%p", c->fd, c->bk);
			printf("\n");
		}
	}
	printf("SHOW SEGMENT END\n");
}

static void alloc_segment(void)
{
	int fd;
	int ret;
	void *p;
	struct segptr *seg, *sp, **psp;
	const size_t ssize = pad_request(sizeof(*seg));
	struct malloc_chunk *c;

	fd = secretmem_open();
	check(fd < 0, "secretmem_open");

	ret = ftruncate(fd, SEG_SIZE);
	check(ret < 0, "ftruncate");

	p = mmap(NULL, SEG_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	check(p == MAP_FAILED, "mmap");
	close(fd);

	DEBUG("initial malloc at 0x%p-0x%p\n", p, p+SEG_SIZE);
	c = p;
	seg = chunk2mem(c);
	memset(seg, 0, sizeof(*seg));
	seg->base = p;
	c->h.head = ssize | CINUSE_BIT | PINUSE_BIT;
	c->h.prev_foot = 0;

	if (m.seg == NULL) {
		m.seg = seg;
	} else {
		for (sp = m.seg; sp->next != NULL; sp = sp->next)
			;
		sp->next = seg;
	}
	c = next_chunk(c);
	c->h.head = (size_t)(((char *)p + SEG_SIZE) - (char *)c) | PINUSE_BIT;
	c->h.prev_foot = ssize;
	DEBUG("next chunk at 0x%p, head=%zu, prev=%zu\n", c, chunk_size(c), c->h.prev_foot);

	link_free_chunk(c);
}

void __attribute__ ((constructor)) preload_setup(void)
{
	/*
	 * if this fails, the MIN_CHUNK, which must be a power of 2
	 * isn't big enough.  If that happens, chunk splitting will
	 * fail because the free list pointers get overwritten by the
	 * split
	 */
	ASSERT(sizeof(struct malloc_chunk) < MIN_CHUNK);
	if (getenv("MALLOC_DEBUG") != NULL)
		debug = atoi(getenv("MALLOC_DEBUG"));
	if (getenv("NO_SECRET_MEM") != NULL)
		use_secret = 0;
	if (getenv("SECRET_UNCACHED") != NULL)
		secret_option = MFD_SECRET_UNCACHED;

	m.free.fd = &m.free;
	m.free.bk = &m.free;
	m.seg = NULL;
}

static struct malloc_chunk *find_free(size_t size)
{
	struct malloc_chunk *c, *found = NULL;

	for (c = m.free.fd; c != &m.free; c = c->fd) {
		ASSERT(prev_in_use(c));
		if (chunk_size(c) < size)
			continue;
		if (found && chunk_size(found) < chunk_size(c))
			continue;
		found = c;
	}
	return found;
}

static void split_free_chunk(struct malloc_chunk *c, size_t size)
{
	struct malloc_chunk *new_c, *n;
	size_t csize = chunk_size(c);

	if (csize - size < pad_request(1)) {
		/* nothing to split, just give everything up */
		unlink_free_chunk(c);
		c->h.head |= CINUSE_BIT;
		new_c = next_chunk(c);
		if (new_c) {
			ASSERT(!prev_in_use(new_c));
			ASSERT(new_c->h.prev_foot == chunk_size(c));

			new_c->h.head |= PINUSE_BIT;
		}
		return;
	}

	/* set the old chunk to the size */
	c->h.head = size | CINUSE_BIT | PINUSE_BIT;
	/* get the new part of the split */
	new_c = next_chunk(c);
	new_c->h.head = (csize - size) | PINUSE_BIT;
	new_c->h.prev_foot  = size;
	/* now replace the new chunk with the old chunk */
	new_c->fd = c->fd;
	new_c->bk = c->bk;
	c->bk->fd = new_c;
	c->fd->bk = new_c;
	/* and adjust the next prev_foot if there is one */
	n = next_chunk(new_c);
	if (n) {
		ASSERT(!prev_in_use(n));
		ASSERT(n->h.prev_foot == csize);
		n->h.prev_foot = chunk_size(new_c);
	}
}

static void *dlmalloc(size_t size)
{
	struct malloc_chunk *c;

	size = pad_request(size);
	c = find_free(size);
	if (c == NULL) {
		alloc_segment();
		c = find_free(size);
		ASSERT(c != NULL);
	}
	DEBUG("found chunk 0x%p\n", c);

	split_free_chunk(c, size);
	return chunk2mem(c);
}

void *CRYPTO_malloc(size_t size, const char *file, int line)
{
	void *ret = NULL;
	if (size < SEG_SIZE)
		ret = dlmalloc(size);
	INFO("in crypto malloc from %s:%d %zu@%p\n", file, line, size, ret);
	show_segment();
	return ret;
}

void CRYPTO_free(void *ptr, const char *file, int line)
{
	struct malloc_chunk *c, *n;

	INFO("in crypto free from %s:%d: %p\n", file, line, ptr);
	if (ptr == NULL)
		return;

	c = mem2chunk(ptr);
	ASSERT(in_use(c));
	/* shred the data */
	memset(ptr, 0, chunk_size(c) - CHUNK_SIZE);

	n = next_chunk(c);
	DEBUG("free c=%p, n=%p, next_in_use=%d, prev_in_use=%d\n",
	       c,n,n && in_use(n),prev_in_use(c));

	/* now check for consolidation with previous */
	if (!prev_in_use(c)) {
		struct malloc_chunk *p = prev_chunk(c);

		p->h.head += chunk_size(c);
		if (n)
			n->h.prev_foot = chunk_size(p);

		/* the new consolidated chunk becomes our current
		 * chunk for the next free check below.  The previous
		 * chunk was already linked */
		c = p;
	} else {
		DEBUG("linking %p\n", c);
		link_free_chunk(c);
	}

	/* and finally consolidation with next */
	if (n && !in_use(n)) {
		unlink_free_chunk(n);
		c->h.head += chunk_size(n);
		n = next_chunk(c);
		if (n)
			n->h.prev_foot = chunk_size(c);
	} else if (n) {
		ASSERT(prev_in_use(n));
		n->h.head &= ~PINUSE_BIT;
	}
	c->h.head &= ~CINUSE_BIT;
	show_segment();
}

void CRYPTO_clear_free(void *str, size_t num, const char *file, int line)
{
    if (str == NULL)
        return;

    CRYPTO_free(str, file, line);
}
