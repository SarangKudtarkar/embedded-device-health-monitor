
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>

#define REPORT_INTERVAL_MS 500

static uint32_t report_count;
static uint32_t simulated_fault_count;

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "Health: %s",
                simulated_fault_count == 0 ? "OK" : "FAULT");
    shell_print(sh, "Uptime: %lld ms", (long long)k_uptime_get());
    return 0;
}

static int cmd_stats(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "Reports: %u", report_count);
    shell_print(sh, "Faults: %u", simulated_fault_count);
    return 0;
}

static int cmd_fault(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    simulated_fault_count++;
    shell_print(sh, "Simulated fault recorded.");
    return 0;
}

static int cmd_clear(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    simulated_fault_count = 0;
    shell_print(sh, "Fault state cleared.");
    return 0;
}

SHELL_CMD_REGISTER(status, NULL, "Show device health and uptime", cmd_status);
SHELL_CMD_REGISTER(stats, NULL, "Show report and fault counters", cmd_stats);
SHELL_CMD_REGISTER(fault, NULL, "Simulate a device fault", cmd_fault);
SHELL_CMD_REGISTER(fault_clear, NULL, "Clear simulated faults", cmd_clear);

int main(void)
{
    printk("\n=== Embedded Device Health Monitor ===\n");
    printk("Type 'help' to see available commands.\n");

    while (1) {
        report_count++;
        k_msleep(REPORT_INTERVAL_MS);
    }

    return 0;
}

