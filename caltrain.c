#include <stdio.h>
#include "pintos_thread.h"

struct station
{
    // khai bao cho ngoi, thoi gian cho tau, khach dang len tau
    int seats, waiting, boarding;

    struct lock lock; // khai bao khoa
    struct condition train_leave; // khai bao bien dieu kien tau roi di
    struct condition seat_avalable; // khai bao bien dieu kien cho ngoi san sang
};

void station_init(struct station *station)
{
    station->seats = 0;
    station->boarding = 0;
    station->waiting = 0;

    lock_init(&station->lock);
    cond_init(&station->train_leave); // dieu kien tau roi di, tat ca hanh khach da len tau
    cond_init(&station->seat_avalable); // dieu kien tau den, cho ngoi da san sang
}

void station_load_train(struct station *station, int count)
{
    lock_acquire(&station->lock);
    
    station->seats = count;


    while (station->seats > 0 && station->waiting > 0)
    {
        cond_broadcast(&station->seat_avalable,&station->lock);
        cond_wait(&station->train_leave,&station->lock);
        
    }

    station->seats = 0;
    
    lock_release(&station->lock);
}

void station_wait_for_train(struct station* station)
{

    lock_acquire(&station->lock);
    station->waiting++;

    while (station->seats == 0 || station->boarding == station->seats)
    {
        cond_wait(&station->seat_avalable, &station->lock);
    }

    station->boarding++;
    station->waiting--;
    
    lock_release(&station->lock);
}

void station_on_board(struct station *station)
{
    lock_acquire(&station->lock);
    station->boarding--;
    station->seats--;
    if (station->seats == 0 || station->boarding == 0)
    {
        cond_signal(&station->train_leave, &station->lock);
    } 

    lock_release(&station->lock);
}
