#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x7d09bae2, "module_layout" },
	{ 0xa6a0a1ef, "blk_init_queue" },
	{ 0x2f18b4f5, "alloc_disk" },
	{ 0x4a3a00a9, "blk_cleanup_queue" },
	{ 0xb72397d5, "printk" },
	{ 0x17baf033, "del_gendisk" },
	{ 0xb4390f9a, "mcount" },
	{ 0x401e1f07, "put_disk" },
	{ 0x625ff490, "blk_fetch_request" },
	{ 0xbd944b7d, "add_disk" },
	{ 0x7e89ece4, "blk_end_request_all" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3013A81F4DBC9489BCB89D6");
