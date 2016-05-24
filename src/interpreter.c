#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "ast.h"
#include "interpreter.h"


	      




int obj_type (Obj* o) {
	return o->obj_type;
}

NullObj* make_null_obj (){
	NullObj* null;
	null = (NullObj* ) malloc (sizeof(NullObj)); 
	null->obj_type = NULL_TYPE;
	return null;
}

IntObj* make_int_obj (int value){
	IntObj* obj;
	obj = (IntObj*) malloc (sizeof(IntObj));
	obj->obj_type = INT_TYPE;
	obj->value = value;
	return obj;	
}

IntObj* add (IntObj* x, IntObj* y){
	IntObj* res;
	res = (IntObj*) malloc (sizeof(IntObj));
	res->value = x->value + y->value;
	return res;	
}
IntObj* sub (IntObj* x, IntObj* y){
	IntObj* res;
	res = (IntObj*) malloc (sizeof(IntObj));
	res->value = x->value - y->value;
	return res;	
}

Obj* lt (IntObj* x, IntObj* y){
   Obj* res;
   res = (Obj*) malloc (sizeof(Obj));
   if ( x->value < y->value ){
	   res->tag = INT_TYPE;
	   res->value.num = make_int_obj(0);
   }
	else{
	   res->tag = NULL_TYPE;
	   res->value.null = make_null_obj();
   }
	return res;	
}
Obj* le (IntObj* x, IntObj* y){
   Obj* res;
   res = (Obj*) malloc (sizeof(Obj));
   if ( x->value <= y->value ){
	   res->tag = INT_TYPE;
	   res->value.num = (IntObj*)make_int_obj(0);
   }
	else{
	   res->tag = NULL_TYPE;
	   res->value.null = (NullObj*)make_null_obj();
   }
	return res;	
}

Obj* make_var_object(char* name, Type tag, void* value){
   Obj* res;
   res = (Obj*) malloc (sizeof(Obj));
   res->tag = tag;
   res->name = name;
   switch(tag){
	   case INT_TYPE:
	     res->value.num = (IntObj*)value;
	     break;
	   case NULL_TYPE:
	     res->value.null = (NullObj*)value;
	     break;
	   case CODE:
	     res->value.code = (CodeEntry*)value;
	     break;
	   case DATA:
		 res->value.data = (DataEntry*)value;
		 break;
	   case ENV_OBJ:
		 res->value.env = (EnvObj*)value;
		 break;
	   default:
          printf("Unrecognized object with tag %d\n", tag);
          exit(-1);
   }
   
   return res;
}

Obj* gt (IntObj* x, IntObj* y){
   Obj* res;
   res = (Obj*) malloc (sizeof(Obj));
   if ( x->value > y->value ){
	   res->tag = INT_TYPE;
	   res->value.num = make_int_obj(0);
   }
	else{
	   res->tag = NULL_TYPE;
	   res->value.null = make_null_obj();
   }
	return res;		
}

ArrayObj* make_array_obj (int length){
	ArrayObj* arr ;
	arr = (ArrayObj*) malloc(sizeof(ArrayObj));
	arr->obj_type = ARRAY_TYPE;
	arr->length = length;
	arr->elems = (Obj**) malloc(length * sizeof(Obj*));	
	return arr;
}

IntObj* array_length (ArrayObj* a){
	return make_int_obj(a->length);
}

NullObj* array_set (ArrayObj* a, IntObj* i, Obj* v){
	a->elems[i->value] = v;
	return make_null_obj();
}


Obj* array_get (ArrayObj* a, IntObj* i){
	return a->elems[i->value];
}

/*Environment Objects */
EnvObj* make_env_obj (Obj* parent){
	EnvObj* env = (EnvObj*) malloc(sizeof(EnvObj));
	env->obj_type = ENV_OBJ;
	env->parent = parent;
	return env;
}

Obj* get_entry_obj(EnvObj* env, char* name){
  Entry* e = get_entry(env, name); 
  if (e == NULL ){ 
	 printf("reference to undefined variable \'%s\'  \n", name); 
	 exit(-1);
  }
  return e->object;
}
void add_entry (EnvObj* env, char* name,  Entry* entry){
   EntryDescriptor* desc = (EntryDescriptor*) malloc(sizeof(EntryDescriptor));
   desc->key = name;
   desc->val = entry;
   if(env->head == NULL){
	   env->head = desc;
   }else{
	 EntryDescriptor* node = env->head;  /*start at the first node*/
	 while(node->next != NULL){/*go to the end*/
	   node = node->next;
     }
     node->next = desc; 
   }
   
	
}


Entry* get_entry (EnvObj* env, char* name){
	
	EntryDescriptor* desc = env->head;
	Entry* entry = NULL;
	while(desc != NULL){
		if(strcmp(desc->key, name)== 0){
		    entry = desc->val;
		    return entry;
		}
		desc = desc->next;
	}

	return entry;
}
 /* INT_EXP,
  NULL_EXP,
  PRINTF_EXP,
  ARRAY_EXP,
  OBJECT_EXP,
  SLOT_EXP,
  SET_SLOT_EXP,
  CALL_SLOT_EXP,
  CALL_EXP,
  SET_EXP,
  IF_EXP,
  WHILE_EXP,
  REF_EXP,*/

Entry* make_entry(Type type, Obj* obj){
	Entry* entry = (Entry*)malloc(sizeof(Entry));
	entry->obj_type = type;
	entry->object = obj;
	return entry;
}
  
Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* e){
  
  Obj* res =  (Obj*) malloc(sizeof(Obj));
  res->obj_type = OBJ_TYPE;
  switch(e->tag){
  case INT_EXP:{
    IntExp* e2 = (IntExp*)e;
    res->tag = INT_TYPE;
    res->value.num = make_int_obj(e2->value);
    break;
  }
  case NULL_EXP:{
	res->tag = NULL_TYPE;
    res->value.null = make_null_obj();
    break;
  }
  case PRINTF_EXP:{
    PrintfExp* e2 = (PrintfExp*)e;
    Obj* args[e2->nexps];
    
    for(int i=0; i<e2->nexps; i++){
      /*printf(", ");
      print_exp(e2->exps[i]);*/
      /*evaluate the expression to semantic object*/
      args[i] = eval_exp(genv, env, e2->exps[i]);
    }
    size_t size = strlen(e2->format);
    char* buf = malloc((size * sizeof(char))+1);
    memset(buf, '\0', sizeof(char)*size );
    char* c = e2->format;
    /* iterate through char array*/
    int j,k ;
    for(int i=0,j=0, k=0 ; i<size; i++){
		/*if we reached a tilde stop and print arg*/
	    if(c[i] == '~'){
			printf("%s",buf);
			Obj* arg = args[j];
			switch(arg->tag){
				case INT_TYPE:{
					int num = arg->value.num->value;
					printf("%d",num);
					break;					
				}
				case OBJ_TYPE:{
					break;
				}
				case NULL_TYPE:{
					printf("<null>");
				}
				default:
                    printf("Unrecognized value with tag %d\n", arg->tag);
                    exit(-1);
			}
			memset( buf, '\0', sizeof(char)*size );
			j++; /*increment arg  index*/
			k=0; /*reset buffer index*/
		}
		else if (i == size-1) {
			buf[k] = c[i];
			buf[k+1] = '\0';
			printf("%s\n",buf);
			memset(buf, '\0', sizeof(char)*size );
			k=0;
		}
		else{
		   buf[k]= c[i];
		   k++ ;
	   }
	}
    res->tag = NULL_TYPE;
    res->value.null = make_null_obj();
    break;
  }
  case ARRAY_EXP:{
    ArrayExp* e2 = (ArrayExp*)e;
    int len = eval_exp(genv, env, e2->length)->value.num->value;
    Obj* init_val = eval_exp(genv, env, e2->init);
    ArrayObj* a = make_array_obj(len);
    for (int i=0; i < len; i++){
		a->elems[i] = (Obj*) malloc(sizeof(Obj));
		a->elems[i]->value = init_val->value;
	}
	
	res->tag = ARRAY_TYPE;
	res->value.arr = a;
    /*printf("array(");
    print_exp(e2->length);
    printf(", ");
    print_exp(e2->init);
    printf(")");*/
    break;
  }
  case OBJECT_EXP:{
    ObjectExp* e2 = (ObjectExp*)e;
    Obj* o  =  (Obj*) malloc(sizeof(Obj));
    Obj* p = eval_exp(genv, env, e2->parent);

    EnvObj* objenv = make_env_obj(p);
    for(int i=0; i< e2->nslots; i++){
	  Obj* obj = eval_stmt(genv, env, e2->slots[i]);
	  if(obj->tag == DATA){
		Entry* ent = make_entry(DATA, obj);
		add_entry(objenv,obj->name, ent);
	  }else{
		obj->value.code->nargs = e2->nslots; /* add num of args to codeval*/
		Entry* ent = make_entry(CODE, obj); 
		add_entry(objenv, obj->name, ent);
	  }
    }
    
    Obj* raw_object = make_var_object("", ENV_OBJ, objenv); 
    res->tag = ENV_OBJ;
    res = raw_object;
    
    break;
  }
  case SLOT_EXP:{
    SlotExp* e2 = (SlotExp*)e;
    res = eval_exp(genv, env, e2->exp);
    res->name = e2->name;
    break;
  }
  case SET_SLOT_EXP:{
    SetSlotExp* e2 = (SetSlotExp*)e;
    Obj* object_ref = eval_exp(genv, env, e2->exp);
    Obj* slot_ref = get_entry_obj(object_ref->value.env, e2->name);
    Obj* value = eval_exp(genv, object_ref->value.env, e2->value);
    slot_ref->tag = value->tag;
    slot_ref->value = value->value;
    /*
    switch(value->tag){
	   case INT_TYPE:
	     slot_ref->value.num = (IntObj*)value->value.num;
	     break;
	   case NULL_TYPE:
	     slot_ref->value.null = (NullObj*)value->value.null;
	     break;
	   case CODE:
	     slot_ref->value.code = (CodeEntry*)value->value.code;
	     break;
	   case DATA:
		 slot_ref->value.data = (DataEntry*)value->value.data;
		 break;
	   case ENV_OBJ:
		 slot_ref->value.env = (EnvObj*)value->value.env;
		 break;
	   default:
          printf("Unrecognized object with tag %d\n", tag);
          exit(-1);
	}*/
    res = slot_ref;
    break;
  }
  case CALL_SLOT_EXP:{
    CallSlotExp* e2 = (CallSlotExp*)e;
    Obj* object_ref = eval_exp(genv, env, e2->exp);
    Obj* fn_ref = get_entry_obj(object_ref->value.env, e2->name);
    Obj* parent = make_var_object("", ENV_OBJ, genv);
    EnvObj* nenv = make_env_obj(parent);
    if(e2->nargs != fn_ref->value.code->nargs){
	    printf("method %s called with wrong number of args \n",e2->name);
	    exit(-1);
	}
		
	Entry* ent = make_entry(ENV_OBJ, object_ref);/*should be a var value*/
	add_entry(nenv, "this", ent);
	for(int i=0; i<e2->nargs; i++){
		Obj* method_arg = eval_exp(genv, env, e2->args[i]);
		Entry* method_arg_entry = make_entry(method_arg->tag, method_arg);
		add_entry(nenv, fn_ref->value.code->args[i], method_arg_entry);
	}
	res = eval_stmt(genv, nenv, fn_ref->value.code->stmt);
/*print_exp(e2->exp);
    printf(".%s(", e2->name);
    for(int i=0; i<e2->nargs; i++){
      if(i > 0) printf(", ");
      print_exp(e2->args[i]);
    }
    printf(")");*/
    break;
  }
  case CALL_EXP:{
    CallExp* e2 = (CallExp*)e;
    Obj* fn = get_entry_obj(genv, e2->name);
    Obj* parent = make_var_object("", ENV_OBJ, genv);
    EnvObj* nenv = make_env_obj(parent);
    
    for(int i=0; i<e2->nargs; i++){
       Obj* function_arg = eval_exp(genv, env, e2->args[i]);
       Entry* function_arg_entry = make_entry(function_arg->tag, function_arg);
	   add_entry(nenv, fn->value.code->args[i], function_arg_entry);
    }
    res = eval_stmt(genv, nenv, fn->value.code->stmt);

    break;
  }
  case SET_EXP:{
    SetExp* e2 = (SetExp*)e;
    printf("%s = ", e2->name);
    Obj* obj = get_entry_obj(env, e2->name);
    Obj* val = eval_exp(genv, env, e2->exp);
    obj->value = val->value;
    res = obj;
    break;
  }
  case IF_EXP:{
    IfExp* e2 = (IfExp*)e;
    printf("if ");
    print_exp(e2->pred);
    printf(" : (");
    print_scopestmt(e2->conseq);
    printf(") else : (");
    print_scopestmt(e2->alt);
    printf(")");
    break;
  }
  case WHILE_EXP:{
    WhileExp* e2 = (WhileExp*)e;
    printf("while ");
    print_exp(e2->pred);
    printf(" : (");
    print_scopestmt(e2->body);
    printf(")");
    break;
  }
  case REF_EXP:{
    RefExp* e2 = (RefExp*)e;
    Obj* o = get_entry_obj(env, e2->name);
    res = o;
    /*make_var_object(e2->name, ENV_OBJ, o)
    printf("%s", e2->name);*/
    break;
  }
  default:
    printf("Unrecognized Expression with tag %d\n", e->tag);
    exit(-1);
  }
  
  return res;
}

void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s ){

  switch(s->tag){
  case VAR_STMT:{
    ScopeVar* s2 = (ScopeVar*)s;
    Obj* obj = eval_exp(genv, env,s2->exp);
    Entry* entry = (Entry* ) malloc (sizeof(Entry)); 
    if(get_entry(env, s2->name)!= NULL){
		/*throw error*/
		printf("variable %s cannot be defined twice\n", s2->name);
        exit(-1);
	}
    entry->obj_type = DATA;
    entry->object = obj;
    add_entry(env, s2->name, entry); 
    break;
  }
  case FN_STMT:{
    ScopeFn* s2 = (ScopeFn*)s;
    if(get_entry(genv, s2->name)!= NULL){
		/*throw error*/
		printf("variable %s cannot be defined twice\n", s2->name);
        exit(-1);
	}
    Entry* entry = (Entry* ) malloc (sizeof(Entry));
    entry->obj_type = ENTRY;
    CodeEntry* code = (CodeEntry* ) malloc (sizeof(CodeEntry));
    code->obj_type = CODE;
	
	code->args = s2->args;
	code->stmt = s2->body;
	
	Obj* code_object = (Obj*) malloc (sizeof(Obj));
	code_object->obj_type = OBJ_TYPE;
	code_object->name = s2->name;
	code_object->tag = CODE;
	code_object->value.code = code;
	
	entry->object  = code_object;
	
	add_entry(env, s2->name, entry);
    break;
  }
  case SEQ_STMT:{
    ScopeSeq* s2 = (ScopeSeq*)s;
    Obj* ret1 = eval_stmt(genv, env, s2->a);
    Obj* ret2 = eval_stmt(genv, env, s2->b);
    break;
  }

  default:
    printf("Unrecognized scope statement with tag(exec_stmt) %d\n", s->tag);
    exit(-1);
  }

}

Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s ){
  Obj* res;
  switch(s->tag){
    case VAR_STMT:{
    ScopeVar* s2 = (ScopeVar*)s;
    Obj* obj = eval_exp(genv, env,s2->exp);
    Entry* entry = (Entry* ) malloc (sizeof(Entry)); 
    if(get_entry(env, s2->name)!= NULL){
		/*throw error*/
		printf("variable %s cannot be defined twice\n", s2->name);
        exit(-1);
	}
    entry->obj_type = DATA;
    entry->object = obj;
    add_entry(env, s2->name, entry); 
    Obj* o = make_var_object("", NULL_TYPE, make_null_obj);
    res = o;
    break;
  }
  case FN_STMT:{
    ScopeFn* s2 = (ScopeFn*)s;
    if(get_entry(genv, s2->name)!= NULL){
		/*throw error*/
		printf("variable %s cannot be defined twice\n", s2->name);
        exit(-1);
	}
    Entry* entry = (Entry* ) malloc (sizeof(Entry));
    entry->obj_type = ENTRY;
    CodeEntry* code = (CodeEntry* ) malloc (sizeof(CodeEntry));
    code->obj_type = CODE;
	code->args = s2->args;
	code->stmt = s2->body;
	
	Obj* code_object = (Obj*) malloc (sizeof(Obj));
	code_object->obj_type = OBJ_TYPE;
	code_object->name = s2->name;
	code_object->tag = CODE;
	code_object->value.code = code;
	
	entry->object  = code_object;
	
	add_entry(env, s2->name, entry);
    Obj* o = make_var_object("", NULL_TYPE, make_null_obj);
    res = o;
    break;
  }
  case SEQ_STMT:{
    ScopeSeq* s2 = (ScopeSeq*)s;
    res = eval_stmt(genv, env, s2->a);
    res = eval_stmt(genv, env, s2->b);
    break;
  }
  case EXP_STMT:{
    ScopeExp* s2 = (ScopeExp*)s;
    res = eval_exp(genv, env,s2->exp);
    break;
  }
  default:
    printf("Unrecognized scope statement with tag %d\n", s->tag);
    exit(-1);
  }
  return res;
}


void interpret (ScopeStmt* stmt) {
  printf("Interpret program:\n");
  
  EnvObj* genv = make_env_obj(NULL);
  exec_stmt(genv, genv, stmt);
  printf("\n");  
}
