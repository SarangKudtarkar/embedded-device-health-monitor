#include "health_monitor.h"

void health_monitor_init(struct health_monitor *monitor)
{
    monitor->fault_count = 0U;
}

void health_monitor_record_fault(struct health_monitor *monitor)
{
    if (monitor->fault_count < UINT32_MAX) {
        monitor->fault_count++;
    }
}

void health_monitor_clear_faults(struct health_monitor *monitor)
{
    monitor->fault_count = 0U;
}

bool health_monitor_is_healthy(const struct health_monitor *monitor)
{
    return monitor->fault_count == 0U;
}

uint32_t health_monitor_fault_count(const struct health_monitor *monitor)
{
    return monitor->fault_count;
}
