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
	{ 0x20000329, "simple_strtoul" },
	{ 0xe4783c19, "filp_close" },
	{ 0xf94e938b, "kobject_del" },
	{ 0x94de3391, "kobject_create_and_add" },
	{ 0xb72397d5, "printk" },
	{ 0xe2be5557, "sysfs_create_group" },
	{ 0xb4390f9a, "mcount" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x21522420, "vfs_write" },
	{ 0xe914e41e, "strcpy" },
	{ 0x8b0975aa, "filp_open" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "78A1F563147B8E2BABE7BC9");
