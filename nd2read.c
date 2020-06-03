// nd2read.c
#include <stdio.h>
#include <regex.h>
#include <string.h>
int iter_match(int max_matches, regmatch_t *matches, char *string, char *expr);

/* 
 *  Function: iter_match
 *  --------------------
 *  Find up to *max_matches* separate instances of a regular
 *  expression in a long string.
 *
 *  Parameters
 *  ----------
 *       int max_matches        :   the maximum number of matches
 *                                  equal to the size of matches
 *       regmatch_t *matches    :   pointer to array of regmatch_t,
 *                                  to store results
 *       char *string           :   string to search 
 *       char *expr             :   the regular expression
 *
 *  Returns
 *  -------
 *       int n_matches, the number of matches identified
 *
*/
int iter_match(int max_matches, regmatch_t matches[max_matches], char *string, char *expr) {

    int return_value;
    int n_matches = 0;
    int str_size = strlen(string);
    regex_t regex;
    regoff_t offset;
    regoff_t match_end;

    // Compile the regular expression
    return_value = regcomp(&regex, expr, 0);
    if (return_value == 0) {
        printf("Compiled successfully\n");
    } else {
        printf("Compilation failed\n");
        return 0;
    }

    // Start the iteration by searching the string for the 
    // first instance
    return_value = regexec(&regex, string, 1, &matches[0], 0);
    if (return_value == 0) {
        n_matches++;
        offset = matches[0].rm_eo;
    } else {
        offset = 0; // unused
    }

    // Search the string iteratively until there are no more 
    // substrings to find
    while ((return_value==0) & (offset<str_size) & (n_matches<max_matches)) {
        return_value = regexec(&regex, string+offset, 1, &matches[n_matches], 0);
        if (return_value == 0) {
            match_end = matches[n_matches].rm_eo;
            matches[n_matches].rm_so += offset;
            matches[n_matches].rm_eo += offset;
            offset += match_end;
            n_matches++;
        }
    }

    // Print the results    
    printf("offset = %lld\n", offset);
    printf("n_matches = %d\n", n_matches);
    for (int i=0; i<n_matches; i++) {
        printf("MATCH:\n");
        printf("matches[%d].rm_so = %lld\n", i, matches[i].rm_so);
        printf("matches[%d].rm_eo = %lld\n", i, matches[i].rm_eo);
    }

    return n_matches;
}

