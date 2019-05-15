#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include<time.h>
#include <ctime>        
#include <algorithm>    
#include <vector>      
#include "mpi.h"
#include <semaphore.h>

////////////////////////////////////////////////
//	COMPILE WITH "-PTHREAD" FLAG
//
//
//////////////////////////////////////////////////

struct Message
{
	int reciever;
	int sender;
	int value;
	int sent_time;
}

//===============GLOBAL VARIABLES=====================

//=====CONSTS
#define MSG_LAZARET_APPROVAL 1
#define MSG_TELEPORT_APPROVAL 2
#define MSG_LAZARET_REQUEST 3
#define MSG_TELEPORT_REQUEST 4
#define MSG_SIZE 6

//======VECTORS
std::vector<int> lazaret_queue;
std::vector<int> teleport_queue;
std::vector<Message> to_send_queue;


//======SEMAPHORES
//every semaphore should be initialized in initialize_semaphores method
sem_t SEMAPHORE_CRITICAL;

//======BOOLEANS
bool critical, apply_for_critical,apply_for_teleport;

//======INTEGERS
int teleport_number, my_apply_for_critical_time, rank;

//===============GLOBAL VARIABLES===================

//verified
initialize_semaphores(){
    sem_init(&SEMAPHORE_CRITICAL, 0, 1);
}

//verified
void wait_random_time(){
	sleep(3+(int)(rand() % 2));
}

void print_vector(std::vector<int>& data)
{
	for(int val : data){
		printf("%d", val);
	}
	printf("\n");
}

//verified
void receive()
{
	MPI_Status status;
	double msg[MSG_SIZE];

	while(1==1)
	{
		MPI_Recv(msg, MSG_SIZE, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 

		switch(status.MPI_TAG)
		{
			case MSG_LAZARET_APPROVAL: break;
			case MSG_TELEPORT_APPROVAL: break;
			case MSG_LAZARET_REQUEST: break;
			case MSG_TELEPORT_REQUEST: break;
		}
	}
}

bool can_answer_lazaret_approval(int request_time, int process_id)
{
	sem_wait(&SEMAPHORE_CRITICAL);
	
	bool send_approval = !critical && !apply_for_critical ||
			apply_for_critical && request_time < my_apply_for_critical_time ||
			apply_for_critical && request_time == my_apply_for_critical_time && process_id < rank;

	
	if(send_approval)
		send_approval_to_process(process_id, MSG_LAZARET_APPROVAL);	
	else
		lazaret_queue.push_back(process_id)	

	sem_post(&SEMAPHORE_CRITICAL);
}

//verified
void send_approval_to_process(int proc_id, int type)
{    
	double msg[MSG_SIZE];
	MPI_Send( msg, MSG_SIZE, MPI_DOUBLE, proc_id, type, MPI_COMM_WORLD );
}

//verified
void process_queue(std::vector<int>& data, int approval_type)
{
	int size = data.size();

	//remove duplicates
	for( size_t i = 0; i < size-1; i++ )
		for( size_t j = i+1; j < size; j++ )
			if(data[i] == data[j])
			{
				data.erase(data.begin()+j);
				size--;
				j--;
			}
	printf("Wysyłam %d do:\n", approval_type);
	print_vector(data);
	
	//Send approval for all in queue
	for( size_t i = 0; i < size-1; i++ )
		send_approval_to_process(data[i], approval_type);
}



int main( int argc, char **argv ){

	initialize_semaphores();

	//semaphore works
	sem_wait(&SEMAPHORE_CRITICAL);
   //your critical section code
	sem_post(&SEMAPHORE_CRITICAL);

	// int rank,size,sender=0,receiver=1;
	// int msg[MSG_SIZE];
	// MPI_Status status;

	// MPI_Init(&argc, &argv);
	// MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    // //MPI_COMM_WORLD - stała oznaczająca grupę, do której należą wszyscy
    // //rank - numer procesu
	// MPI_Comm_size( MPI_COMM_WORLD, &size );

	// srand(time(0)+rank);//inicjalizowanie zmiennych losowych

	// if ( rank == sender)
	// {
	// 	send_approval_to_process(receiver,MSG_LAZARET_APPROVAL);
	// }
	// if(rank==receiver)
	// {
	// 	receive();
	// }

	// msg[0]=0;
	// int lastToken=0;
	// int token;
	// pthread_t t;
	// pthread_create(&t, NULL, petla, &token);
	// pthread_mutex_lock(&lock);
	
	// do
	// {
	// 	sleep(3+(int)(rand() % 2));
	// 	pthread_mutex_lock(&lock);

	// 	token++;
	// 	lastToken = token;
	// 	msg[0]=token;

	// 	MPI_Send( msg, MSG_SIZE, MPI_INT, (rank+1)%size, MSG_HELLO, MPI_COMM_WORLD );
	// }
	// while(msg[0]+size-1<=100);

	// pthread_cancel(t);
	// MPI_Finalize();
}