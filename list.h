#pragma once
struct list_header_t {
    struct list_header_t *next;
};
#define make_list(type_without_struct_enum_or_union, type) \
struct type_without_struct_enum_or_union##_list_t { \
    struct list_header_t *next; \
    type value; \
}

#define make_find_by_str(type, type_without_struct_enum_or_union, str_in_value) \
type *find_in_##type_without_struct_enum_or_union##_list_by_##str(type##_list_t *list, char *str_in_value) { \
    for(type##_list_t *cur = list; cur != NULL; cur = (type##_list_t *)cur->next) { \
        if(strcmp(cur->value.str_in_value, name) == 0) { \
            return &cur->value; \
        } \
    } \
    return NULL; \
}

#define make_add(type_without_struct_enum_or_union, type) \
void add_##type_without_struct_enum_or_union(type##_list_t **list, type value) { \
    type##_list_t *new_list = malloc(sizeof(type##_list_t)); \
    new_list->value = value; \
    new_list->next = (struct list_header_t*)*list; \
    *list = new_list; \
}

#define make_free(type_without_struct_enum_or_union, type) \
void free_##type_without_struct_enum_or_union##_list(type##_list_t *list) { \
    for(type##_list_t *cur = list; cur != NULL; ) { \
        type##_list_t *next = (type##_list_t *)cur->next; \
        free(cur); \
        cur = next; \
    } \
}
