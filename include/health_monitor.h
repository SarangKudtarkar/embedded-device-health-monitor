#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

struct health_monitor {
    uint32_t fault_count;
};

void health_monitor_init(struct health_monitor *monitor);
void health_monitor_record_fault(struct health_monitor *monitor);
void health_monitor_clear_faults(struct health_monitor *monitor);
bool health_monitor_is_healthy(const struct health_monitor *monitor);
uint32_t health_monitor_fault_count(const struct health_monitor *monitor);

#endif
