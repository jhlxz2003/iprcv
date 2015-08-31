#ifndef _UTYPE_H_
#define _UTYPE_H_


#define TYPE_NAME_MAX             32
#define U_TYPE_FUNDAMENTAL_MAX    64

typedef void   (*UClassInitFunc)(void*  u_class);
typedef void   (*UInstanceInitFunc)(void *instance);

typedef struct _UTypeInfo {
	char                     *type_name;	
	void                     *class_obj;
	unsigned short        class_size;
	unsigned short        instance_size;
	UClassInitFunc        class_init;
	UInstanceInitFunc   instance_init;
	unsigned int           ref_cnt;
} UTypeInfo;

int        u_type_register(int parentid, UTypeInfo *type_info);
UTypeInfo* u_type_get(int utype);

int       u_type_ref(int utype);
int       u_type_unref(int utype);
int        u_type_get_ref(int utype);
/*
void     u_type_put_id(int utype, int id);
int       u_type_get_id(int utype);
*/
#endif

