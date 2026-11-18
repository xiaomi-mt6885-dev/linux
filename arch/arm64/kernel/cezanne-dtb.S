/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Embed the cezanne board DTB with a guaranteed 8-byte alignment.
 *
 * The libfdt blob must be at least 8-byte aligned; objcopy's binary
 * wrapper alone does not guarantee that, and an unaligned blob makes
 * of_fdt_unflatten_tree() fail with "Invalid device tree blob header".
 */
	.section .rodata, "a"
	.balign 8
	.global cezanne_dtb_start
	.global cezanne_dtb_end
cezanne_dtb_start:
	.incbin "arch/arm64/boot/dts/mediatek/mt6885-xiaomi-cezanne.dtb"
cezanne_dtb_end:
