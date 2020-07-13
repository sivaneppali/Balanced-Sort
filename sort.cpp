#include <iostream>
#include<vector>
#include<fstream>
#include<pthread.h>
#include<math.h>
#include<semaphore.h>

using namespace std;

int count=0;
string Mode;
bool T;
vector<int>listnumbers;

struct ThreadInput
{
int ThreadId;
int NumberSize;
};

typedef struct ThreadInput InputStruct;

sem_t S1;
sem_t S2;
sem_t S3;

void *BalancedSorting(void *);

int main(int argc,char *argv[])
{
int N,P,S,Threadcount;
ifstream inputfile;
inputfile.open(argv[1]);
ifstream semafile;
semafile.open("sema.init.txt");
semafile>>S;
int Sem[S];
for (int J=0; J<S; J++)
{
semafile>>Sem[J];
}

Mode=argv[2];

inputfile>>N;

sem_init(&S1, 0, Sem[0]);
sem_init(&S2, 0, Sem[1]);
sem_init(&S3, 0, Sem[2]);

while(N != 0)
   {
for (int i=0; i<N; i++)
      {
      inputfile>>P;
      listnumbers.push_back(P);
      cout<<listnumbers[i]<<"\t" ;
     if((i+1)%8==0) cout<<endl;
      }
if(N<8) cout<<endl;

Threadcount=N/2;
pthread_t tid[Threadcount];
for (int K = 0; K < Threadcount; K++ )
    {
  InputStruct *MyInput;
  MyInput = (InputStruct*) malloc(sizeof(InputStruct));
  MyInput->ThreadId=K;
  MyInput->NumberSize=N;
  pthread_create( &tid[K], NULL, BalancedSorting,(void*)MyInput );
    }

for ( int L = 0; L < Threadcount; L++ )
  {
    pthread_join( tid[L], NULL );
   }

for (int M=0; M<N; M++)
      {
      cout<<listnumbers[M]<<"\t" ;
      if((M+1)%8==0)cout<<endl;
      }
if(N<8) cout<<endl;

cout<<"----------------------------------------------------------------"<<endl; //Can Also use -- For loop

 listnumbers.clear();
 inputfile>>N;
  }
inputfile.close();

    return 0;
}


void *BalancedSorting(void *arg)
{
InputStruct *arg_struct= (InputStruct*) arg;

int num_groups,group_size,gindex,mindex,group_start,group_end,data1,data2,temp;

for (int S = 1; S<= (log2(arg_struct->NumberSize)) ; S++)
{
for (int P = 1; P<= (log2(arg_struct->NumberSize)) ; P++)
{

sem_wait(&S1);
   count += 1;

     if (count == arg_struct->NumberSize/2)
         {
           sem_wait(&S3);
           sem_post(&S2);
         }
sem_post(&S1);

      sem_wait(&S2);
      sem_post(&S2);

num_groups = pow(2 ,(P-1));   // you can start from 1, and set num_groups *= 2 after each phase
group_size =arg_struct->NumberSize / num_groups;
gindex = arg_struct->ThreadId/ (group_size / 2);
mindex = arg_struct->ThreadId % (group_size / 2); // mindex: group member index
group_start = gindex * group_size;
group_end = (gindex + 1) * (group_size)- 1;
data1 = group_start + mindex;
data2 = group_end - mindex;

if(listnumbers[data1]>listnumbers[data2])  //Check And Swap If Required
     {
     temp=listnumbers[data1];
     listnumbers[data1]=listnumbers[data2];
     listnumbers[data2]=temp;
     };

for (int J= 1; J<= (data1+data2) ; J++) //loop for (data1+data2) iterations doing nothing (just to introduce different computation times);
    {

    }
sem_wait(&S1);
  if (Mode=="-o")
  {
  cout<< "Thread\t"<<arg_struct->ThreadId<<"\tfinished stage\t"<<S<<"\tphase\t"<<P<<endl;
   }

 count -= 1;
     if (count == 0)
      {
      if (Mode=="-o")
      {
        for (int i=0; i<arg_struct->NumberSize; i++)
          {
          cout<< listnumbers[i]<<"\t";
          if((i+1)%8==0) cout<<endl;
           }
          if(arg_struct->NumberSize<8) cout<<endl;
       }
 for (int K=1; K<arg_struct->NumberSize; K++)
       {
         if (listnumbers[K-1]<=listnumbers[K])T=true;
         else
          {
          T=false;
          break;
          }
        }
      sem_wait(&S2);

      sem_post(&S3);
      }
sem_post(&S1);

sem_wait(&S3);
sem_post(&S3);

}
if(T==true) return 0;
}
return 0;
}
