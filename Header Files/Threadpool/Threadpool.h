/********************************
PROGRAMMING ASSIGNMENT 3 
ThreadPool Library 

Done By:
Megha Sree Yadla
Tanvi Parikh
*********************************/

int k;
int sval;
int free_sem_loc;

/*pthread*/
pthread_t *pth;

/*mutex lock*/
pthread_mutex_t lock;

/*function details*/
struct p_details
{
	void (*fname)(void*);
	void *farg;
};
struct p_details *k1[10];

/*thread details*/
typedef struct th_details
{
	long thread_id;
	int index;
}th_stor;
th_stor th_store[10];
 
/*Semaphore*/
sem_t semaphore[10];

/********************************************************************************
 ************** Class ThreadPool *************
********************************************************************************/
class ThreadPool
{
	public:
		ThreadPool();
		ThreadPool(size_t threadCount); 
		~ThreadPool();
		bool thread_avail();
		void dispatch_function();
		int dispatch_thread(void dispatch_function(void*), void *arg);
	private:
};
/********************************************************************************
 * worker_thread function <-- will be called each time a pthread is created
********************************************************************************/
void *worker_thread(void *arg)
{
	int in;
    in = *((int*)arg);     
	pthread_t self_id;
	self_id=pthread_self();
	//store the self_id and index in a struct
	th_store[in].thread_id = self_id;
	th_store[in].index = in;
	sem_getvalue(&semaphore[th_store[in].index], &sval);
	while(1)
	{
		sem_wait(&semaphore[in]);	
		(k1[in]->fname)(k1[in]->farg);
		pthread_mutex_unlock(&lock);	
	}	
}

/********************************************************************************
 * default constructor
********************************************************************************/
ThreadPool:: ThreadPool()
{
	int count = 10;
}

/********************************************************************************
 *parameterized constructor
********************************************************************************/
ThreadPool:: ThreadPool(size_t threadCount)
{
	//SEMAPHORES
	int a;
	int count = threadCount;
	int ret, i;
	for(k=0; k<10; k++)
	{
		a = sem_init(&semaphore[k], 0, 0);
	}
	
	//Initialize MUTEX
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		cout<<"mutex init failed"<<endl;
	}	
	
	//ARRAY OF PTHREADS
	pth = new pthread_t[threadCount];
	for(k=0;k<threadCount;k++)
	{
		ret = pthread_create(&pth[k], NULL, &worker_thread, (void*)&k);
		if(ret <0)
		{
			cout<<"Error in creating thread number : "<<i<<endl;
		}
		usleep(1000);
	}

	//Value of Semaphores.
	for(int m =0; m<10; m++)
	{
		sem_getvalue(&semaphore[m], &sval);	
	}	
	cout<<"Thread Details Struct"<<endl;
	for( i=0; i<threadCount; i++)
	{
		cout<<"****************************************"<<endl;	
		cout<<"store_th["<<i<<"].thread_id is "<<th_store[i].thread_id<<endl;
		cout<<"store_th["<<i<<"].index is "<<th_store[i].index<<endl;
		cout<<"****************************************"<<endl;
	}
}

/********************************************************************************
 * destructor
********************************************************************************/
ThreadPool:: ~ThreadPool()
{
	pthread_exit(NULL);
}

/********************************************************************************
 *thread_avail <-- check availability of thread
********************************************************************************/
bool ThreadPool:: thread_avail()
{
	//to search for a free thread	
	int semval;
	int retval;
	for(int j=0; j<10;j++)
	{
		retval = sem_getvalue(&semaphore[j], &semval);
		if (semval == 0)	//if 0 then semaphore is free
		{
			pthread_mutex_lock(&lock);	
			free_sem_loc = j;
			//cout<<"Semaphore at free_sem_loc="<<j<<" is free"<<endl;
			return 1;
		}
	}
	cout<<"No semaphore is available"<<endl;
	return 0;
}

/********************************************************************************
 * dispatch_thread 
********************************************************************************/
int ThreadPool:: dispatch_thread(void dispatch_function(void*), void *arg)
{
	int l = free_sem_loc;
	k1[l] = (p_details*)malloc(sizeof(p_details));
	k1[l]->fname = dispatch_function;
	k1[l]->farg = arg;	
	sem_post(&semaphore[l]);
}


