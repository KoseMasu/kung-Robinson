#include "concurrency.h"
#include <vector>
#include <atomic>
#define LOCK_NUM 10
#define THREAD_NUM 4
#define TRANSACTION_NUM 1000
#define READ 1000
#define WRITE 1001
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;



//トランザクションごとのオぺレーションを管理
typedef struct  {
    int operation[LOCK_NUM];
    int accessData[LOCK_NUM];
    int write_num[LOCK_NUM];
} t_order;

typedef struct  {
    int key;
    int value;
} db;

db database[MAX_OBJ];

t_order list[THREAD_NUM][TRANSACTION_NUM];

DATA DataObj[MAX_OBJ];

std::atomic<int> tx_glob(0);

typedef struct  {
    int tx_id;
    vector<int> writeset;
} g_write;
vector<g_write> writeset_glob;
std::atomic<int> tx_start_list[THREAD_NUM];



bool validate(int tx_start , int tx_end ,vector<int> &readset){
  int z;
  for(z=0;z<writeset_glob.size();z++){
    if(writeset_glob[z].tx_id==tx_start+1)break;
  }
  //cout<<tx_start<<" "<<tx_end<<endl;
  //if(tx_end-(tx_start+1)<=0)return true;
  for (int i = 0; i < tx_end-(tx_start+1); i++){
    for(int n= 0;n<writeset_glob[z+i].writeset.size();n++){
      for(int t=0;t<readset.size();t++){
        
        if(writeset_glob[z+i].writeset[n]==readset[t]){
          //cout<<"abort"<<endl;
          return false;
        }
      }
    }
  }
  return true;
}

static void *
worker(void *arg)
{
  int *ip = (int *)arg;
  vector<int>::iterator itr;
  int tx_mine;
  for(int t=0;t<TRANSACTION_NUM;t++){
    //READ PHASE
    int tx_start = tx_glob.load(std::memory_order_seq_cst);
    tx_start_list[*ip].store(tx_start,std::memory_order_seq_cst);
    bool valid = true;
    vector<int> readset,writeset,writeset_num;
    for (int i = 0; i < LOCK_NUM; i++) {
      if(list[*ip][t].operation[i]==READ){
        int a=database[list[*ip][t].accessData[i]].value;
        readset.push_back(list[*ip][t].accessData[i]);
      }
      if(list[*ip][t].operation[i]==WRITE){
        writeset.push_back(list[*ip][t].accessData[i]);
        writeset_num.push_back(list[*ip][t].write_num[i]);
      }
    }
    //VALIDATE&WRITE
    if (pthread_mutex_lock(&m)) ERR;
    valid = validate(tx_start,tx_glob,readset);
    
    
    if(valid==true){
      tx_glob++;
      tx_mine=tx_glob.load(std::memory_order_seq_cst);;
      for(auto itr = writeset.begin(); itr != writeset.end(); ++itr){
        database[*itr].value=writeset_num[*itr];
      }
      g_write a;
      a.tx_id=tx_mine;
      a.writeset=writeset;
      writeset_glob.push_back(a);
      //writeset_glob[tx_glob]=writeset;
    }
    else{
      //abort処理
    }

    //GC
    int min_check=0;//全体のminを見てGCするほうがいい
    for(int i=0;i<THREAD_NUM;i++){
      if(tx_start_list[i].load(std::memory_order_seq_cst) < tx_start){
        min_check++;
      }
    }
    //tx_startまでのglobal_listを削除しておけ
    if(min_check==0){
      for(int i=0;i<writeset_glob.size();i++){
        if(writeset_glob[i].tx_id<=tx_start){
          writeset_glob.erase(writeset_glob.begin()+i);
          i--;
        }
      }
    }
    if(pthread_mutex_unlock(&m)) ERR;
  }
  //printf("%d\n", *ip);
  return NULL;
}

extern int
main(int argc, char *argv[])
{
  // for(int i=0;i<MAX_OBJ;i++){
  //   pthread_mutex_init( &DataObj[i].mutex, NULL );
  // }
  int i;
  int nthread = (int) THREAD_NUM;
  struct timeval begin, end;
	pthread_t *thread;

	//if (argc == 2) nthread = atoi(argv[1]);
	thread = (pthread_t *)calloc(nthread, sizeof(pthread_t));
	if (!thread) ERR;
  // for(int i=0;i<THREAD_NUM*LOCK_NUM+1;i++){
  //   writeset_glob.push_back(vector<int>(0));
  // }
  g_write a;
  a.tx_id=0;
  a.writeset.push_back(0);
  writeset_glob.push_back(a);
	

   //DBの初期化
  for(int i=0;i<LOCK_NUM;i++){
    database[i].key=i;
    database[i].value=0;
  }

  double ratio = 0.5;//atof(argv[1]);
  cout<<"ratio=="<<ratio<<endl;
  
  //各スレッドのオーダーを作成
  for(int i=0;i<nthread;i++){
    for(int n=0;n<TRANSACTION_NUM;n++){
      for(int t=0;t<LOCK_NUM;t++){
        if(drand48()<ratio){
          list[i][n].operation[t]=READ;
        }
        else{
          list[i][n].operation[t]=WRITE;
        }
        list[i][n].accessData[t]= rand() % MAX_OBJ;
        list[i][n].write_num[t]= rand() % 100;
      }
    }
  }
  
  gettimeofday(&begin, NULL);

  for (i = 0; i < nthread; i++) {
    int *tid = (int *)calloc(1, sizeof(int));
    *tid = i;
    int ret = pthread_create(&thread[i], NULL, worker, (void*)tid);
		if (ret < 0) ERR;
	}
  for (i = 0; i < nthread; i++) {
		int ret = pthread_join(thread[i], NULL);
		if (ret < 0) ERR;
	}
  gettimeofday(&end, NULL);
  print_result(begin, end, nthread);
	free(thread);
  for(int i=0;i<writeset_glob.size();i++){
    
    //cout<<writeset_glob[i]<<" ";

  }

  return 0;
}
