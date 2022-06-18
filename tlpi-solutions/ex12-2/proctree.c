#include "mylib.h"
#include "tlpi_hdr.h"

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define CHILD_MAX 10

size_t MaxProctabSize = 1024;

typedef struct proctree {
    pid_t pid;
    pid_t ppid;
    char *name;
    struct proctree *parent;
    struct proctree **children;
    size_t nr_child;
    size_t max_child;
} proctree_t;


int _proc_cmp(const void *x, const void *y)
{
    const proctree_t *x_ = (const proctree_t *) x;
    const proctree_t *y_ = (const proctree_t *) y;
    return x_->pid - y_->pid;
}


/*
 * Create a process tree from a list of sorted processes and return a pointer to
 * the init process. The complexity is O(nlogn).
 */
proctree_t *proctree_build_tree(proctree_t *procs, size_t nr_procs)
{
    proctree_t *root = NULL;
    for (size_t i = 0; i < nr_procs; ++i) {
        if (procs[i].pid == 1) {
            root = &procs[i];
            procs[i].parent = NULL;
            continue;
        } else if (procs[i].ppid == 0) {
            // `/proc` lists also kernel process whose PPID is also 0.
            continue;
        }

        // Look for the parent and build the links
        proctree_t key;
        key.pid = procs[i].ppid;
        proctree_t *parent = bsearch(&key, procs, nr_procs, sizeof(*procs), _proc_cmp);
        assert(parent);

        // Update our links
        procs[i].parent = parent;

        // Update parent's links
        if (parent->nr_child == parent->max_child) {
            // Expand the children array
            size_t new_max_child = parent->max_child << 1;
            proctree_t **new_children = realloc(parent->children,
                                                new_max_child*sizeof(proctree_t *));
            if (!new_children) {
                return NULL;
            }
            parent->children = new_children;
            parent->max_child = new_max_child;
        }
        parent->children[parent->nr_child++] = &procs[i];
    }
    return root;
}

void proctree_delete(proctree_t *procs, size_t nr_procs)
{
    for (size_t i = 0; i < nr_procs; ++i) {
        free(procs[i].name);
        free(procs[i].children);
    }
    free(procs);
}

void _do_proctree_print(const proctree_t *proc, size_t level, const char *prefix)
{
    char line[LINE_MAX];
    *line = '\0';
    for (size_t i = 0; i < level; ++i) {
        strcat(line, "\t");
    }
    if (level) {
        strcat(line, prefix);
        strcat(line, "──");
    }
    strcat(line, proc->name);
    printf("%s (pid: %d)\n", line, proc->pid);
    for (size_t i = 0; i < proc->nr_child; ++i) {
        _do_proctree_print(proc->children[i], level + 1,
                           (i == proc->nr_child-1) ? "└─" : "├─");
    }
}


void proctree_print(const proctree_t *root)
{
    _do_proctree_print(root, 0, "");
}

int main(int argc, char *argv[])
{
    proctree_t *proctab = malloc(MaxProctabSize*sizeof(*proctab));
    if (!proctab) {
        errExit("malloc() failed");
    }
    DIR *dp = opendir("/proc");
    if (!dp) {
        errExit("could not open `/proc' directory");
    }

    size_t nr_procs = 0;
    struct dirent *dir;
    while ( (dir = readdir(dp)) != NULL) {
        if (!isdigit(dir->d_name[0])) {
            // Not a PID
            continue;
        }

        char status_file[PATH_MAX];
        snprintf(status_file, PATH_MAX, "/proc/%d/status", atoi(dir->d_name));
        FILE *fp = fopen(status_file, "r");
        if (!fp) {
            // The process has probably exited
            continue;
        }

        char proc_name[LINE_MAX];
        pid_t proc_pid = -1;
        pid_t proc_ppid = -1;
        char *line = NULL;
        size_t line_size = 0;
        while (getline(&line, &line_size, fp) != -1) {
            if (!strncmp(line, "Name:", 5)) {
                sscanf(line, "Name: %s", proc_name);
            } else if (!strncmp(line, "Pid:", 4)) {
                sscanf(line, "Pid: %d", &proc_pid);
            } else if (!strncmp(line, "PPid:", 4)) {
                sscanf(line, "PPid: %d", &proc_ppid);
            }
        }
        if (proc_pid == -1 || proc_ppid == -1) {
            // This should not happen
            fatal("could not retrieve the PID or PPID of process %s", dir->d_name);
        }

        if (nr_procs == MaxProctabSize) {
            // Not enough space; realloc
            size_t new_size = MaxProctabSize << 1;
            proctab = realloc(proctab, new_size*sizeof(*proctab));
            if (!proctab) {
                errExit("realloc() failed");
            }
            MaxProctabSize = new_size;
        }

        proctree_t *new_entry = &proctab[nr_procs++];
        new_entry->pid = proc_pid;
        new_entry->ppid = proc_ppid;
        new_entry->name = strdup(proc_name);
        new_entry->parent = NULL;
        new_entry->children = malloc(CHILD_MAX*sizeof(proctree_t *));
        new_entry->max_child = CHILD_MAX;
        new_entry->nr_child = 0;
        if (!new_entry->children) {
            errExit("malloc() failed");
        }
        fclose(fp);
        free(line);
    }
    closedir(dp);

    // Sort processes by their PID and then build the process tree
    qsort(proctab, nr_procs, sizeof(*proctab), _proc_cmp);
    proctree_t *ptree = proctree_build_tree(proctab, nr_procs);
    printf("Number of processes: %ld\n", nr_procs);
    proctree_print(proctab);
    return 0;
}
