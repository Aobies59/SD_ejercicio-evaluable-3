#include "server.h"
char *tuples_filename = "tuples.csv";

bool_t
init_server_1_svc(void *result, struct svc_req *rqstp)
{
    printf("Received INIT call\n");
	FILE *file = fopen(tuples_filename, "w");
    if (file == NULL) {
        fclose(file);
        perror("fopen");
        return FALSE;
    }
    fclose(file);
    printf("INIT call completed successfully\n");
    return TRUE;
}

// to be used by other functions that used exists() before
static int
key_exists(int key)
{
    const long MAXLINE = 4096;  // big enough number that endofline will ocurr before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    if (tuples_file == NULL) {
        perror("fopen");
        return -1;
    }
    char line[MAXLINE];
    int tuple_key;
    while (fgets(line, sizeof(line), tuples_file) != NULL) {
        int items = sscanf(line, "%d", &tuple_key);
        if (items != 1) {
            fprintf(stderr, "Error parsing line: %s\n", line);
            fclose(tuples_file);
            return -1;
        }
        if (tuple_key == key) {
            fclose(tuples_file);
            return 1;
        }
    }
    fclose(tuples_file);
    return 0;
}

bool_t
exists_1_svc(int key, int *result,  struct svc_req *rqstp)
{
    printf("Received EXISTS call\n");
    *result = key_exists(key);
    if (*result < 0) {
        return FALSE;
    }
    printf("EXISTS call completed successfully\n");
    return TRUE;
}

bool_t
set_tuple_1_svc(struct tuple given_tuple, int *result,  struct svc_req *rqstp)
{
    printf("Received SET call\n");
    // check if N_value2 is between 1 and 32
    if (given_tuple.N_value2 < 1 || given_tuple.N_value2 > 32) {
        fprintf(stderr, "Error in input parameters\n");
        return FALSE;
    }

    // check if key exists
    int key_exists_return_value = key_exists(given_tuple.key);
    if (key_exists_return_value == 1) {
        *result = -999;
        printf("SET call completed successfully\n");
        return TRUE;
    } else if (key_exists_return_value < 0) {
        return FALSE;
    }

    // write the new tuple at the end of the file
    FILE *tuples_file = fopen(tuples_filename, "a");
    if (tuples_file == NULL) {
        perror("fopen");
        return FALSE;
    }
    fprintf(tuples_file, "%d,%s,%d", given_tuple.key, given_tuple.value1, given_tuple.N_value2);
    for (int i = 0; i < given_tuple.N_value2; i++) {
        fprintf(tuples_file, ",%lf", given_tuple.V_value2[i]);
    }
    fprintf(tuples_file, "\n");
    fclose(tuples_file);
    printf("SET call completed successfully\n");
    return TRUE;
}

bool_t
get_tuple_1_svc(int key, struct tuple *result,  struct svc_req *rqstp)
{
    printf("Received GET call\n");
    if (key_exists(key) == 0) {
        result->key = -999;
        printf("GET call completed successfully\n");
        return TRUE;
    }
	result->key = key;
    const long MAXLINE = 4096;  // big enough number that endofline will occur before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    if (tuples_file == NULL) {
        perror("fopen");
        return FALSE;
    }
    char line[MAXLINE];
	int line_key;
    while (fgets(line, MAXLINE, tuples_file) != NULL) {
        char str_value[MAXLINE];
        sscanf(line, "%d, %s", &line_key, str_value);
        result->value1 = strtok(str_value, ",");
        result->N_value2 = atoi(strtok(NULL, ","));
        for (int i = 0; i < result->N_value2; i++) {
            result->V_value2[i] = atof(strtok(NULL, ","));
        }
        if (line_key == key) {
            for (int i = 0; i < result->N_value2; i++) {
                result->V_value2[i] = result->V_value2[i];
            }
            fclose(tuples_file);
            printf("GET call completed successfully\n");
            return TRUE;
        }
    }
    fclose(tuples_file);
    fprintf(stderr, "Error: existing key not found in file\n");
    return FALSE;
}

bool_t
modify_tuple_1_svc(struct tuple given_tuple, int *result,  struct svc_req *rqstp)
{
    printf("Received MODIFY call\n");
    if (key_exists(given_tuple.key) == 0) {
        *result = -999;
        printf("MODIFY call completed successfully\n");
        return TRUE;
    }

    const long MAXLINE = 4096;  // big enough number that endofline will occur before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    FILE *temp_tuples_file = fopen("temp.csv", "w");
    if (tuples_file == NULL) {
        perror("fopen");
        return FALSE;
    }
    char *line = malloc(MAXLINE * sizeof(char));    
    line = fgets(line, MAXLINE, tuples_file);
    int read_key;
    while (line != NULL) {
        sscanf(line, "%d", &read_key);
        if (read_key != given_tuple.key) {
            fputs(line, temp_tuples_file);
        } else {
            // write the new tuple to the temp file
            fprintf(temp_tuples_file, "%d,%s,%d", given_tuple.key, given_tuple.value1, given_tuple.N_value2);
            for (int i = 0; i < given_tuple.N_value2; i++) {
                fprintf(temp_tuples_file, ",%lf", given_tuple.V_value2[i]);
            }
            fprintf(temp_tuples_file, "\n");
        }
        line = fgets(line, MAXLINE, tuples_file);
    }
    free(line);
    fclose(tuples_file);
    fclose(temp_tuples_file);
    // remove original file and replace it with temp file
    remove(tuples_filename);
    rename("temp.csv", tuples_filename);

    printf("MODIFY call completed successfully\n");
    return TRUE;
}

bool_t
delete_tuple_1_svc(int key, int *result,  struct svc_req *rqstp)
{
    printf("Received DELETE call\n");
    if (key_exists(key) == 0) {
        *result = -999;
        printf("DELETE call completed successfully\n");
        return TRUE;
    }

    const long MAXLINE = 4096;  // big enough number that endofline will occur before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    FILE *temp_tuples_file = fopen("temp.csv", "w");
    if (tuples_file == NULL) {
        perror("fopen");
        return FALSE;
    }
    char line[MAXLINE];
    int read_key;
    while (fgets(line, MAXLINE, tuples_file) != NULL) {
        sscanf(line, "%d", &read_key);
        if (read_key != key) {
            // write the read line to the temp file
            fputs(line, temp_tuples_file);
        }
    }
    fclose(tuples_file);
    fclose(temp_tuples_file);
    // remove original file and replace it with temp file
    remove(tuples_filename);
    rename("temp.csv", tuples_filename);
    printf("DELETE call completed successfully\n");
    return TRUE;
}

bool_t
close_server_1_svc (void *result, struct svc_req *rqstp)
{
    exit(0);
    return TRUE;
}

bool_t
server_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
    // commented this line because it results in a segmentation fault
    // I souldn't need to free the result, it is an int
	//xdr_free (xdr_result, result);
	return 1;
}
