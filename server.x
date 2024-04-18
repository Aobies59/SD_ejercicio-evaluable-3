const VERNUM = 1;
const INITVER = 1;
const EXISTVER = 2;
const SETVER = 3;
const GETVER = 4;
const MODIFYVER = 5;
const DELETEVER = 6;

typedef double vector_32[32];

struct tuple {
    int key;
    string value1<256>;
    int N_value2 ;
    vector_32 V_value2;
};

program server {
    version VERNUM {
        void init_server() = INITVER;
        int exists(int key) = EXISTVER;
        void set_tuple(struct tuple given_tuple) = SETVER;
        struct tuple get_tuple(int key) = GETVER;
        void modify_tuple(struct tuple given_tuple) = MODIFYVER;
        void delete_tuple(int key) = DELETEVER;
    } = 1;
} = 1;
