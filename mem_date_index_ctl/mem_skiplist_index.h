
#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_
#ifdef __cplusplus

extern "C" {

#endif
#include"mem_table_ctl.h"
#include"mem_index_no_manager.h"
#include "../util/util_random.h"


#define CREATE_SKIPLIST_INDEX_ERR_TABLE_PTR      38001
#define CREATE_SKIPLIST_INDEX_ERR_NULL_CONFIG    38002
#define CREATE_SKIPLIST_INDEX_ERR_NO_NAME        38003
#define CREATE_SKIPLIST_INDEX_ERR_NAME_TOO_LONG  38004
#define CREATE_SKIPLIST_INDEX_ERR_NO_TABLE_NO    38005
#define OPEN_MEM_SKIPLIST_ERR_NULL_SKIPLIST_PTR  38006
#define CLOSE_MEM_SKIPLIST_INDEX_ERR_NULL_PTR    38007
#define SKIPLIST_INDEX_ERR_NULL_INDEX_PRT        38008
#define SKIPLIST_INDEX_ERR_NULL_KEY_PRT          38009
#define SKIPLIST_INDEX_ERR_BIG_LEVEL             38010
#define SKIPLIST_INDEX_ERR_DELETE_NOT_FOUND      38011
#define SKIPLIST_INDEX_SEARCH_NOT_FOUND					 38012


//��ʱ��д������ʵ������������
#define SKIPLIST_LOCK_T             rwlock_t
#define SKIPLIST_LOCK(x)            rwlock_wlock(x); \
   DEBUG(" SKIPLIST_LOCK() \n");

#define SKIPLIST_UNLOCK(x)          rwlock_wunlock(x); \
   DEBUG("SKIPLIST_UNLOCK \n");

//#define SKIPLIST_TRYLOCK(x)         pthread_spin_trylock(x) 
#define SKIPLIST_RLOCK(x)            rwlock_rlock(x);
#define SKIPLIST_RUNLOCK(x)          rwlock_runlock(x);   
#define SKIPLIST_LOCK_INIT(x)       rwlock_init((x));



//�ڴ� rbtree ����������_�����ڷ�Χ��������
typedef struct  mem_skiplist_index_config_t
{
long                        index_no;                     //������
long                        owner_table_no;               //��������
int               *         field_order;                  //����������˳���
int                         field_num;                    //�������������ֶεĵĸ���
char                        index_name[128];              //������
long                        owner_id;                     //�����������û���ID
unsigned  long              heap_block_size;              //�����ռ�����ô�С
int                         max_level;                    //������ //ע�� ������ level �Ǵ� 1 ��ʼ��
struct mem_random_t         random;
} __attribute__ ((packed, aligned (64))) mem_skiplist_index_config_t;

#define MEM_SKIPLIST_CONFIG_SIZE  sizeof(mem_skiplist_index_config_t)


//�ڴ�skiplist����������_�����ڷ�Χ����
typedef struct  mem_skiplist_index_t
{
struct mem_skiplist_index_config_t   config;                       //hash ����
struct mem_table_t                   *heap_space;                	 //�ѿռ�
mem_skiplist_entry_t                 *root;                        //���ڵ�
SKIPLIST_LOCK_T                      locker;                       //��
struct  mem_skiplist_entry_t				 * nil;                         //�սڵ�
} __attribute__ ((packed, aligned (64))) mem_skiplist_index_t;


#define MEM_SKIPLIST_INDEX_SIZE (sizeof(struct mem_skiplist_index_t))


//��SKIPLIST����
inline int mem_skiplist_create(
														 mem_skiplist_index_t         **  mem_skiplist_index,	
                             struct mem_table_t           *  mem_table,// ԭ��
                             struct mem_skiplist_index_config_t  *  mem_index_config
                             )
 {
  if(NULL == mem_table                          )  return CREATE_SKIPLIST_INDEX_ERR_TABLE_PTR;
	if(NULL == mem_index_config                   )  return CREATE_SKIPLIST_INDEX_ERR_NULL_CONFIG;
	if(NULL == mem_index_config->index_name       )  return CREATE_SKIPLIST_INDEX_ERR_NO_NAME;
	if(strlen (mem_index_config->index_name)>=120 )  return CREATE_SKIPLIST_INDEX_ERR_NAME_TOO_LONG;
  if(mem_index_config->owner_table_no<=0        )  return CREATE_SKIPLIST_INDEX_ERR_NO_TABLE_NO;
  int len = strlen(mem_index_config->index_name);
  DEBUG("Enter mem_skiplist_create() \n");
  DEBUG("Max level is %d \n",mem_index_config->max_level);
  	
  		//1.���� heap_space �� block
     int i=0;
	   struct  mem_block_t * mem_block_temp = mem_table->config.mem_blocks_table;

        //���ɿ�������
       mem_block_t *mb = (mem_block_t *) malloc(MEM_BLOCK_HEAD_SIZE);
	     DEBUG("create mem_skiplist block %0x\n",*mb);
	 
			 
			 char tem_buf[256]={0};
       strcpy(tem_buf,mem_index_config->index_name);
       strcat(tem_buf,"_skip.dat\0");
       
			 mem_block_config( mb  ,mem_index_config->heap_block_size , tem_buf );
			 
			 //2. ���� fields_table
			 struct field_t *_fields_table   = (struct field_t *) malloc(FIELD_SIZE);
		   _fields_table[0].field_type     = FIELD_TYPE_SKIPLIST_ENTRY;

			 // 3.���� mem_table_config_t
		  struct mem_table_config_t * heap_space_config = (struct mem_table_config_t *)malloc(MEM_TABLE_CONFIG_SIZE); 
		  //array_space_config->mem_table_no             = allocat_mem_table_no();
      //array_space_config->owner_id                   = mem_table->owner_id;
      heap_space_config->fields_table               = _fields_table;
      heap_space_config->field_used_num             = 1;
      heap_space_config->mem_block_used             = 1;
      heap_space_config->auto_extend                = 1; //�����Զ���չ
      heap_space_config->mem_blocks_table           = mb;
      heap_space_config->mem_blocks_table->next     = 0;
      strcpy(heap_space_config->table_name,mem_index_config->index_name);
      strcat(heap_space_config->table_name,"_skip\0");
      DEBUG(" skip_space_config->table_name is %s \n",heap_space_config->table_name);

		  heap_space_config->extend_block_size   = mem_index_config->heap_block_size; //�Զ���չ��С

			//4.���� mem_table_t
			struct mem_table_t      *heap_space_temp ;  		
			int err=0;
      err = mem_table_create(
                             &heap_space_temp,
                             heap_space_config
                             );
      DEBUG(" heap_space_temp is %0x \n",heap_space_temp);

                    
       //5 ����  mem_skiplist_index              
      (*mem_skiplist_index) = (mem_skiplist_index_t  *)malloc(MEM_SKIPLIST_INDEX_SIZE);

      allocate_index_no(&((*mem_skiplist_index) -> config.index_no ));
      set_index_no_addr((*mem_skiplist_index) -> config.index_no,(void *)mem_skiplist_index);
      (*mem_skiplist_index) -> config.owner_table_no   = mem_index_config->owner_table_no;
      (*mem_skiplist_index) -> config.field_order      = mem_index_config->field_order;
      (*mem_skiplist_index) -> config.field_num        = mem_index_config->field_num;
      (*mem_skiplist_index) -> config.owner_id         = mem_index_config->owner_id;
      (*mem_skiplist_index) -> config.heap_block_size  =  heap_space_config->mem_blocks_table->block_size;
      (*mem_skiplist_index) -> config.max_level        =  mem_index_config->max_level;
      strcpy((*mem_skiplist_index) -> config.index_name,mem_index_config->index_name);
      //���ö��ڴ�
       (*mem_skiplist_index)->heap_space = heap_space_temp;
        SKIPLIST_LOCK_INIT(&((*mem_skiplist_index)->locker))
       (*mem_skiplist_index)->root      = NULL;                                       
    //  DEBUG(" mem_skiplist_create() ,mem_skiplist_index is %0x \n",(*mem_skiplist_index));

      return err;
}                 	
                             	
                   

//������                             
inline int mem_skiplist_index_open  (mem_skiplist_index_t *  mem_skiplist_index)
{
	
 if(NULL == mem_skiplist_index    )  return OPEN_MEM_SKIPLIST_ERR_NULL_SKIPLIST_PTR;
  
  int err;
	err = mem_table_open(mem_skiplist_index->heap_space);
	return err;
	
	
}
//������
inline	int mem_skiplist_index_close(mem_skiplist_index_t *  mem_skiplist_index)
{
	
		if(NULL == mem_skiplist_index    )  return CLOSE_MEM_SKIPLIST_INDEX_ERR_NULL_PTR;
	  DEBUG("Begin to close a mem_index��%s, addr is %0x .\n",(mem_skiplist_index)->config.index_name,(mem_skiplist_index)->config);
    SKIPLIST_LOCK(&(mem_skiplist_index->locker))
	  int err = 0;
	 //struct mem_table_t *heap_space_temp = mem_skiplist_index->heap_space;
	 //DEBUG("heap_space_temp's name is %s, addr is %0x .\n",heap_space_temp->config.table_name,heap_space_temp);

	  err  = mem_table_close( (mem_skiplist_index)->heap_space);
	  del_index_no_addr(mem_skiplist_index->config.index_no);
	  
    if(mem_skiplist_index)free(mem_skiplist_index);
		SKIPLIST_UNLOCK(&(mem_skiplist_index->locker))
		return err;	
}


/*��ʼ��*/
inline int  mem_skiplist_init(mem_skiplist_index_t *mem_skiplist_index)
{
	DEBUG("Enter mem_skiplist_inint() \n");
	 if(NULL == mem_skiplist_index  )  return SKIPLIST_INDEX_ERR_NULL_INDEX_PRT;   
   
   // ��ʼ�������
   mem_random_init(&(mem_skiplist_index->config.random),0xdeadbeef);
   int err;
   //��ͨ��nil�Ƿ��ǵ�һ��ĵ�һ�����ж��Ƿ��ǵ�һ������
   struct record_t * record_ptr;
   
   get_record_by_record_num(mem_skiplist_index->heap_space,
   													mem_skiplist_index->heap_space->config.mem_blocks_table, 
   													0,&record_ptr);
   // nil �ǿգ�������Ҫ��ʼ��
   if(record_ptr->is_used)return;
   
   
   //DEBUG("Enter mem_skiplist_insert(),insert value is %ld ;\n",key->skiplist_lkey);
       char buf[FIELD_SKIPLIST_ENTRY_SIZE];
       
			 long  block_no;
			 // ��һ��Ϊ nil
			 err = mem_table_insert_record(mem_skiplist_index->heap_space ,&record_ptr,&block_no,buf);
       mem_skiplist_index->nil = (mem_skiplist_entry_t *)((char *)(record_ptr) + RECORD_HEAD_SIZE);
			 // �� max_level ��Ϊ ��level ͷ�ڵ�
			 int i = 1;
			 //struct  record_t* prev_record_ptr;
			 // ���� �� level ͷ�ڵ�
			 for( ;i<mem_skiplist_index->config.max_level+1;++i  )
			 {
			 	DEBUG("Insert level Head Node: %d \n",i);
				err = mem_table_insert_record(mem_skiplist_index->heap_space ,&record_ptr,&block_no,buf);
        if(err)
        	{
        		ERROR("mem_skiplist_insert Err! err is %d\n",err);
        		return err;
        	}
        	        		
        mem_skiplist_entry_t * cur_entry = (mem_skiplist_entry_t *)(((char *)(record_ptr) + RECORD_HEAD_SIZE) );
				cur_entry->right_record_num      =  mem_skiplist_index->nil->record_num;
				cur_entry->right_block_no        =  block_no;
        if( 1 != i)
        	{
        	  //mem_skiplist_entry_t * prev_record_ptr = (mem_skiplist_entry_t *)(((char *)(prev_record_ptr) + RECORD_HEAD_SIZE));
						cur_entry->down_record_num  =  record_ptr->record_num - 1;
        	  cur_entry->down_block_no    =  block_no;
        	  DEBUG("cur_entry->down_record_num is %d,cur_entry->down_block_no  is %d \n ",cur_entry->down_record_num,cur_entry->down_block_no);

        	}	
        	
        //prev_record_ptr = 	record_ptr;
        	
        }
        DEBUG("mem_skiplist_inint end \n");
        return 0;
}

// �������ͷ
inline mem_skiplist_entry_t *  mem_skiplist_getlevel_head(mem_skiplist_index_t *mem_skiplist_index,int head_num)
{
	
   if(NULL == mem_skiplist_index   ){
   	ERROR("SKIPLIST_INDEX_ERR_NULL_INDEX_PRT\n");
   	 return (mem_skiplist_entry_t *)(-1);
   	} 
   	
   	if( 0 >= head_num   ){
   	ERROR("SKIPLIST_LEVEL_ERR_IS_NEGTIVE\n");
   	 return (mem_skiplist_entry_t *)(-1);
   	} 

   if(head_num > mem_skiplist_index->config.max_level ){
   	ERROR("SKIPLIST_INDEX_ERR_BIG_LEVEL,%d > %d .\n",head_num,mem_skiplist_index->config.max_level );
   	 return (mem_skiplist_entry_t *)(-1);
   } 
   	
   mem_skiplist_entry_t * level_head_ptr = (mem_skiplist_entry_t *)( ( (char *)(mem_skiplist_index->nil) + FIELD_SKIPLIST_ENTRY_SIZE   )+ (head_num - 1 ) * ( FIELD_SKIPLIST_ENTRY_SIZE + RECORD_HEAD_SIZE )  );
	 DEBUG("level_head_ptr is %0x \n ", level_head_ptr + RECORD_HEAD_SIZE );

	 
	 return (mem_skiplist_entry_t *)( (char *)level_head_ptr + RECORD_HEAD_SIZE );
}

// �������level
int mem_skiplist_randlevel(mem_skiplist_index_t *mem_skiplist_index) 
{
	// Increase height with probability 1 in kBranching
  static const unsigned int kBranching = 4;
  int height = 1;
  while (height < mem_skiplist_index->config.max_level && ( mem_random_next(&(mem_skiplist_index->config.random) ) % kBranching) == 0 ) {
    height++;
  }
  //assert(height > 0);
  //assert(height <= kMaxHeight);
  return height;
}

int mem_skiplist_insert(mem_skiplist_index_t *mem_skiplist_index, 
												mem_skiplist_entry_t *in,
 												struct  record_t **  inserted
 												)
{
	int maxLevel = mem_skiplist_index->config.max_level;
	int level    = mem_random_next(&(mem_skiplist_index->config.random) );
	return mem_skiplist_insert_help(mem_skiplist_index,
																	NULL,	mem_skiplist_randlevel(mem_skiplist_index) ,
																	in,
																	inserted);
} 												

// ����
int mem_skiplist_insert_help(mem_skiplist_index_t *mem_skiplist_index, 
 												mem_skiplist_entry_t *prev, int level,
 												mem_skiplist_entry_t *in,
 												struct  record_t ** last_insert_skiplist_entry 
 											 )
 {
	// 1������߲㣬��ǰ�̽ڵ����
	mem_skiplist_entry_t * prev_entry;
	
	if(NULL == prev)
	{
	  prev_entry = mem_skiplist_getlevel_head( mem_skiplist_index,level );
	}
	else //���ϲ�ڵ����
	  prev_entry = prev;
	
	DEBUG("prev_entry is %0x\n",prev_entry);	
	//DEBUG("prev_entry->down_record_num is %d ,prev_entry->down_block_no is %d \n ",prev_entry->down_record_num,prev_entry->down_block_no);

		
	//2 �ӵ��㣬�����һ�� entry
	 record_t * right_record;
	 mem_skiplist_entry_t * right_entry;
	 unsigned  long   right_record_num ;
	 long             right_block_no   ;
	 unsigned  long   down_record_num ;
	 long             down_block_no   ;
	 long mem_table_no = mem_skiplist_index->heap_space->config.mem_table_no;
	 
	  struct  record_t * prev_ptr; 
	 int is_countinue ;
	 int have_insert_next_level = 0;
	 int err = 0 ;
	 do{
	 is_countinue = 0;
	 // ���ҵ��� >= key �Ľڵ�
	

	 do{
   prev_ptr  = (struct  record_t *)((char *)prev_entry - RECORD_HEAD_SIZE );
	
	 row_rlock   (  &(prev_ptr->row_lock) );
	 right_record_num = prev_entry->right_record_num;
	 right_block_no   = prev_entry->right_block_no;
	 get_record(mem_skiplist_index->heap_space,right_block_no,right_record_num,&right_record) ;
   right_entry = (mem_skiplist_entry_t *)((char *)(right_record) + RECORD_HEAD_SIZE);
	 if( mem_skiplist_index->nil != right_entry && right_entry->lkey < in->lkey)prev_entry = right_entry;;
	 row_runlock   (  &(prev_ptr->row_lock) );
	 
	}while(right_entry != mem_skiplist_index->nil && right_entry->lkey < in->lkey);
	 
	 if ( right_entry == mem_skiplist_index->nil || right_entry->lkey > in->lkey) 
	 	{
	 		 DEBUG("level is %d \n",level);
	 		//�ǵײ���˲��������������봦�� down �ڵ�
	 	if( level > 1 )
	 		{
	 		struct  record_t * down_ptr = NULL;
	 		down_record_num = prev_entry->down_record_num;
	    down_block_no   = prev_entry->down_block_no;
	    DEBUG("down_record_num iS %d ,down_block_no is %d \n ",down_record_num,down_block_no);
	   
	    get_record_by_3NO(mem_table_no,down_block_no, down_record_num,&down_ptr);
	    mem_skiplist_entry_t * down_entry = (mem_skiplist_entry_t *)( (char *)down_ptr +  RECORD_HEAD_SIZE);
	    
	    //�ݹ������һ��
	    if ( 0 == have_insert_next_level) // ��ֹ�ظ�������һ��
	    {
	    	err = mem_skiplist_insert_help( mem_skiplist_index, down_entry, level - 1, in , &down_ptr);
	      have_insert_next_level = 1;
	    }
	    
	 		if ( down_ptr != NULL ) 
	 				{
	 					DEBUG("Back from mem_skiplist_insert(),now level is %d \n",level);
	 					
				  	//������޸Ĺ���������ѭ��
	 			    if( mem_skiplist_index->nil != right_entry &&  right_entry->lkey < in->lkey )
	 			    	{
	 			    		DEBUG("is_continue =1");
	 			    		is_countinue = 1;
	 			    		continue;
	 			    	}
	 			    // �޸� prev_entry ����	
	 			    	struct record_t * new_record_ptr;
	 			    	long block_no;
	 			    	
	 			    	prev_ptr = (struct  record_t *)((char *)prev_entry - RECORD_HEAD_SIZE );
	 			    	row_wlock   (  &(prev_ptr->row_lock) );
	 			    	//1. �½ڵ��right ָ�� prev_entry �ĺ�̽ڵ�	 			    	
	 			    	in->right_record_num = prev_entry->right_record_num;
	 			    	in->right_block_no   = prev_entry->right_block_no;
	 			    	// ���� down ָ��
	 			    	in->down_record_num  = down_ptr->record_num;
	 			    	
	 			    	struct  mem_block_t * down_block;
	 			    	get_block_by_record(mem_skiplist_index->heap_space ,down_ptr,&down_block);
	 			    	in->down_block_no   = down_block->block_no;
	 			    	
	 			    	//2. �����½ڵ�
	 			    	mem_table_insert_record( mem_skiplist_index->heap_space ,
                          &new_record_ptr,
                          &block_no, /* in */
                          (char *)in
                          );
	 			    	//3. prev_entry �ĺ�� ָ�� �½ڵ�
	 			    	prev_entry->right_record_num = new_record_ptr->record_num;
	 			      prev_entry->right_block_no = block_no;
	 			    	
	 			    	row_wunlock   (  &(prev_ptr->row_lock) );
					}
	 		}
	 		
	 		//��ײ�Ͳ�������
	 		if( level == 1 )
	 			{
	 			    //������޸Ĺ���������ѭ��
	 			    if(right_entry->lkey < in->lkey && right_entry != mem_skiplist_index->nil )
	 			    	{
	 			    		DEBUG("is_countinue =1");
	 			    		is_countinue = 1;
	 			    		continue;
	 			    	}
	 			    // �޸� prev_entry ����	
	 			    
	 			    	struct record_t * new_record_ptr;
	 			    	long block_no;
	 			    	//1. �½ڵ��right ָ�� prev_entry �ĺ�̽ڵ�	 		
	 			    	prev_ptr = (struct  record_t *)((char *)prev_entry - RECORD_HEAD_SIZE );
	 			    	row_wlock   (  &(prev_ptr->row_lock) );
	 			    		 			    	
	 			    	in->right_record_num = prev_entry->right_record_num;
	 			    	in->right_block_no   = prev_entry->right_block_no;
	 			    	//2. �����½ڵ�
	 			    	mem_table_insert_record( mem_skiplist_index->heap_space ,
                          &new_record_ptr,
                          &block_no, /* in */
                          (char *)in
                          );
	 			    	//3. prev_entry �ĺ�� ָ�� �½ڵ�
	 			    	prev_entry->right_record_num = new_record_ptr->record_num;
	 			      prev_entry->right_block_no = block_no;
	 			    	
	 			    	row_wunlock   (  &(prev_ptr->row_lock) );  
	 			}
	 	}
	 	
	  
	}while(is_countinue);
	
	 *last_insert_skiplist_entry = prev_ptr;
	 return err;
} 


inline int mem_skiplist_delete(mem_skiplist_index_t *mem_skiplist_index ,mem_skiplist_entry_t *in )
{
	return mem_skiplist_delete_help( mem_skiplist_index,NULL,0,in	);
}

  // ɾ��ָ��Ԫ��
  // int level = mem_skiplist_index,mem_skiplist_index->config.max_level - 1;

inline int mem_skiplist_delete_help(mem_skiplist_index_t *mem_skiplist_index, mem_skiplist_entry_t *prev, int level ,mem_skiplist_entry_t *in )
 {
 	DEBUG("Enter mem_skiplist_delete_help()\n");
	// 1������߲㣬��ǰ�̽ڵ����
	mem_skiplist_entry_t * prev_entry;
	int level_tmp  ;
	
	if(NULL == prev)
	{
		DEBUG("NO prenode �� Use Max level��\n");	
	  prev_entry = mem_skiplist_getlevel_head( mem_skiplist_index,mem_skiplist_index->config.max_level );
	  level_tmp  = mem_skiplist_index->config.max_level;
	}
	else //���ϲ�ڵ����
	  {
	  	prev_entry = prev;
	  	level_tmp = level;
	  }
	
	 DEBUG("prev_entry is %0x\n",prev_entry);	
	//2 �ӵ��㣬�����һ�� entry
	 record_t * right_record;
	 mem_skiplist_entry_t * right_entry;
	 unsigned  long   right_record_num ;
	 long             right_block_no   ;
	 unsigned  long   down_record_num ;
	 long             down_block_no   ;
	 long mem_table_no = mem_skiplist_index->heap_space->config.mem_table_no;
	 
	 
	 int is_continue ;
	  // ���ҵ��� >= key �Ľڵ�
	 struct  record_t * prev_ptr;
	 
	 do{
	 is_continue = 0;
	 do{
   prev_ptr = (struct  record_t *)((char *)prev_entry - RECORD_HEAD_SIZE );
	
	 row_rlock   (  &(prev_ptr->row_lock) );
	 right_record_num = prev_entry->right_record_num;
	 right_block_no   = prev_entry->right_block_no;
	 get_record(mem_skiplist_index->heap_space,right_block_no,right_record_num,&right_record) ;
   right_entry = (mem_skiplist_entry_t *)((char *)(right_record) + RECORD_HEAD_SIZE);
	 if( right_entry != mem_skiplist_index->nil && right_entry->lkey < in->lkey )prev_entry = right_entry;
	 //�����ǰ���Ҳ�������ת����һ��
	 if( (right_entry == mem_skiplist_index->nil || right_entry->lkey < in->lkey ) && level_tmp != 1 )	
	 	{
	 		struct  record_t * down_ptr = NULL;
	 		down_record_num = prev_entry->down_record_num;
	    down_block_no   = prev_entry->down_block_no;
	    
	    get_record_by_3NO(mem_table_no,down_block_no, down_record_num,&down_ptr);
	    row_runlock   (  &(prev_ptr->row_lock) );
	    prev_entry = (mem_skiplist_entry_t *)( (char *)down_ptr +  RECORD_HEAD_SIZE);
	  	--level_tmp ;
	  	continue;
	  }
	 row_runlock   (  &(prev_ptr->row_lock) );
	 
	}while(right_entry != mem_skiplist_index->nil && right_entry->lkey < in->lkey);
	 
	 if(right_entry == mem_skiplist_index->nil && level_tmp == 1)
	 {
	 	 DEBUG("SKIPLIST_INDEX_ERR_DELETE_NOT_FOUND\n");
	 	 return SKIPLIST_INDEX_ERR_DELETE_NOT_FOUND;//û�ҵ�
	 }
	 
	 //�ҵ���ʱ���ɾ��
	 if (  right_entry->lkey == in->lkey) 
	 	{
	 		DEBUG("Found delete target!");
	 		//�ǵײ���˲��������������봦�� down �ڵ�
	 	if( level_tmp >= 1 ) 
	 		{
	 			// ��ɾ�ϲ�
	 			row_rlock   (  &(right_record->row_lock) );
	 			prev_entry->right_record_num = right_entry->right_record_num;
	 			prev_entry->right_block_no   = right_entry->right_block_no;
	 			row_runlock (  &(right_record->row_lock) );
	 			mem_table_del_record(mem_skiplist_index->heap_space ,right_record);
	 		 
	 		
	 					//�������һ�㣬����Ҫ����ɾ�� down �ڵ��
	 					if( level_tmp > 1 )
	 						{	
	 						struct  record_t * down_ptr = NULL;
	 						row_rlock   (  &(right_record->row_lock) );
	 						down_record_num = right_entry->down_record_num;
	   					down_block_no   = right_entry->down_block_no;
	   					
	   					get_record_by_3NO(mem_table_no,down_block_no, down_record_num,&down_ptr);
	   					mem_skiplist_entry_t * down_entry = (mem_skiplist_entry_t *)( (char *)down_ptr +  RECORD_HEAD_SIZE);
	   					row_runlock   (  &(right_record->row_lock) );
	   				
	    				return mem_skiplist_delete_help( mem_skiplist_index, down_entry ,level_tmp, in);
	    			  }
	    			  
	 		
	    }
}
	
} while(is_continue);

 	DEBUG(" mem_skiplist_delete_help() End\n");

return 0;
}

// ����
inline int mem_skiplist_search(mem_skiplist_index_t *mem_skiplist_index ,mem_skiplist_entry_t *in,mem_skiplist_entry_t ** out,int * level )
 {
 	  DEBUG("Enter mem_skiplist_delete()\n");
	  // 1������߲㣬��ǰ�̽ڵ����
	  mem_skiplist_entry_t * prev_entry;
	  int level_tmp  =  mem_skiplist_index->config.max_level;
   
		DEBUG("Use Max level As prev_entry ��\n");	
	  prev_entry = mem_skiplist_getlevel_head( mem_skiplist_index, level_tmp );

	
	 DEBUG("prev_entry is %0x\n",prev_entry);	
	//2 �ӵ��㣬�����һ�� entry
	 record_t * right_record;
	 mem_skiplist_entry_t * right_entry;
	 unsigned  long   right_record_num ;
	 long             right_block_no   ;
	 unsigned  long   down_record_num ;
	 long             down_block_no   ;
	 long mem_table_no = mem_skiplist_index->heap_space->config.mem_table_no;
	 
	 
	 int is_continue ;
	  // ���ҵ��� >= key �Ľڵ�
	 struct  record_t * prev_ptr;
	 
	 do{
   prev_ptr = (struct  record_t *)((char *)prev_entry - RECORD_HEAD_SIZE );
	
	 row_rlock   (  &(prev_ptr->row_lock) );
	 right_record_num = prev_entry->right_record_num;
	 right_block_no   = prev_entry->right_block_no;
	 get_record(mem_skiplist_index->heap_space,right_block_no,right_record_num,&right_record) ;
   right_entry = (mem_skiplist_entry_t *)((char *)(right_record) + RECORD_HEAD_SIZE);
	 if( right_entry != mem_skiplist_index->nil && right_entry->lkey < in->lkey )prev_entry = right_entry;
	 //�����ǰ���Ҳ�������ת����һ��
	 if( (right_entry == mem_skiplist_index->nil || right_entry->lkey < in->lkey) && level_tmp != 1)	
	 	{
	 		struct  record_t * down_ptr = NULL;
	 		down_record_num = prev_entry->down_record_num;
	    down_block_no   = prev_entry->down_block_no;
	    
	    get_record_by_3NO(mem_table_no,down_block_no, down_record_num,&down_ptr);
	    row_runlock   (  &(prev_ptr->row_lock) );
	    prev_entry = (mem_skiplist_entry_t *)( (char *)down_ptr +  RECORD_HEAD_SIZE);
	  	--level_tmp ;
	  	continue;
	  }
	 row_runlock   (  &(prev_ptr->row_lock) );
	 
	}while(right_entry != mem_skiplist_index->nil && right_entry->lkey < in->lkey);
	
	if(right_entry == mem_skiplist_index->nil && level_tmp == 1)
	 {
	 	 DEBUG("SKIPLIST_INDEX_ERR_DELETE_NOT_FOUND\n");
	 	 return SKIPLIST_INDEX_SEARCH_NOT_FOUND;//û�ҵ�
	 }
	 
	 if (  right_entry->lkey == in->lkey) 
	 {
	 	*out   = right_entry;
	 	*level = level_tmp;
	 	 return 0;
	 }
	 return SKIPLIST_INDEX_SEARCH_NOT_FOUND;
	 
	}


#ifdef __cplusplus

}

#endif

#endif