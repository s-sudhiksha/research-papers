#include "performance_metrics.h"
#include <time.h>

double now_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

long ru_maxrss_diff_kb(const struct rusage *start, const struct rusage *end) {
    return (end->ru_maxrss - start->ru_maxrss);
}

long ru_majflt_diff(const struct rusage *start, const struct rusage *end) {
    return (end->ru_majflt - start->ru_majflt);
}

long ru_minflt_diff(const struct rusage *start, const struct rusage *end) {
    return (end->ru_minflt - start->ru_minflt);
}