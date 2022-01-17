#include <allegro.h>

#define CMP_PFX_32 12628
#define CMP_PFX_64 12632
#define PLAYER_32 4148
#define PLAYER_64 4160
#define SHARD_32 212
#define SHARD_64 224
#define MASK_32 28
#define MASK_64 40
#define TUTORIAL 288

static char buf[12800];
static PACKFILE *f32, *f64;

static void convert_section(int len32, int len64, int nptr)
{
	/*
	 * 32-bit and 64-bit saves are incompatible, because saving involves
	 * writing raw structs (which contain pointers) to disk.
	 * Conversion involves copying everything else verbatim, and filling
	 * pointers and padding with zeroes.
	 */
	int off = len32 - nptr * 4;
	pack_fread(buf, len32, f32);
	memset(buf + off, 0, len64 - off);
	pack_fwrite(buf, len64, f64);
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return 1;

	f32 = pack_fopen(argv[1], F_READ_PACKED);  // game0.bka
	f64 = pack_fopen(argv[2], F_WRITE_PACKED); // game0.cmp

	if (!f32 || !f64)
		return 2;

	if (pack_getc(f32) != 'o' || pack_getc(f32) != 'k' || pack_getc(f32) != 2)
		return 3;

	pack_fwrite("ok\002", 3, f64);
	convert_section(CMP_PFX_32, CMP_PFX_64, 0);
	int shard_count = ((int *)buf)[3];

	for (int i = 0; i < 33; ++i)
		convert_section(PLAYER_32, PLAYER_64, 2);

	for (int i = 0; i < shard_count; ++i)
	{
		convert_section(SHARD_32, SHARD_64, 3);
		convert_section(MASK_32, MASK_64, 3);
	}

	convert_section(TUTORIAL, TUTORIAL, 0);

	pack_fclose(f32);
	pack_fclose(f64);

	return 0;
}
