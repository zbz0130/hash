## 对原代码做的修改
```
struct collision_strategy{
    virtual int get_effective_size(int table_size)
    {return table_size;} //新添加的函数
};
```
在collision_strategy中新添加函数get_effective_size,一般的collision_strategy直接继承，而public_overflow进行重写
```
 int get_effective_size(int table_size){return (int)((double)table_size*0.8);}
 ```
并在后续hashtable::insert和hashtable::query中用real_size记录hashtable主表大小，并传递给hash函数做模数
```
  bool insert(hash_entry entry){ 
        int real_size = my_collision->get_effective_size(table_size);
       int last_choice = (*my_hashing)(entry.my_string,real_size);
      //原代码
    }
    int query(char* query_string){
        int real_size = my_collision->get_effective_size(table_size);
        int last_choice = (*my_hashing)(query_string,real_size);
      //原代码
    }

```
其作用是返回实际哈希表的大小，即主表的大小。对于一般的collision_strategy,real_size=table_size
而对于公共溢出区策略，由于hashtable中有$\frac {1}{5}$被用作公共溢出区，对应的real_size = table_size*0.8

## 哈希函数和冲突策略的实现方法
* bad_hashing的实现方法为对字符串的每一位字符转化为int直接相加，再模主表大小。
 这种实现不同排列组合产生相同和 (abc = cba)，且值域很窄，有大量冲突
 * good_hashing的实现方式为 
 ```
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
 ```
 用h记录数据，每一次对h进行循环移位，效果相当于h乘32再加h>>27起到搅拌作用，使作用效果更随机，之后再加上str对应位字符的ascii值。最终得出的总和再modN.
 * 冲突策略quadratic_probe的实现
  在quadratic中维护count来记录试探的次数，每一次查找的位置在上一次的基础上+2*count-1,相当于初始位置+$count^2$，实现平方试探冲突策略。
  * 冲突策略public_overflow的实现
  将hashtable分为两部分，前80%为基本区，后20%为公共溢出区。将公共溢出区的起始位置设为limit，如果发生冲突的idx< limit，下次从limit试探；若发生冲突的idx>=limit,下次试探idx+1.
  ## 进行测试
  实验结果见result.txt
  通过generator.cpp产生三组实验数据:
2000 insertions,2000 queries 80% repeat probability
2000 insertions,2000 queries 20% repeat probability
2000 insertions,4000 queries 20% repeat probability
三个数字分别代表插入的次数，询问的次数，查询的字符串在哈希表中存在的百分比，由generator.cpp生成
构造方法为将poj.txt全部数据存入vector dataset中，根据随机数进行shuffle重排，选定前argv[1]组数据作为插入数据，argv[2]决定询问数据的数量，根据argv[3]确定字符串在哈希表中存在的百分比。
数据特征为
* good_hashing在几乎所有情况下都优于bad_hashing
* linear_probe搭配good_hashing的性能远远好于搭配bad_hashing，且性能与询问数据的命中率有关，询问数据在哈希表中概率高时性能好，命中率低时性能差
* quadratic_probe 鲁棒性强，搭配bad_hashing为最快的collision_strategy,搭配good_hashing性能也较好
* public_overflow在搭配bad_hashing时性能远不如搭配good_hashing,搭配good_hashing时性能与linear_probe接近
## 分析结果
* 1 good_hashing几乎在所有情况下性能都远好于bad_hashing,因为bad_hashing哈希值分布不均匀，引发了极高的哈希冲突率，使程序频繁执行collision_strategy,时间复杂度高
good_hashing哈希值分布均匀，哈希冲突率低，执行冲突策略少。
* 2 对于bad_hashing quadratic_probe的性能好于linear_probe，对于good_hashing quadratic的性能优于linear_probe
  这是因为bad_hashing哈希分布聚集，平方试探可以快速跳过聚集区，而线性试探不能。
  good_hashing哈希分布均匀，哈希冲突少，线性试探因为cpu的缓存局部性性能好于平方试探。
* 3 封闭散列占优势，开放散列在某些实时系统中，主表用于保证O(1)的快速访问，一旦冲突直接扔进溢出区（慢路径），避免复杂的探测逻辑影响主流程的预测性时更占优势，开放散列对删除操作有高频需求时性能也更好。
* 字符分布不均会导致哈希值分布不均匀，会集中在某些特定的区间，导致哈希冲突概率增加，使性能退化。
* hashtable维护装填因子，当装填因子超过某个阈值时扩容，低于某个阈值时缩容。
* 扩容和缩容的实现为申请新空间，大小为原来的两倍或一半左右的素数，再进行rehash，把数据重新散列的新表