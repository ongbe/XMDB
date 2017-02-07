#ifndef TRANSACTION_H
#define TRANSACTION_H

#ifdef __cplusplus

extern "C" {

#endif

#include "mem_table_ctl.h"
#include "mem_redo_log.h"
#include "mem_hash_index.h"
#include "mem_rbtree_index.h"
#include "../util/file_util.h"
#include <stdint.h>
#include <stdlib.h>
#include <emmintrin.h>

#ifndef likely
#define likely(x)     __builtin_expect((x), 1)
#endif

#ifndef unlikely
#define unlikely(x)   __builtin_expect((x), 0)
#endif

#define CACHE_LINE_SIZE     64

//��������ring
#define _LOCK_FREE_QUEUE_

#define TRANS_ERR_FULL_USED 						  45001
#define ERR_TRANS_IS_NULL   						  45002
#define ERR_TRANS_ISUSED_ZERO 					  45003
#define TRANS_ERR_QUEUE_ALLOC_FAILED      45004
#define TRANS_ERR_QUEUE_FULL 					    45005
#define TRANS_ERR_QUEUE_EMPTY 				    45006
#define START_TRANS_REDO_DATA_WRITER_ERR  45007
#define TRANS_ERR_STACK_ALLOC_FAILED      45008
#define TRANS_ERR_STACK_FULL 					    45009
#define TRANS_ERR_STACK_EMPTY 				    45010
#define ROLLBACK_SPACE_ERR_TRANS_TOO_BIG  45011
#define RECOVER_ITEM_IS_NULL							45012
#define UNKONW_REDO_TYPE									45013
#define TRANS_ERR_PATH_NULL								45014
#define TRANS_ERR_FILE_NULL								45015
#define NOT_FOUND_TRANSACTION_ENTRY       45016
#define TRANS_ERR_NO_FILE_FOUNDED					45017

//��������еȴ�ʱ��
#define EN_TRANS_DATA_QUEUE_TIME   1000000


// ��չ����
#define    DOUBLE_EXTEND      1
/* ��������
*  ����������ݴ洢��2���ط�������һ������Ŀ¼���ְ����� redoĿ¼��undo Ŀ¼
*  redoĿ¼��undo Ŀ¼ͳ�� transaction_action ����Ҫ��¼��������ͺ���ز���������¼������޸����ݡ�����У���־Ŀ¼�ļ���
*  ��һ���������ݲ��֣������ļ��У����� redo �� undo������.����У���־�����ļ�
*  undo������ ������һ�ݵ��ɵ�ǰ�������Ļع��������У����ڿ��ٻع���
*	 ��������ʱ�����ع�������������commit ʱ���ͷŻع���������
*/

//��ʱ�û�������ʾ������
#define MEM_TRANSACTION_LOCK_T        pthread_mutex_t
#define MEM_TRANSACTION_LOCK(x)       pthread_mutex_lock(x)
#define MEM_TRANSACTION_UNLOCK(x)     pthread_mutex_unlock(x)   
#define MEM_TRANSACTION_LOCK_INIT(x)  pthread_mutex_init(x,NULL)   
#define MEM_TRANSACTION_LOCK_DEST(x)  pthread_mutex_destroy(x)   


//��ʱ����������ʾ������
#define TRANS_QUEUE_LOCK_T        pthread_spinlock_t
#define TRANS_QUEUE_LOCK(x)       pthread_spin_lock(x)
#define TRANS_QUEUE_UNLOCK(x)     pthread_spin_unlock(x)   
#define TRANS_QUEUE_LOCK_INIT(x)  pthread_spin_init(x,0)

//��ʱ�û�������ʾ˯����
#define TRANS_QUEUE_SLEEP_LOCK_T        pthread_mutex_t
#define TRANS_QUEUE_SLEEP_LOCK(x)       pthread_mutex_lock(x)
#define TRANS_QUEUE_SLEEP_UNLOCK(x)     pthread_mutex_unlock(x)   
#define TRANS_QUEUE_SLEEP_LOCK_INIT(x)  pthread_mutex_init(x,0)
#define TRANS_QUEUE_SLEEP_LOCK_DEST(x)  pthread_mutex_destroy(x)   


//��ʱ�û�������������
#define TRANS_QUEUE_SLEEP_COND_T        pthread_cond_t  
#define TRANS_QUEUE_SLEEP_COND_INIT(x)  pthread_cond_init(x, NULL)
#define TRANS_QUEUE_SLEEP_COND_DEST(x)  pthread_cond_destroy(x)
#define TRANS_QUEUE_SLEEP_COND_WAIT(x,y)  pthread_cond_wait(x, y)
#define TRANS_QUEUE_SLEEP_COND_SIGN(x)  pthread_cond_signal(x)


//��ʱ�û�������ʾ������
#define TRANS_WRITER_LOCK_T        pthread_spinlock_t
#define TRANS_WRITER_LOCK(x)       pthread_spin_lock(x)
#define TRANS_WRITER_UNLOCK(x)     pthread_spin_unlock(x)   
#define TRANS_WRITER_LOCK_INIT(x)  pthread_spin_init(x,0)

//��������ü���
#define TRANS_REF_LOCK_T            rwlock_t
#define TRANS_REF_RLOCK(x)   		    rwlock_rlock((x));
#define TRANS_REF_RUNLOCK(x)        rwlock_runlock((x));
#define TRANS_REF_WLOCK(x)   		    rwlock_wlock((x));
#define TRANS_REF_WUNLOCK(x)        rwlock_wunlock((x));
#define TRANS_REF_LOCK_INIT(x)      rwlock_init((x));

//���������scn ��
#define SYS_SCN_LOCK_T            rwlock_t
#define SYS_SCN_RLOCK(x)   		    rwlock_rlock((x));
#define SYS_SCN_RUNLOCK(x)        rwlock_runlock((x));
#define SYS_SCN_WLOCK(x)   		    rwlock_wlock((x));
#define SYS_SCN_WUNLOCK(x)        rwlock_wunlock((x));
#define SYS_SCN_LOCK_INIT(x)      rwlock_init((x));


// redo �� undo �Ĳ�������

// ���￪ʼ
#define OPT_START_TRANS  1
// �������
#define OPT_STOP_TRANS   2
// ���������
#define OPT_DATA_INSERT  3
// �� update ����
#define OPT_DATA_UPDATE  4
// �� delete ����
#define OPT_DATA_DELETE  5
// �� truncat ����
#define OPT_DATA_TRUNC   6
// �� rbtree insert ����
#define OPT_INDEX_RBTREE_INSERT    7
// �� rbtree delete ����
#define OPT_INDEX_RBTREE_DELETE    8
// �� hash insert  ����
#define OPT_INDEX_HASH_INSERT      9
// �� hash delete  ����
#define OPT_INDEX_HASH_DELETE      10
// �� hash update ����
#define OPT_INDEX_HASH_UPDATE      11

// ���ﶯ����һ�����һ��������
//�浽��ʱ���У������Ժ�д��Ӳ�̣���Ϊ��־�����ļ���
typedef struct mem_transaction_entry_t
{
unsigned  long long         		scn;                //����¼���߼�ID
long long                   		trans_no;						//��ǰ�����
time_t                      		create_time;        //action ʱ��
short                       		is_close;           //�Ƿ���д���ر�fd
short                       		redo_type;          //redo ��������
short                       		undo_type;          //undo �������� insert update delete truncate index_op
long                        		object_no;          //��������� no  --- ��Ҫ��ҵ������� ��������
unsigned  long              		block_no;           //���					 --- ��Ҫ��ҵ������� ��������
unsigned  long              		record_num;			    //�к�           --- ��Ҫ��ҵ������� ��������
char                        		name[128];          //�������������
char                        		block_name[128];          //�������������
char                        		log_data_file[128]; //��ǰ��־�����ļ���
long                        		redo_data_start;		// redo �������ļ��е���ʼλ�ã����� index_entry ����ʼλ��
off_t                       		redo_data_length;   // redo ���ݳ���
long                        		undo_data_start;		// undo �������ļ��е���ʼλ��
off_t                       		undo_data_length;   // undo ���ݳ���
void *                      		ori_data_start;     //ԭʼ������ʼ��ַ
void *                      		undo_addr_ptr;      // undo �ڻع����ڴ��еĵ�ַ
//unsigned  long long             ahead_free_pose;    // ���ڻع���������
}  mem_transaction_entry_t;

#define TRANSACTION_ENTRY_SIZE  sizeof(mem_transaction_entry_t)
//#define TRANSACTION_ENTRY_WRITE_SIZE  TRANSACTION_ENTRY_SIZE-2*sizeof(void *)


// ����д��־�� ���ݿ�����
typedef struct mem_trans_data_entry_t
{
mem_transaction_entry_t trans;                // ������һ�����д������
FILE *                  fd  ;                  //Ҫ������ �ļ� fd
struct mem_trans_data_entry_t * next; // ��һ��undo ��ַ
} __attribute__ ((packed, aligned (64))) mem_trans_data_entry_t;

#define TRANS_DATA_ENTRY_SIZE  sizeof(mem_trans_data_entry_t)

#ifndef _LOCK_FREE_QUEUE_
// ������ mem_trans_data_entry_t�� ѭ�����л�����
typedef struct trans_data_queue_t{  
  
    mem_trans_data_entry_t * item;  
    long front;  
    long tear;  
    long max;
    TRANS_QUEUE_LOCK_T       locker;
    TRANS_QUEUE_SLEEP_LOCK_T sleep_locker; //������˯����
    short                    is_sleeping;  //�Ƿ���˯��
    TRANS_QUEUE_SLEEP_COND_T sleep_cond;   //������˯����������
}trans_data_queue_t;  
#else
typedef struct trans_data_queue_t {
    struct {
        uint32_t mask;
        uint32_t size;
        volatile uint32_t first;
        volatile uint32_t second;
    } head;
    char pad1[CACHE_LINE_SIZE - 4 * sizeof(uint32_t)];

    struct {
        uint32_t mask;
        uint32_t size;
        volatile uint32_t first;
        volatile uint32_t second;
    } tail;
    char pad2[CACHE_LINE_SIZE - 4 * sizeof(uint32_t)];
    
    uint32_t max;
    TRANS_QUEUE_SLEEP_LOCK_T sleep_locker; //������˯����
    short                    is_sleeping;  //�Ƿ���˯��
    TRANS_QUEUE_SLEEP_COND_T sleep_cond;   //������˯����������

    mem_trans_data_entry_t * item;
}trans_data_queue_t;
#endif

// ϵͳ�õ����黺���������ڻ���д��־����
static trans_data_queue_t sys_trans_data_queue;

// ������ mem_trans_data_entry_t�� �ع�ջ�����ڻع�����
typedef struct trans_data_stack_t{  
  
    mem_trans_data_entry_t * item;  
    long end;  
    long max;
    TRANS_QUEUE_LOCK_T       locker;
    struct trans_data_stack_t*       next;
}trans_data_stack_t;  

// ����������(��ָһ������)
// ���ݿ�Ϊÿһ�����ﴴ��һ����ʱ�� tmp_table����������־��¼
// ÿ��������ʱ�������ڴ����Ϊ�ع��ռ�
typedef struct mem_transaction_t
{
short                       is_used;							//�Ƿ���ʹ��
unsigned  long long         scn;                	//����¼���߼�ID
time_t                      start_time;         	//���￪ʼʱ��
time_t                      end_time;           	//�������ʱ��
MEM_TRANSACTION_LOCK_T      locker;             	//������
char                        log_index_file[128];  //��ǰ ��־Ŀ¼�ļ���
char                        log_data_file[128];   //��ǰ ��־�����ļ���
struct trans_data_stack_t   rollback_stack;       //�ع�ջ
off_t                       rollback_stack_size;  //�ع�ջ��С
off_t                       rollback_cur_pos;     //�ع���ǰдλ��
long												rollback_space_num; 	//�ع��ռ�����
mem_block_t       *         rollback_space;     	//�ع��ռ��
off_t                       rollback_space_size;	//�ع��ռ��С
long                        ref;                  //����� ���������ü���
TRANS_REF_LOCK_T            ref_locker;           //����������ü�����
} __attribute__ ((packed, aligned (64))) mem_transaction_t;

#define TRANSACTION_SIZE  sizeof(mem_transaction_t)

// ���������ļ�д��־ writer
typedef struct trans_redo_data_writer_t
{
pthread_t               th;                			// ������һ�����д������
void*( *writer_fun)(void *);                			// ��������
short                   stop;              			// ֹͣ��־     
long                    sleep_micro_seconds;    // Ҫ������ �ļ� fd
TRANS_WRITER_LOCK_T     locker;            			// ��
} __attribute__ ((packed, aligned (64))) trans_redo_data_writer_t;

#define TRANS_DATA_WRITER_SIZE  sizeof(trans_redo_data_writer_t)

static trans_redo_data_writer_t sys_trans_redo_data_writer;

// ϵͳ���������
typedef struct sys_transaction_manager_t
{
unsigned  long long					scn;              				//���ڷ���scn�ŵ�����id
mem_transaction_t *         transaction_tables;   		//�����,Ҳ���Խ������
long                        transaction_table_num;		//�����洢�������
MEM_TRANSACTION_LOCK_T      locker;               		//��
off_t                       rollback_space_max_size;	//�ع��ռ����ֵ
int                   			extend_police;            // ��չ���ԣ�Ĭ��ʹ�ö�����չ
} __attribute__ ((packed, aligned (64))) sys_transaction_manager_t;

#define TRANSACTION_MANAGER_SIZE  sizeof(sys_transaction_manager_t)


static sys_transaction_manager_t transaction_manager;


//____________________________________________________________________________________
//�ع����� ÿ�������½�һ��������
//���񴴽���ʱ����������������ʱ������
//�����ع��εĺ���
inline int create_rollback_block(mem_transaction_t * trans);
//��չ�ع���
inline int extern_rollback_block(mem_transaction_t * trans);
//�ͷŻع���
inline int release_rollback_block(mem_transaction_t * trans);

//��ʼ�� ϵͳ���������
inline int init_sys_transaction_manager();

//���� transaction_manager�� ����ֵ ��  �ع��ռ����ֵ
inline int config_sys_transaction_manager(unsigned  long long	scn,off_t rollback_space_max_size);
//���� scn�ı���
inline int save_trans_scn(char * str_path);
//���� scn�ļ���
inline int load_trans_scn(char * str_path);
//���� transaction_manager�� ����ֵ ��  �ع��ռ����ֵ
inline int dest_sys_transaction_manager(unsigned  long long	scn,off_t rollback_space_max_size);
//��չ�����
inline int extend_transaction_manager();

//����һ�������
inline int allocate_trans(  long long * trans_no) ;
// �ͷ������
inline int release_trans(  long long  trans_no);
// ��ʼһ������
inline int start_trans(  long long  trans_no);
// ����һ������
inline int stop_trans(  long long  trans_no);
// �����²���һ�� action ʱ�����ü��� ++
inline int inc_trans_ref(  long long  trans_no);
// ���ü��� --
inline int dec_trans_ref(  long long  trans_no);

// ������������н��̹��õģ�ֻ��һ��
#ifndef _LOCK_FREE_QUEUE_
// ��ʼ��ѭ�����黺����,�ڳ�ʼ�����������ʱ����
inline int init_trans_data_queue(trans_data_queue_t * trans_data_queue,long max);
// ͣ��־��,�ؽ� ������
inline int reinit_trans_data_queue(trans_data_queue_t * trans_data_queue,long max); 
#else
inline int init_trans_data_queue(trans_data_queue_t * trans_data_queue,uint32_t max);
// ͣ��־��,�ؽ� ������
inline int reinit_trans_data_queue(trans_data_queue_t * trans_data_queue,uint32_t max); 

#endif

// �����
inline int en_trans_data_queue(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t* item);
// ������
inline int de_trans_data_queue(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t * item);


//�����־�������
int destroy_trans_data_queue(trans_data_queue_t * trans_data_queue_t);

//�ع�ջ��ÿ��������һ�������񴴽���ʱ����������������ʱ������
// ��ʼ���ع�ջ
inline int init_trans_data_stack(trans_data_stack_t * trans_data_stack,long max);
// ��ջ
inline int push_trans_data_stack(trans_data_stack_t * trans_data_stack, mem_trans_data_entry_t* item,mem_trans_data_entry_t ** out); 
// ��ջ
inline int pop_trans_data_stack(trans_data_stack_t * trans_data_stack, mem_trans_data_entry_t * item);
//��չ�ع�ջ
inline int extend_trans_data_stack(trans_data_stack_t * trans_data_stack);
//����ع�ջ
int destroy_trans_data_stack(trans_data_stack_t * trans_data_stack_t);

// redo_entry д��index�ļ�
inline int redo_file_write_index(mem_transaction_entry_t * trans_entry,FILE * data_fd,mem_trans_data_entry_t ** out);
// ��������� trans_entry �� log_data_file��redo_data_start��undo_data_start����д�ļ�����ֶ�
// ���������������ᣬֻ�Ǵ��߼��ϣ�����д��־�ļ��е�λ�ã�ֻдĿ¼�ļ�����д�����ļ�
inline int fill_trans_entry_to_write(mem_transaction_entry_t * trans_entry,mem_trans_data_entry_t ** out);
// redo_data д��data�ļ�
inline int redo_file_write_data(mem_transaction_entry_t * trans_entry,FILE * fd);
/* �����ǹ���д��־writer �̵߳Ĳ���*/
void * redo_file_writer_fun( void *arg );
// ��ʼ��������־д��
inline int init_trans_redo_data_writer(trans_redo_data_writer_t * writer,long sleep_micro_seconds);

// ���� writer �߳�д��־
inline int start_trans_redo_data_writer();
// �ر� writer �߳�д��־
inline int stop_trans_redo_data_writer();
// �ύ����
inline int commit_trans(  long long  trans_no);

// �ع�����
inline int rollback_trans(  long long  trans_no);


// �����ĳ���ʵ��
inline int redo_recover(mem_transaction_entry_t * item);
//____________________________________________________________________________________
//�����ع���ĺ���

inline int create_rollback_block(mem_transaction_t * trans)
{
	if(NULL==trans)
		{
			ERROR("ERR_TRANS_IS_NULL\n");
			return ERR_TRANS_IS_NULL;
		}
	DEBUG("in create_rollback_block\n");
	//������
	mem_block_t *mb =(mem_block_t *) malloc(MEM_BLOCK_HEAD_SIZE);
	DEBUG("create_block %0x\n",*mb);
  trans->rollback_space = mb;
  
  char rollback_file[128];
  //ƴ���������к�
  sprintf(rollback_file,"rollback_%ld_",trans->scn);
  //ƴ��ʱ��
  GetTimeForNAME(rollback_file);
  
	//���ÿ�������
	mem_block_config( mb  ,trans->rollback_space_size , rollback_file );
	
		//��ʼ����
	INIT_MEM_BLOCK(mb)
	DEBUG("fd = %d\n",(mb)->fd);
	int err;
	if(0!=(err=mem_block_get_mem_from_os(mb))){
			ERROR("mem_block_get_mem_from_os err is %d.\n",err);
			return err;
			}  //�����ڴ�
		DEBUG("trans->rollback_space_num++\n");
		trans->rollback_space_num++;
		DEBUG("mem_block_get_mem_from_os() ok\n");

		err = mem_block_mmap(mb);  //ӳ���ļ�
		if(err != 0)DEBUG("mem_block_get_mem_from_os err is %d.\n",err);;
	  DEBUG("mem_block_mmap() ok\n");
	return 0;
}
//��չ�ع���
inline int extern_rollback_block(mem_transaction_t * trans)
{
	if(NULL==trans)
		{
			ERROR("ERR_TRANS_IS_NULL\n");
			return ERR_TRANS_IS_NULL;
		}
	//MEM_TRANSACTION_LOCK(&(trans->locker));  // ����
  // �½������
  int err;
  mem_block_t *first_mb = trans->rollback_space;
  mem_block_t *last_mb = first_mb;
  
  //�ҵ��ع��οռ�
	long rollback_space_num = trans->rollback_space_num;
	int j = 0;
	DEBUG("rollback_space_num is %d\n",rollback_space_num);
	for(;j<rollback_space_num-1;++j)
	{
		last_mb = last_mb->next;
	}
  DEBUG("last_mb = last_mb->next done!\n");

  err = create_rollback_block(trans);
  last_mb->next = trans->rollback_space;
  trans->rollback_space = first_mb;
  DEBUG("extern_rollback_block(),rollback_space_num is %ld\n",trans->rollback_space_num);
  //MEM_TRANSACTION_UNLOCK(&(trans->locker)); // ����
	return 0;
}

//�ͷŻع���
inline int release_rollback_block(mem_transaction_t * trans)
{
	if(NULL==trans)
		{
			ERROR("ERR_TRANS_IS_NULL\n");
			return	ERR_TRANS_IS_NULL;
		}
	MEM_TRANSACTION_LOCK(&(trans->locker));// ����
  int err;  			  
	long i = 0;
	struct  mem_block_t * mem_block_temp = trans->rollback_space;
	struct  mem_block_t * to_free;
	DEBUG("rollback_space's shmid is %d.\n",mem_block_temp->shmid);
	DEBUG("trans->rollback_space_num is %d.\n",trans->rollback_space_num);
	for(;i<trans->rollback_space_num;++i)
	{
		err = mem_block_munmap       (mem_block_temp);               		 //ӳ����ļ�
		if(err)ERROR("rollback_space's mem_block_munmap() err: %d.\n",err);
		err = mem_block_put_mem_to_os(mem_block_temp);               		 //�ͷ��ڴ�
		if(err)ERROR("rollback_space's mem_block_put_mem_to_os() err: %d.\n",err);
		to_free        = mem_block_temp;													 //��ǰ����Ҫfree�Ŀ�
    mem_block_temp = mem_block_temp->next;                     //��һ����
    free(to_free);
	}
  trans->rollback_space_num = 0;
  MEM_TRANSACTION_UNLOCK(&(trans->locker));// ����
	return 0;
}


//____________________________________________________________________________________
/*
*  �����ǹ��� ϵͳ����������Ĳ���
*/

//��ʼ�� seq_manager ��
// ע���ڳ�ʼ�� ���������֮ǰ��Ҫ�ȳ�ʼ��ϵͳ���й�����
inline int init_sys_transaction_manager()
{
	// ��ʼ�� �����������
	MEM_TRANSACTION_LOCK_INIT(&(transaction_manager.locker));
	//��ʼ��  scn ����
	//allocate_seq_no(&(transaction_manager.scn_seq_no));
	//��ʼ�������
	mem_transaction_t * mem_transaction_temp = (mem_transaction_t *)malloc(DEFAULT_MAX_TRANS_NUM*TRANSACTION_SIZE);
	transaction_manager.transaction_tables 			=  mem_transaction_temp;
	transaction_manager.transaction_table_num 	=  DEFAULT_MAX_TRANS_NUM;
	transaction_manager.rollback_space_max_size =  DEFAULT_ROLLBACK_SPACE_SIZE;
	DEBUG("transaction_manager.transaction_tables is %0x\n",transaction_manager.transaction_tables);

	//��ʼ��������� ������� �� ʹ�ñ�־Ϊ0
	unsigned  long  i = 0;
	mem_transaction_t * trans;
	for(;i<transaction_manager.transaction_table_num;++i)
	{
		trans = &(transaction_manager.transaction_tables[i]);
		MEM_TRANSACTION_LOCK_INIT(&(trans->locker));
		trans->ref = 0;
		TRANS_REF_LOCK_INIT(&(trans->ref_locker));
		trans->rollback_space_size = transaction_manager.rollback_space_max_size;
	}
	
	//��ʼ����־�������
	int err;
	//���濼�� �������С
	err = init_trans_data_queue(&sys_trans_data_queue,DEFAULT_QUEUE_MAX_SIZE);
	if(err)return err;
	return 0;
}

//���� transaction_manager�� ����ֵ ��  �ع��ռ����ֵ
inline int config_sys_transaction_manager(unsigned  long long	scn,off_t rollback_space_max_size)
{
	
	MEM_TRANSACTION_LOCK(&(transaction_manager.locker));
	transaction_manager.scn = scn;
	transaction_manager.rollback_space_max_size = rollback_space_max_size;
	MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker));
	return 0;
}

//���� scn�ı��� �� ����
inline int save_trans_scn(char * str_path)
{
MEM_TRANSACTION_LOCK(&(transaction_manager.locker));  //����
FILE *fp;  // ����һ���ļ�ָ��fp
fp = fopen(str_path, "wt");  // �Զ����ƿ�д��ʽ��stu.dat�ļ�
char buf[512];
sprintf(buf,"%d",transaction_manager.scn);
int ret = !(fwrite(buf, strlen(buf), 1, fp)==1);
int ret2 =  fclose( fp );
MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker));  //����

return ret|ret2;
}

inline int load_trans_scn(char * str_path)
{
int ret	=-1;
int ret2=-1;
if(!access(str_path,0)){
MEM_TRANSACTION_LOCK(&(transaction_manager.locker));  //����
FILE *fp;  // ����һ���ļ�ָ��fp
fp = fopen(str_path, "rt");  // �Զ����ƿ�д��ʽ��stu.dat�ļ�
ret = !(fscanf(fp,"%d",&(transaction_manager.scn)) == 1);
ret2 =  fclose( fp );
MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker));  //����
}
return ret|ret2;
}

//���� transaction_manager�� ����ֵ ��  �ع��ռ����ֵ
inline int dest_sys_transaction_manager(unsigned  long long	scn,off_t rollback_space_max_size)
{
	int err;
	MEM_TRANSACTION_LOCK(&(transaction_manager.locker));   //����
	free(transaction_manager.transaction_tables);			    	//��������
	err = destroy_trans_data_queue(&sys_trans_data_queue);	//����������
	MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker)); //����
	if(err)return err;
	return 0;
}

//��չ�����
inline int extend_transaction_manager()
{
	MEM_TRANSACTION_LOCK(&(transaction_manager.locker));   //����
	
	void   * old =  transaction_manager.transaction_tables;
	void   * _new;
	
	//���ݲ������·����ٲ���ڴ�
	switch(transaction_manager.extend_police)
	{
		case DOUBLE_EXTEND:
	  _new =  malloc( 2*(transaction_manager.transaction_table_num)*(TRANSACTION_ENTRY_SIZE));
	  break;
	
  }
  memcpy(_new,old,transaction_manager.transaction_table_num);
  transaction_manager.transaction_table_num =  2*(transaction_manager.transaction_table_num);
	transaction_manager.transaction_tables    = (mem_transaction_t*) _new;
	
  free(old);
  mem_transaction_t * trans;
  unsigned  long   i = transaction_manager.transaction_table_num/2;
	for(;i<transaction_manager.transaction_table_num;++i)
	{
		trans = &(transaction_manager.transaction_tables[i]);
		trans->is_used = 0;
		MEM_TRANSACTION_LOCK_INIT(&(trans->locker));
		trans->rollback_space_size = transaction_manager.rollback_space_max_size;
	}
  
	MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker)); //����
  return 0 ;
}

//___________________________________________________________________________________
/* �����ǹ�������Ĳ���*/

//����һ�������
//��ϵͳ����̫��δ�ύ������ʱ����Ӱ�������������ܣ������ٿ����Ż�
inline int allocate_trans(  long long * trans_no) 
{
MEM_TRANSACTION_LOCK(&(transaction_manager.locker));  //����
	 long long i = 0;
	 int err,err1;
	 for(;i<transaction_manager.transaction_table_num;++i)
	 {
	 	//printf("seq_tables[%d].is_used=%d,cur_num=%ld\n",i,sys_sequens_manager.seq_tables[i].is_used,sys_sequens_manager.seq_tables[i].cur_num);
	 	if(transaction_manager.transaction_tables[i].is_used == 0)
	 		{
	 			transaction_manager.transaction_tables[i].is_used = 1;
	 			*trans_no =  i;	
	 			DEBUG("allocate_trans_no is %ld\n",i);
	 			break;
	 		}
	}
	//
	if(i<transaction_manager.transaction_table_num)
		{
			transaction_manager.transaction_tables[i].rollback_cur_pos   = 0;
			//������۷���ع���
			err = create_rollback_block(&(transaction_manager.transaction_tables[i]));
			//����ع�ջ
			err1 = init_trans_data_stack(&(transaction_manager.transaction_tables[i].rollback_stack),DEFAULT_STACK_MAX_SIZE/*transaction_manager.transaction_tables[i].rollback_stack_size*/);
		}
MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker));  //����

  if(err!=0)return err;
  if(err1!=0)return err1;
  	//������������������չ���ٷ���ʧ�ܣ�Ӧ��Ӧ�� try_again
  if(i == transaction_manager.transaction_table_num){
  	err = extend_transaction_manager();
  	if(err!=0)return err;
  	ERROR("TRANS_ERR_FULL_USED\n");
  	return TRANS_ERR_FULL_USED;
  }
	return 0;
}

// �ͷ������
inline int release_trans( long long  trans_no)
{	
	DEBUG("release_trans,trans_no is %d\n",trans_no);
MEM_TRANSACTION_LOCK(&(transaction_manager.locker));  //����
	 transaction_manager.transaction_tables[trans_no].is_used = 0;
	 transaction_manager.transaction_tables[trans_no].scn = 0;
	 transaction_manager.transaction_tables[trans_no].start_time = 0;
	 transaction_manager.transaction_tables[trans_no].end_time   = 0;
	 
	 //��������ͷŻع�ջ
	 destroy_trans_data_stack(&(transaction_manager.transaction_tables[trans_no].rollback_stack));
	 //��������ͷŻع���
	 release_rollback_block(&(transaction_manager.transaction_tables[trans_no]));
	 
MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker));  //����

MEM_TRANSACTION_LOCK_DEST(&(transaction_manager.locker));//�ͷ���
	return 0;
	
}

// ��ʼһ������
inline int start_trans( long long  trans_no)
{	
	DEBUG("start_trans_no is %ld\n",trans_no);
	if( transaction_manager.transaction_tables[trans_no].is_used == 0)
		{
			ERROR("ERR_TRANS_ISUSED_ZERO\n");
			return ERR_TRANS_ISUSED_ZERO;
			
		}
	mem_transaction_t * trans = &(transaction_manager.transaction_tables[trans_no]);
	
MEM_TRANSACTION_LOCK(&(trans->locker));  //����
// ��������� scn
MEM_TRANSACTION_LOCK(&(transaction_manager.locker));   //���� 
++transaction_manager.scn;
trans->scn = transaction_manager.scn;
MEM_TRANSACTION_UNLOCK(&(transaction_manager.locker));  //����
// ����ʼʱ��
trans->start_time = get_systime();
trans->ref = 0;
MEM_TRANSACTION_UNLOCK(&(trans->locker));  //����
	return 0;
}

// ����һ������
inline int stop_trans( long long  trans_no)
{	
	DEBUG("stop_trans_no is %ld\n",trans_no);
	if( transaction_manager.transaction_tables[trans_no].is_used == 0)
		{
			ERROR("ERR_TRANS_ISUSED_ZERO\n");
			return ERR_TRANS_ISUSED_ZERO;
		}
	mem_transaction_t * trans = &(transaction_manager.transaction_tables[trans_no]);
	
MEM_TRANSACTION_LOCK(&(trans->locker));  //����
// �������ʱ��
trans->end_time = get_systime();
// �ļ�ˢ����صĲ���--------------------------------------------------

MEM_TRANSACTION_UNLOCK(&(trans->locker));  //����
	return 0;
}

// �����²���һ�� action ʱ�����ü��� ++
inline int inc_trans_ref( long long  trans_no)
{	
	if( transaction_manager.transaction_tables[trans_no].is_used == 0)
		{
			ERROR("ERR_TRANS_ISUSED_ZERO\n");
			return ERR_TRANS_ISUSED_ZERO;
		}
	mem_transaction_t * trans = &(transaction_manager.transaction_tables[trans_no]);
	
TRANS_REF_WLOCK(&(trans->ref_locker));  //����
// �������ʱ��
++trans->ref;
TRANS_REF_WUNLOCK(&(trans->ref_locker));  //����
	return 0;
}

// �����²���һ�� action ʱ�����ü��� --
inline int dec_trans_ref( long long  trans_no)
{	
	if( transaction_manager.transaction_tables[trans_no].is_used == 0)
		{
			ERROR("ERR_TRANS_ISUSED_ZERO\n");
			return ERR_TRANS_ISUSED_ZERO;
		}
	mem_transaction_t * trans = &(transaction_manager.transaction_tables[trans_no]);
	
TRANS_REF_WLOCK(&(trans->ref_locker));  //����
// �������ʱ��
--trans->ref;
TRANS_REF_WUNLOCK(&(trans->ref_locker));  //����
	return 0;
}

//___________________________________________________________________________________
/* �����ǹ���ѭ�����黺�����Ĳ�����صĲ���*/
// ��ʼ��ѭ�����黺����
#ifndef _LOCK_FREE_QUEUE_
inline int init_trans_data_queue(trans_data_queue_t * trans_data_queue,long max)  
{  
    trans_data_queue->item = (mem_trans_data_entry_t*)malloc(max * sizeof(mem_trans_data_entry_t));  
    trans_data_queue->max  = max;
    if(!trans_data_queue->item)  
    {  
        DEBUG("%s\n","Alloc failed,not memory enough");  
        return TRANS_ERR_QUEUE_ALLOC_FAILED;  
    }  
    TRANS_QUEUE_LOCK_INIT(&(trans_data_queue->locker));
    TRANS_QUEUE_SLEEP_LOCK_INIT(&(trans_data_queue->sleep_locker));
    TRANS_QUEUE_SLEEP_COND_INIT(&(trans_data_queue->sleep_cond));
    trans_data_queue->is_sleeping = 1;
    trans_data_queue->front = trans_data_queue->tear = 0;  
  
    return 0;  
}  
 // �����
inline int en_trans_data_queue(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t* item)  
{  
//	  DEBUG("en_trans_data_queue\n"	);
    TRANS_QUEUE_LOCK(&(trans_data_queue->locker));
    if((trans_data_queue->tear+1) % trans_data_queue->max == trans_data_queue->front)  
    {  
    	  DEBUG("trans_data_queue->tear+1 is %d,trans_data_queue->front is %d\n",trans_data_queue->tear+1,trans_data_queue->front);
        printf("%s\n","The trans_data_queue_t is full");
        TRANS_QUEUE_UNLOCK(&(trans_data_queue->locker));  
        return TRANS_ERR_QUEUE_FULL;  
    } 
     
    //DEBUG("trans_data_queue->tear  is %ld \n",trans_data_queue->tear );
    //DEBUG("&(trans_data_queue->item[trans_data_queue->tear]) is %0x \n",&(trans_data_queue->item[trans_data_queue->tear]) );
    //DEBUG("en item->object_no is %ld \n",item->trans.object_no );

    memcpy(&(trans_data_queue->item[trans_data_queue->tear]),item,sizeof(mem_trans_data_entry_t));
		    
    trans_data_queue->tear = (trans_data_queue->tear + 1) % trans_data_queue->max;  
    //����о�������������ü�������ʾδд����־��action
    inc_trans_ref(item->trans.trans_no);
    TRANS_QUEUE_UNLOCK(&(trans_data_queue->locker)); 
    
    // ���д����˯�߾ͻ�����
    if( 1 == trans_data_queue->is_sleeping)
   {
    //DEBUG("en_trans TRANS_QUEUE_SLEEP_LOCK\n"	);
     TRANS_QUEUE_SLEEP_LOCK(&(trans_data_queue->sleep_locker));
        if( 1 == trans_data_queue->is_sleeping)
   		{
   			trans_data_queue->is_sleeping = 0;
   			//DEBUG("TRANS_QUEUE_SLEEP_COND_SIGN\n"	);
   			TRANS_QUEUE_SLEEP_COND_SIGN(&(trans_data_queue->sleep_cond));
   	  }
    TRANS_QUEUE_SLEEP_UNLOCK(&(trans_data_queue->sleep_locker));
     }
    
    return 0;  
}  
  // ������
inline int de_trans_data_queue(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t * item)  
{  
//	  DEBUG("de_trans_data_queue\n"	);
		TRANS_QUEUE_LOCK(&(trans_data_queue->locker));
    if(trans_data_queue->front == trans_data_queue->tear)  
    {  
        DEBUG("%s\n","The trans_data_queue_t is empty"); 
        TRANS_QUEUE_UNLOCK(&(trans_data_queue->locker));  
        return TRANS_ERR_QUEUE_EMPTY;  
    }  

    //DEBUG("de trans_data_queue->front is %d, item->redo_data_start is %ld ,item->undo_data_start is %ld \n",trans_data_queue->front,trans_data_queue->item[trans_data_queue->front].trans.redo_data_start , trans_data_queue->item[trans_data_queue->front].trans.undo_data_start );
    //DEBUG("de item->object_no is %ld \n",trans_data_queue->item[trans_data_queue->front].trans.object_no );
   
    memcpy(item,&(trans_data_queue->item[trans_data_queue->front]),sizeof(mem_trans_data_entry_t));
    //DEBUG("de trans_data_queue->front is %d, item->redo_data_start is %ld ,item->undo_data_start is %ld \n",trans_data_queue->front,item->trans->redo_data_start , item->trans->undo_data_start );
    trans_data_queue->front = (trans_data_queue->front + 1) % trans_data_queue->max;  
    //�����оͼ�����������ü�������ʾ��д����־��action
    //DEBUG("dec_trans_ref(%d)\n",item->trans.trans_no	);
    dec_trans_ref(item->trans.trans_no);
    TRANS_QUEUE_UNLOCK(&(trans_data_queue->locker)); 
    return 0;  
}  

/* �����ǹ���ѭ�����黺�����Ĳ�����صĲ���*/
// ͣ��־��,�ؽ� ������
inline int reinit_trans_data_queue(trans_data_queue_t * trans_data_queue,long max)  
{  
    free(trans_data_queue->item);  
  
    return init_trans_data_queue(trans_data_queue, max);  
} 


 // �����_unsafe
inline int en_trans_data_queue_unsafe(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t* item )  
{  
	  DEBUG("en_trans_data_queue_unsafe\n"	);
    if((trans_data_queue->tear+1) % trans_data_queue->max == trans_data_queue->front)  
    {  
    	  DEBUG("trans_data_queue->tear+1 is %d,trans_data_queue->front is %d\n",trans_data_queue->tear+1,trans_data_queue->front);
        printf("%s\n","The trans_data_queue_t is full");
        return TRANS_ERR_QUEUE_FULL;  
    }  
  
    DEBUG("&(trans_data_queue->item[trans_data_queue->tear]) is %0x \n",&(trans_data_queue->item[trans_data_queue->tear]) );
    memcpy(&(trans_data_queue->item[trans_data_queue->tear]),item,sizeof(mem_trans_data_entry_t));
    //DEBUG("en trans_data_queue->tear is %d,trans->redo_data_start is %ld ,trans->undo_data_start is %ld \n",trans_data_queue->tear,trans_data_queue->item[trans_data_queue->tear].trans.redo_data_start , trans_data_queue->item[trans_data_queue->tear].trans.undo_data_start );
		
		//int k = 0;
    //for(;k<trans_data_queue->tear;++k)DEBUG("K en trans_data_queue->tear is %d,trans->redo_data_start is %ld ,trans->undo_data_start is %ld \n",k,trans_data_queue->item[k].trans.redo_data_start , trans_data_queue->item[k].trans.undo_data_start );
    
    trans_data_queue->tear = (trans_data_queue->tear + 1) % trans_data_queue->max;  
    //����о�������������ü�������ʾδд����־��action
    inc_trans_ref(item->trans.trans_no);
    return 0;  
}  
  // ������_unsafe
inline int de_trans_data_queue_unsafe(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t * item)  
{  
	  DEBUG("de_trans_data_queue_unsafe\n"	);
    if(trans_data_queue->front == trans_data_queue->tear)  
    {  
        DEBUG("%s\n","The trans_data_queue_t is empty"); 
        return TRANS_ERR_QUEUE_EMPTY;  
    }  

    DEBUG("de trans_data_queue->front is %d, item->redo_data_start is %ld ,item->undo_data_start is %ld \n",trans_data_queue->front,trans_data_queue->item[trans_data_queue->front].trans.redo_data_start , trans_data_queue->item[trans_data_queue->front].trans.undo_data_start );
    memcpy(item,&(trans_data_queue->item[trans_data_queue->front]),sizeof(mem_trans_data_entry_t));
    //DEBUG("de trans_data_queue->front is %d, item->redo_data_start is %ld ,item->undo_data_start is %ld \n",trans_data_queue->front,item->trans->redo_data_start , item->trans->undo_data_start );
    trans_data_queue->front = (trans_data_queue->front + 1) % trans_data_queue->max;  
    //�����оͼ�����������ü�������ʾ��д����־��action
    DEBUG("dec_trans_ref(%d)\n",item->trans.trans_no	);
    dec_trans_ref(item->trans.trans_no);
    return 0;  
}  

#else
//lockfree
inline int init_trans_data_queue(trans_data_queue_t * trans_data_queue,uint32_t max)  
{  
    trans_data_queue->item = (mem_trans_data_entry_t*)calloc(1,(max+1) * sizeof(mem_trans_data_entry_t));  
    trans_data_queue->max  = max;
    if(!trans_data_queue->item)  
    {  
        DEBUG("%s\n","Alloc failed,not memory enough");  
        return TRANS_ERR_QUEUE_ALLOC_FAILED;  
    }  
    TRANS_QUEUE_SLEEP_LOCK_INIT(&(trans_data_queue->sleep_locker));
    TRANS_QUEUE_SLEEP_COND_INIT(&(trans_data_queue->sleep_cond));
    trans_data_queue->is_sleeping = 1;
    
    trans_data_queue->head.size = trans_data_queue->tail.size = max;
    trans_data_queue->head.mask = trans_data_queue->tail.mask = max-1;
 
  
    return 0;  
}  
 // �����
inline int en_trans_data_queue(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t* item)  
{  
//	  DEBUG("en_trans_data_queue\n"	);
    //// lock_free push
    uint32_t head, tail, mask, next;
    int ok;
    mask = trans_data_queue->head.mask;
    do {
        head = trans_data_queue->head.first;
        tail = trans_data_queue->tail.second;
        if ((head - tail) > mask)
        	{
        		printf("%s\n","The trans_data_queue_t is full");
            return TRANS_ERR_QUEUE_FULL;
          }
        next = head + 1;
        ok = __sync_bool_compare_and_swap(&trans_data_queue->head.first, head, next);
    } while (!ok);

    memcpy(&(trans_data_queue->item[head & mask]),item,sizeof(mem_trans_data_entry_t));
    //����о�������������ü�������ʾδд����־��action
    inc_trans_ref(item->trans.trans_no);
    asm volatile ("":::"memory");

    while (unlikely((trans_data_queue->head.second != head)))
        _mm_pause();

    trans_data_queue->head.second = next;
    
    
    // ���д����˯�߾ͻ�����
    if( 1 == trans_data_queue->is_sleeping)
   {
    //DEBUG("en_trans TRANS_QUEUE_SLEEP_LOCK\n"	);
     TRANS_QUEUE_SLEEP_LOCK(&(trans_data_queue->sleep_locker));
        if( 1 == trans_data_queue->is_sleeping)
   		{
   			trans_data_queue->is_sleeping = 0;
   			//DEBUG("TRANS_QUEUE_SLEEP_COND_SIGN\n"	);
   			TRANS_QUEUE_SLEEP_COND_SIGN(&(trans_data_queue->sleep_cond));
   	  }
    TRANS_QUEUE_SLEEP_UNLOCK(&(trans_data_queue->sleep_locker));
     }
    
    return 0;  
}  
  // ������
inline int de_trans_data_queue(trans_data_queue_t * trans_data_queue, mem_trans_data_entry_t * item)  
{     
    uint32_t tail, head, mask, next;
    int ok;
    mask = trans_data_queue->tail.mask;

    do {
        tail = trans_data_queue->tail.first;
        head = trans_data_queue->head.second;
        //if ((head - tail) < 1U)
        if ((tail == head) || (tail > head && (head - tail) > mask))
        	{
        		DEBUG("%s\n","The trans_data_queue_t is empty"); 
            return TRANS_ERR_QUEUE_EMPTY;
          }
        next = tail + 1;
        ok = __sync_bool_compare_and_swap(&trans_data_queue->tail.first, tail, next);
    } while (!ok);

    memcpy(item,&(trans_data_queue->item[tail & mask]),sizeof(mem_trans_data_entry_t));
    dec_trans_ref(item->trans.trans_no);
    asm volatile ("":::"memory");

    while (unlikely((trans_data_queue->tail.second != tail)))
        _mm_pause();

    trans_data_queue->tail.second = next;
    return 0;
}  
/* �����ǹ���ѭ�����黺�����Ĳ�����صĲ���*/
// ͣ��־��,�ؽ� ������
inline int reinit_trans_data_queue(trans_data_queue_t * trans_data_queue,uint32_t max)  
{  
    free(trans_data_queue->item);  
  
    return init_trans_data_queue(trans_data_queue, max);  
} 
#endif
 



  //�����־�������
int destroy_trans_data_queue(trans_data_queue_t * trans_data_queue_t)  
{  
	  TRANS_QUEUE_SLEEP_COND_DEST(&(trans_data_queue_t->sleep_cond));
	  trans_data_queue_t->is_sleeping = 0;
	  TRANS_QUEUE_SLEEP_LOCK_DEST(&(trans_data_queue_t->sleep_locker));
    free(trans_data_queue_t->item);  
}  
  
  //___________________________________________________________________________________
/* �����ǹ��ڻع�ջ����ز���*/
// ��ʼ���ع�ջ

inline int init_trans_data_stack(trans_data_stack_t * trans_data_stack,long max)  
{  
    trans_data_stack->item = (mem_trans_data_entry_t*)malloc(max * sizeof(mem_trans_data_entry_t));  
    trans_data_stack->max  = max;
    if(!trans_data_stack->item)  
    {  
        DEBUG("%s\n","Alloc failed,not memory enough");  
        return TRANS_ERR_STACK_ALLOC_FAILED;  
    }  
    TRANS_QUEUE_LOCK_INIT(&(trans_data_stack->locker));
    trans_data_stack->end =  0;  
  
    return 0;  
}  
 // ��ջ stack
inline int push_trans_data_stack(trans_data_stack_t * trans_data_stack, mem_trans_data_entry_t* item,mem_trans_data_entry_t ** out)  
{  
    TRANS_QUEUE_LOCK(&(trans_data_stack->locker));
    //DEBUG("trans_data_stack end is %d,max is %d,item->trans.undo_addr_ptr is %0x\n",trans_data_stack->end,trans_data_stack->max,item->trans.undo_addr_ptr);
    if( trans_data_stack->end >= trans_data_stack->max-1)  
    {  
        DEBUG("%s\n","The trans_data_stack_t is full");
        TRANS_QUEUE_UNLOCK(&(trans_data_stack->locker));  
        return TRANS_ERR_STACK_FULL;  
    }  
  
    *out = &(trans_data_stack->item[trans_data_stack->end]);
    memcpy(&(trans_data_stack->item[trans_data_stack->end]),item,sizeof(mem_trans_data_entry_t));
    ++trans_data_stack->end;  
    TRANS_QUEUE_UNLOCK(&(trans_data_stack->locker)); 
    return 0;  
}  
 // ��ջ
inline int push_trans_data_stack_unsafe(trans_data_stack_t * trans_data_stack, mem_trans_data_entry_t* item,mem_trans_data_entry_t ** out)  
{  
    //DEBUG("trans_data_stack end is %d,item->trans.undo_addr_ptr is %d,item is %0x\n",trans_data_stack->end,trans_data_stack->max,item->trans.undo_addr_ptr);
    if( trans_data_stack->end >= trans_data_stack->max-1)  
    {  
        DEBUG("%s\n","The trans_data_stack_t is full");
        return TRANS_ERR_STACK_FULL;  
    }  
  
    *out = &(trans_data_stack->item[trans_data_stack->end]);
    memcpy(&(trans_data_stack->item[trans_data_stack->end]),item,sizeof(mem_trans_data_entry_t));
    ++trans_data_stack->end;  
    return 0;  
}
  // ��ջ
inline int pop_trans_data_stack(trans_data_stack_t * trans_data_stack, mem_trans_data_entry_t * item)  
{  
	
		TRANS_QUEUE_LOCK(&(trans_data_stack->locker));
		//DEBUG("trans_data_stack->end is %d\n",trans_data_stack->end);
    if(trans_data_stack->end == 0)  
    {  
        printf("%s\n","The trans_data_stack_t is empty"); 
        TRANS_QUEUE_UNLOCK(&(trans_data_stack->locker));  
        return TRANS_ERR_STACK_EMPTY;  
    }  

    memcpy(item,&(trans_data_stack->item[trans_data_stack->end]),sizeof(mem_trans_data_entry_t));
    --trans_data_stack->end;  
    TRANS_QUEUE_UNLOCK(&(trans_data_stack->locker)); 
    return 0;  
}  
  // ��ջ
inline int pop_trans_data_stack_unsafe(trans_data_stack_t * trans_data_stack, mem_trans_data_entry_t * item)  
{  
	
		//DEBUG("trans_data_stack->end is %d\n",trans_data_stack->end);
    if(trans_data_stack->end == 0)  
    {  
        printf("%s\n","The trans_data_stack_t is empty"); 
        return TRANS_ERR_STACK_EMPTY;  
    }  

    memcpy(item,&(trans_data_stack->item[trans_data_stack->end]),sizeof(mem_trans_data_entry_t));
    --trans_data_stack->end;  
    return 0;  
}  

//��չ�ع�ջ
inline int extend_trans_data_stack(trans_data_stack_t * trans_data_stack)
{
	TRANS_QUEUE_LOCK(&(trans_data_stack->locker));
	
	void   * old =  trans_data_stack->item;
	void   * _new;
	
	//Ĭ����2��
	_new =  malloc( 2*(trans_data_stack->max)*(sizeof(mem_trans_data_entry_t)));
	
  memcpy(_new,old,trans_data_stack->max);
  trans_data_stack->max            =  2*(trans_data_stack->max);
	mem_table_no_manager.mem_table_no_table = (void**)_new;
  free(old);
  TRANS_QUEUE_UNLOCK(&(trans_data_stack->locker)); 
  return 0 ;
}


  //����ع�ջ
int destroy_trans_data_stack(trans_data_stack_t * trans_data_stack_t)  
{  
    free(trans_data_stack_t->item);  
}    
  
//___________________________________________________________________________________
/* �����ǹ���д��־��صĲ���*/
// ��дһ��  sys_redo_log_manager ������д��־�ļ�

// redo_entry д��index�ļ�
inline int redo_file_write_index(mem_transaction_entry_t * trans_entry,FILE * data_fd,mem_trans_data_entry_t ** out)
{
	// �����Ҫ�л��ļ�������� fd ���ݴ�
	FILE * to_close_fd = NULL;
	
	REDO_LOG_LOCK(&(redo_log_manager.index_locker));
	FILE * write_fd = redo_log_manager.index_fd;
	//DEBUG("index_fd is %d\n",redo_log_manager.index_fd);
  //DEBUG("trans_entry->name is [%s]\n",trans_entry->name);

	fwrite((void*)trans_entry,TRANSACTION_ENTRY_SIZE,1,write_fd);
	// �����������
	
	//�����ļ���С
  SIZE_LOCK(&(redo_log_manager.index_size_locker));// Ŀ¼�ļ���С����
	redo_log_manager.cur_index_size += TRANSACTION_ENTRY_SIZE;
	SIZE_UNLOCK(&(redo_log_manager.index_size_locker));// Ŀ¼�ļ���С����
	
	
	// ����ļ���С���ˣ��ر����ڵ�Ŀ¼�ļ��������µ�Ŀ¼�ļ������������������
	if(redo_log_manager.cur_index_size >=redo_log_manager.index_max_size)
	{
		DEBUG("Switch redo_log index file,redo_log_manager.cur_index_size is %ld\n",redo_log_manager.cur_index_size);
		to_close_fd = redo_log_manager.index_fd;
		unregist_opened_file(redo_log_manager.index_path);

		
		strcpy(redo_log_manager.index_path,redo_log_manager.path);
		strcat(redo_log_manager.index_path,"/redo_index_\0");
    GetTimeForNAME(redo_log_manager.index_path);
    
    DEBUG("New redo_log_manager.index_path is %s\n",redo_log_manager.index_path);
    //���´����ļ�
    
     //DEBUG("old_fd is %d\n",redo_log_manager.index_fd );
     redo_log_manager.index_fd = fopen(redo_log_manager.index_path,"wb+");
     //DEBUG("new_fd is %d\n",redo_log_manager.index_fd );
    
    regist_opened_file(redo_log_manager.index_path ,redo_log_manager.index_fd);
    
    //����Ŀ¼�ļ���С=0;
    SIZE_LOCK(&(redo_log_manager.index_size_locker));// Ŀ¼�ļ���С����
		redo_log_manager.cur_index_size = 0;
		SIZE_UNLOCK(&(redo_log_manager.index_size_locker));//Ŀ¼�ļ���С����

	}
	//������У��첽д�����ļ�
	mem_trans_data_entry_t val;
	//val.trans = trans_entry;
	memcpy(&(val.trans),trans_entry,TRANSACTION_ENTRY_SIZE);
	val.fd    = data_fd;
	
	// �����ʧ�ܾ�����
	while(en_trans_data_queue(&sys_trans_data_queue,  &val))
	{
		usleep(EN_TRANS_DATA_QUEUE_TIME);
  };
  ///////////////////////////////////////////////////////////////////////////////////
 //���ﴦ��һ�»ع���
	mem_block_t *mb = transaction_manager.transaction_tables[trans_entry->trans_no].rollback_space;
	//�ҵ��ع��οռ�
	long rollback_space_num = transaction_manager.transaction_tables[trans_entry->trans_no].rollback_space_num;
	int j = 0;
	//DEBUG("rollback_space_num is %d\n",rollback_space_num);
	for(;j<rollback_space_num-1;++j)
	{
		mb = mb->next;
	}
	//DEBUG("mb is %0x\n",mb);
	//DEBUG("trans_entry->trans_action is %0x\n",transaction_manager.transaction_tables);
	//DEBUG("transaction_manager.transaction_tables[trans_entry->trans_no] is %0x\n",(char *)(transaction_manager.transaction_tables)+(trans_entry->trans_no)*TRANSACTION_SIZE );
  //DEBUG("transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos is %ld\n",transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos);
  
  MEM_TRANSACTION_LOCK(&(transaction_manager.transaction_tables[trans_entry->trans_no].locker));  // ����
	//����ع��β������������д��
	if(
		//((mem_transaction_t *)((char *)(transaction_manager.transaction_tables)+(trans_entry->trans_no)*TRANSACTION_SIZE ))
		//->rollback_cur_pos+
		transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos+
		trans_entry->undo_data_length 
		> mb->space_size
		 )
	{
		//IMPORTANT_INFO("Before extern_rollback_block,rollback_cur_pos is %ld\n",transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos);

		extern_rollback_block(&(transaction_manager.transaction_tables[trans_entry->trans_no]));
	  
	  IMPORTANT_INFO("extern_rollback_block done!,rollback_space_num is %ld\n",transaction_manager.transaction_tables[trans_entry->trans_no].rollback_space_num);

		transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos = 0;
	  mb = mb->next;
	}
		MEM_TRANSACTION_UNLOCK(&(transaction_manager.transaction_tables[trans_entry->trans_no].locker)); // ����
		
		//��Ҫ�ж�һ�δ�С,����󻹷Ų��»ع����ݵĻ��ͱ���
		if(transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos+trans_entry->undo_data_length > mb->space_size )
	  {
	  REDO_LOG_UNLOCK(&(redo_log_manager.index_locker));//����
	  ERROR("ROLLBACK_SPACE_ERR_TRANS_TOO_BIG\n");
	  return ROLLBACK_SPACE_ERR_TRANS_TOO_BIG;
   }
	
	//�������ݵ��ع���
	memcpy(mb->space_start_addr+transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos
				,trans_entry->ori_data_start
				,trans_entry->undo_data_length
				);
							
  val.trans.undo_addr_ptr = 		mb->space_start_addr+transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos;
  
  
  DEBUG("trans_entry->undo_addr_ptr is %0x\n", val.trans.undo_addr_ptr);
	transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos += trans_entry->undo_data_length;
  /////////////////////////////////////////////////////////////////////////////////// 
  	// ��ع�ջʧ�ܾ�����
	while(
	push_trans_data_stack(&(transaction_manager.transaction_tables[trans_entry->trans_no].rollback_stack),
	 &val, 
	 out
	 ))
	{
		usleep(EN_TRANS_DATA_QUEUE_TIME);
  };
	
	
	REDO_LOG_UNLOCK(&(redo_log_manager.index_locker));//����
  
  //�����Ժ��� close ��߲�������
	if(NULL!=to_close_fd)
		{
			fclose(to_close_fd);
			DEBUG("fclose(to_close_fd);\n");
  
		}
	return 0;
}

// ȱ�ٶ� undo ��֧��

// trans_entry ��scn create_time redo_type undo_type redo_data_length undo_data_length 
// ���������
// ��������� trans_entry �� log_data_file��redo_data_start��undo_data_start�����ֶ�
// ���������������ᣬֻ�Ǵ��߼��ϣ�����д��־�ļ��е�λ�ã�ֻдĿ¼�ļ�����д�����ļ�
inline int fill_trans_entry_to_write(mem_transaction_entry_t * trans_entry,mem_trans_data_entry_t ** out)
{
	REDO_LOG_LOCK(&(redo_log_manager.data_locker));//�����ļ�����

	//�����������ݿռ�,����չ�ļ���С
	
	//��ȡredo �Ŀ�ʼλ�ã����ƶ��ļ���С
	//trans_entry->redo_data_start=ftell(redo_log_manager.data_fd);
	//fseek(redo_log_manager.data_fd,trans_entry->redo_data_length-1,SEEK_END);
	//fwrite("0",1,1,redo_log_manager.data_fd);
	////��ȡundo �Ŀ�ʼλ�ã����ƶ��ļ���С
	//trans_entry->undo_data_start=ftell(redo_log_manager.data_fd);
	//fseek(redo_log_manager.data_fd,trans_entry->undo_data_length-1,SEEK_END);
	//fwrite("0",1,1,redo_log_manager.data_fd);
	
	SIZE_LOCK(&(redo_log_manager.data_size_locker));// �����ļ���С����
//	DEBUG("redo_log_manager.cur_data_size is %d\n",redo_log_manager.cur_data_size);
	trans_entry->create_time = get_systime();
	//  trans_entry->scn = ��������Ӧ�� scn
	trans_entry->scn = transaction_manager.transaction_tables[trans_entry->trans_no].scn;
	trans_entry->redo_data_start = redo_log_manager.cur_data_size;
	trans_entry->undo_data_start = redo_log_manager.cur_data_size + trans_entry->redo_data_length;  

//  DEBUG("fill trans_entry->object_no is %d\n",trans_entry->object_no);
  //�����ļ���С
	redo_log_manager.cur_data_size += trans_entry->redo_data_length + trans_entry->undo_data_length;
	SIZE_UNLOCK(&(redo_log_manager.data_size_locker));// �����ļ���С����
	
	//DEBUG("redo_log_manager.cur_data_size is %ld\n",redo_log_manager.cur_data_size);

	int err = 0;
	//��� entry���ļ���
	strcpy(trans_entry->log_data_file,redo_log_manager.redo_path);
	trans_entry->is_close = 0;
	
	// ����ļ���С���ˣ��ر����ڵ������ļ��������µ������ļ������������������
	if(redo_log_manager.cur_data_size >=redo_log_manager.data_max_size)
	{
		IMPORTANT_INFO("Create new redo_date file,trans_entry is %0x\n",trans_entry);
	  IMPORTANT_INFO("Redo_log_manager.cur_data_size is %ld\n",redo_log_manager.cur_data_size);

		// �ӳٵ�����Ĳ����ر��ļ�
		trans_entry->is_close = 1;
		unregist_opened_file(redo_log_manager.redo_path);

		err = redo_file_write_index(trans_entry,redo_log_manager.data_fd,out);
		if(0!=err)ERROR("redo_file_write_index retur err is %d\n",err);
			
		strcpy(redo_log_manager.redo_path,redo_log_manager.path);
		strcat(redo_log_manager.redo_path,"/redo_data_");
    GetTimeForNAME(redo_log_manager.redo_path);
   
    DEBUG("New_redo_date file_name is %s\n",redo_log_manager.redo_path);
    DEBUG("Old_redo_date file_fd is %ld\n",redo_log_manager.data_fd);
    

    //���´����ļ�
    redo_log_manager.data_fd = fopen(redo_log_manager.redo_path,"wb+");
    IMPORTANT_INFO("New_redo_date file_fd is %ld\n",redo_log_manager.data_fd);
    
    regist_opened_file(redo_log_manager.redo_path ,redo_log_manager.data_fd);
    
    //���������ļ���С=0;
    SIZE_LOCK(&(redo_log_manager.data_size_locker));// �����ļ���С����
		redo_log_manager.cur_data_size = 0;
		SIZE_UNLOCK(&(redo_log_manager.data_size_locker));// �����ļ���С����
		
	}
	//
  else
  {
	err = redo_file_write_index(trans_entry,redo_log_manager.data_fd,out);

  }
	REDO_LOG_UNLOCK(&(redo_log_manager.data_locker));//�����ļ�����	
	
	if(err!=0)return err;
	return 0;
}

// redo_data д��data�ļ�
// ���ﻹҪϸ��������������������������������������������������������������������������������������������������������������������������������������������������������������
inline int redo_file_write_data(mem_transaction_entry_t * trans_entry,FILE * fd)
{
	//DEBUG("redo_file_write_data\n");
	//DEBUG("trans_entry->object_no=%ld\n",trans_entry->object_no);
	switch(trans_entry->redo_type)
	{
	case OPT_DATA_UPDATE:
	case OPT_DATA_INSERT:
	case OPT_DATA_DELETE:
	//case OPT_INDEX_RBTREE_INSERT:
	//case OPT_INDEX_RBTREE_DELETE:
	//case OPT_INDEX_HASH_INSERT:
	//case OPT_INDEX_HASH_DELETE:		
	//case OPT_INDEX_HASH_UPDATE:
	
	//DEBUG("redo_write:pos is %ld,len is %ld,fd is %ld,ori is %0x\n",trans_entry->redo_data_start,trans_entry->redo_data_length,fd,trans_entry->ori_data_start);
	fseek(fd,trans_entry->redo_data_start,SEEK_SET);
	fwrite(trans_entry->ori_data_start,1,trans_entry->redo_data_length,fd);
	break;
	default:
			ERROR("trans_entry->redo_type not found,trans_entry->redo_type is %d\n",trans_entry->redo_type);

	}
	//����undo �� UPDATE�ķ�������update
	//DELETE �ķ������� insert
	//INSERT �ķ������� delete
	//����undo ���� UPDATE�ķ�������update
	switch(trans_entry->undo_type)
	{
	case OPT_DATA_UPDATE:
	case OPT_DATA_DELETE:
  case OPT_DATA_INSERT:
	//case OPT_INDEX_RBTREE_INSERT:
	//case OPT_INDEX_RBTREE_DELETE:
	//case OPT_INDEX_HASH_INSERT:
	//case OPT_INDEX_HASH_DELETE:
	//case OPT_INDEX_HASH_UPDATE:
	
	//DEBUG("undo_write:pos is %ld,len is %ld,fd is %ld,ori is %0x\n",trans_entry->undo_data_start,trans_entry->undo_data_length,fd,trans_entry->ori_data_start);

	fseek(fd,trans_entry->undo_data_start,SEEK_SET);
	fwrite(trans_entry->ori_data_start,1,trans_entry->undo_data_length,fd);
	
 ///���ﴦ��һ�»ع���
	//mem_block_t *mb = transaction_manager.transaction_tables[trans_entry->trans_no].rollback_space;
	////�ҵ��ع��οռ�
	//long rollback_space_num = transaction_manager.transaction_tables[trans_entry->trans_no].rollback_space_num;
	//int j = 0;
	//DEBUG("rollback_space_num is %d\n",rollback_space_num);
	//for(;j<rollback_space_num-1;++j)
	//{
	//	mb = mb->next;
	//}
	////transaction_manager.transaction_tables
	//DEBUG("mb is %0x\n",mb);
	//DEBUG("trans_entry->trans_action is %0x\n",transaction_manager.transaction_tables);
	//DEBUG("transaction_manager.transaction_tables[trans_entry->trans_no] is %0x\n",(char *)(transaction_manager.transaction_tables)+(trans_entry->trans_no)*TRANSACTION_SIZE );
	////����ع��β������������д��
	//if(
	//	//((mem_transaction_t *)((char *)(transaction_manager.transaction_tables)+(trans_entry->trans_no)*TRANSACTION_SIZE ))
	//	//->rollback_cur_pos+
	//	transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos+
	//	trans_entry->undo_data_length 
	//	> mb->space_size
	//	 )
	//{
	//	extern_rollback_block(&(transaction_manager.transaction_tables[trans_entry->trans_no]));
	//	transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos = 0;
	//  mb = mb->next;
	//}
	//	//��Ҫ�ж�һ�δ�С,����󻹷Ų��»ع����ݵĻ��ͱ���
	//	if(transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos+trans_entry->undo_data_length > mb->space_size )
	//  return ROLLBACK_SPACE_ERR_TRANS_TOO_BIG;
	//
	////�������ݵ��ع���
	//memcpy(mb->space_start_addr+transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos
	//			,trans_entry->ori_data_start
	//			,trans_entry->undo_data_length
	//			);
	//			
 
	//transaction_manager.transaction_tables[trans_entry->trans_no].rollback_cur_pos += trans_entry->undo_data_length;
	break;
	default:
			ERROR("trans_entry->undo_type not found,trans_entry->undo_type is %d\n",trans_entry->undo_type);

}
	if( 1 == trans_entry->is_close ) 
		{
			IMPORTANT_INFO("trans_entry->redo_data_start  = %ld ��fclose(fd),fd = %ld\n",trans_entry->redo_data_start ,fd);
			fclose(fd);
			
		}
	return 0;
}

  
//_______________________sys_trans_redo_data_writer____________________________________________________________
/* �����ǹ���д��־writer �̵߳Ĳ���*/
#ifndef _LOCK_FREE_QUEUE_
void * redo_file_writer_fun( void *arg )
{
	mem_trans_data_entry_t val;
	long sleep = sys_trans_redo_data_writer.sleep_micro_seconds;
	while(!sys_trans_redo_data_writer.stop){
	// ѭ���Ӷ�����ȡ���ݣ�д���ļ�
		//DEBUG("redo_file_writer_fun cycle\n");

	if(sys_trans_data_queue.front != sys_trans_data_queue.tear)sleep = sys_trans_redo_data_writer.sleep_micro_seconds;
	while(sys_trans_data_queue.front != sys_trans_data_queue.tear)
	{
	de_trans_data_queue(&sys_trans_data_queue, &val) ;
	//DEBUG("val.trans.object_no= %ld\n",val.trans.object_no);
	redo_file_write_data(&(val.trans),val.fd);
  }
   //usleep(sleep);
   // ��˯�߶��ӳ�
   if(sleep <1000)
   	{
   		volatile static int itime = 0;
   		for(;itime<sleep*1000;++itime);
   		sleep<<=1;
   		//DEBUG("sleep<<1,sleep is %d\n",sleep);
   	}
  else //����ӳ�>1 s
  {
   //DEBUG("TRANS_QUEUE_SLEEP_LOCK\n");
  TRANS_QUEUE_SLEEP_LOCK(&(sys_trans_data_queue.sleep_locker));
  while( 0 == sys_trans_data_queue.is_sleeping && !sys_trans_redo_data_writer.stop \
  				&& sys_trans_data_queue.front == sys_trans_data_queue.tear								)
  {
  //DEBUG("TRANS_QUEUE_SLEEP_COND_WAIT\n");
  sys_trans_data_queue.is_sleeping = 1;
  TRANS_QUEUE_SLEEP_COND_WAIT(&(sys_trans_data_queue.sleep_cond),&(sys_trans_data_queue.sleep_locker));
  
  }
  TRANS_QUEUE_SLEEP_UNLOCK(&(sys_trans_data_queue.sleep_locker));
	}

}
}
#else
void * redo_file_writer_fun( void *arg )
{
	mem_trans_data_entry_t val;
	long sleep = sys_trans_redo_data_writer.sleep_micro_seconds;
	  
	  uint32_t tail, head, mask, next;
    int ok;
    mask = sys_trans_data_queue.tail.mask;
    
	while(!sys_trans_redo_data_writer.stop){

	while(0 == de_trans_data_queue(&sys_trans_data_queue, &val))
	{
	//DEBUG("val.trans.object_no= %ld\n",val.trans.object_no);
	redo_file_write_data(&(val.trans),val.fd);
	if(sleep) sleep = 0;
  }
   //usleep(sleep);
   // ��˯�߶��ӳ�
   if(sleep <1000)
   	{
   		volatile static int itime = 0;
   		for(;itime<sleep*1000;++itime);
   		sleep<<=1;
   		//DEBUG("sleep<<1,sleep is %d\n",sleep);
   	}
  else //����ӳ�>1 s
  {
   //DEBUG("TRANS_QUEUE_SLEEP_LOCK\n");
  TRANS_QUEUE_SLEEP_LOCK(&(sys_trans_data_queue.sleep_locker));
   tail = sys_trans_data_queue.tail.first;
   head = sys_trans_data_queue.head.second;
  while( 0 == sys_trans_data_queue.is_sleeping && !sys_trans_redo_data_writer.stop \
  				&& ((tail == head) || (tail > head && (head - tail) > mask))							)
  {
  //DEBUG("TRANS_QUEUE_SLEEP_COND_WAIT\n");
  sys_trans_data_queue.is_sleeping = 1;
  TRANS_QUEUE_SLEEP_COND_WAIT(&(sys_trans_data_queue.sleep_cond),&(sys_trans_data_queue.sleep_locker));
  tail = sys_trans_data_queue.tail.first;
  head = sys_trans_data_queue.head.second;
  }
  TRANS_QUEUE_SLEEP_UNLOCK(&(sys_trans_data_queue.sleep_locker));
	}

}
}
#endif

// ��ʼ��������־д��
inline int init_trans_redo_data_writer(trans_redo_data_writer_t * writer,long sleep_micro_seconds)
{
	writer->writer_fun 					= redo_file_writer_fun;
	writer->stop       					= 0;
	writer->sleep_micro_seconds = sleep_micro_seconds;
	TRANS_WRITER_LOCK_INIT(&(writer->locker));
	return 0;
}

// ����writer�߳�д��־
inline int start_trans_redo_data_writer()
{
	  int ret;  
    int arg = 0;  
    ret = pthread_create( &(sys_trans_redo_data_writer.th), NULL, redo_file_writer_fun, &arg ); 
	  if( ret != 0 ){  
        ERROR( "Create log writer thread error!\n");  
        return START_TRANS_REDO_DATA_WRITER_ERR;  
    }  
		return 0;
}

inline int stop_trans_redo_data_writer()
{
	TRANS_WRITER_LOCK(&(sys_trans_redo_data_writer.locker));
	IMPORTANT_INFO("sys_trans_redo_data_writer.stop = 1\n");
	sys_trans_redo_data_writer.stop = 1;
	TRANS_QUEUE_SLEEP_COND_SIGN(&(sys_trans_data_queue.sleep_cond));
	TRANS_WRITER_UNLOCK(&(sys_trans_redo_data_writer.locker)) ;
	return 0;
}


//����������������������������������������������������������������������������������������������������������������������������������������
// �ύ����
inline int commit_trans( long long  trans_no)
{
	IMPORTANT_INFO("commit_trans_no is %ld\n",trans_no);
	if( transaction_manager.transaction_tables[trans_no].is_used == 0)
		{
			ERROR("ERR_TRANS_ISUSED_ZERO\n");
			return ERR_TRANS_ISUSED_ZERO;
		}
	mem_transaction_t * trans = &(transaction_manager.transaction_tables[trans_no]);
  // ��������ü���= 0 ��˵��ȫ��д��
	while(trans->ref != 0)
	{
	 usleep(sys_trans_redo_data_writer.sleep_micro_seconds);
  }
  // ����ǰδˢ�̵� fwrite ��������ˢ��
 fflush_redo_log_manager();
 // ͬ������
 int err;
// ����һ������
if(0!=(err=stop_trans(trans_no)))ERROR("stop_trans failed,trans_no is %d\n",err);  
if(err)return err;
// �ͷ������
if(0!=(err=release_trans(trans_no)))ERROR("release_trans failed,trans_no is %d\n",err); 
 if(err)return err;
  return 0;
}
//����������������������������������������������������������������������������������������������������������������������������������������
// �ع�����
inline int rollback_trans( long long  trans_no)
{
	IMPORTANT_INFO(" ---------- Begin rollback_trans and wait for Starting , trans_no is %ld ----------\n",trans_no);
	int err;
	if( transaction_manager.transaction_tables[trans_no].is_used == 0)
		{
			ERROR("ERR_TRANS_ISUSED_ZERO\n");
			return ERR_TRANS_ISUSED_ZERO;
		}
	IMPORTANT_INFO(" ---------- Start rollback_trans , trans_no is %ld ----------\n",trans_no);
	
	mem_transaction_t * trans = &(transaction_manager.transaction_tables[trans_no]);
 
  struct trans_data_stack_t   rollback_stack = trans->rollback_stack;       //�ع�ջ
  off_t                  rollback_stack_size = trans->rollback_stack_size;  //�ع�ջ��С
  while(trans->ref != 0)
	{
	 DEBUG("trans->ref is %ld \n",trans->ref);
	 usleep(sys_trans_redo_data_writer.sleep_micro_seconds);
  }
  
  int k = 0;
  mem_trans_data_entry_t  item;
  //��ջ�ع�
  while( TRANS_ERR_STACK_EMPTY != pop_trans_data_stack(&(transaction_manager.transaction_tables[trans_no].rollback_stack)
           , &item)  )
  {
   //DEBUG("pop_trans_data_stack\n");
  	void *                      undo_addr_ptr    = item.trans.undo_addr_ptr   ;   // undo �ڻع����ڴ��еĵ�ַ
    off_t                       undo_data_length = item.trans.undo_data_length;   // undo ���ݳ���
  	short                       undo_type        = item.trans.undo_type       ;
  	void *                      ori_data_start   = item.trans.ori_data_start ;     //ԭʼ������ʼ��ַ
    long                        object_no        = item.trans.object_no ; 
    
    // ���mem_table ָ��
    //DEBUG("object_no is %d\n",object_no);
    mem_table_t *               mem_table;
    get_table_no_addr(object_no,(void **)(&mem_table));
    ++k;
    //if(0 == k || ( k>2007860  ) )IMPORTANT_INFO("pop item is %d\n",k);
    //if(k>2697860)IMPORTANT_INFO("mem_table is %0x, k is %d\n",mem_table,k);	
    //IMPORTANT_INFO("mem_table's name %s\n",((mem_table_t *)mem_table)->config.table_name);
    
  //����undo �� UPDATE�ķ�������update
	//DELETE �ķ������� insert
	//INSERT �ķ������� delete
	//����undo ���� UPDATE�ķ�������update
	switch(undo_type)
	{
	case OPT_DATA_UPDATE:
		{
		DEBUG("OPT_DATA_UPDATE\n");
		DEBUG("ori_data_start is %0x,undo_addr_ptr is %0x\n",ori_data_start,undo_addr_ptr);
	  record_t * record_ptr = (struct record_t *)((char *)ori_data_start-RECORD_HEAD_SIZE);
	      err = mem_table_del_record(mem_table ,record_ptr);
	//    err = mem_table_update_record(mem_table , record_ptr,undo_addr_ptr);
	  if(err){ERROR("ROLLBACK OPT_DATA_UPDATE on delete err is %d\n",err);return err;}	  
	  // �޸�ԭʼ��¼��Ļع���Ϣ
  	  if(NULL !=record_ptr->undo_info_ptr  )
  	  {
  	  record_ptr->undo_info_ptr = (void *)(((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->next);
	    }
	  
	   struct record_t * record_ptr2 = NULL ;
     long   block_no;
	   err = mem_table_insert_record( mem_table ,&record_ptr2,&block_no, (char*)undo_addr_ptr);
	   if(err){ERROR("ROLLBACK OPT_DATA_UPDATE on insert  err is %d\n",err);return err;}
	   	// �޸�ԭʼ��¼��Ļع���Ϣ
    if(NULL !=record_ptr->undo_info_ptr  )
    {
    record_ptr->undo_info_ptr = (void *)(((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->next);
    }
    if(NULL !=record_ptr->undo_info_ptr  )
    {
    ((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->trans.ori_data_start = (void *)((char *)record_ptr2 + RECORD_HEAD_SIZE);
    }
	  
		break;
	}
  case OPT_DATA_DELETE:
  	{
  	//if(0 == k || ( k>2007860  ) )IMPORTANT_INFO("OPT_DATA_DELETE\n");
   	   //memcpy(ori_data_start,undo_addr_ptr,undo_data_length);
  	   //((struct record_t *)((char *)ori_data_start-RECORD_HEAD_SIZE))->is_used = 1;
  	   //((struct record_t *)((char *)ori_data_start-RECORD_HEAD_SIZE))->scn     = trans_no;
  	 
  	 	 //mem_block_t * mem_block_temp;
  	 	 //record_t * record_ptr = (struct record_t *)((char *)ori_data_start-RECORD_HEAD_SIZE);
  	 	 //get_block_by_record(mem_table ,record_ptr,&mem_block_temp);
  	 	 //if(0 != (err=get_block_by_record(mem_table ,record_ptr,&mem_block_temp)))
	     // { 	
			 //  ERROR("GET_BLOCK_FAILD\n");
			 //  break;
	     //}
	    DEBUG("OPT_DATA_DELETE___________\n");
	    record_t * record_ptr = (struct record_t *)((char *)ori_data_start- RECORD_HEAD_SIZE);
	    struct record_t * record_ptr2 = NULL; 
	    long  block_no;
  	  err =  mem_table_insert_record(mem_table ,
  	 															 &record_ptr2,
  	 															 &block_no,
  	  														(char*) undo_addr_ptr);
			DEBUG("Old record_ptr is %0x,new record_ptr is %0x \n",record_ptr,record_ptr2);

  		  // �޸�ԭʼ��¼��Ļع���Ϣ
  	      if(NULL !=record_ptr->undo_info_ptr)
  	     {
  	     	DEBUG("record_ptr->undo_info_ptr = record_ptr->undo_info_ptr->next\n",record_ptr,record_ptr2,err);
  	      DEBUG("record_ptr->undo_info_ptr->next is %0x\n",record_ptr->undo_info_ptr);
  	      record_ptr->undo_info_ptr = (void *)(((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->next);
  	      DEBUG("record_ptr->undo_info_ptr->next is %0x\n",record_ptr->undo_info_ptr);
         
  	     }
  	      if(NULL !=record_ptr->undo_info_ptr  )
  	      {
  	      ((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->trans.ori_data_start = (void *)((char *)record_ptr2 + RECORD_HEAD_SIZE);
   	      DEBUG("reset ori_data_start to %0x\n",(void *)(((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->trans.ori_data_start)  );
         
  	      }
  	if(err){ERROR("ROLLBACK OPT_DATA_DELETE,old record_ptr is %0x,new record_ptr is %0x, err is %d\n",record_ptr,record_ptr2,err);}//return err;}
	 	break;
	}
	case OPT_DATA_INSERT:
		{
			DEBUG("OPT_DATA_INSERT___________\n");

			//if(0 == k || ( k>2007860  ) )IMPORTANT_INFO("OPT_DATA_INSERT\n");
     	//mem_block_t * mem_block_temp;

     	DEBUG("ori_data_start is %0x \n",ori_data_start);

  	 	record_t * record_ptr = (struct record_t *)((char *)ori_data_start- RECORD_HEAD_SIZE);

      err = mem_table_del_record(mem_table ,record_ptr);
      if(err){ERROR("ROLLBACK OPT_DATA_INSERT,record_ptr is %0x, err is %d\n",record_ptr,err);}//return err;}
      // �޸�ԭʼ��¼��Ļع���Ϣ
      if(NULL !=record_ptr->undo_info_ptr)
     {
    	  record_ptr->undo_info_ptr = (void *)(((mem_trans_data_entry_t *)(record_ptr->undo_info_ptr))->next);
      }
    
    
   // LIST_LOCK  (  &(mem_block_temp->mem_free_list.list_lock)    );  
   // //�����������
   //record_ptr->is_used = 0;
   //record_ptr->scn = 0;
   //
   //record_t *next_free_record;		
   //if(mem_block_temp->mem_free_list.head!=0)
   //{
   //	get_record_by_record_num(mem_table,mem_block_temp, mem_block_temp->mem_free_list.head,&next_free_record);
   //	next_free_record->next_free_pos = record_ptr->record_num;
   //}
   //
   //record_ptr->last_free_pos = mem_block_temp->mem_free_list.head;
   //mem_block_temp->mem_free_list.head = record_ptr->record_num;
   //LIST_UNLOCK    (  &(mem_block_temp->mem_free_list.list_lock)   );
   
	 	break;
	}
	case OPT_INDEX_HASH_INSERT:
		{
		struct mem_hash_index_input_long * input =(struct mem_hash_index_input_long *)(item.trans.undo_addr_ptr); // undo_addr_ptr ��Ӧ input
		struct    record_t   **  out_record_ptr;																					
		struct mem_hash_index_t * mem_hash_index = (struct mem_hash_index_t *)ori_data_start;											// ori_data_start ��Ӧ mem_hash_index_t
		long  mem_table_no;
		long  block_no;
		err = mem_hash_index_del_l(
                        /* in */ mem_hash_index,
                        				 input,
                        /* out */out_record_ptr,
                        /* out */&block_no,
                        &mem_table_no
                        );
    if(err){ERROR("OPT_INDEX_HASH_INSERT err is %d\n",err);return err;}
		break;
  }
  case OPT_INDEX_HASH_DELETE:
		{
		struct mem_hash_index_input_long * input =(struct mem_hash_index_input_long *)(item.trans.undo_addr_ptr); // undo_addr_ptr ��Ӧ input
		struct    record_t   **  out_record_ptr;																					
		struct mem_hash_index_t * mem_hash_index = (struct mem_hash_index_t *)ori_data_start;											// ori_data_start ��Ӧ mem_hash_index_t
		long  mem_table_no;
		long block_no;
		err = mem_hash_index_insert_l(
                        /* in */ mem_hash_index,
                        				 input,
                        /* out */out_record_ptr,
                        /* out */&block_no,
                        &mem_table_no
                        );
    if(err){ERROR("OPT_INDEX_HASH_DELET err is %d\n",err);return err;}
		break;
  }
    case OPT_INDEX_RBTREE_INSERT:
		{
		mem_rbtree_entry_t * input =(mem_rbtree_entry_t *)(item.trans.undo_addr_ptr); // undo_addr_ptr ��Ӧ mem_rbtree_entry_t
		struct    record_t   **  out_record_ptr;																					
		mem_rbtree_index_t *mem_rbtree_index = (mem_rbtree_index_t *)ori_data_start;	// ori_data_start ��Ӧ mem_rbtree_index_t
		err = mem_rbtree_delete(
                        /* in */ mem_rbtree_index,
                        				 mem_rbtree_index->root,
                        /* out */input
                        );
    if(err){ERROR("OPT_INDEX_RBTREE_INSERT err is %d\n",err);return err;}
		break;
  }
    case OPT_INDEX_RBTREE_DELETE:
		{
		mem_rbtree_entry_t * input =(mem_rbtree_entry_t *)(item.trans.undo_addr_ptr); // undo_addr_ptr ��Ӧ mem_rbtree_entry_t
		struct    record_t   **  out_record_ptr;																					
		mem_rbtree_index_t *mem_rbtree_index = (mem_rbtree_index_t *)ori_data_start;	// ori_data_start ��Ӧ mem_rbtree_index_t
		err = mem_rbtree_insert(
                        /* in */ mem_rbtree_index,
                        				 mem_rbtree_index->root,
                        /* out */input
                        );
    if(err){ERROR("OPT_INDEX_HASH_DELETE err is %d\n",err);return err;}
		break;
  }
}
  
 } 
  // ����ǰδˢ�̵� fwrite ��������ˢ��
 fflush_redo_log_manager();

 // ͬ������
// ����һ������
if(0!=(err=stop_trans(trans_no)))ERROR("stop_trans failed,trans_no is %d\n",err);  
if(err)return err;
// �ͷ������
if(0!=(err=release_trans(trans_no)))ERROR("release_trans failed,trans_no is %d\n",err); 
if(err)return err;
IMPORTANT_INFO(" ---------- Stop rollback_trans , trans_no is %ld ----------\n",trans_no);

return 0;


}
// ��� ��־ index �е�һ����Ŀ ��������
inline int redo_recover(mem_transaction_entry_t * item)
{
	if(NULL == item)
		{
			ERROR("Recover Item is NULL\n");
			return RECOVER_ITEM_IS_NULL;
		}
int err;

unsigned  long long       scn           = item->scn;                //����¼���߼�ID
long long                 trans_no			= item->trans_no;						//��ǰ�����
time_t                    create_time		= item->create_time;        //action ʱ��
short                     is_close			= item->is_close;           //�Ƿ���д���ر�fd
short                     redo_type			= item->redo_type;          //redo ��������
long                      object_no			= item->object_no;          //��������� no  --- ��Ҫ��ҵ������� ��������
unsigned   long           block_no			= item->block_no;           //���					 --- ��Ҫ��ҵ������� ��������
unsigned   long           record_num		= item->record_num;			    //�к�           --- ��Ҫ��ҵ������� ��������
char                      name[256];														    //�������������
strcpy(name,item->name);
char                      block_name[256];														    //�������
strcpy(block_name,item->block_name);

char                      log_data_file[256];												//��ǰ��־�����ļ���
strcpy(log_data_file,item->log_data_file);
long                      redo_data_start	 = item->redo_data_start;	// redo �������ļ��е���ʼλ�ã����� index_entry ����ʼλ��
off_t                     redo_data_length = item->redo_data_length;// redo ���ݳ���

//������Ҫ�Ƶ���Ԫ����
FILE 								* fd;
mem_table_t         * mem_table;
struct mem_block_t  * mb;
struct record_t   	* record_ptr;

char recover_buf[redo_data_length]; // ��������
    
// 0 ��ȡ�ع�����    
err = search_opened_file(log_data_file,&fd);
DEBUG("Search_opened_file %s return  %d\n",log_data_file,err);
//1 û�ҵ�fd ����ͼ���ļ�
	if(err)
	{
		IMPORTANT_INFO("NOT FOUND and OPEN IT\n");

		fd = fopen(log_data_file,"rb+");	
		if(fd <= 0)
			{
				ERROR("Open Redo File Failed!\n");
				return err;
			}
		regist_opened_file(log_data_file ,fd);
		if(is_close)
			{
				unregist_opened_file(log_data_file);
				fclose(fd);
			}
		
	}
	DEBUG("fseek fd: %ld . start is %ld\n",fd,redo_data_start);

	//�ҵ��ļ� ��ͼ��� �ع�����
	fseek(fd,redo_data_start,SEEK_SET);
  DEBUG("fseek  end \n");

  size_t ret = fread ( recover_buf, 1, redo_data_length, fd) ;
  if(ret < 0)
  {
  	//��ûع�����ʧ��
  	ERROR("Read Redo File Failed on pose [%ld,%ld] \n",redo_data_start,redo_data_start+redo_data_length);
  	return ret;
  }
   
  DEBUG("Get redo data ok\n");
  
	   

	  
	switch(redo_type)
{
	case OPT_DATA_UPDATE:
  case OPT_DATA_DELETE:
	case OPT_DATA_INSERT:
	case OPT_INDEX_RBTREE_INSERT:
  case OPT_INDEX_RBTREE_DELETE:
  case OPT_INDEX_HASH_INSERT:
  case OPT_INDEX_HASH_DELETE:
		{
		 long long table_no ;
  	 search_table_name(name,&table_no);
  	 long long real_block_no ;
  	 search_block_name(block_name,&real_block_no);

  
    //2 ����3��no��� ��ָ��
     err = get_record_by_3NO(table_no,real_block_no,record_num,&record_ptr);
     	DEBUG("----- recover a record ! -----\n");
			memcpy(record_ptr,recover_buf,redo_data_length);
     break;
		}

  default:
 			ERROR("Unkonw redo_type [%d]\n",redo_type);
  		return UNKONW_REDO_TYPE;
}

  if(err)
  {
  	//��ûع�����ʧ��
  	ERROR("Get record by 3NO failed!,err is %d \n",err);
  	return ret;
  }
	return 0;
}

//����һ���ļ���·��,���ļ���
//�Ը��ļ������� redo
inline int redo_file_redo_index(char * path,char * filename)
{
	if(NULL == path)
		{
			ERROR("path is NULL \n");
			return TRANS_ERR_PATH_NULL;
		}
	if(NULL == filename)
		{
			ERROR("filename is NULL \n");
			return TRANS_ERR_FILE_NULL;
		}
  DEBUG("redo_file_redo_index() , path is %s filename is %s\n",path,filename);
	FILE * work_fd = NULL;
	
	chdir (path);
	work_fd = fopen(filename,"rb+");
	if(work_fd<=0)
	{
		ERROR("NO_FILE_FOUNDED\n");
		return TRANS_ERR_NO_FILE_FOUNDED;
	}
  
  mem_transaction_entry_t  trans_entry;
  size_t ret = fread ( &trans_entry, 1, TRANSACTION_ENTRY_SIZE, work_fd);
  
  DEBUG("First item create time is %ld\n",trans_entry.create_time);
  
  if(ret < 0)
  {
  	//��ȡ��������ʧ��
  	ERROR("Read Redo File Failed! \n");
  	fclose(work_fd);
  	return ret;
  }
  
  struct stat sb; 
	/* ȡ���ļ���С */
	stat(filename, &sb); 
	long long n = sb.st_size/TRANSACTION_ENTRY_SIZE;
	if(sb.st_size%TRANSACTION_ENTRY_SIZE)++n;
	
	long right = 0;
  //long left, right, middle;  
  //left = -1, right = n;  
  //  
	// if(trans_entry.create_time < recover_time)
	// {		
	// 	// ʹ���۰�����ļ��е�ʱ��
  //  while (left + 1 != right)  
  //  {  
  //      middle = left + (right - left) / 2;  
  //      fseek(work_fd,middle*TRANSACTION_ENTRY_SIZE,SEEK_SET);
  //      ret = fread ( &trans_entry, 1, TRANSACTION_ENTRY_SIZE, work_fd);
  //      DEBUG("Middle item create time is %ld,recover_time is %ld\n",trans_entry.create_time,recover_time);
  //
  //      if ( trans_entry.create_time < recover_time )  
  //      {  
  //          left = middle;  
  //      }  
  //      else  
  //      {  
  //          right = middle;  
  //      }  
  //  }  
  //
  //  if (right >= n || trans_entry.create_time < recover_time  )  
  //  {  
  //      ERROR("NOT_FOUND_TRANSACTION_ENTRY\n");
  //      fclose(work_fd);
  //      return  NOT_FOUND_TRANSACTION_ENTRY;
  //  }  
	// }
	// else right = 0;
	
	 int err;
   for( ;right<n;++right)
   {
   		fseek(work_fd,right*TRANSACTION_ENTRY_SIZE,SEEK_SET);
   		ret = fread ( &trans_entry, 1, TRANSACTION_ENTRY_SIZE, work_fd);
			err = redo_recover(&trans_entry);
   	  if(err)break;
   }
   
   fclose(work_fd);
return err;
}

// redo ��������
// path ������־Ŀ¼
// start_str_in ������ʼʱ���ַ������� "20161206221236" 
int redo(char * path,char * start_str_in)  
{  
	IMPORTANT_INFO("redo data in %s,start_str_in %s\n",path,start_str_in);	
	struct dirent * dirent_array = NULL;
	long file_count = 0;
	char * cmp_str ="redo_index_2";
	
	if( NO_FILE_FOUNDED != get_dir_file_list(path, &file_count,&dirent_array))
		{ 
			long i = 0;
			
			dirent_list_filter_head(&dirent_array,&file_count,
														cmp_str,strlen(cmp_str));
	
			for(i = 0;i<file_count;++i)
			{
				DEBUG("After filter %s \n",dirent_array[i].d_name);
			}
			
		}
	long ret_pos = 0;
	
	char start_str[512];
	strcpy(start_str,"redo_index_");
	strcat(start_str,start_str_in);
	strcat(start_str,".000000");
	dirent_array_search_min(dirent_array, file_count, start_str,&ret_pos);
  //���ڻָ���ʱ�����λ����һ���ļ���β�������� ret_pos Ҫ��һ
	if(ret_pos>0)--ret_pos;
	
	DEBUG("ret_pos = %d \n",ret_pos);
	for(;ret_pos<=file_count;++ret_pos)
	{
	redo_file_redo_index(path,dirent_array[ret_pos].d_name);
  }
	if( NULL != dirent_array)	free (dirent_array);
	return 0;
}

#ifdef __cplusplus

}

#endif

#endif 

