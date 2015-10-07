#include "UIK.h"
#include "PQ.h"

TCB tcb[100];
TCB RunningTask;
uint16_t TaskStack[MaxTask][80];
uint8_t currentSP_storage[2];	//store the values of Stack Pointer Register SP_L and SP_H for tasks.
uint8_t RTOS_SP_storage[2];
uint8_t currentSP_pointer[2];	//store the two halves of the address of currentSP_storage
uint8_t FunctionPointer[2];
unsigned int CountTicks;
unsigned int UIKTickNum;
unsigned int NumberofTasks;
int PQsize;  //extern in PQ
bool firstimeflag;
int numberofschdule;
void main(void)
{
	UIKInitialize(1);
	UIKAddTask(task1,3);
	//UIKAddTask(task3,3);
	UIKAddTask(task2,3);

	sei();
	while(1);
}

void UIKInitialize(uint16_t UIKTickLen)	//UIKTickLen 0.128 milisecond ~ 32.64 miliseconds (1~32 for integer)
{
   //DDRB=0xff;  //led
   //PORTB=0xff;
   TCCR0 = (1<<WGM01)|(1<<CS02)|(1<<CS00); // CTC mode | no toggling oc0 pin | 1024 prescaler
   TIFR = (1<<OCF0);
   TIMSK = (1<<OCIE0);
   OCR0=(int)round(8000000*UIKTickLen/1000.0/1024.0);//CPU=8,000,000Hz prescalar=1024;	
   CountTicks=SwitchingTime/UIKTickLen;
   PQsize=0;
   NumberofTasks=0;
   firstimeflag=true;
   numberofschdule=0;
   UIKTickNum=0;
   currentSP_pointer[0]=(uint16_t)currentSP_storage & 0x00ff;
   currentSP_pointer[1]=((uint16_t)currentSP_storage & 0xff00)>>8;
   UIKAddTask(UIKIdle,0);
}



bool Check_exsit_ID(uint8_t id)
{
   int i;
   for(i=0;i<PQsize;i++)
	{
	 if(id==tcb[i].TaskID)
	 return false;
	}
   return true;
}
uint8_t UIKAddTask(void(*task)(void), uint8_t priority)
{

	
   /*srand(1);
   uint8_t taskid=rand()%256;  //assign generated 0-255 random task ID number to the new task
   while(!Check_exsit_ID(taskid))
	{
	   taskid=rand()%256;	//regenerate a different task id
	}*/
	
		
   uint8_t taskid=NumberofTasks;
	NumberofTasks++;
	

   TCB item;
   item.Priority=priority;
   item.TaskID=taskid;
   item.status=NEW;
   item.functionpointer=task;
   enPQ(tcb,item);
   return taskid;	
}


void UIKRun(TCB item)
{

	RunningTask=item;

//prepare the self-defined specified stack address for the system stack pointer register, it will be loaded momentarily in the following assembly code.
	currentSP_storage[0]=((uint16_t)TaskStack[RunningTask.TaskID]) & 0x00ff;
	currentSP_storage[1]=(((uint16_t)TaskStack[RunningTask.TaskID]) & 0xff00)>>8;
	
	//RunningTask.functionpointer();	//call the function

	FunctionPointer[0]=((unsigned int)RunningTask.functionpointer) & 0x00ff;		//store lower  8-bit into FunctionPointer[0]
	FunctionPointer[1]=(((unsigned int)RunningTask.functionpointer) & 0xff00)>>8;		//store higher 8-bit into FunctionPointer[1]


//change (load) the system stack pointer register with new defined task specified stack address
	asm volatile
		(
			"lds r26, currentSP_pointer" "\n\t" 
			"lds r27, currentSP_pointer+1" "\n\t"
			"ld r28, x+" "\n\t"
			"out __SP_L__, r28" "\n\t"
			"ld r29, x+" "\n\t"
			"out __SP_H__, r29" "\n\t"
		);


//load FunctionPointer[0] into r26
//load FunctionPointer[1] into r27

//push functionPointer (in the X register) onto stack
	asm volatile
		(
			"lds r26, FunctionPointer" "\n\t"		
			"lds r27, FunctionPointer+1" "\n\t"		
			"push r26" "\n\t"
			"push r27" "\n\t"
		);
	asm volatile("reti");	//reti will automatically pop the functionPointer into "Program Counter Register" then the prgram runs.


	
}

void UIKSchedule(void)
{



		TCB item;
		if(firstimeflag)	//since no task running in the memory, dePQ first.
		{
			RTOS_SP_storage[0]=currentSP_storage[0];
			RTOS_SP_storage[1]=currentSP_storage[1];
			item=dePQ(tcb); 
			firstimeflag=false;
		}
		else
		{
			RunningTask.StackPointer[0]=currentSP_storage[0];
			RunningTask.StackPointer[1]=currentSP_storage[1];
			RunningTask.status=READY;
			enPQ(tcb,RunningTask);
			item=dePQ(tcb);
		}
		if(item.status==NEW)
			{
				item.status=RUN;
				UIKRun(item);
			}
		else if(item.status==READY)
			{
			  UIKDispatcher(item);
			  currentSP_storage[0]=RunningTask.StackPointer[0];
			  currentSP_storage[1]=RunningTask.StackPointer[1];
			}
		else ;



		//asm volatile("ret");

}

void UIKDispatcher(TCB item)
{
	item.status=RUN;
	RunningTask=item;
}

void UIKIdle(void)
{
 while(1)
	{
	;
	}
}


bool UIKTickHandler(void)
{
UIKTickNum++;
if(UIKTickNum > CountTicks)
	{
	UIKTickNum=0;
	return true;
	}
else
	return false;
}


ISR(TIMER0_COMP_vect,ISR_NAKED)
{
	SAVE_CONTEXT();

	if(UIKTickHandler())
		UIKSchedule();

	RESTORE_CONTEXT();



	asm volatile("reti");
}

void delay(unsigned int dly)
{
   int i;

   for(i=dly;i!=0;i--);
}

void task1(void)
{
	DDRB=0xff;  //led
	PORTB=0xff;
	uint8_t light;
	int i;
 while(1)
  {	
	light=0;
	for(i=7;i>-1;i--)
	{
	light|=1<<i;
	PORTB=~light;
	delay(20000U);
	}
	PORTB=0xff;
  }

}

void task2(void)
{
	DDRB=0xff;  //led
	PORTB=0xff;
	uint8_t light;
	int i;
 while(1)
  {	
	light=0;
	for(i=0;i<8;i++)
	{
		light|=1<<i;
		PORTB=~light;
		delay(20000U);
	}
	PORTB=0xff;
  }
}

void task3(void)
{
 while(1)
	{
		DDRB=0xff;  //led
		PORTB=~24;
		delay(1000U);		
		PORTB=0xff;
		
		
	}
}

