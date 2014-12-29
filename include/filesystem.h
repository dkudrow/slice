/* include/filesystem.h -- TODO */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define FS_MAX_NAME 255

void fs_init();
void fs_dump_part_table();
int fs_str_to_name(char *short_name, const char *filename);
void fs_name_to_str(char *filename, const char *short_name);

#endif /* FILESYSTEM_H */

