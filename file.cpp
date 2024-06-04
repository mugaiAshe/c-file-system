#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

struct FAT{
	int link[1024];
	FAT(){
		for(int i=0;i<1024;i++){
			link[i] = -1;//初始化所有块的下一块地址为-1
		}
	}
	int find_blk(int num){//该函数为文件分配num块地址空间，为其在FAT表中建立链接 
		int f1 = find_rd();
		int final = f1;
		for(int i =0;i<num;i++){
			int f2 = find_rd();
			link[f1] = f2;
			f1 = f2;
		}
		return final;
	}
	int find_rd(){//采取随机访问的方式，寻找FAT表中一个空闲块，用来分配
		int rd = rand()%1024;
		int num = link[rd];
		while(num != -1){
			rd = rand()%1024;
			num = link[rd];
		}
		return rd;
	}
} fat1;

struct openlist{
	int open[1024];
	openlist(){
		for(int i=0;i<1024;i++){
			open[i] = 0;//为每个文件初始化打开个数为0 
		}
	}
} list;

struct FCB{
	string name=string{""};//文件名 
	int length = 0;//文件大小 
	int block1 = 0;//第一块的地址 
};

FCB *fcbs = new FCB[1024];//最多每个文件分配一块，最多1024个文件

void open(string s){
	int index = 0;
	while(fcbs[index].name!=s && fcbs[index].length!=0)//搜索文件直到找到同名文件为止
		index++;
	if(fcbs[index].length==0){//fcbs[index].length为0进入无效区，表明没有该文件
		cout<<"no such file:"<<s<<"\n";
		return;
	}
	list.open[index]++;//打开文件 
}

void close(string s){
	int index = 0;
	while(fcbs[index].name!=s && fcbs[index].length!=0)//搜索文件直到找到同名文件为止
		index++;
	if(fcbs[index].length==0){//进入无效区，表明没有该文件
		cout<<"no such file:"<<s<<"\n";
		return;
	}
	if(list.open[index]<=0){//防止无效关闭 
		cout<<"file is not open:"<<s<<"\n";
		return;
	}
	list.open[index]--;
}

void create(string s, int len){
	int index = 0, tip =0;
	while(fcbs[index].length!=0 && fcbs[index].length!=-1 && index<1024){//搜索文件直到找到可写入的PCB为止
		if(fcbs[index].name==s){//有同名文件则报错退出 
			cout<<"file already exists:"<<s<<"\n";
			return;
		}
		index++;
	}
	tip = index;//用tip记录第一个空闲的PCB的索引 
	
	while(fcbs[index].length!=0 && index<1024){//接着在整个有效区搜索同名文件
		if(fcbs[index].name==s){
			cout<<"file already exists:"<<s<<"\n";
			return;
		}
		index++;
	}
	//写入PCB 
	fcbs[tip].name = s;
	fcbs[tip].length = len;
	fcbs[tip].block1 = fat1.find_blk(len);
	}

void del(string s){
	int index = 0;
	while(fcbs[index].name!=s && fcbs[index].length!=0)//搜索文件直到找到同名文件为止
		index++;
	if(fcbs[index].length==0){//进入无效区，表明没有该文件
		cout<<"no such file:"<<s<<"\n";
		return;
	}
	
	if(list.open[index]>0){//在文件被打开时禁止删除 
		cout<<"delete wrong: file "<<s<<" is opened by "<<list.open[index]<<" process.\n";
		return;
	}
	
	int temp = fcbs[index].block1;
	for(int i =0;i<fcbs[index].length;i++){//清空FAT表中的记录 
		int tip = fat1.link[temp];
		fat1.link[temp] = -1;
		temp = tip;
	}
	fcbs[index].length = -1;//清空PCB中的记录
	fcbs[index].block1 = 0;
	fcbs[index].name=string{""};
}
	
void myls()
{
	cout<<"ls:\n";
	int index = 0;
	while(fcbs[index].length!=0){//在有效区内打印所有存在的PCB的文件名 
		if(fcbs[index].length!=-1)
			cout<<fcbs[index].name<<":"<<fcbs[index].length<<"\n";
		index++;
	}
	cout<<"\n";
	return;
}

//FAT fat1;
//openlist list;

int main()
{
	//string *blocks = new string[1024];
	create("file1", 10);
	create("file2",5);
	create("file3",8);
	myls();
	
	create("file2",9);
	open("file1");
	del("file1");
	del("file2");
	myls();
	
	close("file1");
	del("file1");
	create("file4",6);
	close("file4");
	myls();
	
	for(int i=0;i<1024;i++){
		cout<<fat1.link[i];
	}
	cout<<"\n";
	del("file3");
	del("file4");
	myls();
	for(int i=0;i<1024;i++){
		cout<<fat1.link[i];
	}
	
	return 0;
}
