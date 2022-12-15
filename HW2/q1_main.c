
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define Q1_BUFFER_SIZE 40


int has_char(char *str, char ch) {

    char *chr = strchr(str, ch);

    if (chr == NULL)
        return 0;

    return 1;

}

int str_size(char *str)
{
    int offset = 0;
    int count = 0;

    while (*(str + offset) != '\0' && *(str + offset) != '\n')
    {
        ++count;
        ++offset;
    }
    return count;
}

char* get_char(char *str, char ch) {

    int size = str_size(str);

    for (int i = 0; i < size; i++) {
        if (str[i] == ch)
            return &str[i];
    }

    return NULL;
}

char* slice_string(char *str, int start_pos, int end_pos, char *buffer) {
    char *start = &str[start_pos];

    for (int i = 0; i < end_pos - start_pos; i++)
    {
        buffer[i] = start[i];
    }

    buffer[end_pos - start_pos] = '\0';

    return buffer;
    
    
}

 void ltoa(long n, char s[])
 {
     long i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}

void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = str_size(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
} 

void print_error(const char * err) {
    fputs(err, stderr);
    exit(-1);
}

long parse_number(char *str) {
    return atol(str);
}

int match_str(char *str1, char *str2) {
    int diff = str_size(str2) - str_size(str1) + 1;
    char * split=strtok(str1,"#");
    int index1 = 0;
    int index2 = 0;
    if (str1[0] == '#') {
        index1 = 1;
        index2 += diff;
    }
    while(split != NULL)
    {
        char* word=split;
        int size = str_size(split);
        for (int i = 0; i < size; i++) {
            if (str1[index1] != str2[index2])
                return 0;
            index1++;
            index2++;
        }
        split=strtok(NULL,"#");
        index1++;
        index2 += diff;
    }

    return 1;
}

int main() {
    char buffer[Q1_BUFFER_SIZE];

    memset(buffer, '\0', Q1_BUFFER_SIZE);

    fgets(buffer, Q1_BUFFER_SIZE, stdin);

    char *add_sign = get_char(buffer, '+');
    char *eq_sign = get_char(buffer, '=');

    if (add_sign == NULL || eq_sign == NULL) {
        print_error("input is wrong\0");
    }

    int first_number_length = (int)(add_sign - buffer) - 1;
    char first_number[first_number_length + 1];
    slice_string(buffer, 0, first_number_length, first_number);

    int second_number_start = first_number_length + 3;
    int second_number_end = (int)(eq_sign - buffer) - 1;
    char second_number[second_number_end - second_number_start + 1];
    slice_string(buffer, second_number_start, second_number_end, second_number);

    int third_number_start = second_number_end + 3;
    char third_number[str_size(buffer) - third_number_start + 1];
    slice_string(buffer, third_number_start, str_size(buffer), third_number);

    if (has_char(first_number, '#')) {
        long sn = parse_number(second_number);
        long tn = parse_number(third_number);
        long fn = tn - sn;
        char fnc[11];
        ltoa(fn, fnc);
        if (match_str(first_number, fnc)) {
            printf("%s + %s = %s\n", fnc, second_number, third_number);
        }
        else {
            printf("-1\n");
        }
    } else if (has_char(second_number, '#')) {
        long fn = parse_number(first_number);
        long tn = parse_number(third_number);
        long sn = tn - fn;
        char snc[11];
        ltoa(sn, snc);
        if (match_str(second_number, snc)) {
            printf("%s + %s = %s\n", first_number, snc, third_number);
        }
        else {
            printf("-1\n");
        }
    } else {
        long sn = parse_number(second_number);
        long fn = parse_number(first_number);
        long tn = sn + fn;
        char tnc[11];
        ltoa(tn, tnc);
        if (match_str(third_number, tnc)) {
            printf("%s + %s = %s\n", first_number, second_number, tnc);
        }
        else {
            printf("-1\n");
        }
    }
}