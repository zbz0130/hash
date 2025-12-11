#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<ctime>
#include<algorithm>
#include<cstdlib>
#include<random>
using namespace std;
struct User
{
    string id;
    int ranking;
};
vector<User> dataset;
void load_data(string filename)
{
    ifstream fin(filename);
    if(!fin)
    {
        cerr<<"ERROR: Cannot open"<<filename<<endl;
    }
    string id;
    int num,rank,solved;
    int m = 50000;
  
    while( fin>>num>>id>>rank>>solved)
    {
       
        dataset.push_back({id,rank});
    }
    fin.close();
}

int main(int argc,char *argv[]) //通过命令行参数进行调控，argv[1]为插入次数,argv[2]为查询次数,argv[3]可选，代表查询的字符串在哈希表中存在的百分比，默认为80%
{
    if (argc < 3) {
       
        std::cerr << "Usage: " << argv[0] << " <insert_count> <query_count>" << std::endl;
        return 1; 
    }
    int insert_count = atoi(argv[1]);
    int query_count = atoi(argv[2]);
    int repeat = 80;
    if(argv[3])
    {
        repeat = atoi(argv[3]);
    }
    load_data("poj.txt");
    srand(time(0));
    random_device rd;
    mt19937 g(rd());
    shuffle(dataset.begin(),dataset.end(),g);
    insert_count = (insert_count<=dataset.size())?insert_count:dataset.size();
    vector<User> insert_set;
    vector<User> query_set;
    for(int i=0;i<insert_count;i++)
    {
        insert_set.push_back(dataset[i]);
    }
    for(int i=0 ;i<query_count;i++)
    {
        if(rand()%100<repeat)
        {
            query_set.push_back(insert_set[rand()%insert_count]);
        }
        else
        query_set.push_back(dataset[insert_count+i]);
    }
    freopen("input.in","w",stdout);
    for(int i=0;i<insert_count;i++)
    {
        printf("0 %s %d\n",insert_set[i].id.c_str(),insert_set[i].ranking);
    }
    for(int i=0;i<query_count;i++)
    {
        printf("1 %s\n",query_set[i].id.c_str());
    }
    printf("2\n");
    return 0;
}
