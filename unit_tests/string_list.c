#include <stdio.h>

#include "../riggerd/forwards_data_types.h"
#include "../riggerd/forwards_data_types_tools.h"

int main(){
    printf("Hello, World!\n");

    struct string_list list;
    char* buf = "aaaa";

    string_list_init(&list);
    string_list_push_back(&list, buf, 4);
    string_list_push_back(&list, buf, 4);
    string_list_push_back(&list, buf, 4);

    for (struct string_entry* iter = list.first; NULL != iter; iter = iter->next) {
        printf("%s\n", iter->string);
    }

    string_list_clear(&list);

    return 0;
}
