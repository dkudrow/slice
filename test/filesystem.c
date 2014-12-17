#include <filesystem.h>
#include <emmc.h>
#include <stdio.h>

int main() {
	fat32_init();
	fat32_dump_part_table();
}
