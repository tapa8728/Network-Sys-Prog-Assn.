/********************************
PROGRAMMING ASSIGNMENT 3 
Message Library 

Done By:
Megha Sree Yadla
Tanvi Parikh
*********************************/
#include<list>

/********************************************************************************
 ************** Class Message*************
********************************************************************************/
class Message
{
	public:  
		Message( );
		Message(char* msg, size_t len);
		~Message( );
		void msgAddHdr(char *hdr, size_t length);
		char* msgStripHdr(int len);
		int msgSplit(Message& secondMsg, size_t len);
		void msgJoin(Message& secondMsg);
		size_t msgLen( );
		void msgFlat(char *buffer);
	private:
		size_t msg_length;
		list<char*> *msgs=NULL;
};

/********************************************************************************
 * Default Constructor
********************************************************************************/
Message::Message()
{
    msg_length=0;
    msgs=new list<char*>();
}

/********************************************************************************
 * Parameterized Constructor
********************************************************************************/
Message::Message(char* msg, size_t len)
{
    msg_length=len;
    msgs=new list<char*>();
    
    for (int i=0;i<(int)len;i++) 
    {
        msgs->push_back(msg+i);
    }    
}

/********************************************************************************
 * Destructor
********************************************************************************/
Message::~Message( )
{
    delete msgs;
}

/********************************************************************************
 *Add Header
********************************************************************************/
void Message::msgAddHdr(char *hdr, size_t length)
{
    for (int i=0;i<(int)length;i++) 
    {
		msgs->push_front(hdr+(int)length-1-i);
    }
    msg_length=msg_length+length;
}

/********************************************************************************
 *Strip Header
********************************************************************************/
char* Message::msgStripHdr(int len)
{
    char *stripped_content;
    if ((msg_length < len) || (len == 0)) 
		return NULL;

    stripped_content = new char[len];    
    for (int i=0;i<(int)len;i++)
    {
        stripped_content[i]=*(msgs->front());
        msgs->pop_front();
    }    
    msg_length=msg_length-len;
    return stripped_content;
}

/********************************************************************************
 *Split Message
********************************************************************************/
int Message::msgSplit(Message& secondMsg, size_t len)
{
    size_t length = msg_length;
    if ((len < 0) || (len > msg_length)) 
		return 0;

    for(int i=(int)length-1; i>(int)len-1; i--)
    {
		secondMsg.msgAddHdr(msgs->back(),1);
		msgs->pop_back();
    }    
	msg_length=len;       
    secondMsg.msg_length=length-len;
    return 1;
}

/********************************************************************************
 *Join Message
********************************************************************************/
void Message::msgJoin(Message& secondMsg)
{   
    size_t len;
    len = secondMsg.msgLen();
    
    for(int i=0;i<(int)len;i++)
    {
        msgs->push_back(secondMsg.msgs->front());
        secondMsg.msgs->pop_front();
    }    
    msg_length=msg_length+len;
}

/********************************************************************************
 *Get Message Length
********************************************************************************/
size_t Message::msgLen( )
{
    return msg_length;
}

/********************************************************************************
 *Save Message into Buffer
********************************************************************************/
void Message::msgFlat(char *buffer)
{
    int j=0;
    for (list<char*>::iterator i = msgs->begin(); i!=msgs->end(); i++) 
    {
        *(buffer+j) = *(*i);
		j++;
    }
}
