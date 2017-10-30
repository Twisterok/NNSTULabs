#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
using namespace std;

struct node
{
  unsigned char code;
  int weight;
  node *left, *right;
  std::vector<bool> seq;

  node() : code(0), weight(0), left(0), right(0) {}
  ~node()  { delete left; delete right; }
  bool isLeaf();
  void create_seq();
};

bool node::isLeaf()
{
	if (this->left==0 && this->right==0)
		return true;
	else
		return false;
}

void node::create_seq()
{
	if (left!=0)
	{
		left->seq = seq;
		left->seq.push_back(false);
		left->create_seq();
	}
	if (right!=0)
	{
		right->seq = seq;
		right->seq.push_back(true);
		right->create_seq();
	}
}

bool compare(node* i, node* j)
{
	return (i->weight < j->weight) || (i->weight == j->weight && i->code < j->code);
}

node* Create_tree(const std::vector<node*>& leaf)
{
	std::vector<node*> nodes;
	auto i = leaf.begin();
	while (i!=leaf.end())
	{
		if (*i != 0)
		{
			node* tmp = *i;
			nodes.push_back(tmp);			
		}
		++i;
	}


	while (nodes.size()>1)
	{
		std::vector<node*>::iterator pos = std::min_element(nodes.begin(),nodes.end(),compare);
		node* a = *pos;
		nodes.erase(pos);
		
		pos = std::min_element(nodes.begin(),nodes.end(),compare);
		node* b = *pos;
		nodes.erase(pos);

		node* tmp = new node();
		tmp->code = std::min(a->code,b->code);
		tmp->weight = a->weight + b->weight;
		tmp->left = a->code < b->code ? a : b;
		tmp->right = a->code < b->code ? b : a;
		nodes.push_back(tmp);
		
	
	}

	node* root = nodes.empty()?  0 : nodes[0];
	if (root !=0)
	{
		root->create_seq();

	/*	if (root->isLeaf())
		{
			root->seq.push_back(0);
		}*/
	}

	return root;
}




bool read_byte(ifstream& inFile,unsigned char& byte)
{
	char tmp;
	inFile.get(tmp);
	byte = unsigned char(tmp);
	return inFile.good();
}

bool read_byte(ifstream& inFile,queue<bool>& bits)
{
	unsigned char tempCh(0);
	read_byte(inFile,tempCh);
	for (int i(7);i>=0;--i)
	{
		bits.push(tempCh & 1<<i? true : false);
	}
	return inFile.good();
}

void write_byte(ofstream& output, unsigned char byte)
{
	output.put(char(byte));
}



void write_byte(ofstream& output,queue<bool>& bits)
{
	unsigned char tmp(0);
	for (int i(7);i>=0 && !bits.empty();--i)
	{
		if (bits.front())
		{
			tmp = tmp | (1<<i);
		}
		bits.pop();
	}
	write_byte(output,tmp);
}

bool big_andian()
{
	unsigned short int a(1);
	return !(*(unsigned char*)&a);       //  первый байт в биг ендиане будет = 0 , в литле 1
}

void zip(string& input,string& output)
{
	ifstream inFile(input,std::ios_base::in|std::ios_base::binary);
	inFile.seekg(0,ios::beg);
	unsigned char utmpCh;
	std::vector<node*> leafs(256,0);
//----------------------//
//		STATISTIK		//
//----------------------//
	while (read_byte(inFile,utmpCh))
	{
		if (!leafs[utmpCh])
		{
			 node* tmp = new node();
			 tmp->code = utmpCh;
			 leafs[utmpCh] = tmp;
		}
		++leafs[utmpCh]->weight;
	}
	inFile.close();

	node* root = Create_tree(leafs);
	
	ofstream outFile(output,std::ios_base::out|std::ios_base::binary);

	write_byte(outFile,'H');
	write_byte(outFile,'U');
	write_byte(outFile,'F');
	write_byte(outFile,'1');
	write_byte(outFile,'1');
	
	for (int i(0);i<leafs.size();++i)
	{
		if (big_andian())
		{
				unsigned int tmp = leafs[i]? leafs[i]->weight :  0;
				unsigned char* byte = (unsigned char*)&tmp;
				write_byte(outFile,byte[0]);
				write_byte(outFile,byte[1]);
				write_byte(outFile,byte[2]);
				write_byte(outFile,byte[3]);
		}	
		else
		{
				unsigned int tmp = leafs[i]? leafs[i]->weight :  0;
				unsigned char* byte = (unsigned char*)&tmp;
				write_byte(outFile,byte[3]);
				write_byte(outFile,byte[2]);
				write_byte(outFile,byte[1]);
				write_byte(outFile,byte[0]);
		}
	}

	inFile.open(input,std::ios_base::in|std::ios_base::binary);
	inFile.seekg(0,ios::beg);
	std::queue<bool> bits;
	unsigned char byte_char;
	
	while (read_byte(inFile,byte_char))
	{
		//from char to queue
		for (int i(0);i<int(leafs[byte_char]->seq.size());++i)
		{
			bits.push( leafs[byte_char]->seq[i] );
		}
		//print from queue to file byte by byte

		while (bits.size() >=8)         
		{
			write_byte(outFile,bits);
		}
	}

	// if there is something else
	if (!bits.empty())
	{
		write_byte(outFile,bits);
	}
	inFile.close();
	outFile.close();
	delete root;
}

void unzip(string& input,string& output)
{
	ifstream inFile(input,ios_base::in|ios_base::binary);
	ofstream outFile(output,std::ios_base::out|std::ios_base::binary);
	unsigned char byte;
	if (!read_byte(inFile,byte) || byte != 'H'  ||
		!read_byte(inFile,byte) || byte != 'U'  ||
		!read_byte(inFile,byte) || byte != 'F'  ||
		!read_byte(inFile,byte) || byte != '1'  ||
		!read_byte(inFile,byte) || byte != '1' 
		)
	{
		inFile.close();
		outFile.close();
		return;
	}
	
	//statistics
	std::vector<node*> leafs(256,0);
	unsigned int howmuch(0);
	for (int i(0);i<256;++i)
	{
		unsigned char data[4];
		if (big_andian())
		{
			read_byte(inFile,data[0]);
			read_byte(inFile,data[1]);
			read_byte(inFile,data[2]);
			read_byte(inFile,data[3]);
		}
		else
		{
			read_byte(inFile,data[3]);
			read_byte(inFile,data[2]);
			read_byte(inFile,data[1]);
			read_byte(inFile,data[0]);
		}
		
		unsigned int weight(*(unsigned int*)data);
		
		if (weight)
		{
			node* tmp = new node();
			tmp->code = (unsigned char)i;
			tmp->weight = weight;
			leafs[i] = tmp;
		}

		howmuch += weight;
	}


	node* root = Create_tree(leafs);

	queue<bool> bits;
//	unsigned char byte;
	while (howmuch>0)
	{
		node* pos(root);
		while (pos!=0 && !pos->isLeaf())
		{
			if (bits.empty())
			{
				read_byte(inFile,bits);
			}

			pos = bits.front() ? pos->right : pos->left;
			bits.pop();
		}
		
		if (pos != 0 && pos->isLeaf())
		{
			write_byte(outFile,pos->code);
			--howmuch;	
		}
	}
	// free resources
	  inFile.close();
	  outFile.close();
	  delete root;
}


// command arguments example:
// zip D:\temp\main.cpp D:\temp\test.huf
// unzip D:\temp\test.huf D:\temp\main1.cpp
int main(int argc , char** argv)
{
  if (argc == 4)
  {
	  string tmp1 = argv[2];
	  string tmp2 = argv[3];
	  if (argv[1][0] == 'z' && argv[1][1] =='i' && argv[1][2]=='p')
	  {
		  zip(tmp1,tmp2);
	  }
	  else if (argv[1][0] =='u' && argv[1][1]=='n' && argv[1][2] == 'z' && argv[1][3] =='i' && argv[1][4]=='p')
	  {
		  unzip(tmp1,tmp2);
	  }
  }
  return 0;
}