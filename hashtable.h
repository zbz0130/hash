#pragma once
#include <cstring>
#include <cstdio>
const int TABLE_SIZE = 499883; // = 124970 * 4 + 3
struct hash_entry{
    char* my_string;
    int my_data; 
    hash_entry(){
        my_string = NULL;my_data = 0;
    }
    hash_entry(char* str, int data):my_data(data){
        my_string = new char[strlen(str)+1];
        strcpy(my_string,str);
    }
};
struct hashing_strategy{
    virtual int operator()(char* str, int N)=0;
};
struct naive_hashing: public hashing_strategy{
    int operator()(char* str, int N) override;
};
struct collision_strategy{
    virtual void init()=0;// pure virtual function
    virtual int operator()(hash_entry* Table, int table_size, int last_choice)=0;
    virtual int get_effective_size(int table_size){return table_size;}
};
struct linear_probe: public collision_strategy{
    void init();
    int operator()(hash_entry* Table, int table_size, int last_choice) override;
};
struct hashtable{
    hash_entry* Table;
    int table_size;
    hashing_strategy* my_hashing; // 如果改为hashing_strategy my_hashing, 即不是用指针作为hashtable的成员, 而是让hashing_strategy结构体直接作为hashtable的成员. 会发生什么bug?
    collision_strategy* my_collision;
    hashtable(int size, hashing_strategy* hashing, collision_strategy* collision)
        :table_size(size),my_hashing(hashing),my_collision(collision)
    {
        Table = new hash_entry[table_size];
    }
    bool insert(hash_entry entry){ 
        int real_size = my_collision->get_effective_size(table_size);
       int last_choice = (*my_hashing)(entry.my_string,real_size);
       my_collision->init();
       while(Table[last_choice].my_string!=NULL){ // loop infinitely? return false when no more space?
           last_choice = (*my_collision)(Table, table_size, last_choice);
       }
       Table[last_choice] = entry;
       return true;
    }
    int query(char* query_string){
        int real_size = my_collision->get_effective_size(table_size);
        int last_choice = (*my_hashing)(query_string,real_size);
        my_collision->init();
        while(Table[last_choice].my_string!=NULL && 
            strcmp(Table[last_choice].my_string, query_string)!=0){ // 未处理的情况: 哈希表已满?
            last_choice = (*my_collision)(Table, table_size, last_choice);
        }
        if(Table[last_choice].my_string == NULL){
            return -1;
        }
        else {
            return Table[last_choice].my_data;
        }
    }
};
struct bad_hashing : public hashing_strategy{
    int operator()(char* str, int N)override{
        int ans=0;
        if(str==NULL)
        return 0;
        for(int i=0;str[i];i++)
        {
            ans+=str[i];
        }
        return ans%N;
    }
};

struct good_hashing : public hashing_strategy{
    int operator()(char* str,int N)override{
        int n = strlen(str);
        unsigned int h = 0;
        for(int i=0;i<n;i++)
        {
            h = (h<<5)|(h>>27);
            h+=str[i];
        }
        return h%N;
    }
};
struct quadratic_probe : public collision_strategy{
    int count;
    void init()override{
        count = 0;
    }
    int operator()(hash_entry* Table,int table_size, int last_choice)override{
        count++;
        return (int) ((long long)last_choice+2*count-1)%table_size;
    }
};
struct public_overflow : public collision_strategy{
    void init()override{} 

    int operator()(hash_entry* Table,int table_size, int last_choice)override{
        int limit = table_size*0.8;
        if(last_choice<limit)
        {
            last_choice = limit;
        }
        else{
            last_choice++;
            if(last_choice>=table_size)
            last_choice=limit;
        }
        return last_choice;
    }
};