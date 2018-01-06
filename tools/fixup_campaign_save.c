#include <stdio.h>
#include <allegro.h>

#define MAX_SHARD_BONUS 5

enum bonus_types {
	EMPTY, GOLD_RESERVE, GEM_RESERVE, GOLD_INCOME, GEM_INCOME, ITEM, UNIT, BUILDING, CHEAPER_RESOURCE, GUARD_CONTRACT
};

struct Shard
{
	char shard_name[32]; int foo[8];
	struct shard_bonus { unsigned int type, val; } bonus[MAX_SHARD_BONUS];
	int bar[34];
};

struct Campaign
{
	int foo[3], iMaxShards, bar[37374];
};

static struct Campaign cmp;

int main(int argc, char **argv)
{
	int i, j;
	char buf[288];

	if (argc != 3) return 1;

	PACKFILE *in  = pack_fopen(argv[1], F_READ_PACKED);
	PACKFILE *out = pack_fopen(argv[2], F_WRITE_PACKED);

	if (!in || !out) return 2;

	if (pack_getc(in) != 'o' || pack_getc(in) != 'k' || pack_getc(in) != 2) return 3;

	pack_fread(&cmp, sizeof cmp, in);
	pack_fwrite("ok\002", 3, out);
	pack_fwrite(&cmp, sizeof cmp, out);

	for (i = 0; i < cmp.iMaxShards; ++i)
	{
		struct Shard shard;
		pack_fread(&shard, sizeof shard, in);
		printf("%-12s ", shard.shard_name);
		for (j = 0; j < MAX_SHARD_BONUS; ++j) {
			struct shard_bonus *bonus = &shard.bonus[j];
			printf("%u,%u ", bonus->type, bonus->val);
			if (bonus->type == EMPTY)
				bonus->val = 0; // FORCE VAL TO 0
			if (bonus->type == BUILDING && bonus->val > 20)
				bonus->val = 0; // NO BUILDING
		}
		putchar('\n');
		pack_fwrite(&shard, sizeof shard, out);
	}

	pack_fread(buf, sizeof buf, in);
	pack_fwrite(buf, sizeof buf, out);

	pack_fclose(in);
	pack_fclose(out);

	return 0;
}
