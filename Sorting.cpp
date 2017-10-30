#include <iostream>
#include <string>
#include <windows.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <assert.h>
#include <math.h>
#define ArrSize 100
#define MaxWordSize 100
using namespace std;

void BubbleSort(vector<string>& library)
{
	bool flag(1);
	while (flag)
	{
		flag = 0;
		for (int index =0; index<(int)library.size()-1;++index)
		{
			if (strcmp(library[index].c_str(),library[index+1].c_str())>0)
			{
					string vasya = library[index];
					library[index] = library[index+1];
					library[index+1] = vasya;
					flag = true;
				}
		}
	}
}

int partition(vector<string>& library,int start,int end)
{
	string x = library[start];
	int i = start -1, j = end+1;
	while (true)
	{
		do {--j;} while (strcmp(library[j].c_str(),x.c_str()) > 0);
		do {++i;} while (strcmp(library[i].c_str(),x.c_str()) < 0);
		if (i < j)
			swap(library[i],library[j]);
		else
			return j;
	}
}

void QuickSort(vector<string>& library, int start, int end)
{
	if (start<end)
	{
		int q = partition(library, start, end);
		QuickSort(library, start, q);
		QuickSort(library, q+1, end);
	}
}

void InsertionSort(vector<string>& library)
{
	for (int i(1);i<(int)library.size();++i)
	{
		string newElement = library[i];
		int location = i -1;
		while (location>=0 && strcmp(library[location].c_str(),newElement.c_str())>0)
		{
			library[location+1] = library[location];
			location -=1;
		}
		library[location+1] = newElement;
	}
}

bool check(vector<string>& library, int pos, const string& fragment)
{
	return pos < (int)library.size() && library[pos].find(fragment) == 0;
} 

int BinarySearch(vector<string>& library,const string& fragment)
{
	int start(0), end(library.size()-1);
	while (start < end)
	{
		int middle = (start+end)/2, cmp = strcmp(library[middle].c_str(),fragment.c_str());
	//	if (cmp == 0)
	//		return middle;
		if (cmp >= 0)
			end = middle-1;
		else
			start = middle+1;
	}	
	if (check(library, start, fragment) || check(library, ++start, fragment))
		return start;
	return -1;
}



int BinarySearch_last(vector<string>& library,const string& fragment)
{
	int start(0), end(library.size()-1);
	while (start < end)
	{
		int middle = (start+end)/2, cmp = strcmp(library[middle].c_str(),fragment.c_str());
		//if (cmp == 0)
		//	return middle;
		if (cmp >= 0)
			if (string(library[middle]).find(fragment) == 0)
				start = middle+1;
			else
				end = middle-1;
		else
			start = middle+1;
			
	}	
	if (check(library, start, fragment))
		return start;
	if (start>0 && check(library, --start, fragment))
		return start;
	return -1;
}



int main(int argc,char** argv)
{
	ifstream inFile(argv[1],ios::in);
	char temp;
	inFile.get(temp);
	if (inFile.eof())	return 0;
	
	vector<string> library;
	library.reserve(200*1000);
//	INITIALIZATION
	int lib_it(0),max_size(0);
	while (true)
	{
		int it(0);
		string tmp;
		while (temp != '\n')
		{
			tmp.push_back(temp);
			inFile.get(temp);
			//assert(it>MaxWordSize)
			if (inFile.eof())	break;
		}
		//assert(it>max_size);
//		if (it>max_size);
//			max_size = it;
		library.push_back(tmp);
		if (inFile.eof())	break;
		inFile.get(temp);
		if (inFile.eof())	break;

	}
	cout<<"Choose the sorting method"<<endl;
	cout<<"0 - Bubble Sort"<<endl;
	cout<<"1 - Insertion Sort"<<endl;
	cout<<"2 - Quick Sort"<<endl;
	int Get_type;
	cin>>Get_type;
	switch (Get_type)
	{
	case 0:
		BubbleSort(library);
		break;
	case 1:
		InsertionSort(library);
		break;
	case 2:
		QuickSort(library,0,library.size()-1);
		break;
	}
/*
	cout<<max_size<<endl;
	for (int i(0);i<library.size()-1;)
	{
		if (strcmp(library[i].c_str(),library[i+1].c_str())==0)
		{
			std::vector<string>::iterator iterator = library.begin()+i+1;
			library.erase(iterator);
		}
		else
			++i;
	}
*/
	string str;
	ifstream input2(argv[2],ios::in);
	while (!input2.eof())
	{
		char temporary;
		input2.get(temporary);
		if (input2.eof())
			break;
		str.push_back(temporary);
	}

	
	int First_Index = BinarySearch(library,str);
	int Last_index = BinarySearch_last(library,str);
	if (Last_index <0)
	{
		bool flag(true);
		while (Last_index<0 && flag == true)
		{
			flag = false;
			if (str.size()>0)
			{
				flag = true;
				string::iterator ITERATOR = str.end() -1;
				str.erase(ITERATOR);
			}
			Last_index = BinarySearch_last(library,str);
		}
	}
	if (Last_index <0)
	{
		cout<<"There are no such words"<<endl;
		return -1;
	}
	input2.close();
	
	
		//------------------//
		//		OUTPUT		//
		//------------------//	

	ofstream output("out.txt",ios::out|ios::app);
	output.put('\n');	output.put('\n');
	output<<"result is"<<endl;
	output<<endl;
	if (First_Index >0)
		output<<library[First_Index]<<endl;
	else
	{
		output<<library[Last_index]<<endl;
		return -1;
	}
	for (int u(First_Index+1);u<=Last_index;++u)
	{
		if (strcmp(library[u].c_str(),library[u-1].c_str())!=0)
			output<<library[u]<<endl;
	}

	
	
		//--------------------------//
		//		Free resources		//
		//--------------------------//	

	library.clear();

	
	return 0;
}