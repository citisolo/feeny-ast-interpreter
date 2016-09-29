#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

#define check_ptr(ptr) \
        if (ptr == NULL) {  \
            (void)fprintf(stderr, "ERROR: Malloc failed"); \
            (void)exit(EXIT_FAILURE); }   /* or return EXIT_FAILURE; */
        
         

typedef enum {
	NULL_TYPE, 
	INT_TYPE, 
	OBJ_TYPE, 
	ARRAY_TYPE,
	ENV_OBJ,
	ENTRY,
	CODE,
	DATA 
	}Type;

typedef struct DataEntry DataEntry;
typedef struct NullObj NullObj;
typedef struct IntObj IntObj;
typedef struct CodeEntry CodeEntry;
typedef struct ArrayObj ArrayObj;
typedef struct EnvObj EnvObj;
typedef struct Entry Entry;
typedef struct EntryDescriptor  EntryDescriptor ;
typedef struct Parameter Parameter;
typedef struct Obj Obj;

struct NullObj {
    Type obj_type; 
	
};

struct IntObj {
	Type obj_type;
	int value;
	
};

union Value{
		IntObj* num;
		NullObj* null;
		CodeEntry* code;
		DataEntry* data;
		EnvObj* env;
		ArrayObj* arr;
	} ;
	
struct Obj{
	Type obj_type;
	Type tag;
	char* name;
    union Value value;
};

struct ArrayObj {
	Type obj_type;
	int length;
	Obj** elems;
	
};

struct EnvObj {
	Type obj_type;
	Obj* parent;
	EntryDescriptor* head;
	EntryDescriptor* tail;
};

struct Entry{
	Type obj_type;
    Obj* object;
};

struct EntryDescriptor{
	char* key;
	Entry* val;
	EntryDescriptor* next;
};

struct Parameter  {
    Obj* value;
    Obj* next;
};

struct  CodeEntry{
	Type obj_type;
	char** args;
	int nargs;
	char* name;
	ScopeStmt* stmt;
};

struct DataEntry{
	Type obj_type;
	char* name;
	Obj* val;
};

int obj_type (Obj* o);
NullObj* make_null_obj ();

/*Integer Objects*/
IntObj* make_int_obj (int value);
IntObj* add (IntObj* x, IntObj* y);
IntObj* sub (IntObj* x, IntObj* y);
IntObj* mul (IntObj* x, IntObj* y);
IntObj* divd (IntObj* x, IntObj* y);
IntObj* mod (IntObj* x, IntObj* y);
Obj* eq (IntObj* x, IntObj* y);
Obj* lt (IntObj* x, IntObj* y);
Obj* le (IntObj* x, IntObj* y);
Obj* gt (IntObj* x, IntObj* y);
Obj* ge (IntObj* x, IntObj* y);


/*Array objects */
ArrayObj* make_array_obj (int length);
IntObj* array_length (ArrayObj* a);
NullObj* array_set (ArrayObj* a, IntObj* i, Obj* v);
Obj* array_get (ArrayObj* a, IntObj* i);


/*Environment Objects */
EnvObj* make_env_obj (Obj* parent);
void add_entry (EnvObj* env, char* name,  Entry* entry);
Entry* get_entry (EnvObj* env, char* name);
Entry* make_entry( Obj* obj);

Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* e);

void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s );
Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s );

void interpret (ScopeStmt* stmt);

#endif
