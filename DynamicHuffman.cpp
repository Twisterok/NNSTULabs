#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>

using namespace std;
struct node
{
  unsigned char code;
  int weight;
  node *left, *right,*parent;
  

  node() : code(0), weight(0), left(0), right(0),parent(0) {}
  ~node()  { delete left; delete right; }
  bool isLeaf();
};

bool node::isLeaf()
{
	if (this->left==0 && this->right==0)
		return true;
	else
		return false;
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

void getPath_in_bits(unsigned short int tmpCh,std::queue<bool>& bits,std::vector<node*>& leafs)
{
		node* tmp = leafs[tmpCh];
		stack<bool> stk;
		while (tmp->parent != 0)
			{
				if (tmp->parent->left == tmp)
				{
					stk.push(false);					
				}
				else
				{
					stk.push(true);
				}
				tmp = tmp->parent;
			}

			while (!stk.empty())
			{
				bits.push(stk.top());
				stk.pop();
			}
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

void Init(std::vector<node*>& brothers, std::vector<node*>& leafs)
{
	brothers.clear();

	node* root = new node();

	node* esc = new node();
	esc->parent = root;
	esc->weight = 1;

	node* eof = new node();
	eof->parent = root;
	eof->weight = 1;

	root->left = eof;
	root->right = esc;
	root->weight = root->left->weight + root->right->weight;

	brothers.push_back(root);
	brothers.push_back(eof);
	brothers.push_back(esc);
	leafs.resize(256,0);
	leafs.push_back(eof);
	leafs.push_back(esc);
}

void Add(unsigned char A,std::vector<node*>& leafs,std::vector<node*>& brothers)
{	
	node* esc = leafs[257];
	node* new_node = new node();
	node* new_parent = new node();

	new_node->code = A;

	leafs[new_node->code] = new_node;


	// insert new nodes

	new_parent->right = esc;
	new_parent->left = new_node;
	new_parent->parent = esc->parent;

	
	if (esc->parent->left == esc)
	{
		esc->parent->left = new_parent;		
	}
	else
	{
		esc->parent->right = new_parent;
	}

	new_node->parent = new_parent;
	
	esc->parent = new_parent;
	new_parent->weight +=1;


	//change brothers
	brothers.pop_back();
	brothers.push_back(new_parent);
	brothers.push_back(new_parent->left);
	brothers.push_back(new_parent->right);
}

void Update(unsigned char A,std::vector<node*>& brothers)
{
	
	int i = brothers.size() - 1 ;
	for (i; i>0;--i)
	{
		if ( brothers[i]->code == A)
		{
			break;
		}
	}
	
	while (i>0)
	{
		node* current = brothers[i];
		if (brothers[i-1]->weight == current->weight)
		{
			node* tmp;
			int j = i;
			while (brothers[j-1]->weight == current->weight)
			{
				
				tmp = brothers[j];
				--j;
			}
//---------------------------------//
//			START SWAP			   //
//---------------------------------//
//change child of first parent
			if (tmp->parent->left == tmp)
			{
				tmp->parent->left = current;
			}
			else
			{
				tmp->parent->right = current;
			}

//change child of second parent
			if (current->parent->left == current)
			{
				current->parent->left = tmp;
			}
			else
			{
				current->parent->right = tmp;
			}
			node* tmp_parent = current->parent;
			current->parent = tmp->parent;
			tmp->parent = tmp_parent;

			std::swap(brothers[i],brothers[j]);
			i = j;
		}

// increase weight
			current->weight +=1;			
			while ( brothers[i] != current->parent)
			{
				--i;
			}
			//current = brothers[i];
	}
	brothers[0]->weight +=1;
}

void write_byte_and_add(ofstream& output,queue<bool>& bits,std::vector<node*>& leafs,std::vector<node*>& brothers)
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
	Add(tmp,leafs,brothers);
	Update(tmp,brothers);
}

void zip(char* input,char* output)
{
	ifstream inFile(input,std::ios_base::in|std::ios_base::binary);
	ofstream outFile(output,std::ios_base::app|std::ios_base::binary);
	inFile.seekg(0,ios::beg);
	outFile.clear();

	outFile.put('D');
	outFile.put('H');
	outFile.put('U');
	outFile.put('F');
	outFile.put('1');
	outFile.put('1');
	
	std::vector<node*> brothers;
	std::vector<node*> leafs(256,0);
//Initialize
	Init(brothers,leafs);
	std::queue<bool> bits;
	unsigned char tempCh;
	while (read_byte(inFile,tempCh))
	{		
		//unsigned char tempCh = (unsigned char)inFile.get();
		if (inFile.eof()) {		break;		}
		
		if ( leafs[tempCh] == 0)
		{
			getPath_in_bits(257,bits,leafs);
			
			for (int i(7);i>=0;--i)
			{
				bits.push(tempCh & 1<<i ? true : false);
			}

			while (bits.size()>7)
			{
				write_byte(outFile,bits);
			}

			Add(tempCh,leafs,brothers);
			Update(tempCh,brothers);
		}
		else
		{
			getPath_in_bits(tempCh,bits,leafs);
			while (bits.size()>7)
			{
				write_byte(outFile,bits);
			}
			Update(tempCh,brothers);
		}
	}
	getPath_in_bits(256,bits,leafs);
	while (!bits.empty()) write_byte(outFile,bits);
/*
	while (bits.size()>7)
	{
		write_byte(outFile,bits);
	}
	write_byte(outFile,bits);
*/
	int a =0;
}

void unzip(char* input,char* output)
{
	ifstream inFile(input,std::ios_base::in|std::ios_base::binary);
	inFile.seekg(0,ios::beg);
	unsigned char check[6];
	if (!read_byte(inFile,check[0]) || check[0] != 'D' ||
		!read_byte(inFile,check[1]) || check[1] != 'H' ||
		!read_byte(inFile,check[2]) || check[2] != 'U' ||
		!read_byte(inFile,check[3]) || check[3] != 'F' ||
		!read_byte(inFile,check[4]) || check[4] != '1' ||
		!read_byte(inFile,check[5]) || check[5] != '1')
	{
		return;
	}

	ofstream outFile(output,std::ios_base::app|std::ios_base::binary);
	
	std::vector<node*> brothers;
	std::vector<node*> leafs(256,0);
//Initialize
	Init(brothers,leafs);
	std::queue<bool> bits;
	
	while (!inFile.eof())
	{
		node* tmp = brothers[0];
		
		while (!tmp->isLeaf())
		{
			if (bits.empty())			{read_byte(inFile,bits);}
			if (bits.front())		{tmp = tmp->right;}
			else					{tmp = tmp->left;}
			bits.pop();
		}

		if (tmp == leafs[256])		{return;}

		if (tmp == leafs[257])	
		{
			if (bits.size()<8)		{read_byte(inFile,bits);}

			write_byte_and_add(outFile,bits,leafs,brothers);	
		}
		else
		{
			outFile.put(tmp->code);
			Update(tmp->code,brothers);
		}
	}
}


//examples:
//	"unzip" "D:\temp\out.txt" "D:\temp\vasya.txt"
//	"zip" "D:\temp\gg.txt" "D:\temp\out.txt"
//	"zip" "D:\temp\IMG_0864.JPG" "D:\temp\out.txt"

int main(int argc,char** argv)
{
	if (argv[1][0] == 'z' && argv[1][1] == 'i' && argv[1][2] == 'p' )
	{
		zip(argv[2],argv[3]);
	}
	else if (argv[1][0] == 'u' && argv[1][1] == 'n' && argv[1][2] == 'z' && argv[1][3] == 'i' && argv[1][4] == 'p')
	{
		unzip(argv[2],argv[3]);
	}
	return 0;
}