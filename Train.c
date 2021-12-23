

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct
{
     
    int train_in_station;//train in station or not used for acknowldging the passengers
   
}station;
typedef struct
{
    station ST;
    int count;
}station_plus_trainseats;


void *station_load_train(station_plus_trainseats *mainstation);
void *station_wait_for_train(station *station);
void station_on_board(station *station);

pthread_mutex_t mutex_station_trainpresent;
pthread_cond_t  condition_var;
pthread_cond_t  condition_var_train;
pthread_cond_t  condition_var_train_full;
 
 int passengers_waiting;//number of passengers waiting
 int flag=0;
 int No_of_seats_available;
#define NO_OF_TRAINS 3
int main()
{

    pthread_mutex_init(&mutex_station_trainpresent , NULL);
    pthread_mutex_init(&condition_var, NULL);
    pthread_mutex_init(&condition_var_train, NULL);
    pthread_mutex_init(&condition_var_train_full, NULL);


//declaring struct that contains the struct station and the count of passengers empty in the train
station_plus_trainseats ST_plus_trainseats[NO_OF_TRAINS];

//declaring the station
station mainstation;
//initally no trains and no pasengers in mainstation
mainstation.train_in_station=0;
passengers_waiting=20;
int initial_passengers_waiting=passengers_waiting;
//declaring a pointer for the Struct (station struct and count)
station_plus_trainseats * ptr_ST_plus_trainseats[NO_OF_TRAINS];

//assinging the data in the Struct (station struct and count)
for(int i=0;i<NO_OF_TRAINS;i++)
{
ST_plus_trainseats[i].ST=mainstation;
//pointer
ptr_ST_plus_trainseats[i]=&ST_plus_trainseats[i];
}
ST_plus_trainseats[0].count=15;
 ST_plus_trainseats[1].count=4;
 ST_plus_trainseats[2].count=3;
// ST_plus_trainseats[3].count=4;
// ST_plus_trainseats[4].count=15;


pthread_t passenger_thread[passengers_waiting];
int ret_passenger_thread[passengers_waiting];
// craeting threads for passengers
station *ptr_mainstation;
ptr_mainstation=&mainstation;
for(int i=0;i<initial_passengers_waiting;i++)
{
    ret_passenger_thread[i] = pthread_create(&passenger_thread[i], NULL,station_wait_for_train, (void *)ptr_mainstation);
    if (ret_passenger_thread[i])
    {
        fprintf(stderr, "Error - pthread_create() return code: %d\n",ret_passenger_thread[i]);
        exit(EXIT_FAILURE);
    }


}

pthread_t train_thread[NO_OF_TRAINS];
int ret_train_thread[NO_OF_TRAINS];
//creating threads for train
     for(int i=0;i<NO_OF_TRAINS;i++)
     {
      ret_train_thread[i] = pthread_create(&train_thread[i], NULL,station_load_train, (void *)ptr_ST_plus_trainseats[i]);
     
    
    if (ret_train_thread[i])
    {
        fprintf(stderr, "Error - pthread_create() return code: %d\n",ret_train_thread[i]);
        exit(EXIT_FAILURE);
    }
     }




for(int i=0;i<NO_OF_TRAINS;i++)
{
    pthread_join(train_thread[i], NULL);
}
// for(int i=0;i<NO_OF_TRAINS;i++)
// {
//    printf("no of places remaining in train %d after leaving- %d\n",i+1,ST_plus_trainseats[i].count);
// }
for(int i=0;i<initial_passengers_waiting;i++)
{
    pthread_join(passenger_thread[i], NULL);
}
printf("passengers waiting - %d",passengers_waiting);

}

void *station_load_train(station_plus_trainseats *mainstation)
{
//lock for passengers and trains
pthread_mutex_lock( &mutex_station_trainpresent );

//incase there is another train in station wait till it leaves
while(flag == 1)
pthread_cond_wait(&condition_var_train, &mutex_station_trainpresent);

static train_number=0;
train_number++;
printf("train %d arrived \n",train_number);
No_of_seats_available=mainstation->count;
flag=1;
printf("available seats when arriving in train %d- %d\n",train_number,No_of_seats_available);
printf("no of passengers waiting when arriving in train %d- %d\n",train_number,passengers_waiting);


//inform the waiting passengers that a train has arrived
pthread_cond_broadcast(&condition_var);


//it is signaled when a passenger gets in a train
//keeps iterating until the staition is full or there is no passengers
 while( (No_of_seats_available > 0)  && passengers_waiting > 0)
       {
        pthread_cond_wait(&condition_var_train_full,&mutex_station_trainpresent);
 
       }
mainstation->count=No_of_seats_available;
flag=0;



printf("available seats before leaving in train %d -%d \n",train_number,((*mainstation).count));
printf("passengers waiting before leaving in train %d -%d\n",train_number,passengers_waiting);

pthread_cond_signal( &condition_var_train);
printf("train %d left\n",train_number);
pthread_mutex_unlock( &(mutex_station_trainpresent) );
}


void *station_wait_for_train(station *station)
{

pthread_mutex_lock( &mutex_station_trainpresent);

//in case no train in station or there is a train but is full
while( flag == 0    || No_of_seats_available==0 ) 
    pthread_cond_wait(&condition_var,&mutex_station_trainpresent);
station_on_board(station);

//inform the train that a passengers gets in a train
pthread_cond_signal(&condition_var_train_full);
//printf("p---------passengers onn - %d\n",passengers_waiting);
pthread_mutex_unlock( &mutex_station_trainpresent );

return;
}

//decrease the number of passengers waiting and the no of seats available in the train
void station_on_board(station *station)
{
    static int passenger_number=0;
No_of_seats_available--;
passenger_number++;
passengers_waiting--;
printf("PASSENGER %d - %d\n",passenger_number,passengers_waiting);
return;
}
