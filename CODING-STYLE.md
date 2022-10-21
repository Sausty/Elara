# The Elara project follows the following coding style.

## Misc

- Source and include files must start with a file header.
- Source and include files must named with the project name in the beginning (e.g. elara_xxx.c)
- Header guards must be done using this format: ELARA_FILENAME_H_
- Every scope must be indented using 4 spaces.

## Variables, functions, macros

Variables, functions and macros must be named using camel case. This applies to loop iterators.
```c
#define Double(Parameter) Parameter * 2

int MyFunction(float MyParameter)
{
    return Double(MyParameter);
}

int MyVariable = 5;
for (int Iterator = 0; Iterator < MyVariable; Iterator++) {
    ...
}
```

## Structs

Structs must be named using snake case. Struct members are named using camel case.
```c
typedef struct my_struct {
    int MyMember;
} my_struct;
```