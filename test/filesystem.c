#include <emmc.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>

/*
 * Private function prototypes to include in tests
 */
int fs_lookup(const char *name, struct dirent_t *ret);
int fs_str_to_name(char *short_name, const char *filename);
void fs_name_to_str(char *filename, const char *short_name);

const char *test_name = "FS";

const char *test_file = "FS_TEST.TXT";
const char *test_str = "'Twas brillig, and the slithy toves\n"
	"Did gyre and gimble in the wabe;\n"
	"All mimsy were the borogoves,\n"
	"And the mome raths outgrabe.\n";

const char *run_test()
{
        int ret;
        char short_name[12];
        char filename[13];
        struct dirent_t dirent;

        fs_init();
        /*fs_dump_part_table();*/

        /* fs_str_to_name */
        memset(short_name, '\0', 12);
        memset(filename, '\0', 13);

        ret = fs_str_to_name(short_name, "Eight888.txt");
        if (ret || strcmp(short_name, "EIGHT888TXT"))
                return "converting 8.3 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "four.txt");
        if (ret || strcmp(short_name, "FOUR    TXT"))
                return "converting 4.3 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "Eight888.t");
        if (ret || strcmp(short_name, "EIGHT888T  "))
                return "converting 8.1 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "1.TW");
        if (ret || strcmp(short_name, "1       TW "))
                return "converting 1.2 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "four.");
        if (ret || strcmp(short_name, "FOUR       "))
                return "converting 4. formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "four");
        if (ret || strcmp(short_name, "FOUR       "))
                return "converting 4 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "NineChars");
        if (ret || strcmp(short_name, "NINECHAR   "))
                return "converting 9 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "IamALongFileName.txt");
        if (ret || strcmp(short_name, "IAMALONGTXT"))
                return "converting 16.3 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "IamALongFileName.t");
        if (ret || strcmp(short_name, "IAMALONGT  "))
                return "converting 16.1 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "IamALongFileName.LongExt");
        if (ret || strcmp(short_name, "IAMALONGLON"))
                return "converting 16.7 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "I.LongExt");
        if (ret || strcmp(short_name, "I       LON"))
                return "converting 1.7 formatted string to 8.3 name";

        ret = fs_str_to_name(short_name, "");
        if (!ret)
                return "converting empty string to 8.3 name";

        ret = fs_str_to_name(short_name, "Glob*File.[]");
        if (!ret)
                return "converting invalid char to 8.3 name";

        ret = fs_str_to_name(short_name, ".dotfile");
        if (!ret)
                return "converting string beginning with . to 8.3 name";

        ret = fs_str_to_name(short_name, "dots.....ext");
        if (ret || strcmp(short_name, "DOTS    EXT"))
                return "converting string with too many . to 8.3 name";

        ret = fs_str_to_name(short_name, "f.ex1.ex2");
        if (ret || strcmp(short_name, "F       EX1"))
                return "converting string with too many extensions to 8.3 name";

        /* fs_name_to_str */
        fs_name_to_str(filename, "FILENAMEEXT");
        if (strcmp(filename, "FILENAME.EXT"))
                return "converting 8.3 name to string";

        fs_name_to_str(filename, "FILE    EXT");
        if (strcmp(filename, "FILE.EXT"))
                return "converting 4.3 name to string";

        fs_name_to_str(filename, "FILENAMEE  ");
        if (strcmp(filename, "FILENAME.E"))
                return "converting 8.1 name to string";

        fs_name_to_str(filename, "F       E  ");
        if (strcmp(filename, "F.E"))
                return "converting 1.1 name to string";


        /* fs_lookup */
        ret = fs_lookup("bootcode.bin", &dirent);
        if (ret)
                return "could not find first file";

        ret = fs_lookup("kernel.img", &dirent);
        if (ret)
                return "could not find last file";

        ret = fs_lookup("bootcode.bi", &dirent);
        if (!ret)
                return "found non-existant file";

        /* fs_read */
        unsigned char read_buf[1024];
        int bytes_read, pos = 0;
        do {
                bytes_read = fs_read(test_file, &read_buf[pos], pos, 64);
                pos += bytes_read;
                read_buf[pos] = '\0';
        } while (bytes_read);
	if (strcmp((char *)read_buf, test_str))
		return "failed reading from short file";

        return NULL;
}
