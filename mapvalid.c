#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "dobutsu.h"

/* create a prototype for the database: every valid position gets a 1,
 * every invalid position stays at 0.
 */

extern int
main(int argc, char *argv[])
{
	pos_code i, invalid = 0, sente = 0, gote = 0, other;
	struct position p;
	int fd, ret, pos;
	unsigned char *map;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.db\n", argv[0]);
		return (1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT, 0777);
	if (fd == -1) {
		perror("Cannot open database");
		return (1);
	}

	if (ftruncate(fd, MAX_POS) == -1) {
		perror("Cannot truncate database");
		close(fd);
		return (1);
	}

	map = mmap(NULL, MAX_POS, PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		perror("Cannot map file");
		close(fd);
		return (1);
	}

	for (i = 0; i < MAX_POS; i++) {
		if (i % 10000000 == 0)
			fprintf(stderr, "\r%10u", i);

		switch (pos = decode_pos(&p, i)) {
		case POS_INVALID:
			invalid++;
			break;

		case POS_SENTE:
			sente++;
			break;

		case POS_GOTE:
			gote++;
			break;

		default:
			;
		}

		map[i] = pos;
	}

	fprintf(stderr, "\r%10u\n\n", i);

	fprintf(stderr, "invalid: %10u (%5.2f%%)\n", invalid, (100.0*invalid)/i);
	fprintf(stderr, "won:     %10u (%5.2f%%)\n", sente,   (100.0*sente  )/i);
	fprintf(stderr, "lost:    %10u (%5.2f%%)\n", gote,    (100.0*gote   )/i);
	other = i - invalid - sente - gote;
	fprintf(stderr, "other:   %10u (%5.2f%%)\n", other,   (100.0*other  )/i);

	munmap(map, MAX_POS);
	close(fd);

	return (0);
}
