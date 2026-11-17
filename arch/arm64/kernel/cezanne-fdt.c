// SPDX-License-Identifier: GPL-2.0-only
/*
 * CEZANNE: replace the Android device tree handed over by LK with the
 * mainline device tree embedded in the kernel image.
 *
 * LK only accepts a boot image whose dtb field is a valid DTBO container
 * and applies the stock dtbo partition on top of it.  Those overlays use
 * vendor bindings and describe hardware the mainline kernel must not
 * drive blindly.  We therefore keep the stock tree only long enough for
 * setup_machine_fdt() to move /memory and /chosen into memblock and for
 * the LK reserved-memory scan in setup.c to reserve the bootloader's
 * dynamically placed windows, then switch to our own tree.
 */

#include <linux/acpi.h>
#include <linux/init.h>
#include <linux/libfdt.h>
#include <linux/of_fdt.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <asm/setup.h>

void __init cezanne_fdt_setup(void)
{
	extern char _binary_arch_arm64_boot_dts_mediatek_mt6885_xiaomi_cezanne_dtb_start[];
	extern char _binary_arch_arm64_boot_dts_mediatek_mt6885_xiaomi_cezanne_dtb_end[];
	size_t dtb_size =
		(size_t)(_binary_arch_arm64_boot_dts_mediatek_mt6885_xiaomi_cezanne_dtb_end -
			 _binary_arch_arm64_boot_dts_mediatek_mt6885_xiaomi_cezanne_dtb_start);
	void *lk_fdt = initial_boot_params;

	if (!acpi_disabled || !lk_fdt)
		return;

	pr_info("CEZANNE-DTB: replacing LK FDT %px with embedded mt6885-xiaomi-cezanne.dtb (%zu bytes)\n",
		lk_fdt, dtb_size);
	initial_boot_params =
		_binary_arch_arm64_boot_dts_mediatek_mt6885_xiaomi_cezanne_dtb_start;

	/*
	 * boot_command_line was captured from LK's tree by
	 * setup_machine_fdt(); our embedded /chosen/bootargs is not read.
	 * Append the parameters that only the mainline tree needs.
	 */
	if (!strstr(boot_command_line, "ignore_loglevel"))
		strncat(boot_command_line, " ignore_loglevel",
			COMMAND_LINE_SIZE - strlen(boot_command_line) - 1);

	if (!strstr(boot_command_line, "earlycon"))
		strncat(boot_command_line,
			" earlycon=mtk8250,mmio32,0x11002000,921600n8",
			COMMAND_LINE_SIZE - strlen(boot_command_line) - 1);

	if (!strstr(boot_command_line, "clk_ignore_unused"))
		strncat(boot_command_line, " clk_ignore_unused",
			COMMAND_LINE_SIZE - strlen(boot_command_line) - 1);

	/*
	 * LK passes ramoops.mem_address=0x4d010000 and ramoops.mem_size=...
	 * as module parameters.  ramoops_register_dummy() would register a
	 * dummy device at that address before our DT node probes; force
	 * mem_size to zero (last occurrence wins) so our node is the only
	 * ramoops instance.
	 */
	strncat(boot_command_line, " ramoops.mem_size=0",
		COMMAND_LINE_SIZE - strlen(boot_command_line) - 1);
}
