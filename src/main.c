#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>

#include "health_monitor.h"

#define REPORT_INTERVAL_MS 500
#define MONITOR_STACK_SIZE 1024
#define MONITOR_PRIORITY 5

K_MUTEX_DEFINE(monitor_mutex);

static struct health_monitor monitor;
static uint32_t report_count;

K_THREAD_STACK_DEFINE(monitor_stack, MONITOR_STACK_SIZE);
static struct k_thread monitor_thread;

static void monitor_thread_entry(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    while (1) {
        k_mutex_lock(&monitor_mutex, K_FOREVER);
        report_count++;
        k_mutex_unlock(&monitor_mutex);

        k_msleep(REPORT_INTERVAL_MS);
    }
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    bool healthy;
    int64_t uptime = k_uptime_get();

    k_mutex_lock(&monitor_mutex, K_FOREVER);
    healthy = health_monitor_is_healthy(&monitor);
    k_mutex_unlock(&monitor_mutex);

    shell_print(sh, "Health: %s", healthy ? "OK" : "FAULT");
    shell_print(sh, "Uptime: %lld ms", (long long)uptime);
    return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    uint32_t reports;
    uint32_t faults;

    k_mutex_lock(&monitor_mutex, K_FOREVER);
    reports = report_count;
    faults = health_monitor_fault_count(&monitor);
    k_mutex_unlock(&monitor_mutex);

    shell_print(sh, "Reports: %u", reports);
    shell_print(sh, "Faults: %u", faults);
    return 0;
}

static int cmd_fault(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    k_mutex_lock(&monitor_mutex, K_FOREVER);
    health_monitor_record_fault(&monitor);
    k_mutex_unlock(&monitor_mutex);

    shell_print(sh, "Simulated fault recorded.");
    return 0;
}

static int cmd_clear(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    k_mutex_lock(&monitor_mutex, K_FOREVER);
    health_monitor_clear_faults(&monitor);
    k_mutex_unlock(&monitor_mutex);

    shell_print(sh, "Fault state cleared.");
    return 0;
}

SHELL_CMD_REGISTER(status, NULL, "Show device health and uptime", cmd_status);
SHELL_CMD_REGISTER(stats, NULL, "Show report and fault counters", cmd_stats);
SHELL_CMD_REGISTER(fault, NULL, "Simulate a device fault", cmd_fault);
SHELL_CMD_REGISTER(fault_clear, NULL, "Clear simulated faults", cmd_clear);

int main(void)
{
    health_monitor_init(&monitor);

    printk("\n=== Embedded Device Health Monitor ===\n");
    printk("Type 'help' to see available commands.\n");

    k_thread_create(&monitor_thread, monitor_stack,
                    K_THREAD_STACK_SIZEOF(monitor_stack),
                    monitor_thread_entry,
                    NULL, NULL, NULL,
                    MONITOR_PRIORITY, 0, K_NO_WAIT);

    k_thread_name_set(&monitor_thread, "health_monitor");

    return 0;
}
