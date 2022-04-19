#include "mylib.h"
#include "tlpi_hdr.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>


#define MIN_HEAP_ALLOC_SIZE 4096
#define MIN_HEAP_FREE_SIZE  8192


// Each memory block allocated contains a `freelist_node` at its beginning

typedef struct freelist_node {
    struct freelist_node *prev, *next;
    size_t length;
} freelist_node_t;

#define user_addr(block)    ((void *) (((char *) block) + sizeof(freelist_node_t)))
#define malloc_addr(block)  ((void *) (((char *) block) - sizeof(freelist_node_t)))

static freelist_node_t *FreeList = NULL;

// Insert a header block and returns its address
static void block_init(freelist_node_t *block)
{
    block->prev = NULL;
    block->next = NULL;
    block->length = 0;
}

static void block_split(freelist_node_t *block, size_t size,
                        freelist_node_t **left, freelist_node_t **right)
{
    assert(block->length >= size + sizeof(freelist_node_t));

    // The right block
    *right = (freelist_node_t *) ((char *) block + sizeof(freelist_node_t) + size);
    (*right)->length = block->length - size - sizeof(freelist_node_t);

    // The left block
    *left = block;
    (*left)->length = size;

    // Adjust the links now
    (*right)->prev = *left;
    (*right)->next = block->next;
    (*left)->next = *right;
}

// Merge right block into the left one and return the new block
static freelist_node_t *block_merge(freelist_node_t *left,
                                    freelist_node_t **right)
{
    left->length += (*right)->length + sizeof(freelist_node_t);
    left->next = (*right)->next;

    if ((*right)->next) {
        (*right)->next->prev = left;
    }

    // Reset the right block
    block_init(*right);
    return left;
}


static void freelist_remove(freelist_node_t *block)
{
    freelist_node_t *prev = block->prev;
    freelist_node_t *next = block->next;

    if (prev && next) {
        prev->next = block->next;
        next->prev = block->prev;
    } else if (prev) {
        // That was the last node; adjust our predecessor
        prev->next = NULL;
    } else if (next) {
        // That was the first node; adjust the FreeList
        FreeList = next;
        FreeList->prev = NULL;
    } else {
        // That was the only node; now FreeList is empty
        FreeList = NULL;
    }
}


static void freelist_insert(freelist_node_t *block)
{
    // Free blocks are added in an increasing address order
    if (!FreeList) {
        FreeList = block;
        return;
    }

    freelist_node_t *last = NULL;
    freelist_node_t *fn = FreeList;
    while (fn) {
        if (block == fn)
            // Trying to add an existing block
            return;

        if (block < fn) {
            block->next = fn;
            block->prev = fn->prev;
            fn->prev = block;
            if (last) {
                last->next = block;
            } else {
                // This is now the first block in the list
                FreeList = block;
            }
            return;
        }
        last = fn;
        fn = fn->next;
    }

    // Append the block at the end of the list
    last->next = block;
    block->prev = last;
    block->next = NULL;
}


freelist_node_t *freelist_get_free_block(freelist_node_t *start, size_t size)
{
    freelist_node_t *free_block = start;
    while (free_block) {
        size_t free_block_len = free_block->length;
        if (free_block_len >= size + sizeof(freelist_node_t)) {
            // Suitable block with enough space to split it
            freelist_node_t *left, *right;
            block_split(free_block, size, &left, &right);
            freelist_remove(left);
            block_init(left);
            left->length = size;
            return left;
        } else if (free_block_len >= size) {
            // Suitable block, but without enough space to split it
            freelist_remove(free_block);
            block_init(free_block);
            free_block->length = free_block_len;
            return free_block;
        }

        free_block = free_block->next;
    }

    assert(!free_block);
    return free_block;
}

// Utility functions to inspect the free list

size_t freelist_size()
{
    freelist_node_t *fp = FreeList;
    size_t ret = 0;
    while (fp) {
        ++ret;
        fp = fp->next;
    }

    return ret;
}


const freelist_node_t *freelist_item(size_t index)
{
    size_t i = 0;
    freelist_node_t *fp = FreeList;
    while (fp) {
        if (i == index) {
            return fp;
        }

        fp = fp->next;
        ++i;
    }

    return NULL;
}

bool is_freelist_sane()
{
    // Check that the free list is not broken
    freelist_node_t *last = NULL;
    freelist_node_t *fp = FreeList;
    while (fp) {
        if (fp->prev != last)
            return false;

        last = fp;
        fp = fp->next;
    }

    return true;
}


void *xmalloc(size_t size)
{
    // Walk the free list and try to find a suitable free block
    // We follow a first-fit policy here for simplicity
    freelist_node_t *new_block = freelist_get_free_block(FreeList, size);
    if (new_block)
        return user_addr(new_block);

    // Not a big enough block was found; we need to increase the heap
    size_t alloc_size = XMAX(size + sizeof(freelist_node_t), MIN_HEAP_ALLOC_SIZE);
    new_block = sbrk(alloc_size);
    if ((void *) new_block == (void *) -1) {
        errno = ENOMEM;
        return NULL;
    }

    // Create a new free block with the memory we got
    block_init(new_block);
    new_block->length = (char *) sbrk(0) - (char *) new_block - sizeof(freelist_node_t);
    freelist_insert(new_block);

    // Get a right sized block from the list starting from the freshly inserted
    // block
    new_block = freelist_get_free_block(new_block, size);
    assert(new_block);
    return user_addr(new_block);
}

void xfree(void *ptr)
{
    if (!ptr)
        return;

    freelist_node_t *free_block = malloc_addr(ptr);
    freelist_insert(free_block);

    // Check if you can merge the block with its adjacent ones
    freelist_node_t *prev = free_block->prev;
    freelist_node_t *next = free_block->next;
    if (prev && (char *) prev + prev->length + sizeof(freelist_node_t) == (char *) free_block) {
        // We can merge with the previous block
        free_block = block_merge(prev, &free_block);
    }

    if (next && (char *) free_block + free_block->length + sizeof(freelist_node_t) == (char *) next) {
        // We can merge with the next block
        block_merge(free_block, &next);
    }

    // Shrink the heap if needed by removing the last free block
    freelist_node_t *last = NULL;
    freelist_node_t *fp = FreeList;
    while (fp) {
        last = fp;
        fp = fp->next;
    }

    if (last && last->length >= MIN_HEAP_FREE_SIZE) {
        // Remove the last block and shrink the heap
        freelist_remove(last);
        brk(last);
    }
}

void test_xmalloc(void **state)
{
    const int AllocSize = 1024;

    int *nums = xmalloc(AllocSize*sizeof(int));
    for (int i = 0; i < AllocSize; ++i) {
        nums[i] = 10;
    }

    assert_int_equal(freelist_size(), 0);

    xfree(nums);

    // Assert that we have a single 4KB free block
    assert_true(is_freelist_sane());
    assert_int_equal(freelist_size(), 1);
    const freelist_node_t *block = freelist_item(0);
    assert_int_equal(block->length, AllocSize*sizeof(int));

    // Allocate 4x 1024KB arrays
    block = freelist_item(0);
    char *xchar[4];
    for (int i = 0; i < 4; ++i) {
        xchar[i] = xmalloc(AllocSize);
    }

    // There are two free blocks: one from the initial allocation that did not
    // have enough space to accommodate this allocation and the remaining of the
    // newly allocated one (remember there is a minimum heap allocation size).
    assert_true(is_freelist_sane());
    assert_int_equal(freelist_size(), 2);
    block = freelist_item(0);
    assert_int_equal(block->length, 1024 - 3*sizeof(freelist_node_t));

    // We subtract two freelist_node_t from the final free block because one is
    // allocated and one to account for the remaining free block.
    block = freelist_item(1);
    assert_int_equal(block->length,
                     MIN_HEAP_ALLOC_SIZE - 1024 - 2*sizeof(freelist_node_t));

    // Free the two blocks in the middle and check that the resulting free
    // blocks are merged into the first free block.
    xfree(xchar[1]);
    assert_true(is_freelist_sane());
    xfree(xchar[2]);
    assert_true(is_freelist_sane());

    assert_int_equal(freelist_size(), 2);
    block = freelist_item(0);
    assert_int_equal(block->length, 3048);

    // The last free block remains unchanged
    block = freelist_item(1);
    assert_int_equal(block->length,
                     MIN_HEAP_ALLOC_SIZE - 1024 - 2*sizeof(freelist_node_t));

    // Allocate a big enough chunk of memory and then release it and check that
    // the heap is also shrunk
    size_t nums_size = 1024*AllocSize*sizeof(int);
    nums = xmalloc(nums_size);
    assert_true(is_freelist_sane());
    assert_int_equal(freelist_size(), 2);

    // We check that the heap is shrunk at least as much as the last
    // allocation's size. The heap may shrink more since the last free block may
    // be merged with the last one
    char *orig_heap_limit = sbrk(0);
    xfree(nums);
    assert_true(orig_heap_limit - (char *) sbrk(0) >= (int) nums_size);
}

void test_xmalloc_multiple(void **state)
{
    // Do multiple allocations
    const int AllocSize = 1024;

    int **nums = xmalloc(AllocSize*sizeof(int *));
    for (int i = 0; i < AllocSize; ++i) {
        nums[i] = xmalloc(AllocSize*sizeof(int));
    }

    for (int i = 0; i < AllocSize; ++i) {
        for (int j = 0; j < AllocSize; ++j) {
            nums[i][j] = 5;
        }
    }

    // Free arrays at even locations
    for (int i = 0; i < AllocSize; i += 2) {
        xfree(nums[i]);
    }

    // Now allocate smaller regions that should fit within the freed areas
    char **str = xmalloc(AllocSize*sizeof(char *));
    for (int i = 0; i < AllocSize; ++i) {
        str[i] = xmalloc(AllocSize*sizeof(char));
    }
}


int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_xmalloc),
        cmocka_unit_test(test_xmalloc_multiple),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
