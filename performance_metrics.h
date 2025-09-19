#ifndef PERFORMANCE_METRICS_H
#define PERFORMANCE_METRICS_H

#include <sys/resource.h>

double now_ms(void);
long ru_maxrss_diff_kb(const struct rusage *start, const struct rusage *end);
long ru_majflt_diff(const struct rusage *start, const struct rusage *end);
long ru_minflt_diff(const struct rusage *start, const struct rusage *end);

#endif