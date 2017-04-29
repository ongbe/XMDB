#include <stdio.h>  
#include <stdlib.h>  
#include <stdint.h> 
#include <unistd.h>  
#include <sys/types.h>  
#include <pthread.h>  
#include <assert.h>  
#include <emmintrin.h>
#include <string.h>

#ifndef __CTHREAD_POOL__
#define __CTHREAD_POOL__

#define aligend_2_n(x)  (1<<((32-__builtin_clz(x))))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef likely
#define likely(x)     __builtin_expect((x), 1)
#endif

#ifndef unlikely
#define unlikely(x)   __builtin_expect((x), 0)
#endif

#define CACHE_LINE_SIZE     64
  
#define THREAD_TASK_ERR_QUEUE_ALLOC_FAILED      45014
#define THREAD_TASK_ERR_QUEUE_FULL 					    45015
#define THREAD_TASK_ERR_QUEUE_EMPTY 				    45016  
#define THREAD_POOL_IS_STOPED                   45017
 
#define PThread_T   pthread_t
#define PTHREAD_CREATE(a,b,c,d)    pthread_create(a,b,c,d)
//��ʱ�û�������ʾ˯����
#define THREAD_TASK_QUEUE_SLEEP_LOCK_T        pthread_mutex_t
#define THREAD_TASK_QUEUE_SLEEP_LOCK(x)       pthread_mutex_lock(x)
#define THREAD_TASK_QUEUE_SLEEP_UNLOCK(x)     pthread_mutex_unlock(x)   
#define THREAD_TASK_QUEUE_SLEEP_LOCK_INIT(x)  pthread_mutex_init(x,0)
#define THREAD_TASK_QUEUE_SLEEP_LOCK_DEST(x)  pthread_mutex_destroy(x) 

//��ʱ�û�������������
#define THREAD_TASK_QUEUE_SLEEP_COND_T              pthread_cond_t  
#define THREAD_TASK_QUEUE_SLEEP_COND_INIT(x)        pthread_cond_init(x, NULL)
#define THREAD_TASK_QUEUE_SLEEP_COND_DEST(x)        pthread_cond_destroy(x)
#define THREAD_TASK_QUEUE_SLEEP_COND_WAIT(x,y)      pthread_cond_wait(x, y)
#define THREAD_TASK_QUEUE_SLEEP_COND_SIGN(x)        pthread_cond_signal(x)
#define THREAD_TASK_QUEUE_SLEEP_COND_BROADCAST(x)   pthread_cond_broadcast(x)
  
/* 
*�̳߳����������к͵ȴ���������һ��CThread_worker 
*����ѭ��ring�� 
*/  
typedef struct thread_task_entry_t
{  
    /*�ص���������������ʱ����ô˺�����ע��Ҳ��������������ʽ*/  
    void *(*process) (void *arg);  
    void *arg;/*�ص������Ĳ���*/    
} thread_task_entry_t;  
  
  
typedef struct thread_task_queue_t {
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
    THREAD_TASK_QUEUE_SLEEP_LOCK_T  sleep_locker; //������˯����
    int                             is_sleeping;  //�Ƿ���˯��
    THREAD_TASK_QUEUE_SLEEP_COND_T  sleep_cond;   //������˯����������
    thread_task_entry_t    * item;
}thread_task_queue_t;
//__________________________________________________________

inline int init_thread_task_queue  (thread_task_queue_t * thread_task_queue, uint32_t max);
inline int en_thread_task_queue    (thread_task_queue_t * thread_task_queue, thread_task_entry_t* item);
inline int de_thread_task_queue    (thread_task_queue_t * thread_task_queue, thread_task_entry_t * item);
inline int reinit_thread_task_queue(thread_task_queue_t * thread_task_queue, uint32_t max);  
int destroy_task_queue(thread_task_queue_t * thread_task_queue) ; 


//__________________________________________________________
/*�̳߳ؽṹ*/  
typedef struct  CThread_pool_t
{  
    //pthread_mutex_t queue_lock;  
    //pthread_cond_t queue_ready;  
  
    /*�������*/  
    thread_task_queue_t *task_queue;  
  
    /*�Ƿ������̳߳�*/  
    int shutdown;  
    PThread_T *threadid;  
    /*�̳߳�������Ļ�߳���Ŀ*/  
    int max_thread_num;  
    /*��ǰ�ȴ����е�������Ŀ*/  
    int cur_queue_size;  
  
} CThread_pool_t;  
  
  
int pool_add_worker (CThread_pool_t *pool,void *(*process) (void *arg), void *arg);  
void * thread_routine (void *pool);

void  
thread_pool_init (CThread_pool_t *pool,int max_thread_num,uint32_t max_task)  
{  
	  DEBUG("pool->task_queue %ld \n",pool->task_queue);
    pool->task_queue = (thread_task_queue_t*) malloc (sizeof (thread_task_queue_t));  

    init_thread_task_queue  (pool->task_queue, max_task);
  
    pool->max_thread_num = max_thread_num;  
    pool->cur_queue_size = 0;  
    pool->shutdown = 0;  
    pool->threadid = (PThread_T *) malloc (max_thread_num * sizeof (PThread_T));  
    int i = 0;  
    for (i = 0; i < max_thread_num; i++)  
    {   
        PTHREAD_CREATE (&(pool->threadid[i]), NULL, thread_routine,pool);  
    }  
}  
  
  
/*���̳߳��м�������*/  
int  
thread_pool_add_task (CThread_pool_t *pool,void *(*process) (void *arg), void *arg)  
{  
	//printf("thread_pool_add_task %ld,%ld,%ld,%ld\n",pool,process,arg/*,pool->task_queue*/);
  //printf("pool->task_queue %ld \n",pool->task_queue);

    /*����һ��������*/  
    thread_task_entry_t item;  
    item.process = process;  
    item.arg = arg;  
  
    /*��������뵽�ȴ�������*/  
    return en_thread_task_queue(pool->task_queue, &item);  
}  
  
  
  
/*�����̳߳أ��ȴ������е����񲻻��ٱ�ִ�У������������е��̻߳�һֱ 
����������������˳�*/  
int  
thread_pool_destroy (CThread_pool_t *pool)  
{  
    if (pool->shutdown)  
        return -1;/*��ֹ���ε���*/  
    pool->shutdown = 1;  
   
   THREAD_TASK_QUEUE_SLEEP_LOCK(&(pool->task_queue->sleep_locker));
   pool->task_queue->is_sleeping = 2;
   THREAD_TASK_QUEUE_SLEEP_UNLOCK(&(pool->task_queue->sleep_locker));
  
    /*�������еȴ��̣߳��̳߳�Ҫ������*/  
    THREAD_TASK_QUEUE_SLEEP_COND_BROADCAST(&(pool->task_queue->sleep_cond));  
  
    /*�����ȴ��߳��˳�������ͳɽ�ʬ��*/  
    int i;  
    for (i = 0; i < pool->max_thread_num; i++)  
        pthread_join (pool->threadid[i], NULL);  
    free (pool->threadid);  
  
    /*���ٵȴ�����*/  
    destroy_task_queue(pool->task_queue);
    
    /*���������ͻ�����Ҳ����������*/  
    //pthread_mutex_destroy(&(pool->queue_lock));  
    //pthread_cond_destroy(&(pool->queue_ready));  
    free (pool->task_queue);
    free (pool);  
    /*���ٺ�ָ���ÿ��Ǹ���ϰ��*/  
    pool=NULL;  
    return 0;  
}  
 

  
void* thread_routine (void *pool)  
{  
    //DEBUG ("starting thread 0x%x\n", pthread_self ());  
    int ret;
    thread_task_entry_t item; 
    while (1)  
    {  
       //THREAD_TASK_QUEUE_SLEEP_LOCK_T (&(pool->queue_lock));  
       ///*����ȴ�����Ϊ0���Ҳ������̳߳أ���������״̬; ע�� 
       //pthread_cond_wait��һ��ԭ�Ӳ������ȴ�ǰ����������Ѻ�����*/  
       //while (pool->cur_queue_size == 0 && !pool->shutdown)  
       //{  
       //    printf ("thread 0x%x is waiting\n", pthread_self ());  
       //    pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock));  
       //}  
  
        /*�̳߳�Ҫ������*/  
       // if (((CThread_pool_t *)pool)->shutdown)  
        //{  
            /*����break,continue,return����ת��䣬ǧ��Ҫ�����Ƚ���*/  
       //     pthread_mutex_unlock (&(pool->queue_lock));  
            //DEBUG ("thread 0x%x will exit\n", pthread_self ());  
          //  return (void*)0;
        //}  
  
        //printf ("thread 0x%x is starting to work\n", pthread_self ());  
  
        /*assert�ǵ��Եĺð���*/  
       // assert (pool->cur_queue_size != 0);  
       // assert (pool->queue_head != NULL);  
          
        /*�ȴ����г��ȼ�ȥ1����ȡ�������е�ͷԪ��*/  
        //pool->cur_queue_size--; 
       
        ret = de_thread_task_queue(((CThread_pool_t *)pool)->task_queue, &item) ;
        if(1 == ret)return (void*)0;
        /*���ûص�������ִ������*/  
        //if(!(item.process)){
        //	//int* i = (((struct param *)(item.arg))->i);
        //	printf("bad item is %ld\n",1);
        //	continue;
        //	}
        (*(item.process)) (item.arg);  
    }  
    /*��һ��Ӧ���ǲ��ɴ��*/  
   return (void*)0;  
}  
 
//______________________________________________________

inline int init_thread_task_queue(thread_task_queue_t * thread_task_queue,uint32_t max)  
{  
	printf("init_thread_task_queue\n");
    thread_task_queue->item = (thread_task_entry_t*)malloc((max+1) * sizeof(thread_task_entry_t));  

    thread_task_queue->max  = max;
    if(!thread_task_queue->item)  
    {  
        //DEBUG("%s\n","Alloc failed,not memory enough");  
        return THREAD_TASK_ERR_QUEUE_ALLOC_FAILED;  
    }  
    THREAD_TASK_QUEUE_SLEEP_LOCK_INIT(&(thread_task_queue->sleep_locker));
    THREAD_TASK_QUEUE_SLEEP_COND_INIT(&(thread_task_queue->sleep_cond));
    thread_task_queue->is_sleeping = 0;
    
    thread_task_queue->head.size = thread_task_queue->tail.size = max;
    thread_task_queue->head.mask = thread_task_queue->tail.mask = max-1;
    return 0;  
}  
 // �����
inline int en_thread_task_queue(thread_task_queue_t * thread_task_queue, thread_task_entry_t* item)  
{  
	  DEBUG("en_thread_task_queue\n"	);
	  if(thread_task_queue->is_sleeping == 2)return THREAD_POOL_IS_STOPED;
	  
    uint32_t head, tail, mask, next;
    int ok;
    mask = thread_task_queue->head.mask;
    do {
        head = thread_task_queue->head.first;
        tail = thread_task_queue->tail.second;
        // ���� �� �ǿ�
        if (((head - tail) > mask) && (!((tail == head) || (tail > head && (head - tail) > mask))))
        	{
        		DEBUG("%ld %s,head=%ld,tail=%ld\n",__LINE__,"The thread_task_queue_t is full",head,tail);
            return THREAD_TASK_ERR_QUEUE_FULL;
            //continue;
          }
        next = head + 1;
        ok = __sync_bool_compare_and_swap(&thread_task_queue->head.first, head, next);
    } while (!ok);

    memcpy(&(thread_task_queue->item[head & mask]),item,sizeof(thread_task_entry_t));
    asm volatile ("":::"memory");

    while (unlikely((thread_task_queue->head.second != head)))
        _mm_pause();

    thread_task_queue->head.second = next;
    
    
    // ���д����˯�߾ͻ�����
  // if( 1 == thread_task_queue->is_sleeping)
  //{
  // //DEBUG("en_trans TRANS_QUEUE_SLEEP_LOCK\n"	);
  //  THREAD_TASK_QUEUE_SLEEP_LOCK(&(thread_task_queue->sleep_locker));
  //     if( 1 == thread_task_queue->is_sleeping)
  //		{
   			if(thread_task_queue->is_sleeping!=2)thread_task_queue->is_sleeping = 0;
   			DEBUG("TRANS_QUEUE_SLEEP_COND_SIGN\n"	);
   			THREAD_TASK_QUEUE_SLEEP_COND_SIGN(&(thread_task_queue->sleep_cond));
   //	  }
  //  THREAD_TASK_QUEUE_SLEEP_UNLOCK(&(thread_task_queue->sleep_locker));
  //  }
    
    return 0;  
}  
  // ������
inline int de_thread_task_queue(thread_task_queue_t * thread_task_queue, thread_task_entry_t * item)  
{     
    uint32_t tail, head, mask, next;
    int ok;
    mask = thread_task_queue->tail.mask;
   
    //printf("de_thread_task_queue\n"	);
    
    do {

        tail = thread_task_queue->tail.first;
        head = thread_task_queue->head.second;
 
        if ((tail == head) || (tail > head && (head - tail) > mask))
        {
        
        //Ϊ�վ�˯��
        THREAD_TASK_QUEUE_SLEEP_LOCK(&(thread_task_queue->sleep_locker));
        if( 0 == thread_task_queue->is_sleeping)
   		  {
   		  	thread_task_queue->is_sleeping = 1;
   		  }
   		   if(2 == thread_task_queue->is_sleeping)
   		   	{
   		   		THREAD_TASK_QUEUE_SLEEP_UNLOCK(&(thread_task_queue->sleep_locker));
   		   		return 1;
					}
        	//printf("TASK_QUEUE_SLEEP_COND_WAIT,thread_task_queue->is_sleeping = %ld\n",thread_task_queue->is_sleeping	);
   		  	THREAD_TASK_QUEUE_SLEEP_COND_WAIT(&(thread_task_queue->sleep_cond),&(thread_task_queue->sleep_locker));
   	    
   	     THREAD_TASK_QUEUE_SLEEP_UNLOCK(&(thread_task_queue->sleep_locker));
					continue;
         }
         
        next = tail + 1;
        ok = __sync_bool_compare_and_swap(&thread_task_queue->tail.first, tail, next);
    } while (!ok);
   
  // printf("tail & mask= %ld \n",tail & mask);
    memcpy(item,&(thread_task_queue->item[tail & mask]),sizeof(thread_task_entry_t));
	 // if(tail!=(tail & mask) )printf("index = %ld,tail & mask=%ld,item>arg = %ld\n",tail,tail & mask,*(int*)item->arg);
	// printf("de_thread_task_queue out\n"	);
    asm volatile ("":::"memory");

    while (unlikely((thread_task_queue->tail.second != tail)))
        _mm_pause();

    thread_task_queue->tail.second = next;
    return 0;
}  
/* �����ǹ���ѭ�����黺�����Ĳ�����صĲ���*/
// ͣ��־��,�ؽ� ������
inline int reinit_thread_task_queue(thread_task_queue_t * thread_task_queue,uint32_t max)  
{  
    free(thread_task_queue->item);  
  
    return init_thread_task_queue(thread_task_queue, max);  
} 

  //�����־�������
int destroy_task_queue(thread_task_queue_t * thread_task_queue)  
{  
	  THREAD_TASK_QUEUE_SLEEP_COND_DEST(&(thread_task_queue->sleep_cond));
	  thread_task_queue->is_sleeping = 0;
	  THREAD_TASK_QUEUE_SLEEP_LOCK_DEST(&(thread_task_queue->sleep_locker));
    free(thread_task_queue->item);  
} 
//_________________________________________________________ 
 
 
//    �����ǲ��Դ���  
  
void *  
myprocess (void *arg)  
{  
    printf ("threadid is 0x%x, working on task %d\n", pthread_self (),*(int *) arg);  
    sleep (1);/*��Ϣһ�룬�ӳ������ִ��ʱ��*/  
    return NULL;  
}  
  
int  
test_main (int argc, char **argv)  
{  
	CThread_pool_t  *pool;
    thread_pool_init (pool,3,1024);/*�̳߳������������߳�*/  
      
    /*���������Ͷ��10������*/  
    int *workingnum = (int *) malloc (sizeof (int) * 10);  
    int i;  
    for (i = 0; i < 10; i++)  
    {  
        workingnum[i] = i;  
        thread_pool_add_task (pool,myprocess, &workingnum[i]);  
    }  
    /*�ȴ������������*/  
    sleep (5);  
    /*�����̳߳�*/  
    thread_pool_destroy (pool);  
  
    free (workingnum);  
    return 0;  
}  


#ifdef __cplusplus

}

#endif

#endif 