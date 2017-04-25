#include "../../mem_skiplist_index.h"
#define START_TRANS_REDO_DATA_WRITER_ERR  90008

// �̲߳���
typedef struct thread_param_test
{
	mem_skiplist_index_t *mem_skiplist_index  ;
	mem_table_config_t * mem_config;
// ���Բ�������
struct record_t * record_ptr;
long block_no;
long long  trans_no;
}thread_param_test;



void * thread_fun(void * arg)
{
mem_skiplist_index_t *mem_skiplist_index   = ((thread_param_test*)arg)->mem_skiplist_index ;

long k=0;
//long end = 1000*500*2;
long end = 1000*4;
long array[end];

for(;k<end;++k){
   //��������
mem_skiplist_entry_t index1;
index1.lkey = random();
index1.block_no =1;
index1.record_num = 1;

array[k] = index1.lkey;
struct  record_t * last_insert_skiplist_entry;

mem_skiplist_insert( mem_skiplist_index ,&index1,&last_insert_skiplist_entry );
   											
}

k=0;
//for(;k<end;++k){
//   //��������
//mem_skiplist_entry_t index1;
//index1.lkey = array[k];
//index1.block_no =1;
//index1.record_num = 1;
//
//array[k] = index1.lkey;
//mem_skiplist_delete( mem_skiplist_index ,&index1 );										
//}

}


int create_block(mem_block_t **mb,char * file_name)
{
	DEBUG("in create_block\n");

	//������
	*mb =(mem_block_t *) malloc(MEM_BLOCK_HEAD_SIZE);
	DEBUG("create_block %0x\n",*mb);

	//���ÿ�������
	mem_block_config( *mb  ,1024 , file_name );
		//��ʼ����
	//INIT_MEM_BLOCK(*mb)
	
	DEBUG("fd = %d\n",(*mb)->fd);
	return 0;
}


int create_mem_table_config(mem_table_config_t ** mem_config,mem_block_t *mb,char * table_name)
{
	DEBUG("in create_mem_table_config\n");

	*mem_config = (mem_table_config_t *) malloc(MEM_TABLE_CONFIG_SIZE);

	(*mem_config)->owner_id = 1;
	strcpy((*mem_config)->table_name,table_name);
	
	struct field_t *fields_table  = (struct field_t *) malloc(FIELD_SIZE*2);
	(*mem_config)->fields_table      = fields_table;
	
	fields_table[0].field_type    = FIELD_TYPE_INT;
	strcpy(fields_table[0].field_name,"id\0");
	fields_table[1].field_type   = FIELD_TYPE_INT;
	strcpy(fields_table[1].field_name,"try_num\0");
	
	(*mem_config)->field_used_num   = 2;
	(*mem_config)->mem_blocks_table = mb;
	(*mem_config)->mem_block_used   = 1;
	(*mem_config)->auto_extend      = 1;
	(*mem_config)->extend_block_size= 1024;
	
	return 0;
}

int create_mem_skiplist_config(mem_skiplist_index_config_t** skiplist_config,
															unsigned  long long heap_block_size,
															char *index_name,
															int level)
{
	*skiplist_config =(mem_skiplist_index_config_t *)malloc(MEM_SKIPLIST_CONFIG_SIZE);
	(*skiplist_config)->index_no			  = 1;
	(*skiplist_config)->owner_table_no  = 1;
	strcpy((*skiplist_config)->index_name,index_name);
	(*skiplist_config)->heap_block_size = heap_block_size;
	(*skiplist_config)->max_level = level;
	
	return 0;
}

/*
* ���� mem_skiplist ��������
*
*
*
*/
int main(int arcv,char * arc[])
{
	init_mem_block_no_manager();
	init_mem_table_no_manager();
  init_mem_index_no_manager();
	
// ��ԭ�������
mem_block_t *mb = NULL;
create_block(&mb,"./skiplist_table.dat");
//��ԭ������
mem_table_config_t * mem_config = NULL;
int err;
if(0!=(err=create_mem_table_config(&mem_config,mb,"./first_table_name\0")))DEBUG("create_mem_table_config err is %d\n",err);
DEBUG("mem_config is %d\n",mem_config);
//��ԭ��
struct mem_table_t  			* mem_table 			= NULL;
if(0!=(err=mem_table_create(&mem_table,mem_config)))DEBUG("mem_table_create err is %d\n",err);
	
	

//��������������
mem_skiplist_index_config_t * skiplist_config		= NULL;
//��skiplist������Ϣ
if(0!=(err=create_mem_skiplist_config(&skiplist_config,2048*102400,"./first_index_name\0",4)))DEBUG("mem_table_create err is %d\n",err);
//����������
mem_skiplist_index_t  			* mem_skiplist_index	= NULL;
if(0!=(err=mem_skiplist_create(&mem_skiplist_index,mem_table,skiplist_config)))DEBUG("mem_table_create err is %d\n",err);
// ��ʼ������ 
if(0!=(err=mem_skiplist_init( mem_skiplist_index)))DEBUG("mem_table_create err is %d\n",err);

DEBUG("mem_skiplist_index is %0x\n",mem_skiplist_index);


//�߳���
int tnum = 10;
pthread_t th[tnum];

thread_param_test tpt;
tpt.mem_skiplist_index = mem_skiplist_index;

	printf("main begin at %s\n",GetTime());

int i = 0;
for(;i<tnum;++i){
    int ret;  
   
    ret = pthread_create( &(th[i]), NULL, thread_fun, &tpt ); 
	  if( ret != 0 ){  
        ERROR( "Create log writer thread error!\n");  
        return START_TRANS_REDO_DATA_WRITER_ERR;  
    }  
  }
  
int j = 0;
for(;j<tnum;++j){
    int ret;  
   
    ret = pthread_join( (th[j]), NULL);  
  }

	printf("main  end  at %s\n",GetTime());



if(0!=(err=mem_skiplist_index_close(mem_skiplist_index)))DEBUG("mem_skiplist_index_close err is %d\n",err);

if(0!=(err=mem_table_close(mem_table)))DEBUG("mem_table_close err is %d\n",err);
  dest_mem_table_no_manager();
	dest_mem_block_no_manager();
	dest_mem_index_no_manager();
	return 0;
}
