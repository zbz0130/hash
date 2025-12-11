#include <cstdio>
#include<iostream>
#include "hashtable.h"
#include <ctime>
#include<string>
#include <cstring>
#include<fstream>
// #include "hashtable.cpp"
using namespace std;
hashing_strategy* get_hash_strategy(int i)
{
    switch(i)
    {
        case 0:
        return new naive_hashing();
        break;
        case 1:
        return new bad_hashing();
        break;
        case 2:
        return new good_hashing();
        break;
        default:
        return new naive_hashing();
    }
    return NULL;
}
collision_strategy* get_collision_strategy(int i)
{
    switch(i)
    {
        case 0:
        return new linear_probe();
        break;
        case 1:
        return new quadratic_probe();
        break;
        case 2:
        return new public_overflow();
        dsfault:
        return new linear_probe();
    }
    return new linear_probe();
}

int main(int argc,char* argv[]){
    if(argc<3)
    {
        cerr<<"number inefficient"<<endl;
    }
    int type;
    char buffer[1000];int data;
    int hash_type = atoi(argv[1]);
    int collision_type = atoi(argv[2]);
    hashing_strategy* my_hashing = get_hash_strategy(hash_type);
    collision_strategy* my_collision = get_collision_strategy(collision_type);
    hashtable table(20000,my_hashing,my_collision);
    double start_time = clock();
    while(true){
        scanf("%d", &type);
        if(type == 0){
            scanf("%s", buffer);scanf("%d",&data);
            table.insert(hash_entry(buffer, data));
        }else if(type == 1){
            scanf("%s",buffer);
            printf("%d\n", table.query(buffer));
        }else if(type==2)break;
    }
    double end_time = clock();
    double duration = (end_time-start_time)/CLOCKS_PER_SEC;
    ofstream fout("result.txt",ios::out|ios::app);
    if(!fout.is_open())
    {
        cout<<"can't open file"<<endl;
    }
    printf("%d %d %.6f\n",hash_type,collision_type,duration);
    fout<<hash_type<<" "<<collision_type<<" "<<duration<<endl;
    return 0;
}
// int main(){
   
//     int type;
//     char buffer[1000];int data;
//     hashtable table(10000,new good_hashing(),new public_overflow());
//     while(true){
//         scanf("%d", &type);
//         if(type == 0){
//             scanf("%s", buffer);scanf("%d",&data);
//             table.insert(hash_entry(buffer, data));
//         }else if(type == 1){
//             scanf("%s",buffer);
//             printf("%d\n", table.query(buffer));
//         }else break;
//     }
//     return 0;
// }