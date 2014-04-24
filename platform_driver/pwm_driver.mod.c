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
	{ 0x405fe887, "module_layout" },
	{ 0x17a142df, "__copy_from_user" },
	{ 0x98082893, "__copy_to_user" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x8893fa5d, "finish_wait" },
	{ 0x1000e51, "schedule" },
	{ 0x75a17bed, "prepare_to_wait" },
	{ 0x5f754e5a, "memset" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xb9e52429, "__wake_up" },
	{ 0x28874cf2, "platform_driver_register" },
	{ 0x788fe103, "iomem_resource" },
	{ 0x3ec6c705, "kmalloc_caches" },
	{ 0x75d5680c, "misc_register" },
	{ 0x3b810932, "dev_set_drvdata" },
	{ 0x62b72b0d, "mutex_unlock" },
	{ 0xe16b893b, "mutex_lock" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0xfda85a7d, "request_threaded_irq" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x9bce482f, "__release_region" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0xadf42bd5, "__request_region" },
	{ 0xd8066d2d, "platform_get_resource" },
	{ 0x6f3d8ffc, "dev_driver_string" },
	{ 0x6b1e3028, "kmem_cache_alloc" },
	{ 0xbf42b7f, "misc_deregister" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x37a0cba, "kfree" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0xea147363, "printk" },
	{ 0xc47a4c41, "platform_driver_unregister" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

