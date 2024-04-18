const VERNUM = 1;

typedef double vector_32[32];

struct tuple {
    int key;
    string value1<256>;
    int N_value2 ;
    vector_32 V_value2;
};

program server {
    version VERNUM {
        void init() = 1;
        int exists(int key) = 2;
        void set_value(struct tuple given_tuple) = 3;
        struct tuple get_value(int key) = 4;
        void modify_value(struct tuple given_tuple) = 5;
        void delete_key(int key) = 6;
    } = 1;
} = 1;
