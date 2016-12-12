#include "../../mem_transaction.h"
#include "../../mem_index_no_manager.h"

// 线程参数
typedef struct thread_param_test
{
	struct mem_table_t  *  mem_table ;
	mem_table_config_t * mem_config;
// 测试插入数据
struct record_t * record_ptr;
long block_no;
unsigned  long long  trans_no;
}thread_param_test;




int create_block(mem_block_t **mb,char * file_name)
{
	DEBUG("in create_block\n");

	//创建块
	*mb =(mem_block_t *) malloc(MEM_BLOCK_HEAD_SIZE);
	DEBUG("create_block %0x\n",*mb);

	//配置块描述符
	mem_block_config( *mb  ,1024*1024*10 , file_name );
	//初始化块
	INIT_MEM_BLOCK(*mb)
	
	DEBUG("fd = %d\n",(*mb)->fd);
	return 0;
}


int create_mem_table_config(mem_table_config_t ** mem_config,mem_block_t *mb,char * table_name)
{
	DEBUG("in create_mem_table_config\n");

	*mem_config = (mem_table_config_t *) malloc(MEM_TABLE_CONFIG_SIZE);

	(*mem_config)->owner_id = 1;
	strcpy((*mem_config)->table_name,table_name);
	
	struct field_t *fields_table  = (struct field_t *) malloc(FIELD_SIZE*3);
	(*mem_config)->fields_table      = fields_table;
	
	fields_table[0].field_type    = FIELD_TYPE_INT;
	strcpy(fields_table[0].field_name,"id\0");
	fields_table[1].field_type   = FIELD_TYPE_INT;
	strcpy(fields_table[1].field_name,"try_num\0");
	fields_table[2].field_type   = FIELD_TYPE_DATE;
	strcpy(fields_table[2].field_name,"try_time\0");
	
	(*mem_config)->field_used_num   = 3;
	(*mem_config)->mem_blocks_table = mb;
	(*mem_config)->mem_block_used   = 1;
	(*mem_config)->auto_extend      = 1;
	(*mem_config)->extend_block_size= 1024*1024*400;
	
	return 0;
}
void init_trans_redo()
{
//初始化 redo_manager  
init_redo_log_manager();
//配置 config_manager
//config_redo_data_log(char * path,off_t file_max_size)  

// 注意在初始化 事务管理器之前，要先初始化系统序列管理器
init_sys_transaction_manager();
//配置 transaction_manager的 序列值 和  回滚空间最大值
config_sys_transaction_manager(1,DEFAULT_FILE_MAX_SIZE);
 
//扩展事务槽
// extend_transaction_manager();

// 初始化数据日志写者线程
init_trans_redo_data_writer(&sys_trans_redo_data_writer,10);
// 开启 writer 线程写日志
start_trans_redo_data_writer();
}

void dest_trans_redo()
{
	 // 关闭 writer 线程写日志
	 stop_trans_redo_data_writer();
}

void init_mem_no_manager()
{
	init_mem_block_no_manager();
	init_mem_table_no_manager();
	init_mem_index_no_manager();
}

void dest_mem_no_manager()
{
  dest_mem_table_no_manager();
  dest_mem_index_no_manager();
	dest_mem_block_no_manager();
}

void * thread_fun(void * arg)
{

	
struct mem_table_t  *  mem_table = ((thread_param_test*)arg)->mem_table ;
mem_table_config_t * mem_config  = ((thread_param_test*)arg)->mem_config ;
// 测试插入数据
struct record_t * record_ptr     = NULL ;
long block_no = ((thread_param_test*)arg)->block_no ;
unsigned  long long  trans_no    = ((thread_param_test*)arg)->trans_no ;

DEBUG("tpt.mem_config->auto_extend=%d !!!!\n",mem_table->config.auto_extend);


int err;
char buf[]="abcdee";


	//填充1事务，并提交给 缓冲区，供 writer 线程 写掉
mem_trans_data_entry_t * out;
long k=0;
long end = 2000;
mem_transaction_entry_t * trans_entrye =(mem_transaction_entry_t *) malloc(TRANSACTION_SIZE*end);
for(;k<end;++k){
if(0!=(err=mem_table_insert_record(mem_table,&record_ptr,&block_no,buf)))DEBUG("mem_table_insert_record err is %d\n",err);

mem_transaction_entry_t * trans_entry=&trans_entrye[k];
trans_entry->trans_no       				= trans_no;       				//当前事物号
trans_entry->redo_type             = OPT_DATA_UPDATE;				//redo 操作类型
trans_entry->undo_type							= OPT_DATA_UPDATE;				//undo 操作类型 insert update delete truncate index_op
strcpy(trans_entry->name,"try_table\0");          						//表或索引的名字
trans_entry->ori_data_start        = (void *)record_ptr;  		//原始数据起始地址
trans_entry->redo_data_length      = mem_table->record_size; // redo 数据长度
trans_entry->undo_data_length      = mem_table->record_size; // undo 数据长度	
trans_entry->block_no              = block_no;
trans_entry->record_num            = record_ptr->record_num; 
//trans_entry->object_no 						= mem_table->config.mem_table_no;
trans_entry->object_no             = k;         							//表或索引的 no

if(0!=(err=fill_trans_entry_to_write(trans_entry,&out)))DEBUG("fill_trans_entry_to_write failed,trans_no is %d\n",err);
//DEBUG("test trans_entry->redo_data_start is %d,trans_entry->undo_data_start is %d\n",trans_entry.redo_data_start,trans_entry.undo_data_start);

}
	free(trans_entrye);
	return;
}

/*
* 测试 mem_block 基本功能
*
*
*
*/
int main(int arcv,char * arc[])
{
	printf("main begin at %s\n",GetTime());
	int err;
	init_mem_no_manager();
//__________________________________________
	// 初始化事务相关
	init_trans_redo();
	
//   建测试表，并插入数据
//__________________________________________
// 建物理块
mem_block_t *mb = NULL;
create_block(&mb,"./map_table.dat");
//建逻辑表配置
mem_table_config_t * mem_config = NULL;
// 测试插入数据
struct record_t * record_ptr =NULL;
long block_no;




if(0!=(err=create_mem_table_config(&mem_config,mb,"first_table_name\0")))DEBUG("create_mem_table_config err is %d\n",err);
//DEBUG("mem_config is %d\n",mem_config);

//建逻辑表
struct mem_table_t  *  mem_table = NULL;

if(0!=(err=mem_table_create(&mem_table,mem_config)))DEBUG("mem_table_create err is %d\n",err);
	err = 0;
DEBUG("RECORD_HEAD_SIZE is %d\n",RECORD_HEAD_SIZE);




//____________________________________________
//分配一个事务槽
unsigned  long long  trans_no;
if(0!=(err=allocate_trans(&trans_no)))DEBUG("allocate_trans[%d] failed,trans_no is %d\n",trans_no,err); 
// 开始一个事务
if(0!=(err=start_trans(trans_no)))DEBUG("start_trans failed,trans_no is %d\n",err); 


//填充1事务，并提交给 缓冲区，供 writer 线程 写掉
//mem_trans_data_entry_t * out;
//long k=0;
//long end = 1000000;
//mem_transaction_entry_t * trans_entrye =(mem_transaction_entry_t *) malloc(TRANSACTION_SIZE*end);
//for(;k<end;++k){
//mem_transaction_entry_t * trans_entry=&trans_entrye[k];
//trans_entry->trans_no       				= trans_no;       				//当前事物号
//trans_entry->redo_type             = OPT_DATA_UPDATE;				//redo 操作类型
//trans_entry->undo_type							= OPT_DATA_UPDATE;				//undo 操作类型 insert update delete truncate index_op
//strcpy(trans_entry->name,"try_table\0");          						//表或索引的名字
//trans_entry->ori_data_start        = (void *)record_ptr;  		//原始数据起始地址
//trans_entry->redo_data_length      = mem_table->record_size; // redo 数据长度
//trans_entry->undo_data_length      = mem_table->record_size; // undo 数据长度	
//trans_entry->block_no              = block_no;
//trans_entry->record_num            = record_ptr->record_num; 
////trans_entry->object_no 						= mem_table->config.mem_table_no;
//trans_entry->object_no             = k;         							//表或索引的 no
//
//if(0!=(err=fill_trans_entry_to_write(trans_entry,&out)))DEBUG("fill_trans_entry_to_write failed,trans_no is %d\n",err);
////DEBUG("test trans_entry->redo_data_start is %d,trans_entry->undo_data_start is %d\n",trans_entry.redo_data_start,trans_entry.undo_data_start);
//}
//线程数
int tnum =10;
pthread_t th[tnum];

thread_param_test tpt;
tpt.mem_table = mem_table;
tpt.mem_config = mem_config;
tpt.record_ptr = record_ptr;
tpt.block_no = block_no;
tpt.trans_no = trans_no;


int i = 0;
for(;i<tnum;++i){
    int ret;  
   
    ret = pthread_create( &(th[i]), NULL, thread_fun, &tpt ); 
	  if( ret != 0 ){  
        printf( "Create log writer thread error!\n");  
        return START_TRANS_REDO_DATA_WRITER_ERR;  
    }  
  }
  
int j = 0;
for(;j<tnum;++j){
    int ret;  
   
    ret = pthread_join( (th[j]), NULL);  
  }



// 回滚事务
DEBUG("Start rollback_trans!!\n");
if(0!=(err=rollback_trans(trans_no)))DEBUG("rollback_trans failed,trans_no is %d\n",err);
// 提交事务
//if(0!=(err=commit_trans(trans_no)))DEBUG("commit_trans failed,trans_no is %d\n",err); 


//__________________________________________
  
  // 关闭 writer 线程写日志
  dest_trans_redo();
	dest_mem_no_manager();
	
	//关闭表
if(0!=(err=mem_table_close(mem_table)))DEBUG("mem_table_close err is %d\n",err);
	printf("main  end  at %s\n",GetTime());
	return 0;
}
