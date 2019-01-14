#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <unistd.h>

using namespace std;

sem_t hotelRoomSem;
sem_t frontDeskSem;
sem_t outputSem;
sem_t deskGreet1Sem;
sem_t giveRoomSem;
sem_t logGuest;
sem_t deskGreet2Sem;
sem_t giveReceiptSem;
sem_t paidSem;

int roomAndGuest[6][1] = {{0}, {0}, {0}, {0}, {0}, {0}};
int currentGuest;
int currentRoom;

bool checkin = false;
bool checkout = false;

int pool = 0;
int restaurant = 0;
int fitnessCenter = 0;
int businessCenter = 0;
int cost = 0;

void* guest (void *i){
	int id = *((int *) i) + 1;
	int localRoom = 0;
	delete (int*)i;
	sem_wait(&frontDeskSem);
	checkin = true;
	currentGuest = id;
	sem_wait(&outputSem);
	cout << "Guest " << id << " goes to front desk for check in.\n";
	sem_post(&outputSem);
	sem_post(&deskGreet1Sem);
	sem_wait(&giveRoomSem);
	sem_wait(&outputSem);
	cout << "Guest " << id << " receives key for room " << currentRoom << " from front desk.\n";
	sem_post(&outputSem);
	localRoom = currentRoom;
	sem_post(&frontDeskSem);
	int entertainment = ((rand() % 100) + 1);
	int sleepTime = ((rand() % 3) + 1);
	if(entertainment <= 25){
		sem_wait(&logGuest);
		pool++;
		sem_post(&logGuest);
		sem_wait(&outputSem);
		cout << "Guest " << id << " goes to the hotel swimming pool.\n";
		sem_post(&outputSem);
		sleep(sleepTime);
	}
	else if (entertainment <= 50 && entertainment > 25){
		sem_wait(&logGuest);
		restaurant++;
		sem_post(&logGuest);
		sem_wait(&outputSem);
		cout << "Guest " << id << " goes to the hotel restaurant.\n";
		sem_post(&outputSem);
		sleep(sleepTime);
	}
	else if (entertainment <= 75 && entertainment > 50){
		sem_wait(&logGuest);
		fitnessCenter++;
		sem_post(&logGuest);
		sem_wait(&outputSem);
		cout << "Guest " << id << " goes to the hotel fitness center.\n";
		sem_post(&outputSem);
		sleep(sleepTime);
	}
	else if(entertainment > 75){
		sem_wait(&logGuest);
		businessCenter++;
		sem_post(&logGuest);
		sem_wait(&outputSem);
		cout << "Guest " << id << " goes to the hotel business center.\n";
		sem_post(&outputSem);
		sleep(sleepTime);
	}
	sem_wait(&frontDeskSem);
	currentRoom = localRoom;
	currentGuest = id;
	checkout = true;
	sem_wait(&outputSem);
	cout << "Guest " << id << " goes to front desk to return key for room " << currentRoom << ".\n";
	sem_post(&outputSem);
	sem_post(&deskGreet2Sem);
	sem_wait(&giveReceiptSem);
	sem_wait(&outputSem);
	cout << "Guest " << id << " receives receipt for $" << cost << ".\n";
	cout << "Guest " << id << " makes a $" << cost << " payment.\n";
	sem_post(&paidSem);
	sem_post(&outputSem);
	sem_post(&frontDeskSem);
	return NULL;
}

void* frontDesk(void *i){
	while(true){
		if(checkin == true){
			sem_wait(&deskGreet1Sem);
			sem_wait(&outputSem);
			cout << "Guest " << currentGuest << " is greeted by front desk.\n";
			sem_post(&outputSem);
			for(int i = 0; i < 5; i++){
				if(roomAndGuest[i][1] == 0){
					roomAndGuest[i][1] = currentGuest;
					currentRoom = i + 1;
					break;
				}	
			}
			sem_wait(&outputSem);
			cout << "Front desk has booked room " << currentRoom << " for guest "<< currentGuest << ".\n";
			sem_post(&outputSem);
			sem_post(&giveRoomSem);
			checkin = false;
		}
		if(checkout == true){
			sem_wait(&deskGreet2Sem);
			sem_wait(&outputSem);
			cout << "Front desk greets guest " << currentGuest << " and receives key for room " << currentRoom << ".\n";
			sem_post(&outputSem);
			roomAndGuest[currentRoom - 1][1] = 0;
			cost = rand() % (1000 - 350 + 1) + 350;
			sem_wait(&outputSem);
			cout << "Front desk gives guest " << currentGuest << " a receipt for $" << cost << ".\n";
			sem_post(&outputSem);
			sem_post(&giveReceiptSem);
			sem_wait(&paidSem);
			sem_wait(&outputSem);
			cout << "Front desk receives payment from guest " << currentGuest << ", processes the payment and wishes guest " << currentGuest << " a pleasant day.\n";
			sem_post(&outputSem);
			sem_post(&hotelRoomSem);
			checkout = false;
		}
	}
}

int main() {
	sem_init(&hotelRoomSem, 0, 5);
	sem_init(&frontDeskSem, 0, 1);
	sem_init(&outputSem, 0, 1);
	sem_init(&logGuest, 0, 1);
	sem_init(&deskGreet1Sem, 0, 0);
	sem_init(&deskGreet2Sem, 0, 0);
	sem_init(&giveRoomSem, 0, 0);
	sem_init(&giveReceiptSem, 0, 0);
	sem_init(&paidSem, 0, 0);

	pthread_t threads [10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	pthread_t front;

	pthread_create(&front, NULL, frontDesk, NULL);

	srand((int)time(0));

	for(int i = 0; i < 10; i++){
		int a = i;
		sem_wait(&hotelRoomSem);
		sem_wait(&outputSem);
		cout << "Guest " << a + 1 << " has entered the hotel.\n";
		sem_post(&outputSem);
		pthread_create(&threads[i], NULL, guest, (void *) new int(i));
	}
	for(int i = 0; i < 10; i++){
		pthread_join( threads[i], NULL);
	}
	cout << "Total Guests: 10\nPool: " << pool << "\nRestaurant: " << restaurant << "\nFitness Center: " << fitnessCenter << "\nBusiness Center: " << businessCenter <<"\n";
}