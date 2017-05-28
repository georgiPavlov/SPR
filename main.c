#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <pthread.h>
#define  COINS 10
#define  COINS_SHOP 30


float coinsValues[COINS]= {100,50,20,10,5,2,1,0.5,0.2,0.1};
int coinProduserCounts[COINS];
int coinConsumerCounts[COINS];
float differ=0;
void initCountCoins();
void init(float recvamount, float billamont, int coinsCount[]);
int returnCoin(int coin, int coinsCount[]);
void generateRandomCountValues();
void getMoney();
void returnMoney();
void initShopCoins();
float globalBill=0;
float globalRecv=0;
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;
void* producer(void *ptr);
void* consumer(void *ptr);
int buffer=0;

void initShopCoins()
{
    for(int i=0; i<COINS; i++)
    {
        coinConsumerCounts[i] = COINS_SHOP;
    }
}

void returnMoney()
{
    printf("_________ SHOP ____________ \n");
    for(int i=0 ; i < COINS;i++)
    coinConsumerCounts[i] +=coinProduserCounts[i];
    init(globalRecv,globalBill,coinConsumerCounts);
    printf("_________ SHOP ____________ \n");
}


void* producer(void *ptr)
{



    pthread_mutex_lock(&the_mutex);
    while (buffer != 0)
        pthread_cond_wait(&condp, &the_mutex);

    generateRandomCountValues();
    buffer = 1;

    pthread_cond_signal(&condc);
    pthread_mutex_unlock(&the_mutex);

    pthread_exit(0);
}

void* consumer(void *ptr)
{
    int i;


    pthread_mutex_lock(&the_mutex);
    while (buffer == 0)
        pthread_cond_wait(&condc, &the_mutex);

    returnMoney();
    buffer = 0;
    pthread_cond_signal(&condp);
    pthread_mutex_unlock(&the_mutex);

    pthread_exit(0);
}


void getMoney()
{

    for(int i =0 ; i < COINS ; i++ )
    {
        globalRecv += coinProduserCounts[i] * coinsValues[i];
    }
    printf("Money to send %0.1f \n", globalRecv);
}

void generateRandomCountValues()
{
    initCountCoins();
    time_t t;
    srand((unsigned) time(&t));
    float billamont = (rand() % 5000) / 10;
    float recvamount=0;
    while(billamont > recvamount)
    {
        int r = rand() % 10;
        recvamount+=coinsValues[r];
        coinProduserCounts[r] += 1;
    }
    printf("bill:  %0.1f ", billamont);
    printf("recv:  %0.1f ", recvamount);
    printf(" \n _________ CLIENT ____________ \n");
    init(recvamount,billamont,coinProduserCounts);
    printf("_________ CLIENT ____________ \n");
    globalBill = billamont;
    getMoney();


}

int returnCoin(int coin, int coinsCount[])
{

    float coinValue = coinsValues[coin];
    int coinCount = coinsCount[coin];
    int u=0;
    if(differ < 1 && coinValue < 1)
    {
        differ = roundf(differ * 10)  ;
        coinValue *=10;
        u=1;
    }
    int numCoins = (int)floor(differ / coinValue);
    // printf("%d" , numCoins);
    if (numCoins > coinCount)
    {
        differ = differ - (coinCount * coinValue);
        numCoins = coinCount;

    }
    else
    {
        differ = differ - (numCoins * coinValue);


    }
    if(u == 1)
    {
        differ /=10;
    }
    return numCoins;
}




void init(float recvamount, float billamont, int coinsCount[])
{
    differ = recvamount - billamont;
    int tempCoin=0;
    for(int i=0; i < COINS; i++)
    {
        tempCoin=returnCoin(i, coinsCount);
        coinsCount[i] -= tempCoin;
        printf("number of coins return: %d  value of %.1f total coins: %d \n",tempCoin , coinsValues[i],coinsCount[i] );
    }
}
void initCountCoins()
{

    for(int i=0; i < COINS; i++)
    {
        coinProduserCounts[i]=0;
    }
}




int main()
{
    initShopCoins();
    pthread_t pro, con;

    pthread_mutex_init(&the_mutex, NULL);
    pthread_cond_init(&condc, NULL);
    pthread_cond_init(&condp, NULL);


    pthread_create(&con, NULL, consumer, NULL);
    pthread_create(&pro, NULL, producer, NULL);

    pthread_join(&con, NULL);
    pthread_join(&pro, NULL);


    pthread_mutex_destroy(&the_mutex);
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    return 0;
}


