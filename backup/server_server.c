/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "server.h"
char *tuples_filename = "tuples.csv";

bool_t
init_1_svc(void *result, struct svc_req *rqstp)
{
	FILE *file = fopen(tuples_filename, "w");
    if (file == NULL) {
        fclose(file);
        return FALSE;
    }
    fclose(file);
    return TRUE;
}

bool_t
exists_1_svc(int key, int *result,  struct svc_req *rqstp)
{
    const long MAXLINE = 4096;  // big enough number that endofline will ocurr before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    if (tuples_file == NULL) {
        fclose(tuples_file);
        *result = -1;
        return FALSE;
    }
    char *line = malloc(MAXLINE * sizeof(char));    
    line = fgets(line, MAXLINE, tuples_file);
    int tuple_key;
    while (line != NULL) {
        int items = sscanf(line, "%d", &tuple_key);
        if (items != 1) {
            fclose(tuples_file);
            *result = 0;
            return TRUE;
        }
        if (tuple_key == key) {
            fclose(tuples_file);
            *result = 1;
            return TRUE;
        }
        line = fgets(line, MAXLINE, tuples_file);
    }
    free(line);
    fclose(tuples_file);
    return TRUE;
}

bool_t
set_value_1_svc(struct tuple given_tuple, void *result,  struct svc_req *rqstp)
{
    // check if N_value2 is between 1 and 32
    if (given_tuple.N_value2 < 1 || given_tuple.N_value2 > 32) {
        return FALSE;
    }

    // check if key exists
    if (exists(given_tuple.key) == 1) {
        return FALSE;
    }

    // write the new tuple at the end of the file
    FILE *tuples_file = fopen(tuples_filename, "a");
    if (tuples_file == NULL) {
        fclose(tuples_file);
        return FALSE;
    }
    fprintf(tuples_file, "%d,%s,%d", given_tuple.key, given_tuple.value1, given_tuple.N_value2);
    for (int i = 0; i < given_tuple.N_value2; i++) {
        fprintf(tuples_file, ",%lf", given_tuple.V_value2[i]);
    }
    fprintf(tuples_file, "\n");
    fclose(tuples_file);
    return TRUE;
}

bool_t
get_value_1_svc(int key, struct tuple *result,  struct svc_req *rqstp)
{
    if (exists(key) == 0) {
        return FALSE;
    }
	result->key = key;
    const long MAXLINE = 4096;  // big enough number that endofline will occur before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    if (tuples_file == NULL) {
        perror("fopen");
        fclose(tuples_file);
        return FALSE;
    }
    char *line = malloc(MAXLINE * sizeof(char));    
    line = fgets(line, MAXLINE, tuples_file);
	int line_key;
    while (line != NULL) {
        char str_value[MAXLINE];
        sscanf(line, "%d, %s", &line_key, str_value);
        strcpy(result->value1, strtok(str_value, ","));
        result->N_value2 = atoi(strtok(NULL, ","));
        for (int i = 0; i < result->N_value2; i++) {
            result->V_value2[i] = atof(strtok(NULL, ","));
        }
        if (line_key == key) {
            for (int i = 0; i < result->N_value2; i++) {
                result->V_value2[i] = result->V_value2[i];
            }
            free(line);
            fclose(tuples_file);
            return TRUE;
        }
        line = fgets(line, MAXLINE, tuples_file);
    }
    free(line);
    fclose(tuples_file);
    return FALSE;
}

bool_t
modify_value_1_svc(struct tuple given_tuple, void *result,  struct svc_req *rqstp)
{
    const long MAXLINE = 4096;  // big enough number that endofline will occur before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    FILE *temp_tuples_file = fopen("temp.csv", "w");
    if (tuples_file == NULL) {
        fclose(tuples_file);
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
    return TRUE;
}

bool_t
delete_key_1_svc(int key, void *result,  struct svc_req *rqstp)
{
    const long MAXLINE = 4096;  // big enough number that endofline will occur before end of buffer
    FILE *tuples_file = fopen(tuples_filename, "r");
    FILE *temp_tuples_file = fopen("temp.csv", "w");
    if (tuples_file == NULL) {
        fclose(tuples_file);
        return FALSE;
    }
    char *line = malloc(MAXLINE * sizeof(char));    
    line = fgets(line, MAXLINE, tuples_file);
    int read_key;
    while (line != NULL) {
        sscanf(line, "%d", &read_key);
        if (read_key != key) {
            // write the read line to the temp file
            fputs(line, temp_tuples_file);
        }
        line = fgets(line, MAXLINE, tuples_file);
    }
    free(line);
    fclose(tuples_file);
    fclose(temp_tuples_file);
    // remove original file and replace it with temp file
    remove(tuples_filename);
    rename("temp.csv", tuples_filename);
    return TRUE;
}

int
server_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	return 1;
}