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
	{ 0x7ece90df, "platform_device_put" },
	{ 0x9c37a684, "platform_device_add" },
	{ 0x946f41dc, "platform_device_add_resources" },
	{ 0x5bec431d, "platform_device_alloc" },
	{ 0xd0de5a05, "platform_device_unregister" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";
