#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define MAXPOINTERS	(1024)

int		alloc_only = 0;	/* alloc memory; don't paint it */
int		cpu = 0;	/* eat cpu when done allocing */
int		forks = 4;
size_t		maxmem = 4llu * 1024llu * 1024llu * 1024llu;
size_t		bs = 16384;
size_t		each = 0;

void
pain(int no)
{
	size_t		used = 0;
	void		*p = NULL;
	int		x = 4 /* debian! */, i, pc;
	double		d, *dp = NULL;
	void		*pp[MAXPOINTERS];
	int		rounds, printit = 1;

	printf("child %d [%d]: starting\n", no, getpid());

	bzero(pp, sizeof pp);

	for (pc = 0, rounds = 0;; pc++) {
		if (pc > MAXPOINTERS)
			pc = 0;

		if (used > each) {
			if (cpu) {
				printf("child %d [%d]: sleeping\n", no, getpid());
				sleep(60);
			} else {
				if (printit) {
					printf("child %d [%d]: omnomnom cpu\n",
					    no, getpid());
					printit = 0;
				}
				do {
					if (pc > MAXPOINTERS)
						pc = 0;
					p = dp = pp[pc++];
				} while (dp == NULL);

				/* do some math */
				for (i = 0; i < bs; i++) {
					x = arc4random();
					d = sqrt(3.1415 * sin(x) * log(cos(x)));
					d /= 3.3339;
					d += tgamma(d * hypot(x, d));
					d *= 6.6667;
					if ((void *)dp -(void *) p + sizeof(d) < bs)
						dp = p;
					*dp++ = d;
				}

				if (rounds > MAXPOINTERS) {
					printf("child %d [%d]: freeing %d "
					    "pointers\n",
					    no, getpid(), MAXPOINTERS);
					rounds = 0;
					printit = 1;
					for (i = 0; i < MAXPOINTERS; i++) {
						if (pp[i]) {
							free(pp[i]);
							pp[i] = NULL;
							used -= bs;
						}
					}
				} else
					rounds++;
			}
			continue;
		}

		p = malloc(bs);
		if (p == NULL) {
			fprintf(stderr, "memory exhausted at %zu, trying "
			    "again in a bit\n", used);
			sleep(arc4random() % 60 + 1);
			continue;
		} else {
			used += bs;
			if (alloc_only == 0) {
				x = arc4random();
				memset(p, x, bs);
			}
		}

		pp[pc++] = p;
	}
}

void
usage(void)
{
	extern char		*__progname;

	fprintf(stderr, "%s: [-ac][-b blocksize][-f no_forks][-m maxmem]\n",
	    __progname);
	exit(0);
}

int
main(int argc, char *argv[])
{
	int		c;
	pid_t		pid;

	while ((c = getopt(argc, argv, "ab:cf:m:")) != -1) {
		switch (c) {
		case 'a': /* alloc only */
			alloc_only = 1;
			break;
		case 'b':
			bs = atoi(optarg);
			if (bs < 1)
				errx(1, "invalid block size");
			break;
		case 'c':
			cpu = 1;
			break;
		case 'f':
			forks = atoi(optarg);
			break;
		case 'm':
			maxmem = atoll(optarg);
			if (maxmem < 1024)
				errx(1, "invalid maxmem");
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	}

	each = maxmem / forks;
	if (each < 1024)
		errx(1, "each for doesn't have enough memory");

	printf("each process will consume: %zu\n", each);

	for (c = 0; c < forks; c++) {
		switch (pid = fork()) {
		case -1:
			err(1, "can not fork");
			break;
		case 0: /* child */
			pain(c);
			_exit(0);
			break;
		}
	}

	for (;;) {
		printf("sleeping\n");
		sleep(60);
	}


	return (0);
}
