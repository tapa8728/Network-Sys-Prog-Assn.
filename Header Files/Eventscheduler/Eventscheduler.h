/********************************
PROGRAMMING ASSIGNMENT 3 
Eventscheduler Library 

Done By:
Megha Sree Yadla
Tanvi Parikh
*********************************/
#include<cstdlib>

int max_events; //default value
static int k;
pthread_t pth[10];	

/*details about each event*/
struct event_details
{
	void (*fname)(void*);
	void *farg;
	int e_id;
	int t_out;
	int cancel;
};
struct event_details *ed[10];

/********************************************************************************
 ************** Class ThreadPool *************
********************************************************************************/
class EventScheduler
{
	public:
		EventScheduler();
		EventScheduler(size_t maxEvents); 
		~EventScheduler();
		int eventSchedule(void evFunction(void *), void *arg, int timeout);
		void eventCancel(int eventId);
		void evFunction();
	private:
};

/********************************************************************************
 * worker_thread function <-- will be called each time a pthread is created
********************************************************************************/
void *worker_thread(void *arg)
{
	struct timeval timeout;
	int index;
	index = *((int *)arg);
	//setting th timeval struct variables
	timeout.tv_sec =0;
	timeout.tv_usec=ed[index]->t_out;
	if(select(1, NULL, NULL, NULL, (struct timeval*)&timeout) >=0)
	{
		if(ed[index]->cancel==0)
		{
			//if event is not cancelled then run the function
			(ed[index]->fname)(ed[index]->farg);
		}
		else
		{
			cout<<"The event has been cancelled :"<<endl;
		}
	}
	pthread_exit(NULL);
}

/********************************************************************************
 * default constructor
********************************************************************************/
EventScheduler:: EventScheduler()
{
	max_events=10; //default value
	//initializng the structure
	for(int r=0; r<max_events; r++)
	{
		ed[r] = (event_details*)malloc(sizeof(event_details));
		ed[r]->cancel =0; //default cancel value
		ed[r]->e_id=111;  //default event id for all events
	}
}

/********************************************************************************
 * Parameterized constructor
********************************************************************************/
EventScheduler:: EventScheduler(size_t maxEvents)
{
	max_events = maxEvents;
	//initializng the structure	
	for(int r=0; r<max_events; r++)
	{
		ed[r] = (event_details*)malloc(sizeof(event_details));
		ed[r]->cancel =0; //default cancel value
		ed[r]->e_id=111;  //default event id for all events
	}
}

/********************************************************************************
 * destructor
********************************************************************************/
EventScheduler:: ~EventScheduler()
{
	
}

/********************************************************************************
 * eventSchedule function <-- will be called to schedule an event
********************************************************************************/
int EventScheduler::eventSchedule(void evFunction(void *), void *arg, int timeout)
{
	int m;	
	int to = timeout;
	for(m=0; m<max_events; m++)
	{
		if(ed[m]->e_id==111)
		{
			ed[m]->fname = evFunction;
			ed[m]->farg = arg;
			ed[m]->t_out =to;
			ed[m]->e_id = m;
			ed[m]->cancel=0;
			break;
		}
	}
	int ret = pthread_create(&pth[m], NULL, &worker_thread, (void*)&m);
	if(ret<0)
	{
		cout<<"Error in creating thread "<<endl;
	}
	usleep(1000);
	return m;
}

/********************************************************************************
 * eventCancel function <-- will be called to cancel an event
********************************************************************************/
void EventScheduler::eventCancel(int eventId)
{
	for(int j=0; j<max_events; j++)
	{	
		if(ed[j]->e_id == eventId)
		{
			ed[j]->cancel =1;
			cout<<"<~~~~~~~~~~~~~Cancelling Event---------->"<<ed[j]->e_id<<endl;
			break;
		}
	}
}


