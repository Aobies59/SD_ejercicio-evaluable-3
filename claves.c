#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "server.h"


CLIENT *clnt;
enum clnt_stat retval_1;
void *result_1;
enum clnt_stat retval_2;
int result_2;
int exists_1_key;
enum clnt_stat retval_3;
void *result_3;
struct tuple  set_value_1_given_tuple;
enum clnt_stat retval_4;
struct tuple result_4;
int get_value_1_key;
enum clnt_stat retval_5;
void *result_5;
struct tuple  modify_value_1_given_tuple;
enum clnt_stat retval_6;
void *result_6;
int delete_key_1_key;

int start_service(char *host) {
    clnt = clnt_create (host, server, VERNUM, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		return -1;
	}
    return 0;
}

int init () {
	bool_t retval_1 = init_server_1(&result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
        return -1;
	}
    return 0;
}

int set_value(int key, char *value1, int N_value2, vector_32 V_value2) {
	printf("DEBUGGING POINT 1\n");
	set_value_1_given_tuple.key = key;
	set_value_1_given_tuple.value1 = value1;
	printf("DEBUGGING POINT 2\n");
	set_value_1_given_tuple.N_value2 = N_value2;
	memcpy(set_value_1_given_tuple.V_value2, V_value2, N_value2 * sizeof(double));
	printf("DEBUGGING POINT 3\n");
	retval_3 = set_tuple_1(set_value_1_given_tuple, &result_3, clnt);
	printf("DEBUGGING POINT 4\n");
	if (retval_3 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
		printf("DEBUGGING POINT 5A\n");
        return -1;
	}
	printf("DEBUGGING POINT 5B\n");
    return 0;
}

int get_value(int key, char *value1, int *N_value2, vector_32 V_value2) {
	printf("DEBUGGING POINT 1\n");
	retval_4 = get_tuple_1(get_value_1_key, &result_4, clnt);
	printf("DEBUGGING POINT 2\n");
	if (retval_4 != RPC_SUCCESS) {
		printf("DEBUGGING POINT 3A\n");
		clnt_perror (clnt, "call failed");
        return -1;
	}

	printf("DEBUGGING POINT 3B\n");
	strcpy(value1, result_4.value1);
	*N_value2 = result_4.N_value2;
	memcpy(V_value2, result_4.V_value2, *N_value2 * sizeof(double));
	printf("DEBUGGING POINT 4\n");
    return 0;
}

int modify_value(int key, char *value1, int N_value2, vector_32 V_value2) {
	retval_5 = modify_tuple_1(modify_value_1_given_tuple, &result_5, clnt);
	if (retval_5 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
        return -1;
	}

    return 0;
}

int delete_key(int key) {
	retval_6 = delete_tuple_1(delete_key_1_key, &result_6, clnt);
	if (retval_6 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
        return -1;
	}

    return 0;
}

int exist(int key) {
	retval_2 = exists_1(exists_1_key, &result_2, clnt);
	if (retval_2 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
        return -1;
	}
    return result_2;
}

int close_server() {
    clnt_destroy (clnt);
    return 0;
}
