#include <zephyr/ztest.h>

#include "health_monitor.h"

ZTEST(health_monitor, test_initial_state_is_healthy)
{
    struct health_monitor monitor;

    health_monitor_init(&monitor);

    zassert_true(health_monitor_is_healthy(&monitor),
                 "Monitor should start healthy");
    zassert_equal(health_monitor_fault_count(&monitor), 0U,
                  "Initial fault count should be zero");
}

ZTEST(health_monitor, test_recording_fault_marks_unhealthy)
{
    struct health_monitor monitor;

    health_monitor_init(&monitor);
    health_monitor_record_fault(&monitor);

    zassert_false(health_monitor_is_healthy(&monitor),
                  "Monitor should report a fault");
    zassert_equal(health_monitor_fault_count(&monitor), 1U,
                  "Fault count should be one");
}

ZTEST(health_monitor, test_multiple_faults_are_counted)
{
    struct health_monitor monitor;

    health_monitor_init(&monitor);
    health_monitor_record_fault(&monitor);
    health_monitor_record_fault(&monitor);
    health_monitor_record_fault(&monitor);

    zassert_equal(health_monitor_fault_count(&monitor), 3U,
                  "All three faults should be counted");
}

ZTEST(health_monitor, test_clearing_faults_restores_healthy_state)
{
    struct health_monitor monitor;

    health_monitor_init(&monitor);
    health_monitor_record_fault(&monitor);
    health_monitor_clear_faults(&monitor);

    zassert_true(health_monitor_is_healthy(&monitor),
                 "Monitor should be healthy after clearing faults");
    zassert_equal(health_monitor_fault_count(&monitor), 0U,
                  "Fault count should reset to zero");
}

ZTEST_SUITE(health_monitor, NULL, NULL, NULL, NULL, NULL);

