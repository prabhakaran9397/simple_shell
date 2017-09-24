#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <autocorrect.h>
#define MAX_CMD_SIZE 64

int levenshtein(const char *s, int ls, const char *t, int lt);
int min(int x, int y, int z);

char* best_match(char* wrong_command) {
    FILE* fp;
    char command[MAX_CMD_SIZE];
    int min_dist = 5; //Min dist for the match to be logical
    char* match = malloc(MAX_CMD_SIZE * sizeof(char));
    int match_found = 0;
  
    /* Open the command for reading. */
    fp = popen("bash -c 'compgen -c'", "r");

    if (fp == NULL) {
        exit(1);
    }
  
    /* Read the output a line at a time - output it. */
    while (fgets(command, sizeof(command)-1, fp) != NULL) {
        command[strcspn(command, "\n")] = 0;
        int cur = levenshtein(wrong_command, strlen(wrong_command), command, strlen(command));

        /*
            Need to improve the match with a better logic with priority
        */
        if(cur < min_dist) {
            match_found = 1;
            min_dist = cur;
            strcpy(match, command);
        }
    }
    /* close */
    pclose(fp);
    if(!match_found) {
        return NULL;
    }
    return match;
    /* find the best matching command */
}

int levenshtein(const char *s, int m, const char *t, int n) {
    int dp[m+1][n+1], i, j;
    
       // Fill d[][] in bottom up manner
       for (i=0; i<=m; i++)
       {
           for (j=0; j<=n; j++)
           {
               // If first string is empty, only option is to
               // isnert all characters of second string
               if (i==0)
                   dp[i][j] = j;  // Min. operations = j
    
               // If second string is empty, only option is to
               // remove all characters of second string
               else if (j==0)
                   dp[i][j] = i; // Min. operations = i
    
               // If last characters are same, ignore last char
               // and recur for remaining string
               else if (s[i-1] == t[j-1])
                   dp[i][j] = dp[i-1][j-1];
    
               // If last character are different, consider all
               // possibilities and find minimum
               else
                   dp[i][j] = 1 + min(dp[i][j-1],  // Insert
                                      dp[i-1][j],  // Remove
                                      dp[i-1][j-1]); // Replace
           }
       }
    
       return dp[m][n];
}

int min(int x, int y, int z) {
    if((x < y)&&(x < z))
        return x;
    else {
        if(y < z)
            return y;
        else
            return z;
   }
}
