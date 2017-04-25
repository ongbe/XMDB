// ����gcc�����ʱ��Ҫ����ѡ�� -march=i686

#ifndef TABLE_CTL
#define TABLE_CTL
#include "mem_block_t.h"
#include "rwlock.h"
#include "mem_table_no_manager.h"
#include "time_type_t.h"
#ifdef __cplusplus

extern "C" {

#endif

// �ֶ����ͱ��
#define FIELD_TYPE_INT        1
#define FIELD_TYPE_SHORT      2
#define FIELD_TYPE_LONG       3
#define FIELD_TYPE_LONGLONG   4
#define FIELD_TYPE_FLOAT      5
#define FIELD_TYPE_DOUBLE     6
#define FIELD_TYPE_STR        7
#define FIELD_TYPE_ANY        8
#define FIELD_TYPE_DATE       9    //date ����Ϊ str 16 14��ֵ
#define FIELD_TYPE_HASH_ENTRY      10   //date ����Ϊ str 16 14��ֵ
#define FIELD_TYPE_RBTREE_ENTRY    11   //date ����Ϊ str 16 14��ֵ
#define FIELD_TYPE_SKIPLIST_ENTRY  12

// �ֶ�����
 //date ����Ϊ str 16 14��ֵ
#define FIELD_INT        int
#define FIELD_SHORT      short
#define FIELD_LONG       long
#define FIELD_LONGLONG   long long
#define FIELD_FLOAT      float
#define FIELD_DOUBLE     double
#define FIELD_STR        char *
#define FIELD_ANY        void *
#define FIELD_DATE       time_t 
#define FIELD_HASH_ENTRY	   (struct  mem_hash_entry_t    )
#define FIELD_RBTREE_ENTRY	 (struct  mem_rbtree_entry_t  )
#define FIELD_SKIPLIST_ENTRY (struct  mem_skiplist_entry_t)

// �ֶ�ռ�ÿռ��С
#define FIELD_INT_SIZE        			sizeof(int)
#define FIELD_SHORT_SIZE      			sizeof(short)
#define FIELD_LONG_SIZE       			sizeof(long)
#define FIELD_LONGLONG_SIZE   			sizeof(long long)
#define FIELD_FLOAT_SIZE      			sizeof(float)
#define FIELD_DOUBLE_SIZE     			sizeof(double)
#define FIELD_DATE_SIZE       			sizeof(time_t)  //ʹ�� linux �� time_t
		


//��������
#define READ_RECORD_ERR_TABLE_IS_NULL                32001
#define READ_RECORD_ERR_RECORD_IS_NULL               32002
#define READ_RECORD_ERR_BUF_IS_NULL                  32003
#define READ_RECORD_UNUSED                           32004
#define INSERT_RECORD_ERR_FULL                       32005   
#define CREATE_MEM_TABLE_ERR_NULL_FIELD_INFO_PTR     32006
#define CREATE_MEM_TABLE_ERR_NULL_MEM_BLOCK          32007 
#define CREATE_MEM_TABLE_ERR_NO_NAME                 32008
#define CREATE_MEM_TABLE_ERR_TOO_LONG                32009
#define CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR          32010
#define OPEN_MEM_TABLE_ERR_NULL_TABLE_PTR            32011
#define CREATE_MEM_TABLE_ERR_NULL_CONFIG_PTR         32012

#define READ_FIELD_ERR_TABLE_IS_NULL                 32013
#define READ_FIELD_ERR_FIELD_NAME_IS_NULL            32014
#define READ_FIELD_ERR_IN_BUF_IS_NULL                32015
#define READ_FIELD_ERR_OUT_BUF_IS_NULL               32016
#define READ_FIELD_ERR_NOT_FOUND_FIELD               32017
#define MEM_TABLE_EXTEND_ERR                         32018
#define GET_BLOCK_ERR_BLOCK_IS_NULL                  32019
#define GET_BLOCK_ERR_RECORD_IS_IN_BLOCK             32020
#define INSERT_RECORD_ERR_TABLE_IS_NULL              32021
#define INSERT_RECORD_ERR_BUF_IS_NULL                32022
#define ALLOCATE_RECORD_ERR_TABLE_IS_NULL            32023
#define ALLOCATE_N_RECORD_ERR_TABLE_IS_NULL          32024
#define ALLOCATE_RECORD_ERR_NUM_IS_ZERO              32025
#define TRY_LOCK                                     32026
#define MEM_TABLE_EXTEND_ERR_NOT_ALLOW_EXTEND        32027
#define READ_RECORD_SCN_CHANGED                      32028
#define READ_RECORD_SCN_NOT_COMMITED			     			 32029
#define TRANS_NOT_FREE			    										 32030
#define UNDO_BLOCK_IS_NULL   									 32031
#define REDO_PTR_IS_NULL   										 32032
#define UNDO_PTR_IS_NULL   										 32033
#define READ_RECORD_ISUSED										 32034
#define GET_BLOCK_FAILD   										 32035
#define MEM_TABLE_EXTEND_ERR_NAME_IS_NULL			 32036
#define MEM_TABLE_EXTEND_FREE_SIZE_ERR         32037
#define ALLOCTE_RECODR_TOO_TIMES               32038
#define HIGH_LEVEL_TRY_LOCK										 32039
#define UNLOCK_FAILED													 32040

//����ֶθ���
//1��������ڴ����
#define MAX_FIELD_NUM        64
#define MAX_BLOCK_NUM        64

//��ռ���빲����
#define table_rwlock_t        	 pthread_rwlock_t
#define table_rwlock_init(x)     pthread_rwlock_init(x, NULL)
#define table_rwlock_rdlock(x)   pthread_rwlock_rdlock(x)
#define table_rwlock_wrlock(x)   pthread_rwlock_wrlock(x)
#define table_rwlock_unlock(x)   pthread_rwlock_unlock(x)
#define table_rwlock_destroy(x)  pthread_rwlock_destroy(x)


//��ʱ��CAS��ʵ���м���д��
#define row_lock_t    rwlock_t
#define row_lock_init(x)   rwlock_init((x)); 

#define row_rlock(x)       rwlock_rlock((x)); 
#define row_runlock(x)     rwlock_runlock((x)); 

#define row_wlock(x)     \
rwlock_wlock(x); //  \
//__sync_fetch_and_sub(  &(mem_table->lock_manager.row_lock_counter), 1 );  

#define row_wunlock(x)     \
rwlock_wunlock(x);  // \
//__sync_fetch_and_add(  &(mem_table->lock_manager.row_lock_counter), 1 );


//��ʱ�û�������ʾ����
#define table_lock_t            pthread_mutex_t
#define mem_table_lock_t        pthread_mutex_t
#define mem_table_lock(x)       pthread_mutex_lock(x)
#define mem_table_unlock(x)     pthread_mutex_unlock(x)   
#define mem_table_trylock(x)    pthread_mutex_trylock(x)
#define MEM_TABLE_LOCK_INIT(x)  pthread_mutex_init(x,NULL)
#define mem_table_dest(x)    		pthread_mutex_destroy(x)   


//������汾
//#define MEM_TABLE_DEL_CODE																	\
//LIST_LOCK  (  &(mem_block_temp->mem_free_list.list_lock)    );\
//record_ptr->is_used = 0;																			\
//record_ptr->last_free_pos = mem_block_temp->mem_free_list.head;\
//mem_block_temp->mem_free_list.head = record_ptr->record_num;	\
//LIST_UNLOCK    (  &(mem_block_temp->mem_free_list.list_lock)   );

//˫����汾ɾ�����
#define MEM_TABLE_DEL_CODE																				\
LIST_LOCK  (  &(mem_block_temp->mem_free_list.list_lock)    );		\
record_ptr->is_used = 0;																					\
record_ptr->last_free_pos = mem_block_temp->mem_free_list.head;		\
if( -1 != mem_block_temp->mem_free_list.head )										\
{																																	\
struct record_t * record_ptr2;																		\
get_record_by_record_num(mem_table,mem_block_temp, mem_block_temp->mem_free_list.head,&record_ptr2);\
record_ptr2->next_free_pos = record_ptr->record_num;							\
}																																	\
mem_block_temp->mem_free_list.head = record_ptr->record_num;			\
if( -1 == mem_block_temp->mem_free_list.tail )										\
{																																	\
		mem_block_temp->mem_free_list.tail = record_ptr->record_num;	\
}																																	\
LIST_UNLOCK    (  &(mem_block_temp->mem_free_list.list_lock)   );

struct  mem_hash_entry_t
{
unsigned  long     hash_lkey;						  //���� hash   key ֵ
long               block_no;			          //���������ڵĿ��
unsigned  long     record_num; 					  //���������ڵ��к�
long               link_block_no;          //��ͻ�洢�����
unsigned  long     link_record_num;        //��ͻ�����ڳ�ͻ����к�
char               hash_ckey[32];          //�ַ��� hash key ֵ,��ʱ֧�ֵ�32��ô�����ַ���

} __attribute__ ((packed, aligned (64)));

#define MEM_HASH_ENTRY_SIZE  sizeof(struct  mem_hash_entry_t)


/*���������ɫ*/
 typedef enum Color{
 		RED   = 1,
 		BLACK = 0
 }color_t;
typedef struct  mem_rbtree_entry_t
{
unsigned  long     rbtree_lkey;						  //���� rbtree   key ֵ
long               block_no;			          //���������ڱ�Ŀ�˳���
unsigned  long     record_num; 					  //���������ڵ��к�
long               parent_block_no;			  //���ڵ��������ڵĿ��
unsigned  long     parent_record_num; 			//���ڵ��������ڱ�Ŀ�˳���
long               left_block_no;			    //��ڵ��������ڵĿ��
unsigned  long     left_record_num; 				//��ڵ��������ڵ��к�
long               right_block_no;			    //�ҽڵ���������Ŀ�˳���
unsigned  long     right_record_num; 			//�ҽڵ����������ڵ��к�
color_t            color;
//char                    rbtree_ckey[32];          //�ַ��� hash key ֵ,��ʱ֧�ֵ�32��ô�����ַ���

} __attribute__ ((packed, aligned (64))) mem_rbtree_entry_t;

#define MEM_RBTREE_ENTRY_SIZE  sizeof(struct  mem_rbtree_entry_t)


typedef struct  mem_skiplist_entry_t
{
unsigned  long     lkey;						        //����  key ֵ
long               block_no;			          //���������ڱ�Ŀ�˳���
unsigned  long     record_num; 					    //���������ڵ��к�

long               down_block_no;			      //��һ��ڵ��������ڵĿ��
unsigned  long     down_record_num; 				//��һ��ڵ��������ڵ��к�
long               right_block_no;			    //�ҽڵ���������Ŀ�˳���
unsigned  long     right_record_num; 			  //�ҽڵ����������ڵ��к�
//char                    rbtree_ckey[32];          //�ַ��� hash key ֵ,��ʱ֧�ֵ�32��ô�����ַ���

} __attribute__ ((packed, aligned (64))) mem_skiplist_entry_t;


#define FIELD_RBTREE_ENTRY_SIZE  sizeof(struct  mem_rbtree_entry_t)
#define FIELD_HASH_ENTRY_SIZE 			sizeof(struct  mem_hash_entry_t)
#define FIELD_SKIPLIST_ENTRY_SIZE 		sizeof(struct  mem_skiplist_entry_t)

//֧���ֶ�ֵ֮���ת��
typedef union field_value_def{
 FIELD_INT       int_val;
 FIELD_SHORT     short_val;
 FIELD_LONG      long_val;
 FIELD_LONGLONG  longlong_val;
 FIELD_FLOAT     float_val;
 FIELD_DOUBLE    double_val;
 FIELD_STR       str_val;
 FIELD_ANY       any_val;
 FIELD_DATE      date_val;
}field_value_t;

/*
�ֶ� �ڴ沼��

record_t ={ field_t_0,field_t_1,field_t_2,field_3....... }

*/

//�ֶ�������
struct field_t
{
int                field_type;                //�ֶ�����
char               field_name[32];            //�ֶ���
off_t              field_size;                //һ���ֶ�ռ�ÿռ�Ĵ�С
int                field_order;               //�ֶ��ڼ�¼���ŵڼ���
off_t              field_dis;                 //���ֶξ���������ʼ��ַ�ľ���
short              index_nr;                  //��ص�����������0 ��ʾû��������
long               relate_index[8];           //��ص�����id��һ���ֶ���౻8����������
} __attribute__ ((packed, aligned (64)));

#define FIELD_SIZE  sizeof(struct  field_t)



//��� any �� str ����ĸ������͵� field_size;
inline void fill_field_info (struct field_t *field)  
{
	switch ( field->field_type )   
{                                 
	case FIELD_TYPE_INT:            (field)->field_size = FIELD_INT_SIZE;              DEBUG("FIELD_TYPE_INT\n");					break; 
	case FIELD_TYPE_SHORT:          (field)->field_size = FIELD_SHORT_SIZE;            DEBUG("FIELD_TYPE_SHORT\n");				break; 
 	case FIELD_TYPE_LONG:           (field)->field_size = FIELD_LONG_SIZE;             DEBUG("FIELD_TYPE_LONG\n");				break; 
	case FIELD_TYPE_LONGLONG:       (field)->field_size = FIELD_LONGLONG_SIZE;         DEBUG("FIELD_TYPE_LONGLONG\n");		break; 
	case FIELD_TYPE_FLOAT:          (field)->field_size = FIELD_FLOAT_SIZE;            DEBUG("FIELD_TYPE_FLOAT\n");				break; 
	case FIELD_TYPE_DOUBLE:         (field)->field_size = FIELD_DOUBLE_SIZE;           DEBUG("FIELD_TYPE_DOUBLE\n");			break; 
  case FIELD_TYPE_DATE:           (field)->field_size = FIELD_DATE_SIZE;             DEBUG("FIELD_TYPE_DATE\n");				break; 
  case FIELD_TYPE_HASH_ENTRY:     (field)->field_size = FIELD_HASH_ENTRY_SIZE;       DEBUG("FIELD_TYPE_HASH_ENTRY\n");	break; 
  case FIELD_TYPE_RBTREE_ENTRY:   (field)->field_size = FIELD_RBTREE_ENTRY_SIZE;     DEBUG("FIELD_TYPE_RBTREE_ENTRY\n");break; 
  case FIELD_TYPE_SKIPLIST_ENTRY:   (field)->field_size = FIELD_SKIPLIST_ENTRY_SIZE;     DEBUG("FIELD_TYPE_RBTREE_ENTRY\n");break; 
  case FIELD_TYPE_STR:                                                               DEBUG("FIELD_TYPE_STR\n");break; //�ַ���������Ҫ�Լ����ó���
  default :                       (field)->field_size = 0;                          DEBUG("error ZERO_TYPE\n");
}
}

/*
��¼ �ڴ沼��

=====================

struct  mem_block_t

======================
record_t_0 field_0 field_1 field_2 field_3 field_4 field_5 ...
record_t_1
record_t_2
record_t_3
record_t_4
record_t_5
.....
======================
*/

//��¼������������һ������
//ÿ�м�¼���ڴ�ֲ��� record_info+ʵ�����ݣ�record_info ��ÿ�м�¼�Ŀ���
typedef struct record_t
{
long long                   record_num;         // ��¼���߼�ID
short                       is_used;            // �Ƿ�����
row_lock_t                  row_lock;           // ����
unsigned long               last_free_pos;      // ����ά�����������λ����Ϣ
unsigned long               next_free_pos;      // ����ά�����������λ����Ϣ
long                        scn;               // ���ݰ汾����������������
struct undo_record_t*       undo_record_ptr;   //ָ��undo ��ָ�� // �������Էϵ�
void*                       undo_info_ptr;     //ָ��undo ��Ϣ��ָ��
void *                      data;               // ������ʼ��ַ
} __attribute__ ((packed, aligned (64))) record_t;

// ��¼�������Ĵ�С
#define RECORD_HEAD_SIZE  sizeof(struct record_t)

#define INIT_RECORD(record_ptr)																					\
		(record_ptr)->data    =  (char *)record_ptr + RECORD_HEAD_SIZE;	\
		 row_lock_init(&((record_ptr)->row_lock)); //������ʼ��

//����������   
struct mem_lock_manager_t
{
int row_lock_counter;   //����������
} __attribute__ ((packed, aligned (64)));


//�ڴ��������Ϣ
typedef struct mem_table_config_t
{  
	 long                      mem_table_no;           // �ñ���߼�ID
	 long                      owner_id;               // �ñ������û���ID
	 char                      table_name[128];        // ����
   struct field_t     *      fields_table;           // �ñ�ʹ�õ������ֶ�
   int                       field_used_num;         // ʹ���ֶεĶ���
   struct mem_block_t *      mem_blocks_table;       // �ñ�ʹ�õ������ڴ��--���
   int                       mem_block_used;         // ʹ�������ڴ��Ķ���
   int                       auto_extend;            // �Ƿ��Զ���չ
   off_t                     extend_block_size;      // �Զ���չ���С
// struct mem_index_t  *     mem_index_table,        // �ñ�ʹ�õ������ڴ���¼��
} __attribute__ ((packed, aligned (64))) mem_table_config_t;


// �ڴ�������������Ĵ�С
#define MEM_TABLE_CONFIG_SIZE  sizeof(mem_table_config_t)

/*
  �ڴ�� �ڴ沼��
  mem_block_t--->mem_block_t--->mem_block_t--->mem_block_t
*/



//�ڴ��
typedef struct mem_table_t
{
struct mem_table_config_t    config;
table_lock_t                 table_locker;            // ����  
off_t                        record_size;             // 1����¼ռ�ÿռ�Ĵ�С
//long long                   record_used_num;       // ʹ�õļ�¼��
//struct mem_free_list_t      mem_free_list;         // �����������ڸ����Ѿ�ɾ�����ݼ�¼
struct mem_lock_manager_t    lock_manager;            // �������� 
short                        is_externing;            //�Ƿ�������                 
table_rwlock_t               rwlocker;
long long                    writer_trans_no;
} __attribute__ ((packed, aligned (64))) mem_table_t;

// �ڴ���������Ĵ�С
#define MEM_TABLE_SIZE  sizeof(struct mem_table_t)




//��ʼ���ڴ��
#define INIT_MEM_TABLE(mem_table)																					\
//allocate_table_no(&((mem_table->config).mem_table_no));										\
//(mem_table)->config.fields_table                 =    0;									\
//(mem_table)->config.field_used_num               =    0;									\
//(mem_table)->config.mem_blocks_table             =    0;									\
(mem_table)->config.mem_block_used                 =    1;									\
MEM_TABLE_LOCK_INIT(&(mem_table->table_locker ));													\
table_rwlock_init(&(mem_table->rwlocker ));																\
(mem_table)->is_externing													=     0;                \
(mem_table)->writer_trans_no											=			-1;								\
//(mem_table)->record_used_num                      =    0;								\
//(mem_table)->config.index_used_num             =    0;									\
(mem_table)->lock_manager.row_lock_counter        =    0;
//(mem_table)-> redo_log             =    0; 


/////////////////////////////////////////////////////////////////////////////////////////////
inline int mem_table_extend( struct mem_table_t * mem_table, struct  mem_block_t ** out_mem_block);


//��������ֶ���Ϣ�Ĵ�С,��ÿ���ֶξ������ݿ�ʼ��ַ�ľ���
inline void fill_table_field_size ( struct mem_table_t *mem_table )  
{
	int i = 0;
	mem_table->record_size = RECORD_HEAD_SIZE;
	off_t dis =0;
  for(;i<mem_table->config.field_used_num; ++i)
  {
  	fill_field_info ( &(mem_table->config.fields_table[i]) );  
  	mem_table->config.fields_table[i].field_dis = dis;                       //��ʼ�� field_dis
  	mem_table->record_size += mem_table->config.fields_table[i].field_size;	
  	dis += mem_table->config.fields_table[i].field_size;	
  }
 DEBUG("mem_table->record_size is %ld \n",mem_table->record_size);

}

//ͨ���ֶ����ơ����ݵ�ַ���� ����ø����ݵ� recored �ĵ�ַ
inline int get_record_by_field_name(
                                struct mem_table_t *mem_table , //��
                                const char *field_name,         //�ֶ���
                                void * addr,                    //�ֶδ洢������ʼ��ַ
                                struct record_t * record_ptr           /*��Ӧ��¼��ʼ��ַ*/
                              )
{
	int i = 0;
	off_t dff_size = 0;
	//�ҵ���Ӧ���ֶ�
  for(;i<mem_table->config.field_used_num; ++i)
	{
		if(strcmp( field_name,	mem_table->config.fields_table[i].field_name    ) == 0 )break;	
		dff_size += mem_table->config.fields_table[i].field_size;
	}
	
	record_ptr =  (record_t*)(addr - dff_size - RECORD_HEAD_SIZE);  //��¼��ʼ��ַ = ���ݵ�ַ - ǰ�������ֶ�����ռ�õĿռ� - ��¼ͷռ�ÿռ�
	return 0;
}


//ͨ��record��ָ���� mem_block_t ָ��
inline int get_block_by_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,struct  mem_block_t ** mem_block)
{
	if( NULL == mem_table )  return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)  return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == mem_block)   return GET_BLOCK_ERR_BLOCK_IS_NULL;
  
  unsigned  long   record_num = record_ptr->record_num;
  if(record_num<0)return GET_BLOCK_ERR_RECORD_IS_IN_BLOCK;
  
  DEBUG("record_ptr is %0x,record_num is %ld,mem_table->record_size =%ld ,MEM_BLOCK_HEAD_SIZE=%d\n",record_ptr,record_num,mem_table->record_size,MEM_BLOCK_HEAD_SIZE);		
  //�Ż����ܣ��� O(n) �� O(1)��record_ptr ͨ��λ��	 record_num ��MEM_BLOCK_HEAD_SIZE ��� space_start_addr ��ַ,���ڿ�ͷ�������������������ͷʹ������� mem_block_t
  struct  mem_block_t * space_start_addr = ((struct  mem_block_t *) ( (char *)record_ptr - (record_num*(mem_table->record_size))-MEM_BLOCK_HEAD_SIZE ));
  *mem_block = space_start_addr->block_malloc_addr;
  DEBUG("space_start_addr is %0x \n",space_start_addr);	
//  DEBUG("*mem_block is %0x \n",*mem_block);	
  if( 0 == *mem_block)
  	{
  		ERROR("BAD record_ptr is %0x,record_num is %ld,mem_table->record_size =%ld ,MEM_BLOCK_HEAD_SIZE=%d\n",record_ptr,record_num,mem_table->record_size,MEM_BLOCK_HEAD_SIZE);		

  		return GET_BLOCK_ERR_BLOCK_IS_NULL;
  	}
  //struct  mem_block_t * mem_block_temp  = mem_table->config.mem_blocks_table;
  
  //int i=0;
  //void * ptr = (void *) record_ptr;
  //
  //for(;i<mem_table->config.mem_block_used;++i)
	//{ 
	//	//record_ptr λ�����ݿռ��ڣ���recored���ڸÿ�
	//	DEBUG("mem_block_temp->space_start_addr is %0x \n",mem_block_temp->space_start_addr);	
	//	DEBUG("mem_block_temp->space_end_addr is %0x \n",mem_block_temp->space_end_addr);	
  //
	//	if(mem_block_temp->space_start_addr  <= ptr && mem_block_temp->space_end_addr  >= ptr)
	//	   break;
	//	mem_block_temp = mem_block_temp->next;      //��һ����
	//}
	////
	//if( i == mem_table->config.field_used_num ) return GET_BLOCK_ERR_RECORD_IS_IN_BLOCK;
	//*mem_block = mem_block_temp  ;
	
	return 0;
}

//ͨ��record��ָ���� mem_block_t ָ��
inline int get_record_by_record_num(struct mem_table_t *mem_table,struct  mem_block_t * mb, unsigned  long   record_num,struct record_t ** record_ptr)
{
	if( NULL == mem_table )  return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)  return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == mb				)  return GET_BLOCK_ERR_BLOCK_IS_NULL;
  if( record_num<0      )  return GET_BLOCK_ERR_RECORD_IS_IN_BLOCK;
  	
  //�Ż����ܣ��� O(n) �� O(1)��record_ptr ͨ��λ��	 record_num �� MEM_BLOCK_HEAD_SIZE ��� mem_block ͷ��ַ
  *record_ptr = ((struct  record_t *) ( (char *)(mb->space_start_addr) +(record_num*(mem_table->record_size)) ));
  DEBUG("*record_ptr is %0x \n",*record_ptr);	
	
	return 0;
}

//void get_record(struct mem_table_t *mem_table_ptr, long block_no,long long record_num,record_t * record_ptr)  	

//  DEBUG("get_record(),block_no,record_num=( %ld,%ld ) ;\n",block_no,record_num);										\
//���� ��˳��ź��кſ��ٻ�� ��¼ָ��
//#define get_record(mem_table_ptr,block_no,record_num,record_ptr) \
//do{  																																																	\
//  int i = 0;																																													\
//	struct  mem_block_t * mem_block_temp = mem_table_ptr->config.mem_blocks_table;        							\
//	for(;i<mem_table_ptr->config.mem_block_used;++i)				/*�������п�,���ҿ��Բ������ݵ��ڴ��	*/		\
//	{																																																		\
//		if(mem_block_temp->space_start_addr + mem_block_temp->high_level* mem_table_ptr->record_size < mem_block_temp->space_end_addr - mem_table_ptr->record_size )break;	\
//			mem_block_temp = mem_block_temp->next;      /*��һ����                   */																																												\
//		if( i == block_no)break;                     /*�ҵ���Ӧ�Ŀ�                */																																												\
//	}																																																		\
//	record_ptr=( record_t *)((char *)(mem_block_temp->space_start_addr) + (record_num)*((mem_table_ptr)->record_size));/*�����ҵ��Ŀ飬�ҵ� recored_ptr */								\
//	}while(0);

inline int  get_record(mem_table_t *mem_table,long block_no,unsigned long record_num,struct record_t ** record_ptr) 
{  																																																	
  int err;
  int i = 0;																																													
	struct  mem_block_t * mb ;        
	DEBUG("block_no is %d,record_num is %d \n",block_no,record_num);							
	 err = get_block_no_addr(block_no,(void **)(&mb));
	DEBUG("mb is %0x,space_start_addr is %0x \n",mb,mb->space_start_addr);							
 
	 if(err)
	 	{
	 		ERROR("Get_block_no_addr failed!,err is %d\n",err);
	 		return err;
	 	}
  *record_ptr = ((struct  record_t *) ( (char *)(mb->space_start_addr) +(record_num*(mem_table->record_size)) ));
  DEBUG("*record_ptr is %0x \n",*record_ptr);	
	 return 0;
	}

// ͨ��3�� no ������е�ַ
inline int get_record_by_3NO(long mem_table_no,long block_no,long long record_num,struct record_t ** record_ptr) 
{  																																																	
		int err;
		mem_table_t *mem_table ;    																																						
		struct  mem_block_t * mb ; 
		
		DEBUG("mem_table_no is %d\n",mem_table_no);							
		err = get_table_no_addr(mem_table_no,(void **)(&mem_table));
		DEBUG("mem_table is %0x \n",mem_table);							
		 
		if(err)
		{
			ERROR("Get_block_no_addr failed!,err is %d\n",err);
			return err;
		}
		   
		   
		DEBUG("block_no is %d,record_num is %d \n",block_no,record_num);							
		err = get_block_no_addr(block_no,(void **)(&mb));
		DEBUG("mb is %0x,space_start_addr is %0x \n",mb,mb->space_start_addr);							
		
		if(err)
		{
			ERROR("Get_block_no_addr failed!,err is %d\n",err);
			return err;
		}
		*record_ptr = ((struct  record_t *) ( (char *)(mb->space_start_addr) +(record_num*(mem_table->record_size)) ));
		DEBUG("*record_ptr is %0x \n",*record_ptr);	
		 return 0;
}

//�ύ��ȡһ����¼������
inline int mem_table_read_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,char *buf)
{
	if( NULL == mem_table )  return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)  return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == buf       )  return READ_RECORD_ERR_BUF_IS_NULL;	
	//��¼δʹ��
	if( 0 == record_ptr->is_used ) return READ_RECORD_UNUSED;
	DEBUG(" enter mem_table_read_record() \n");	
  //record_ptr->data    = (void *)( (char *)(*record_ptr) + RECORD_HEAD_SIZE);
  
	row_rlock   (  &(record_ptr->row_lock )                                            );
	memcpy     (    buf, (char *)(record_ptr) + RECORD_HEAD_SIZE, mem_table->record_size - RECORD_HEAD_SIZE  );
	row_runlock (  &(record_ptr->row_lock )                                            );
	
	DEBUG("mem_table_read_record is %s \n",buf);	
	return 0;
}

//���ȡһ����¼������
inline int mem_table_force_read_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,char *buf)
{
	if( NULL == mem_table )  return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)  return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == buf       )  return READ_RECORD_ERR_BUF_IS_NULL;	
	//��¼δʹ��
	if( 0 == record_ptr->is_used ) return READ_RECORD_UNUSED;
		
	memcpy(    buf,  (char *)(record_ptr) + RECORD_HEAD_SIZE, mem_table->record_size - RECORD_HEAD_SIZE  );
	DEBUG("mem_table_force_read_record, record_ptr is %0x,buf = %s;\n",record_ptr,buf);	

	return 0;
}

//���л���ȡһ����¼������
inline int mem_table_safe_read_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,char *buf)
{
	if( NULL == mem_table )  return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)  return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == buf       )  return READ_RECORD_ERR_BUF_IS_NULL;	
	//��¼δʹ��
	if( 0 == record_ptr->is_used ) return READ_RECORD_UNUSED;
  DEBUG("mem_table_safe_read_record, record_ptr is %0x;\n",record_ptr);	

	mem_table_lock(&(mem_table->table_locker) );         //����		
	memcpy     (    buf,  (char *)(record_ptr) + RECORD_HEAD_SIZE, mem_table->record_size - RECORD_HEAD_SIZE  );
	mem_table_unlock(&(mem_table->table_locker));        //�����

	return 0;
}


//��ȡһ���ֶε�����
inline int mem_table_read_field(struct mem_table_t *mem_table ,     //��ָ��
                       const char * field_name,           //�ֶ���
                       void* in_buf,                      //��¼����
                       void* out_buf                      //�ֶ�����
                       )
{
	if( NULL == mem_table )  return READ_FIELD_ERR_TABLE_IS_NULL;
	if( NULL == field_name)  return READ_FIELD_ERR_FIELD_NAME_IS_NULL;
	if( NULL == in_buf    )  return READ_FIELD_ERR_IN_BUF_IS_NULL;	
	if( NULL == out_buf   )  return READ_FIELD_ERR_OUT_BUF_IS_NULL;	
	//��¼δʹ��
	int i = 0;
	off_t dff_size = 0;
	//�ҵ���Ӧ���ֶ�
  for(;i<mem_table->config.field_used_num; ++i)
	{
		if(strcmp( field_name,	mem_table->config.fields_table[i].field_name    ) == 0 )break;	
	}
	if(mem_table->config.field_used_num == i)return READ_FIELD_ERR_NOT_FOUND_FIELD;
	memcpy(out_buf,in_buf+(mem_table->config.fields_table[i].field_dis),mem_table->config.fields_table[i].field_size);
	return 0;
}


//����һ����¼������
inline int mem_table_update_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,char *buf)
{
	if( NULL == mem_table )        return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)        return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == buf       )        return READ_RECORD_ERR_BUF_IS_NULL;	
	if( 0 == record_ptr->is_used ) return READ_RECORD_UNUSED; //��¼δʹ��
	DEBUG("mem_table_update_record, record_ptr is %0x,buf = %s;\n",record_ptr,buf);	

	row_wlock   (  &(record_ptr->row_lock) );
	memcpy     (   (char *)(record_ptr) + RECORD_HEAD_SIZE,  buf, mem_table->record_size - RECORD_HEAD_SIZE );
	row_wunlock (  &(record_ptr->row_lock) );
	
	return 0;
}

//ǿ�Ƹ���һ����¼������
inline int mem_table_force_update_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,char *buf)
{
	if( NULL == mem_table )        return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)        return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == buf       )        return READ_RECORD_ERR_BUF_IS_NULL;	
	if( 0 == record_ptr->is_used ) return READ_RECORD_UNUSED; //��¼δʹ��
	DEBUG("mem_table_force_update_record, record_ptr is %0x,buf = %s;\n",record_ptr,buf);	
	//redo_log		
	memcpy(   (char *)(record_ptr) + RECORD_HEAD_SIZE,  buf, mem_table->record_size - RECORD_HEAD_SIZE );	
	return 0;
}

//���л�����һ����¼������
inline int mem_table_safe_update_record(struct mem_table_t *mem_table ,struct record_t * record_ptr,char *buf)
{
	if( NULL == mem_table )        return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr)        return READ_RECORD_ERR_RECORD_IS_NULL;
	if( NULL == buf       )        return READ_RECORD_ERR_BUF_IS_NULL;	
	if( 0 == record_ptr->is_used ) return READ_RECORD_UNUSED; //��¼δʹ��
	
	DEBUG("mem_table_safe_update_record, record_ptr is %0x;\n",record_ptr);	
	
	//redo_log
	mem_table_lock(&(mem_table->table_locker) );         //����				
	memcpy     (   (char *)(record_ptr) + RECORD_HEAD_SIZE,  buf, mem_table->record_size - RECORD_HEAD_SIZE );	
	mem_table_unlock(&(mem_table->table_locker));        //�����
	return 0;
}



//ɾ��һ����¼������
inline int mem_table_del_record(struct mem_table_t *mem_table ,struct record_t * record_ptr)
{
	if( NULL == mem_table        )   return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr       )   return READ_RECORD_ERR_RECORD_IS_NULL;
	if( 0 == record_ptr->is_used )   return READ_RECORD_UNUSED;         //��¼δʹ��
	
	//��ÿ�
	struct mem_block_t * mem_block_temp;
	int err;
  if(0!=(err=get_block_by_record(mem_table ,record_ptr,&mem_block_temp)))return err;
	DEBUG("Begin mem_table_del_record, mem_block_temp is %0x,block_no = %ld,record_num=%ld;\n",mem_block_temp,mem_block_temp->block_no,record_ptr->record_num);	
			
	row_wlock   (  &(record_ptr->row_lock)                  );
  MEM_TABLE_DEL_CODE
	row_wunlock (  &(record_ptr->row_lock)                 );

	return 0;
}

//ǿ��ɾ��һ����¼������
inline int mem_table_force_del_record(struct mem_table_t *mem_table ,struct record_t * record_ptr)
{
	if( NULL == mem_table        )   return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr       )   return READ_RECORD_ERR_RECORD_IS_NULL;
	if( 0 == record_ptr->is_used )   return READ_RECORD_UNUSED;         //��¼δʹ��
	
	//��ÿ�
	struct mem_block_t * mem_block_temp;
	int err;
  if(0!=(err=get_block_by_record(mem_table ,record_ptr,&mem_block_temp)))return err;
  DEBUG("mem_table_force_del_record, record_ptr is %0x;\n",record_ptr);	

	MEM_TABLE_DEL_CODE
	return 0;
}

//���л�ɾ��һ����¼������
inline int mem_table_safe_del_record(struct mem_table_t *mem_table ,struct record_t * record_ptr)
{
	if( NULL == mem_table        )   return READ_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == record_ptr       )   return READ_RECORD_ERR_RECORD_IS_NULL;
	if( 0 == record_ptr->is_used )   return READ_RECORD_UNUSED;         //��¼δʹ��
	
	//��ÿ�
	struct mem_block_t * mem_block_temp;
	int err;
  if(0!=(err=get_block_by_record(mem_table ,record_ptr,&mem_block_temp)))return err;
  DEBUG("mem_table_safe_del_record, mem_block_temp is %0x;\n",mem_block_temp);	

  mem_table_lock(&(mem_table->table_locker) );         //����		
	MEM_TABLE_DEL_CODE
  mem_table_unlock(&(mem_table->table_locker));        //�����

	return 0;
}

//����һ�����м�¼��
inline int mem_table_try_allocate_record(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no)
{
	if( NULL == mem_table )  return ALLOCATE_RECORD_ERR_TABLE_IS_NULL;
	DEBUG(" ----- Enter mem_table_try_allocate_record() ----- \n");
  /* �����߼�
	* 1. ���ȸ��ݸ�ˮλ��,�ҿ���record
	* 2. �����ˮλ��û�п��пռ�,��ӻ���������տռ�
	* 3. ��������Ҳû�У����Զ�����
	*/
	int i = 0;
	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
  
//  DEBUG("mem_block_temp is %0x\n",mem_block_temp);
  //�������п�,���ҿ��Բ������ݵ��ڴ��			
  short is_need_extern = 1;
 
		for(;i<mem_table->config.mem_block_used-1;++i)
		{
			mem_block_temp = mem_block_temp->next; 
		}
		
		if( 0!= (HIGH_LEVEL_TRYLOCK(&(mem_block_temp->high_level_lock))) )
			{
				//IMPORTANT_INFO("HIGH_LEVEL_TRYLOCK =0\n");
				return HIGH_LEVEL_TRY_LOCK;  //��ˮλ������
			}
		
		
		unsigned  long  high_level_temp = mem_block_temp->high_level;  
		if(mem_block_temp->space_start_addr + mem_block_temp->high_level* mem_table->record_size < mem_block_temp->space_end_addr - mem_table->record_size )
			{
				is_need_extern = 0;
		
		   DEBUG("----- try to allocate record by high level -----\n");
		
		 //���ݸ�ˮλ�߻�ȡ���²���λ��    
		//   DEBUG("mem_block_temp->high_level is %ld,mem_table->record_size is %ld;\n",high_level_temp,mem_table->record_size);
		//   DEBUG("mem_block_temp->space_start_addr is %0x \n",mem_block_temp->space_start_addr);
		  
			// �ҵ����õļ�¼λ��
			(*record_ptr) = (struct record_t *) ( (char *)mem_block_temp->space_start_addr + high_level_temp * (mem_table->record_size) );
   
//   DEBUG("allocate_record_ptr is %0x;\n",*record_ptr);

	//���ؿ���߼���
	 (*block_no) = mem_block_temp->block_no;
//   DEBUG("allocate_record's block_no is %ld ;\n",(*block_no));
    ++(mem_block_temp->high_level);
//    DEBUG(" ----- try to allocate record by high level end -----\n");
	}


	//û�ҵ����ڴ���ַ > ������β��ַ,����һ�������
	//  if (is_need_extern)
	//	{
	//		DEBUG(" ----- Try to find record in free_list -----\n");
	//		 	i=0;
	//    struct  mem_block_t * mem_block_temp2 = mem_table->config.mem_blocks_table;
      //�������п�,���ҿ��Բ������ݵ��ڴ���еĻ�������			
	//    for(;i<mem_table->config.mem_block_used;++i)
	//    {
	//	      	//�������������ݣ�������ͷȡһ����ַ����		
 //     if( -1 != mem_block_temp2->mem_free_list.head ) 
	//       {
	//       	//������������
	//       	LIST_LOCK     (  &(mem_block_temp2->mem_free_list.list_lock)  );
	//       	//ȡ�ÿ�������Ķ�Ӧrecord�ĵ�ַ
	//       	(*record_ptr) =  (struct record_t *) ( (mem_block_temp2->mem_free_list.head) * mem_table->record_size + mem_block_temp->space_start_addr );
	//       	DEBUG("Find in Freelist, record_num %ld,record_ptr is %ld,last free pos is %ld\n",mem_block_temp->mem_free_list.head,*record_ptr,(*record_ptr)->last_free_pos);
	//       	high_level_temp = (*record_ptr)->record_num;////////////////////
	//       	//���������head Ϊ��һ�����������λ��
	//       	mem_block_temp2->mem_free_list.head = (*record_ptr)->last_free_pos;
	//       	//�����������
	//       	LIST_UNLOCK   (  &(mem_block_temp2->mem_free_list.list_lock)  );
	//       	//���ؿ���߼���
	//       	*block_no = mem_block_temp2->block_no;
	//       	is_need_extern = 0;
	//       	//DEBUG("----- Free record space in free_list finded!  -----\n");
 //       break;
	//       }	
	//       if(i!=mem_table->config.mem_block_used-1 )
	//       	{
	//       	mem_block_temp2 = mem_block_temp2->next;      //��һ����
	//         }
	//    }
	//   
	// }
// û�п��пռ�ͻ����������Զ�����
	//if (i == mem_table->config.mem_block_used-1)  
	if (is_need_extern)  
{ 
	DEBUG(" ----- Try to externd_table ----- \n");
	     	      //�Զ�����
	     	      struct  mem_block_t * extern_mem_block;
	     	      int err = 0;
	     	      err =  mem_table_extend(mem_table,&extern_mem_block);
	     	      if(0 == err)
	     	      	{
	     	      			DEBUG("extern_mem_block->space_end_addr is %0x\n",extern_mem_block->space_end_addr);
	     	      		  DEBUG("extern_mem_block->space_start_addr is %0x\n",extern_mem_block->space_start_addr);
	     	      		  DEBUG("mem_table->record_size is %d\n",mem_table->record_size);
	     	      			//�Զ�����ɹ�,��¼������չ���еĵ�һ��λ��,���²���
	     	      			IMPORTANT_INFO("Externd_Block OK and Try Allocate Agian!\n");                   
         // check 
				//{
				// int ij = 0;
				// struct  mem_block_t * mem_block_temp1 = mem_table->config.mem_blocks_table;
				// for(;ij<mem_table->config.mem_block_used-1;++ij)
				// {
				// 	mem_block_temp1 = mem_block_temp1->next; 
				// }
				//if( 0!= (err=HIGH_LEVEL_TRYLOCK(&(mem_block_temp1->high_level_lock)) ))
	      // {
        //  IMPORTANT_INFO("Check extern high_leve failed,err is %d ,extern_mem_block is %0x .\n",err,mem_block_temp1);
	      // 	return HIGH_LEVEL_TRY_LOCK;  //��ˮλ������
	      // }
        // else
        // 	{
        // 		IMPORTANT_INFO("Check extern high_leve ok\n");
        // 		HIGH_LEVEL_UNLOCK(&(mem_block_temp1->high_level_lock))
        // 	}
        // } 
                    HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽��� 	
	     	      			return TRY_LOCK;
	     	      			//if ( (extern_mem_block->space_end_addr-extern_mem_block->space_start_addr)>mem_table->record_size)
	     	      		  //  {
	     	      		  //   
	     	      		  //    //��չ���ˮλ�ߵ���
                    //        HIGH_LEVEL_LOCK(&(extern_mem_block->high_level_lock));
                    //         (*record_ptr) =(struct record_t *)( extern_mem_block->space_start_addr); 	
                    //        high_level_temp =  extern_mem_block->high_level;
                    //        ++extern_mem_block->high_level;
                    //        HIGH_LEVEL_UNLOCK(&(extern_mem_block->high_level_lock));
                    //        //���ؿ���߼���
	                  //      	*block_no = extern_mem_block->block_no;
	                  //      	IMPORTANT_INFO("Allocate_block >>>record_ptr is %ld, high_level is %ld,block_no is %d\n",*record_ptr,high_level_temp,*block_no);
                    //
                    //  }
	     	      		  //else 
	     	      		  //	{
	     	      		  //		ERROR("MEM_TABLE_EXTEND_FREE_SIZE_ERR\n");
	     	      		  //		ERROR("extern_mem_block->space_end_addr is %0x\n",extern_mem_block->space_end_addr);
	     	      		  //		ERROR("extern_mem_block->space_start_addr is %0x\n",extern_mem_block->space_start_addr);
	     	      		  //		ERROR("mem_table->record_size is %d\n",mem_table->record_size);
	     	      		  //		return  MEM_TABLE_EXTEND_FREE_SIZE_ERR;
	     	      		  //	}
	     	      		  	
	     	      	}
	     	      else if(TRY_LOCK== err)
	     	      	{
	     	      		  DEBUG(" TRY again! \n");
	     	      		  HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
	     	      			return TRY_LOCK;
	     	      	}
	     	      
	     	      else 
	     	      	{
	     	      		ERROR("MEM_TABLE_EXTEND_ERR err is %d\n",err);
	     	      		HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
	     	      		return  err;
	     	      	}
	     	      	
	DEBUG("----- Try to externd table end -----\n");	     	      	
}
//			DEBUG("----- init a record ! -----\n");
	   (*record_ptr)->record_num = high_level_temp;
     (*record_ptr)->is_used    =  1;
     (*record_ptr)->last_free_pos =  -1;
     (*record_ptr)->next_free_pos =  -1;
     (*record_ptr)->scn           =  0;
     (*record_ptr)->undo_record_ptr= 0;
     (*record_ptr)->data    =  (char *)(*record_ptr) + RECORD_HEAD_SIZE;  
     row_lock_init(&((*record_ptr)->row_lock)); //������ʼ��
     
     // CHEHCK PTR
     // �޸� ������´���ķ���ָ��
    // { 
    // 	  struct  mem_block_t * space_start_addr = ((struct  mem_block_t *) ( (char *)(*record_ptr) - (high_level_temp*(mem_table->record_size))-MEM_BLOCK_HEAD_SIZE ));
    //    if(!space_start_addr)
    //    	{
    //    		ERROR("[check] space_start_addr [%ld] is bad \n",space_start_addr);
    //    		HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
    //    		return GET_BLOCK_ERR_BLOCK_IS_NULL;
    //    	}
    //    
    //    struct  mem_block_t * mem_block = space_start_addr->block_malloc_addr;
    //    //DEBUG("space_start_addr is %0x \n",space_start_addr);	
    //    //DEBUG("*mem_block is %0x \n",*mem_block);	
    //    if( 0 == mem_block)
  	//     {
  	//     	ERROR("[check] record_ptr is %ld,record_num is %ld,mem_table->record_size =%ld ,MEM_BLOCK_HEAD_SIZE=%d\n",record_ptr,high_level_temp,mem_table->record_size,MEM_BLOCK_HEAD_SIZE);		
    //      //(*record_ptr) = (struct record_t *) ( (char *)mem_block_temp->space_start_addr + high_level_temp * (mem_table->record_size) );
    //      //IMPORTANT_INFO("Fix_bad_record >>> %ld \n",*record_ptr);
    //      //
    //      //(*record_ptr)->record_num = high_level_temp;
    //      //(*record_ptr)->is_used    =  1;
    //      //(*record_ptr)->last_free_pos =  0;
    //      //(*record_ptr)->scn           =  0;
    //      //(*record_ptr)->undo_record_ptr= 0;
    //      //(*record_ptr)->data    =  (char *)(*record_ptr) + RECORD_HEAD_SIZE;  
    //      //row_lock_init(&((*record_ptr)->row_lock)); //������ʼ��
  	//      HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
  	//      return GET_BLOCK_ERR_BLOCK_IS_NULL;
  	//     }
	  //
    //}
     HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���

     DEBUG(" ----- Allocate_record >>> %0x , high_level is %ld ----- \n",*record_ptr,high_level_temp);
	   return 0;
	
}


//����һ�����м�¼��
inline int mem_table_allocate_record(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no)
{
	if( NULL == mem_table )  return ALLOCATE_RECORD_ERR_TABLE_IS_NULL;
//  DEBUG("Enter mem_table_allocate_record();\n");	
  int err;
  int i = 0;
	do{
		++i;
		err= mem_table_try_allocate_record(mem_table ,/* out */ record_ptr,block_no);
		if(0!= err && TRY_LOCK != err && HIGH_LEVEL_TRY_LOCK !=err)
  	{
  		return err;
  	} 
  	if( 1 == mem_table->is_externing )
  		{
  			i = 0;
  			continue;
  		}
 //	if(i>1000000)
 //		{
 //			ERROR("mem_table_allocate_record too times, err is %d !,mem_block_used is %d .\n",err,mem_table->config.mem_block_used);
 //			// check 
	//			{
	//			 int ij = 0;
	//			 struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
	//			 for(;ij<mem_table->config.mem_block_used-1;++ij)
	//			 {
	//			 	mem_block_temp = mem_block_temp->next; 
	//			 }
	//        if ( 0!= (err=HIGH_LEVEL_TRYLOCK(&(mem_block_temp->high_level_lock)) ))
	//       	{
	//       	IMPORTANT_INFO("Check extern high_leve failed,err is %d ,extern_mem_block is %0x .\n",err,mem_block_temp);
	//       	return HIGH_LEVEL_TRY_LOCK;  //��ˮλ������
	//       	}
 //       else 
 //       	{
 //       		IMPORTANT_INFO("Check extern high_leve ok\n");
 //       		HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock))
 //       	}
 //       }           	
 //                   	
 //			return ALLOCTE_RECODR_TOO_TIMES;
 //		}
	}while(TRY_LOCK == err || HIGH_LEVEL_TRY_LOCK ==err );
	//if(0 == err)IMPORTANT_INFO("mem_table_allocate_record IS %ld\n",*record_ptr);
  DEBUG("mem_table_try_allocate END,record_ptr is %0x,record_num is %ld;\n",*record_ptr,(*record_ptr)->record_num);	
//	DEBUG("mem_table_allocate_record() ok;\n");	
	return 0;
	
}

//����n�������Ŀ��м�¼�У�ֻ����һ�����з���
//inline int mem_table_try_allocate_n_record(struct mem_table_t *mem_table ,long n,/* out */struct record_t ** record_ptr,long * block_no)
//{
//	if( NULL == mem_table )  return ALLOCATE_N_RECORD_ERR_TABLE_IS_NULL;
//	if( n <= 0 ) return ALLOCATE_RECORD_ERR_NUM_IS_ZERO;
//	int i=0;
//	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
//	
//	HIGH_LEVEL_LOCK(&(mem_block_temp->high_level_lock));
//	
//  //�������п�,���ҿ��Բ�������n�����ݵ��ڴ��			
//	for(;i<mem_table->config.mem_block_used;++i)
//	{
//		if(mem_block_temp->space_start_addr + (n + mem_block_temp->high_level)* mem_table->record_size < mem_block_temp->space_end_addr - mem_table->record_size )break;
//			if(i!=mem_table->config.mem_block_used-1 )mem_block_temp = mem_block_temp->next;      //��һ����
//	}
//	
//	  //���ݸ�ˮλ�߻�ȡ���²���λ��
//    
//   	  
//	// �ҵ����õļ�¼λ��
//	*record_ptr = (struct record_t *) ( mem_block_temp->space_start_addr + mem_block_temp->high_level * mem_table->record_size );
//	//���ؿ���߼���
//	*block_no = mem_block_temp->block_no;
//	   
//
//       unsigned  long  high_level_temp = mem_block_temp->high_level;
//       // ��ˮλ��δ��������������n-1��,��������n��
//      // if ( mem_block_temp->high_level != 0 )  
//  	  // {
//            mem_block_temp->high_level += n;
//      // } 
//      // else
//      // 	    mem_block_temp->high_level += n-1;
//      HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock));
//	
//	// û�������Ŀ��пռ䣬���Զ�����
//	if (i == mem_table->config.mem_block_used)
//{ 
//	     	      //�Զ�����
//	     	      struct  mem_block_t * extern_mem_block;
//	     	      int err;
//	     	      err =  mem_table_extend(mem_table,&extern_mem_block);
//	     	      if(0 == err)
//	     	      	{
//	     	      			//�Զ�����ɹ�,��¼������չ���еĵ�һ��λ��
//	     	      			if ( (extern_mem_block->space_end_addr-extern_mem_block->space_end_addr)>mem_table->record_size)
//	     	      		    {
//	     	      		      *record_ptr =(struct record_t *) extern_mem_block->space_start_addr; 	
//	     	      		      //��չ���ˮλ�ߵ���
//                            HIGH_LEVEL_LOCK(&(extern_mem_block->high_level_lock));
//                            high_level_temp =  extern_mem_block->high_level;
//                            extern_mem_block->high_level +=(n-1);
//                            HIGH_LEVEL_UNLOCK(&(extern_mem_block->high_level_lock));
//                            //���ؿ���߼���
//														*block_no = extern_mem_block->block_no;
//                      }
//	     	      		  else return  MEM_TABLE_EXTEND_ERR;
//	     	      	}
//	     	      else if(TRY_LOCK == err)return TRY_LOCK;
//	     	      else return  MEM_TABLE_EXTEND_ERR;
//}
//     //�������Ŀռ��ʼ��
//     long j = 0;
//     struct record_t ** record_temp = record_ptr;
//     for(;j<n;++j)
//     {
//     (*record_ptr)->record_num= high_level_temp +j ;
//		 (*record_temp)->is_used =  1;
//     (*record_temp)->last_free_pos =  0;
//     (*record_temp)->scn           =  0;
//     (*record_temp)->undo_record_ptr= 0;
//     (*record_temp)->data    =  (char *)(*record_ptr) + RECORD_HEAD_SIZE;  
//     row_lock_init(&((*record_temp)->row_lock)); //������ʼ��
//     *record_temp = (struct record_t *)(((off_t) record_temp + mem_table->record_size));
//     }
//	return 0;
//}
//
////����һ�����м�¼��
//inline int mem_table_allocate_n_record(struct mem_table_t *mem_table ,long n,/* out */struct record_t ** record_ptr,long * block_no)
//{
//	if( NULL == mem_table )  return ALLOCATE_N_RECORD_ERR_TABLE_IS_NULL;
//  int err;
//	do{
//		err= mem_table_try_allocate_n_record(mem_table , n,/* out */ record_ptr, block_no);
//		if(0!= err && TRY_LOCK != err )
//  	{
//  		return err;
//  	} 
//	}while(TRY_LOCK == err );
//	return 0;
//	
//}

//����һ����¼������
inline int mem_table_insert_record(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no, /* in */char *buf)
{
	if( NULL == mem_table )
		{
			ERROR("INSERT_RECORD_ERR_TABLE_IS_NULL\n");
			return INSERT_RECORD_ERR_TABLE_IS_NULL;
		}  
	if( NULL == buf       )
		{
			ERROR("INSERT_RECORD_ERR_BUF_IS_NULL\n");
			return INSERT_RECORD_ERR_BUF_IS_NULL;	
		}  
	DEBUG("Enter mem_table_insert_record();\n");	
  int err;
  err= mem_table_allocate_record(mem_table ,/* out */ record_ptr,block_no);
  DEBUG("mem_table allocate a record,record_ptr is %0x ;\n",*record_ptr);	
	if(0!= err)
  {
  	ERROR("mem_table_allocate_record err \n");
  	return err;
  } 
  DEBUG("record_ptr->data is %0x ;\n",(*record_ptr)->data);	
	//redo_log .......
	
	row_wlock   (  &((*record_ptr)->row_lock) );
	memcpy      (  (*record_ptr)->data,  buf, mem_table->record_size - RECORD_HEAD_SIZE );
	row_wunlock (  &((*record_ptr)->row_lock) );
	return 0;
}

//ɾ��ȫ������
//��ȫ���������������������
inline int mem_table_delete_all(struct mem_table_t *mem_table )
{
	if( NULL == mem_table        ) return READ_RECORD_ERR_TABLE_IS_NULL;
	
  mem_table_lock(&(mem_table->table_locker) ); //����
  // ��������
 	int i=0;
 	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
 	//�������п�,���ҿ��Բ������ݵ��ڴ��			
	for(;i<mem_table->config.mem_block_used;++i)
	{
		int j=0;
		struct record_t * record_temp;
		MEM_BLOCK_LOCK     (&(mem_block_temp->block_lock) ); //����
		for(;j<mem_block_temp->high_level ;++j)
		{
			record_temp = (struct record_t *) (mem_block_temp->space_start_addr + j* mem_table->record_size );
			row_wlock   (  &(record_temp->row_lock) );                 // ����
			if(record_temp->is_used)
				{   
					  record_temp->is_used = 0;
					    
            LIST_LOCK(&(mem_block_temp->mem_free_list.list_lock));  // ����������
					    //������ձ�	  
					  record_temp->last_free_pos = mem_block_temp->mem_free_list.head;
            mem_block_temp->mem_free_list.head = record_temp->record_num;
            
            LIST_UNLOCK  (&(mem_block_temp->mem_free_list.list_lock)); // ����������
				}
			row_wunlock   (  &(record_temp->row_lock) );                // ����
			
    }
    MEM_BLOCK_UNLOCK (&(mem_block_temp->block_lock) );//����
    mem_block_temp = mem_block_temp->next;           //��һ����
	}
	 //redo_log
	  mem_table_unlock(&(mem_table->table_locker));                    //����
	  
	return 0;
}

//ɾ��ȫ������
//�����������
inline int mem_table_truncate(struct mem_table_t *mem_table )
{
	if( NULL == mem_table  ) return READ_RECORD_ERR_TABLE_IS_NULL;
	
  mem_table_lock(&(mem_table->table_locker) );         //����


  // ��������
 	int i=0;
 	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
 	//�������п�,���ҿ��Բ������ݵ��ڴ��			
	for(;i<mem_table->config.mem_block_used;++i)
	{

		MEM_BLOCK_LOCK     (&(mem_block_temp->block_lock) ); //����
		mem_block_temp->high_level = 0;//���ո�ˮλ��
		
		LIST_LOCK(&(mem_block_temp->mem_free_list.list_lock));   // ����������
    mem_block_temp->mem_free_list.head = 0;                  //��������
    LIST_UNLOCK  (&(mem_block_temp->mem_free_list.list_lock));  //����������

		int j=0;
		struct record_t * record_temp;
		struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
		for(;j<mem_block_temp->high_level ;++j)
		{
			record_temp = (struct record_t *) (mem_block_temp->space_start_addr + j*  mem_table->record_size );
			if(record_temp->is_used)
			{
			row_wlock     (  &(record_temp->row_lock) );
			record_temp->is_used = 0;
			row_wunlock   (  &(record_temp->row_lock) );
		  }
    }    
    MEM_BLOCK_UNLOCK (&(mem_block_temp->block_lock) ); //����
    mem_block_temp = mem_block_temp->next;            //��һ����
	}		 
		//redo_log
	  mem_table_unlock(&(mem_table->table_locker));        //����
	return 0;
}


//ǿ��ɾ��ȫ������
inline int mem_table_force_truncate(struct mem_table_t *mem_table )
{
	if( NULL == mem_table  ) return READ_RECORD_ERR_TABLE_IS_NULL;
	

  // ��������
 	int i=0;
 	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
 	//�������п�,���ҿ��Բ������ݵ��ڴ��			
	for(;i<mem_table->config.mem_block_used;++i)
	{

		mem_block_temp->high_level = 0;//���ո�ˮλ��
		
    mem_block_temp->mem_free_list.head = 0;                  //��������

		int j=0;
		struct record_t * record_temp;
		struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
		for(;j<mem_block_temp->high_level ;++j)
		{
			record_temp = (struct record_t *) (mem_block_temp->space_start_addr + j*  mem_table->record_size );
			if(record_temp->is_used)
			{
			record_temp->is_used = 0;
		  }
    }    
    mem_block_temp = mem_block_temp->next;            //��һ����
	}		 
		//redo_log
	return 0;
}

//�򿪱�
inline int mem_table_open( struct mem_table_t * mem_table)
{
	if(NULL == mem_table    )  return OPEN_MEM_TABLE_ERR_NULL_TABLE_PTR;
  DEBUG("Begin to open a mem_table.\n");
  DEBUG("config.mem_block_used is %d.\n",mem_table->config.mem_block_used);
	//���ع����ڴ�
	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
	
  DEBUG("mem_block_temp->block_size is %d\n",mem_block_temp->block_size);
  DEBUG("(mem_table)->config.table_name1 is %s\n",(mem_table)->config.table_name );

	int i=0,err;
	for(;i<mem_table->config.mem_block_used;++i)
	{				

		//��ʼ�����ļ���		
		//����= ����_���
		char tail_fix[256];
	  if( 0 == strlen(mem_block_temp->file_name))
		{
		sprintf(tail_fix,"%d\0",i);
		strcat(mem_block_temp->file_name,mem_table->config.table_name);
		strcat(mem_block_temp->file_name,"_");
		strcat(mem_block_temp->file_name,tail_fix);
		}
		DEBUG("mem_block_temp is %0x.\n",mem_block_temp);
		DEBUG("mem_block file_name is %s\n",mem_block_temp->file_name);
	  DEBUG("mem_block_temp->block_size is %d\n",mem_block_temp->block_size);

		INIT_MEM_BLOCK           (mem_block_temp);   //����������ʼ��
		if(0!=(err=mem_block_get_mem_from_os(mem_block_temp))){
			DEBUG("mem_block_get_mem_from_os err is %d.\n",err);
			return err;
			}  //�����ڴ�
		DEBUG("mem_block_get_mem_from_os() ok\n");

		err = mem_block_mmap           (mem_block_temp);  //ӳ���ļ�
		if(err != 0)break;
	  DEBUG("mem_block_mmap() ok\n");

		mem_block_temp = mem_block_temp->next;      //��һ����
	}
	if(err != 0)return err;
	return 0;
}	
	

	
	
//����
inline int mem_table_create(
                             struct mem_table_t        **  mem_table,
                             struct mem_table_config_t *  mem_table_config
                             )
{
	
	if(NULL == mem_table_config                  )  return CREATE_MEM_TABLE_ERR_NULL_CONFIG_PTR;
	if(NULL == mem_table_config->fields_table    )  return CREATE_MEM_TABLE_ERR_NULL_FIELD_INFO_PTR;
	if(NULL == mem_table_config->mem_blocks_table)  return CREATE_MEM_TABLE_ERR_NULL_MEM_BLOCK;
	if(NULL == mem_table_config->table_name      )  return CREATE_MEM_TABLE_ERR_NO_NAME;
	if(strlen (mem_table_config->table_name)>=120)  return CREATE_MEM_TABLE_ERR_TOO_LONG;
	DEBUG("Begin to greate a mem_table.\n");
  
	DEBUG("mem_table_config->mem_blocks_table->file_name is %s\n",mem_table_config->mem_blocks_table->file_name );
  
	//����
	(*mem_table) = (struct mem_table_t *)malloc(MEM_TABLE_SIZE);
	
  IMPORTANT_INFO("Create table ,table's addr is %0x \n",*mem_table);
	// ��ʼ��������Ϣ
	INIT_MEM_TABLE((*mem_table));  
	memcpy(&((*mem_table)->config),mem_table_config,MEM_TABLE_CONFIG_SIZE);
	strcpy((*mem_table)->config.table_name , mem_table_config->table_name );   
	 allocate_table_no(&((*mem_table)->config.mem_table_no));//�����߼����
	 set_table_no_addr((*mem_table)->config.mem_table_no,(void *) (*mem_table)); 
	//(*mem_table)->config.fields_table      = mem_table_config->fields_table;
	//(*mem_table)->config.field_used_num    = mem_table_config->field_used_num;
	//(*mem_table)->config.mem_blocks_table  = mem_table_config->mem_blocks_table;
	//(*mem_table)->config.mem_block_used    = mem_table_config->mem_block_used;
	
	regist_opened_table_name(mem_table_config->table_name,(*mem_table)->config.mem_table_no);
	
	DEBUG("(*mem_table)->config.table_name1 is %s\n",(*mem_table)->config.table_name );

	//��������ֶ���Ϣ�Ĵ�С
  fill_table_field_size ( *mem_table );  
  
  DEBUG("mem_table_config->mem_blocks_table block_size is %d\n",mem_table_config->mem_blocks_table->block_size );
  DEBUG("(*mem_table)->config.table_name2 is %s\n",(*mem_table)->config.table_name );

	//redo_log
	int err;
	err = mem_table_open(*mem_table);

	return err;
}
//ӳ����ļ�
inline int mem_table_msync(struct mem_table_t *  mem_table)
{
	if(NULL == mem_table    )  return CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR;
	mem_table_lock(&(mem_table->table_locker) );       //����
  
	int i;
	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
	for(;i<mem_table->config.mem_block_used;++i)
	{
		MEM_BLOCK_LOCK              (&(mem_block_temp->block_lock));    //����
    mem_block_msync        (mem_block_temp);                   //����ͬ�����ļ�
		//redo_log....
    MEM_BLOCK_UNLOCK        (&(mem_block_temp->block_lock));    //����
    mem_block_temp = mem_block_temp->next;                     //��һ����
	}
	  mem_table_unlock (&(mem_table->table_locker));      //����
		if(mem_table)free(mem_table);
		return 0;
	}

//�ر�
inline int mem_table_close(struct mem_table_t *  mem_table)
{
	if(NULL == mem_table    )  return CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR;
	DEBUG("Begin to close a mem_table��%s.\n",mem_table->config.table_name);

	mem_table_lock(&(mem_table->table_locker) );       //����
  			  
	long i = 0;
	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
	DEBUG("mem_block_temp is %0x.\n",mem_block_temp);
	for(;i<mem_table->config.mem_block_used;++i)
	{
		mem_block_munmap       (mem_block_temp);               		 //ӳ����ļ�
		mem_block_put_mem_to_os(mem_block_temp);               		 //�ͷ��ڴ�
		//redo_log....
    mem_block_temp = mem_block_temp->next;                     //��һ����
	}

  	del_table_no_addr(mem_table->config.mem_table_no);      ////////////////////////////    
	  mem_table_unlock (&(mem_table->table_locker));      //����
	  DEBUG("Free mem_table_ptr.\n");

		mem_table_dest(&(mem_table->table_locker));
    table_rwlock_destroy(&(mem_table->rwlocker));
    
    unregist_table_name(mem_table->config.table_name);
    
	if(mem_table)
		{

			if(mem_table->config.fields_table    )
			{
				DEBUG("free (mem_table->config.fields_table)\n");
				free(mem_table->config.fields_table    );
			  

				mem_table->config.fields_table = NULL;
			}
			if(mem_table->config.mem_blocks_table)
			{
			  DEBUG("free(mem_table->config.mem_blocks_table)\n");
				free(mem_table->config.mem_blocks_table);
				mem_table->config.mem_blocks_table= NULL;
			}
			
			free(mem_table);
			DEBUG("free(mem_table).\n");
			mem_table = NULL;
		}
		DEBUG("mem_table_close() end.\n");

		return 0;
	}

//����
inline int mem_table_extend( struct mem_table_t * mem_table, struct  mem_block_t ** out_mem_block)
{
	if(NULL == mem_table )  return OPEN_MEM_TABLE_ERR_NULL_TABLE_PTR;
	if(0 == mem_table->config.auto_extend ) return MEM_TABLE_EXTEND_ERR_NOT_ALLOW_EXTEND;
	if(NULL == mem_table->config.mem_blocks_table->file_name)
		{
			ERROR("MEM_TABLE_EXTEND_ERR_NAME_IS_NULL\n");
			return MEM_TABLE_EXTEND_ERR_NAME_IS_NULL;
		}
	
  if(0!=mem_table_trylock(&(mem_table->table_locker) ))  //����
  {
  return TRY_LOCK;
  }
  IMPORTANT_INFO("Try to extend table <%s>\n",mem_table->config.table_name);
  mem_table->is_externing = 1;
	struct  mem_block_t * extend_table = (struct  mem_block_t *)malloc(MEM_BLOCK_HEAD_SIZE);

	//ƴ����չ����С
	char file_name[256]="";
	//��ƴ���һ���ļ����ļ���
	strcpy(file_name,mem_table->config.mem_blocks_table->file_name);
	strcat(file_name,"_");
	//��ƴ��ϵͳʱ��
	GetTimeForNAME(file_name)  ;
	//�����չ���ļ���
	DEBUG("extend_table->file_name is %s\n",extend_table->file_name);
	if(extend_table->file_name == NULL)
		{
			ERROR("MEM_TABLE_EXTEND_ERR_NAME_IS_NULL\n");
			mem_table_unlock(&(mem_table->table_locker));  //�����
			return MEM_TABLE_EXTEND_ERR_NAME_IS_NULL;
			
		}
	
	mem_block_config( extend_table ,/*long block_no ,*/ mem_table->config.extend_block_size , file_name ) ;
	//��ʼ����չ��
	INIT_MEM_BLOCK           (extend_table);   //����������ʼ��
	mem_block_get_mem_from_os(extend_table);   //�����ڴ�
	mem_block_mmap           (extend_table);   //ӳ���ļ�
	
	//�ҵ���������һ����
  struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
  int i = 0;
 // IMPORTANT_INFO("mem_table->config.mem_block_used is %d\n",mem_table->config.mem_block_used);
 // IMPORTANT_INFO("BEFORE Extend table,mem_block_temp is %0x\n",mem_block_temp);
  
  for(;i<mem_table->config.mem_block_used-1;++i)
  {
  	 //IMPORTANT_INFO("mem_table->config.mem_block_used is %d\n",mem_table->config.mem_block_used);
  	 mem_block_temp=mem_block_temp->next; 
  }
  //��չ���������һ����
  mem_block_temp->next = extend_table;
  
//  DEBUG("extend_table->high_level_lock is %0x\n",extend_table->high_level_lock );
  
  //ʹ�ÿ���++
	
	 mem_table->is_externing = 0;
   *out_mem_block = extend_table;                  //����Ϊ�����Ŀ�
   ++(mem_table->config.mem_block_used);
   mem_table_unlock(&(mem_table->table_locker));  //�����.
   IMPORTANT_INFO("Extend table ok! mem_table has used [%d] blocks.\n",mem_table->config.mem_block_used);

   //IMPORTANT_INFO("Extend table ok!,Extend table is %0x\n",extend_table);
   
   
	return 0;
}	

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//���������������������������������������� with_freelist ����������������������������������������
//�˺����������û��������еĿ��пռ䣬���Ȼ�������ɾ������
//_____________________with_freelist ����������������������������������������
//����һ�����м�¼��
inline int mem_table_try_allocate_record_with_freelist(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no,unsigned long long  Tn)
{
	if( NULL == mem_table )  return ALLOCATE_RECORD_ERR_TABLE_IS_NULL;
	DEBUG(" ----- Enter mem_table_try_allocate_record() ----- \n");
  /* �����߼�
	* 1. ���ȸ��ݸ�ˮλ��,�ҿ���record
	* 2. �����ˮλ��û�п��пռ�,��ӻ���������տռ�
	* 3. ��������Ҳû�У����Զ�����
	*/
	int i = 0;
	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
  
//  DEBUG("mem_block_temp is %0x\n",mem_block_temp);
  //�������п�,���ҿ��Բ������ݵ��ڴ��			
  short is_need_extern = 1;
 
	for(;i<mem_table->config.mem_block_used-1;++i)
	{
		mem_block_temp = mem_block_temp->next; 
	}
	
	if( 0!= (HIGH_LEVEL_TRYLOCK(&(mem_block_temp->high_level_lock))) )
		{
			//IMPORTANT_INFO("HIGH_LEVEL_TRYLOCK =0\n");
			return HIGH_LEVEL_TRY_LOCK;  //��ˮλ������
		}


	unsigned  long  high_level_temp = mem_block_temp->high_level;  
	if(mem_block_temp->space_start_addr + mem_block_temp->high_level* mem_table->record_size < mem_block_temp->space_end_addr - mem_table->record_size )
	{
		is_need_extern = 0;

   DEBUG("----- try to allocate record _with_freelist by high level -----\n");

 //���ݸ�ˮλ�߻�ȡ���²���λ��    
//   DEBUG("mem_block_temp->high_level is %ld,mem_table->record_size is %ld;\n",high_level_temp,mem_table->record_size);
//   DEBUG("mem_block_temp->space_start_addr is %0x \n",mem_block_temp->space_start_addr);
  
	// �ҵ����õļ�¼λ��
	(*record_ptr) = (struct record_t *) ( (char *)mem_block_temp->space_start_addr + high_level_temp * (mem_table->record_size) );
   
//   DEBUG("allocate_record_ptr is %0x;\n",*record_ptr);

	//���ؿ���߼���
	 (*block_no) = mem_block_temp->block_no;
//   DEBUG("allocate_record's block_no is %ld ;\n",(*block_no));
    ++(mem_block_temp->high_level);
//    DEBUG(" ----- try to allocate record by high level end -----\n");
	}


	//û�ҵ����ڴ���ַ > ������β��ַ,����һ�������
  if (is_need_extern)
	{
		DEBUG(" ----- Try to find record _with_freelist in free_list -----\n");
		 	i=0;
    struct  mem_block_t * mem_block_temp2 = mem_table->config.mem_blocks_table;
  //�������п�,���ҿ��Բ������ݵ��ڴ���еĻ�������			
    for(;i<mem_table->config.mem_block_used;++i)
    {
	      	//�������������ݣ�������ͷȡһ����ַ����		
    if( -1 != mem_block_temp2->mem_free_list.head ) 
       {
       	//������������
       	LIST_LOCK     (  &(mem_block_temp2->mem_free_list.list_lock)  );
       	//ȡ�ÿ�������Ķ�Ӧrecord�ĵ�ַ
       	(*record_ptr) =  (struct record_t *) ( (mem_block_temp2->mem_free_list.head) * mem_table->record_size + mem_block_temp->space_start_addr );
       	DEBUG("Find in Freelist, record_num %ld,record_ptr is %ld,last free pos is %ld\n",mem_block_temp->mem_free_list.head,*record_ptr,(*record_ptr)->last_free_pos);
       	high_level_temp = (*record_ptr)->record_num;////////////////////
       	// ���Ǳ������޸Ĺ����У�����ʹ��
       	if((*record_ptr)->scn < Tn)
       	{
       	//���������head Ϊ��һ�����������λ��
       	mem_block_temp2->mem_free_list.head = (*record_ptr)->last_free_pos;
       	//�����������
       	LIST_UNLOCK   (  &(mem_block_temp2->mem_free_list.list_lock)  );
       	//���ؿ���߼���
       	*block_no = mem_block_temp2->block_no;
       	is_need_extern = 0;
       	//DEBUG("----- Free record space in free_list finded!  -----\n");
        break;
        }
        LIST_UNLOCK   (  &(mem_block_temp2->mem_free_list.list_lock)  );
       }	
       if(i!=mem_table->config.mem_block_used-1 )
       	{
       	mem_block_temp2 = mem_block_temp2->next;      //��һ����
         }
    }
   
 }
// û�п��пռ�ͻ����������Զ�����
	//if (i == mem_table->config.mem_block_used-1)  
	if (is_need_extern)  
{ 
	DEBUG(" ----- Try to externd_table _with_freelist ----- \n");
	     	      //�Զ�����
	     	      struct  mem_block_t * extern_mem_block;
	     	      int err = 0;
	     	      err =  mem_table_extend(mem_table,&extern_mem_block);
	     	      if(0 == err)
	     	      	{
	     	      			DEBUG("extern_mem_block->space_end_addr is %0x\n",extern_mem_block->space_end_addr);
	     	      		  DEBUG("extern_mem_block->space_start_addr is %0x\n",extern_mem_block->space_start_addr);
	     	      		  DEBUG("mem_table->record_size is %d\n",mem_table->record_size);
	     	      			//�Զ�����ɹ�,��¼������չ���еĵ�һ��λ��,���²���
	     	      			IMPORTANT_INFO("Externd_Block OK and Try Allocate Agian!\n");                   
                    HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽��� 	
	     	      			return TRY_LOCK;     	      		  	
	     	      	}
	     	      else if(TRY_LOCK== err)
	     	      	{
	     	      		  DEBUG(" TRY again! \n");
	     	      		  HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
	     	      			return TRY_LOCK;
	     	      	}
	     	      
	     	      else 
	     	      	{
	     	      		ERROR("MEM_TABLE_EXTEND_ERR err is %d\n",err);
	     	      		HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
	     	      		return  err;
	     	      	}
	     	      	
	DEBUG("----- Try to externd table_with_freelist end -----\n");	     	      	
}
//			DEBUG("----- init a record ! -----\n");
	   (*record_ptr)->record_num = high_level_temp;
     (*record_ptr)->is_used    =  1;
     (*record_ptr)->last_free_pos =  -1;
     (*record_ptr)->next_free_pos =  -1;
     (*record_ptr)->scn           =  0;
     (*record_ptr)->undo_record_ptr= 0;
     (*record_ptr)->data    =  (char *)(*record_ptr) + RECORD_HEAD_SIZE;  
     row_lock_init(&((*record_ptr)->row_lock)); //������ʼ��
     HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���

     DEBUG(" ----- Allocate_record_with_freelist >>> %0x , high_level is %ld ----- \n",*record_ptr,high_level_temp);
	   return 0;
	
}


//����һ�����м�¼��
inline int mem_table_allocate_record_with_freelist(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no,unsigned long long  Tn)
{
	if( NULL == mem_table )  return ALLOCATE_RECORD_ERR_TABLE_IS_NULL;
//  DEBUG("Enter mem_table_allocate_record();\n");	
  int err;
  int i = 0;
	do{
		++i;
		err= mem_table_try_allocate_record_with_freelist(mem_table ,/* out */ record_ptr,block_no,Tn);
		if(0!= err && TRY_LOCK != err && HIGH_LEVEL_TRY_LOCK !=err)
  	{
  		return err;
  	} 
  	if( 1 == mem_table->is_externing )
  		{
  			i = 0;
  			continue;
  		}
	}while(TRY_LOCK == err || HIGH_LEVEL_TRY_LOCK ==err );
	//if(0 == err)IMPORTANT_INFO("mem_table_allocate_record IS %ld\n",*record_ptr);
  DEBUG("mem_table_try_allocate_with_freelist END,record_ptr is %0x,record_num is %ld;\n",*record_ptr,(*record_ptr)->record_num);	
//	DEBUG("mem_table_allocate_record() ok;\n");	
	return 0;
	
}


//����һ����¼������
inline int mem_table_insert_record_with_freelist(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no, /* in */char *buf,unsigned long long  Tn)
{
	if( NULL == mem_table )  return INSERT_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == buf       )  return INSERT_RECORD_ERR_BUF_IS_NULL;	
	DEBUG("Enter mem_table_insert_record();\n");	
  int err;
  err= mem_table_allocate_record_with_freelist(mem_table ,/* out */ record_ptr,block_no,Tn);
  DEBUG("mem_table allocate a record,record_ptr is %0x ;\n",*record_ptr);	
	if(0!= err)
  {
  	return err;
  } 
  DEBUG("record_ptr->data is %0x ;\n",(*record_ptr)->data);	
	//redo_log .......
	
	row_wlock   (  &((*record_ptr)->row_lock) );
	memcpy      (  (*record_ptr)->data,  buf, mem_table->record_size - RECORD_HEAD_SIZE );
	row_wunlock (  &((*record_ptr)->row_lock) );
	return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//���������������������������������������� with_rfreelist ����������������������������������������
//�˺����������û��������еĿ��пռ䣬���Ȼ�������ɾ������
//_____________________with_rfreelist ����������������������������������������
//����һ�����м�¼��
inline int mem_table_try_allocate_record_with_rfreelist(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no,unsigned long long  Tn)
{
	if( NULL == mem_table )  return ALLOCATE_RECORD_ERR_TABLE_IS_NULL;
	DEBUG(" ----- Enter mem_table_try_allocate_record() ----- \n");
  /* �����߼�
	* 1. ���ȸ��ݸ�ˮλ��,�ҿ���record
	* 2. �����ˮλ��û�п��пռ�,��ӻ���������տռ䣬���Ȼ�������ɾ������
	* 3. ��������Ҳû�У����Զ�����
	*/
	int i = 0;
	struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;
  
//  DEBUG("mem_block_temp is %0x\n",mem_block_temp);
  //�������п�,���ҿ��Բ������ݵ��ڴ��			
  short is_need_extern = 1;
 
		for(;i<mem_table->config.mem_block_used-1;++i)
		{
			mem_block_temp = mem_block_temp->next; 
		}
		
		if( 0!= (HIGH_LEVEL_TRYLOCK(&(mem_block_temp->high_level_lock))) )
			{
				//IMPORTANT_INFO("HIGH_LEVEL_TRYLOCK =0\n");
				return HIGH_LEVEL_TRY_LOCK;  //��ˮλ������
			}


	unsigned  long  high_level_temp = mem_block_temp->high_level;  
	if(mem_block_temp->space_start_addr + mem_block_temp->high_level* mem_table->record_size < mem_block_temp->space_end_addr - mem_table->record_size )
		{
			is_need_extern = 0;
	
	   DEBUG("----- try to allocate record_with_rfreelist by high level -----\n");
	
	 //���ݸ�ˮλ�߻�ȡ���²���λ��    
	//   DEBUG("mem_block_temp->high_level is %ld,mem_table->record_size is %ld;\n",high_level_temp,mem_table->record_size);
	//   DEBUG("mem_block_temp->space_start_addr is %0x \n",mem_block_temp->space_start_addr);
	  
		// �ҵ����õļ�¼λ��
		(*record_ptr) = (struct record_t *) ( (char *)mem_block_temp->space_start_addr + high_level_temp * (mem_table->record_size) );
	   
	//   DEBUG("allocate_record_ptr is %0x;\n",*record_ptr);
	
		//���ؿ���߼���
		 (*block_no) = mem_block_temp->block_no;
	//   DEBUG("allocate_record's block_no is %ld ;\n",(*block_no));
	    ++(mem_block_temp->high_level);
	//    DEBUG(" ----- try to allocate record by high level end -----\n");
		}


	//û�ҵ����ڴ���ַ > ������β��ַ,����һ�������
  if (is_need_extern)
	{
		DEBUG(" ----- Try to find record _with_rfreelist in free_list -----\n");
		 	i=0;
    struct  mem_block_t * mem_block_temp2 = mem_table->config.mem_blocks_table;
  //�������п�,���ҿ��Բ������ݵ��ڴ���еĻ�������			
    for(;i<mem_table->config.mem_block_used;++i)
    {
	      	//�������������ݣ�������ͷȡһ����ַ����		
    if( -1 != mem_block_temp2->mem_free_list.tail ) 
       {
       	//������������
       	LIST_LOCK     (  &(mem_block_temp2->mem_free_list.list_lock)  );
       	//ȡ�ÿ�������Ķ�Ӧrecord�ĵ�ַ
       	long long  next_free_pos = mem_block_temp2->mem_free_list.tail ;
 
       	(*record_ptr) =  (struct record_t *) ( (next_free_pos) * mem_table->record_size + mem_block_temp->space_start_addr );
       	if( (*record_ptr)->scn < Tn ) 
				{
       	DEBUG("Find in Freelist, record_num %ld,record_ptr is %ld,last free pos is %ld\n",mem_block_temp->mem_free_list.head,*record_ptr,(*record_ptr)->last_free_pos);
       	high_level_temp = (*record_ptr)->record_num;////////////////////
       	//���������head Ϊ��һ�����������λ��
       	mem_block_temp2->mem_free_list.tail = (*record_ptr)->next_free_pos;
       	//�����������
       	LIST_UNLOCK   (  &(mem_block_temp2->mem_free_list.list_lock)  );
       	//���ؿ���߼���
       	*block_no = mem_block_temp2->block_no;

       	is_need_extern = 0;
       	break;
       	}
       	LIST_UNLOCK   (  &(mem_block_temp2->mem_free_list.list_lock)  );

       	//DEBUG("----- Free record space in free_list finded!  -----\n");
      
       }	
       if(i!=mem_table->config.mem_block_used-1 )
       	{
       	mem_block_temp2 = mem_block_temp2->next;      //��һ����
         }
    }
   
 }
// û�п��пռ�ͻ����������Զ�����
	//if (i == mem_table->config.mem_block_used-1)  
	if (is_need_extern)  
{ 
	DEBUG(" ----- Try to externd_table_with_rfreelist ----- \n");
	     	      //�Զ�����
	     	      struct  mem_block_t * extern_mem_block;
	     	      int err = 0;
	     	      err =  mem_table_extend(mem_table,&extern_mem_block);
	     	      if(0 == err)
	     	      	{
	     	      			DEBUG("extern_mem_block->space_end_addr is %0x\n",extern_mem_block->space_end_addr);
	     	      		  DEBUG("extern_mem_block->space_start_addr is %0x\n",extern_mem_block->space_start_addr);
	     	      		  DEBUG("mem_table->record_size is %d\n",mem_table->record_size);
	     	      			//�Զ�����ɹ�,��¼������չ���еĵ�һ��λ��,���²���
	     	      			DEBUG("Externd_Block OK and Try Allocate Agian!\n");                   
                    HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽��� 	
	     	      			return TRY_LOCK;     	      		  	
	     	      	}
	     	      else if(TRY_LOCK== err)
	     	      	{
	     	      		  DEBUG(" TRY again! \n");
	     	      		  HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
	     	      			return TRY_LOCK;
	     	      	}
	     	      
	     	      else 
	     	      	{
	     	      		ERROR("MEM_TABLE_EXTEND_ERR err is %d\n",err);
	     	      		HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���
	     	      		return  err;
	     	      	}
	     	      	
	DEBUG("----- Try to externd table_with_rfreelist end -----\n");	     	      	
}
//			DEBUG("----- init a record ! -----\n");
	   (*record_ptr)->record_num = high_level_temp;
     (*record_ptr)->is_used    =  1;
     (*record_ptr)->last_free_pos =  -1;
     (*record_ptr)->next_free_pos =  -1;
     (*record_ptr)->scn           =  0;
     (*record_ptr)->undo_record_ptr= 0;
     (*record_ptr)->data    =  (char *)(*record_ptr) + RECORD_HEAD_SIZE;  
     row_lock_init(&((*record_ptr)->row_lock)); //������ʼ��
     HIGH_LEVEL_UNLOCK(&(mem_block_temp->high_level_lock)); //��ˮλ�߽���

     DEBUG(" ----- Allocate_record_with_rfreelist >>> %0x , high_level is [%ld] ----- \n",*record_ptr,high_level_temp);
	   return 0;
	
}


//����һ�����м�¼��
inline int mem_table_allocate_record_with_rfreelist(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no,unsigned long long  Tn)
{
	if( NULL == mem_table )  return ALLOCATE_RECORD_ERR_TABLE_IS_NULL;
//  DEBUG("Enter mem_table_allocate_record();\n");	
  int err;
  int i = 0;
	do{
		++i;
		err= mem_table_try_allocate_record_with_rfreelist(mem_table ,/* out */ record_ptr,block_no,Tn);
		if(0!= err && TRY_LOCK != err && HIGH_LEVEL_TRY_LOCK !=err)
  	{
  		return err;
  	} 
  	if( 1 == mem_table->is_externing )
  		{
  			i = 0;
  			continue;
  		}
	}while(TRY_LOCK == err || HIGH_LEVEL_TRY_LOCK ==err );
	//if(0 == err)IMPORTANT_INFO("mem_table_allocate_record IS %ld\n",*record_ptr);
  DEBUG("mem_table_try_allocate_with_rfreelist END,record_ptr is %0x,record_num is %ld;\n",*record_ptr,(*record_ptr)->record_num);	
//	DEBUG("mem_table_allocate_record() ok;\n");	
	return 0;
	
}


//����һ����¼������
inline int mem_table_insert_record_with_rfreelist(struct mem_table_t *mem_table ,/* out */struct record_t ** record_ptr,long * block_no, /* in */char *buf,unsigned long long  Tn)
{
	if( NULL == mem_table )  return INSERT_RECORD_ERR_TABLE_IS_NULL;
	if( NULL == buf       )  return INSERT_RECORD_ERR_BUF_IS_NULL;	
  int err;
  err= mem_table_allocate_record_with_rfreelist(mem_table ,/* out */ record_ptr,block_no,Tn);
	if(0!= err)
  {
  	return err;
  } 
  //DEBUG("record_ptr->data is %0x ;\n",(*record_ptr)->data);	
	
	row_wlock   (  &((*record_ptr)->row_lock) );
	memcpy      (  (*record_ptr)->data,  buf, mem_table->record_size - RECORD_HEAD_SIZE );
	row_wunlock (  &((*record_ptr)->row_lock) );
	return 0;
}


// ������
int mem_table_rwlock_by_reader(struct mem_table_t *  mem_table)
{
	if(NULL == mem_table    )  return CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR;
  return table_rwlock_rdlock(&(mem_table->rwlocker));
}

//�⹲����
int mem_table_rwunlock_by_reader(struct mem_table_t *  mem_table)
{
	if(NULL == mem_table    )  return CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR;
  return table_rwlock_wrlock(&(mem_table->rwlocker));
}

//��ռ��
int mem_table_rwlock_by_writer(struct mem_table_t *  mem_table, long long  trans_no)
{
	if(NULL == mem_table    )  return CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR;
  int ret;
  mem_table->writer_trans_no = trans_no;
  ret = table_rwlock_wrlock(&(mem_table->rwlocker));
  
  return ret;
}

//���ռ��
int mem_table_rwunlock_by_writer(struct mem_table_t *  mem_table,  long long  trans_no)
{
	if(NULL == mem_table    )  return CREATE_MEM_TABLE_ERR_NULL_TABLE_PTR;
  if(mem_table->writer_trans_no != trans_no)
  	{
  		 ERROR("The Trans %d did not lock table %s ,so it can not unlock it!\n",trans_no,mem_table->config.table_name);
  		 return UNLOCK_FAILED;
  	}
  int ret;
  mem_table->writer_trans_no = -1;
  ret = table_rwlock_wrlock(&(mem_table->rwlocker));
  return ret;
}
#ifdef __cplusplus

}

#endif

#endif 