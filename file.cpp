#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

struct FAT{
	int link[1024];
	FAT(){
		for(int i=0;i<1024;i++){
			link[i] = -1;//��ʼ�����п����һ���ַΪ-1
		}
	}
	int find_blk(int num){//�ú���Ϊ�ļ�����num���ַ�ռ䣬Ϊ����FAT���н������� 
		int f1 = find_rd();
		int final = f1;
		for(int i =0;i<num;i++){
			int f2 = find_rd();
			link[f1] = f2;
			f1 = f2;
		}
		return final;
	}
	int find_rd(){//��ȡ������ʵķ�ʽ��Ѱ��FAT����һ�����п飬��������
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
			open[i] = 0;//Ϊÿ���ļ���ʼ���򿪸���Ϊ0 
		}
	}
} list;

struct FCB{
	string name=string{""};//�ļ��� 
	int length = 0;//�ļ���С 
	int block1 = 0;//��һ��ĵ�ַ 
};

FCB *fcbs = new FCB[1024];//���ÿ���ļ�����һ�飬���1024���ļ�

void open(string s){
	int index = 0;
	while(fcbs[index].name!=s && fcbs[index].length!=0)//�����ļ�ֱ���ҵ�ͬ���ļ�Ϊֹ
		index++;
	if(fcbs[index].length==0){//fcbs[index].lengthΪ0������Ч��������û�и��ļ�
		cout<<"no such file:"<<s<<"\n";
		return;
	}
	list.open[index]++;//���ļ� 
}

void close(string s){
	int index = 0;
	while(fcbs[index].name!=s && fcbs[index].length!=0)//�����ļ�ֱ���ҵ�ͬ���ļ�Ϊֹ
		index++;
	if(fcbs[index].length==0){//������Ч��������û�и��ļ�
		cout<<"no such file:"<<s<<"\n";
		return;
	}
	if(list.open[index]<=0){//��ֹ��Ч�ر� 
		cout<<"file is not open:"<<s<<"\n";
		return;
	}
	list.open[index]--;
}

void create(string s, int len){
	int index = 0, tip =0;
	while(fcbs[index].length!=0 && fcbs[index].length!=-1 && index<1024){//�����ļ�ֱ���ҵ���д���PCBΪֹ
		if(fcbs[index].name==s){//��ͬ���ļ��򱨴��˳� 
			cout<<"file already exists:"<<s<<"\n";
			return;
		}
		index++;
	}
	tip = index;//��tip��¼��һ�����е�PCB������ 
	
	while(fcbs[index].length!=0 && index<1024){//������������Ч������ͬ���ļ�
		if(fcbs[index].name==s){
			cout<<"file already exists:"<<s<<"\n";
			return;
		}
		index++;
	}
	//д��PCB 
	fcbs[tip].name = s;
	fcbs[tip].length = len;
	fcbs[tip].block1 = fat1.find_blk(len);
	}

void del(string s){
	int index = 0;
	while(fcbs[index].name!=s && fcbs[index].length!=0)//�����ļ�ֱ���ҵ�ͬ���ļ�Ϊֹ
		index++;
	if(fcbs[index].length==0){//������Ч��������û�и��ļ�
		cout<<"no such file:"<<s<<"\n";
		return;
	}
	
	if(list.open[index]>0){//���ļ�����ʱ��ֹɾ�� 
		cout<<"delete wrong: file "<<s<<" is opened by "<<list.open[index]<<" process.\n";
		return;
	}
	
	int temp = fcbs[index].block1;
	for(int i =0;i<fcbs[index].length;i++){//���FAT���еļ�¼ 
		int tip = fat1.link[temp];
		fat1.link[temp] = -1;
		temp = tip;
	}
	fcbs[index].length = -1;//���PCB�еļ�¼
	fcbs[index].block1 = 0;
	fcbs[index].name=string{""};
}
	
void myls()
{
	cout<<"ls:\n";
	int index = 0;
	while(fcbs[index].length!=0){//����Ч���ڴ�ӡ���д��ڵ�PCB���ļ��� 
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
