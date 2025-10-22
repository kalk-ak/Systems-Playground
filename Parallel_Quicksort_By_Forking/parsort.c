#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

/* struct representing a child process */
typedef struct Child
{
    pid_t process_id;  // > 0 if a child was created; 0 indicates no fork was done
    int process_state; // 1 if the child is still running or dummy, 0 if waited
    int wait_status;   // Status from waitpid or dummy status
} Child;

/* Compare two int64_t values for qsort */
int compare(const void *left, const void *right);

/* Swap two elements in an array */
void swap(int64_t *arr, unsigned long i, unsigned long j);

/* Partition the subarray around a pivot.
   Returns the final pivot index.
*/
unsigned long partition(int64_t *arr, unsigned long start, unsigned long end);

/* Perform quicksort on the subarray using parallel
   processes. If the subarray size is <= par_threshold, sort sequentially with
   qsort. Returns 1 if sorting succeeded, 0 otherwise.
*/
int quicksort(int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold);

/* Fork a child process to sort the subarray. Helper function*/
Child quicksort_subproc(int64_t *arr, unsigned long start, unsigned long end,
                        unsigned long par_threshold);

/* Check whether the child represented by 'child' terminated successfully.
   Returns 1 if the child exited normally with exit code 0, 0 otherwise. */
int quicksort_check_success(Child *child);

/* Wait for the child represented by 'child' to finish.
   Returns 1 on success, 0 on error. */
int quicksort_wait(Child *child);

int main(int argc, char **argv)
{
    unsigned long par_threshold;
    if (argc != 3 || sscanf(argv[2], "%lu", &par_threshold) != 1)
    {
        fprintf(stderr, "Usage: %s <file> <par threshold>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];
    int fd = open(filename, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        perror("open");
        exit(EXIT_FAILURE);
    }
    struct stat statbuf;
    if (fstat(fd, &statbuf) != 0)
    {
        fprintf(stderr, "Error: fstat failed for file '%s'\n", filename);
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }
    unsigned long file_size = statbuf.st_size;
    unsigned long num_elements = file_size / sizeof(int64_t);
    int64_t *arr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (arr == MAP_FAILED)
    {
        fprintf(stderr, "Error: mmap failed for file '%s'\n", filename);
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    if (!quicksort(arr, 0, num_elements, par_threshold))
    {
        fprintf(stderr, "Error: Parallel quicksort failed\n");
        munmap(arr, file_size);
        exit(EXIT_FAILURE);
    }
    if (munmap(arr, file_size) != 0)
    {
        fprintf(stderr, "Error: munmap failed\n");
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/* Compare two int64_t values for qsort */
int compare(const void *left, const void *right)
{
    int64_t left_val = *(const int64_t *) left;
    int64_t right_val = *(const int64_t *) right;
    if (left_val < right_val)
        return -1;
    else if (left_val > right_val)
        return 1;
    else
        return 0;
}

/* Swap two elements in an array */
void swap(int64_t *arr, unsigned long i, unsigned long j)
{
    int64_t tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

/* Partition the subarray around a pivot.
   Returns the final pivot index.
*/
unsigned long partition(int64_t *arr, unsigned long start, unsigned long end)
{
    assert(end > start);
    unsigned long len = end - start;
    assert(len >= 2);
    unsigned long pivot_index = start + (len / 2);
    int64_t pivot_val = arr[pivot_index];
    swap(arr, pivot_index, end - 1);
    unsigned long left_index = start;
    unsigned long right_index = end - 2;
    while (left_index <= right_index)
    {
        if (arr[left_index] < pivot_val)
        {
            left_index++;
            continue;
        }
        if (arr[right_index] >= pivot_val)
        {
            if (right_index == 0)
                break;
            right_index--;
            continue;
        }
        swap(arr, left_index, right_index);
    }
    swap(arr, left_index, end - 1);
    return left_index;
}

/* Wait for the child represented by 'child' to finish.
   Returns 1 on success, 0 on error. */
int quicksort_wait(Child *child)
{
    if (child->process_id == 0)
        return 1; /* Dummy; nothing to wait for */
    if (child->process_id > 0)
    {
        int status;
        if (waitpid(child->process_id, &status, 0) < 0)
        {
            perror("waitpid");
            child->wait_status = -1;
            child->process_state = 0;
            return 0;
        }
        child->wait_status = status;
        child->process_state = 0;
    }
    return 1;
}

/* Check whether the child represented by 'child' terminated successfully.
   Returns 1 if the child exited normally with exit code 0, 0 otherwise. */
int quicksort_check_success(Child *child)
{
    if (child->process_id == 0)
        return 1; /* Dummy child is always successful */
    if (child->process_id > 0)
    {
        if (WIFEXITED(child->wait_status))
        {
            if (WEXITSTATUS(child->wait_status) == 0)
                return 1;
            else
                return 0;
        }
        return 0;
    }
    return 0;
}

/* Perform quicksort on the subarray using parallel
   processes. If the subarray size is <= par_threshold, sort sequentially with
   qsort. Returns 1 if sorting succeeded, 0 otherwise.
*/
int quicksort(int64_t *arr, unsigned long start, unsigned long end, unsigned long par_threshold)
{
    assert(end >= start);
    unsigned long len = end - start;
    if (len < 2)
        return 1;
    if (len <= par_threshold)
    {
        qsort(arr + start, len, sizeof(int64_t), compare);
        return 1;
    }
    unsigned long mid = partition(arr, start, end);
    Child left = quicksort_subproc(arr, start, mid, par_threshold);
    Child right = quicksort_subproc(arr, mid + 1, end, par_threshold);
    if (!quicksort_wait(&left))
    {
        fprintf(stderr, "Error waiting for left child process\n");
        return 0;
    }
    if (!quicksort_wait(&right))
    {
        fprintf(stderr, "Error waiting for right child process\n");
        return 0;
    }
    if (!quicksort_check_success(&left))
        return 0;
    if (!quicksort_check_success(&right))
        return 0;
    return 1;
}

/* Fork a child process to sort the subarray. Helper function*/
Child quicksort_subproc(int64_t *arr, unsigned long start, unsigned long end,
                        unsigned long par_threshold)
{
    unsigned long len = end - start;
    if (len < 2)
        return (Child) {0, 0, 0}; /* Dummy: no work needed */
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return (Child) {-1, 1, -1};
    }
    else if (pid == 0)
    {
        if (len <= par_threshold)
        {
            qsort(arr + start, len, sizeof(int64_t), compare);
            _exit(0);
        }
        else
        {
            unsigned long mid = partition(arr, start, end);
            Child left = quicksort_subproc(arr, start, mid, par_threshold);
            Child right = quicksort_subproc(arr, mid + 1, end, par_threshold);
            if (!quicksort_wait(&left))
            {
                fprintf(stderr, "Child process error waiting for left subchild\n");
                _exit(1);
            }
            if (!quicksort_wait(&right))
            {
                fprintf(stderr, "Child process error waiting for right subchild\n");
                _exit(1);
            }
            if (!quicksort_check_success(&left))
            {
                fprintf(stderr, "Child process detected failure in left subchild\n");
                _exit(1);
            }
            if (!quicksort_check_success(&right))
            {
                fprintf(stderr, "Child process detected failure in right subchild\n");
                _exit(1);
            }
            _exit(0);
        }
    }
    else
    {
        Child child;
        child.process_id = pid;
        child.process_state = 1;
        child.wait_status = 0;
        return child;
    }
}
