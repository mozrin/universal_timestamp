#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <universal_timestamp.h>

void print_help(const char* prog) {
    printf("Usage: %s <command> [args]\n", prog);
    printf("Commands:\n");
    printf("  now               Print current UTC timestamp (ISO-8601)\n");
    printf("  now-nanos         Print current UTC timestamp (nanoseconds)\n");
    printf("  parse <str>       Parse ISO-8601 string to nanoseconds\n");
    printf("  format <nanos>    Format nanoseconds to ISO-8601 string\n");
    printf("  version           Print library version (requires lib update, using 0.9.0)\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    const char* cmd = argv[1];

    if (strcmp(cmd, "now") == 0) {
        ut_timestamp_t ts = ut_now();
        char buf[64];
        ut_format(ts, buf, sizeof(buf), 1);
        printf("%s\n", buf);
    } else if (strcmp(cmd, "now-nanos") == 0) {
        ut_timestamp_t ts = ut_now();
        printf("%ld\n", ut_to_unix_nanos(ts));
    } else if (strcmp(cmd, "parse") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: missing timestamp string\n");
            return 1;
        }
        ut_timestamp_t ts;
        if (ut_parse_lenient(argv[2], &ts) != UT_OK) {
            fprintf(stderr, "Error: invalid timestamp\n");
            return 1;
        }
        printf("%ld\n", ut_to_unix_nanos(ts));
    } else if (strcmp(cmd, "format") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: missing nanoseconds\n");
            return 1;
        }
        long long nanos = atoll(argv[2]);
        ut_timestamp_t ts = ut_from_unix_nanos(nanos);
        char buf[64];
        ut_format(ts, buf, sizeof(buf), 1);
        printf("%s\n", buf);
    } else if (strcmp(cmd, "version") == 0) {
        printf("0.9.0\n");
    } else {
        print_help(argv[0]);
        return 1;
    }

    return 0;
}
