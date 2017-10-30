#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <stack>
using namespace std;

struct coordinates
{
	int x,y;
	coordinates() : x(0),y(0) {}
	void operator=(const coordinates& );
	bool operator==(const coordinates&) const;
	bool operator<(const coordinates& r) const;
	coordinates(int a,int b)
	{
		this->x = a;
		this->y = b;
	}
};

bool coordinates::operator==(const coordinates& a) const
{
	return this->x == a.x && this->y == a.y;
}
bool coordinates::operator<(const coordinates& r) const
{
	return
		   x < r.x
		|| x == r.x && y < r.y;
}

void coordinates::operator=(const coordinates& a)
{
	this->x = a.x;
	this->y = a.y;
}
struct state
{
	coordinates H,F,S;
	bool operator==(const state& r) const;
	bool operator<(const state& r) const;
	state()  {}
	state(int Hx,int Hy,int Fx,int Fy,int Sx,int Sy)
	{
		this->H.x = Hx;
		this->H.y = Hy;
		this->F.x = Fx;
		this->F.y = Fy;
		this->S.x = Sx;
		this->S.y = Sy;
	}
};

bool state::operator==(const state& r) const
{
	return F == r.F && S == r.S && H == r.H;
}

bool state::operator<(const state& r) const
{
	return
	   H < r.H
	|| H == r.H && F < r.F
	|| H == r.H && F == r.F && S < r.S;
}

bool read_byte(ifstream& inFile,unsigned char& byte)
{
	char tmp;
	inFile.get(tmp);
	byte = unsigned char(tmp);
	return inFile.good();
}

struct node
{
	node* Parent;
	vector<node*> Childs;
	int distance;
	bool Complete1,Complete2;
    static	vector<vector<char>> fields;
	coordinates first,second,human;
	node() : distance(0), Complete1(false),Complete2(false), Parent(0) {}	
	~node() 
	{

		for ( int i(0);i<(int)Childs.size();++i)
		{
			delete Childs[i];
		}
	}
};

vector<vector<char>> node::fields;

bool read_char(ifstream& inFile,char& byte)
{
	if (inFile.good())
	{
		inFile.get(byte);
		return true;
	}
	else
	{
		return false;
	}
}

enum Move{mUp,mDown,mLeft,mRight};

void Step(node* current,vector<node*>& Tree, Move move,bool& check1,bool& check2)
{
	node* temp = new node();
	temp->Parent = current;
	temp->distance = current->distance+1;
	temp->human = current->human;
	switch (move)
	{
	case mUp:		temp->human.x -=1; break;
	case mDown: 		
		temp->human.x +=1; 
		if (current->first.x+1 == '.' && !check1)
			temp->first.x +=1;
		else
		{
			temp->first.x +=1;
			check1 = true;
		}
		if (current->second.x+1 == '.' && !check2)
			temp->second.x +=1;
		else
		{
			temp->second.x +=1;
			check2 = true;
		}
		break;
	case mLeft:		temp->human.y -=1; break;
	case mRight:		temp->human.y +=1; break;
	}
	temp->first = current->first;
	temp->second = current->second;
	current->Childs.push_back(temp);
	Tree.push_back(temp);
}

void Step(node* current,vector<node*>& Tree, state s)
{
	node* temp = new node();
	temp->Parent = current;
	temp->distance = current->distance+1;
	temp->human = s.H;
	temp->first = s.F;
	temp->second = s.S;
	temp->Complete1 = (node::fields[temp->first.x][temp->first.y] == '_');
	temp->Complete2 = (node::fields[temp->second.x][temp->second.y] == '_');
	current->Childs.push_back(temp);
	Tree.push_back(temp);
}


typedef set<state> check_tree;

int main(int argc,char** argv)
{
	check_tree check;
	//vector<vector<char>> FIELD;
	bool check1(false),check2(false);
	for (int j(0);j<20;++j)
	{
		vector<char> tmp(20,'0');
		node::fields.push_back(tmp);
	}
//	Get Field;
	ifstream inFile(argv[1],ios::in);
	int lines(0),column(0),columns(0);
	coordinates Human,First,Second;
	bool isFirstLine(true), isFirstBox(true);
	while (!inFile.eof())
	{
		char BYTE;
		inFile.get(BYTE);
		if (inFile.eof())	break;
		
		if (BYTE == '@')	{Human.x = lines; Human.y = column; BYTE = '.'; }
		else if (BYTE == '$' ) 
		{
			if (isFirstBox)	{First.x = lines;	 First.y = column;	 isFirstBox = false;} 
			else {Second.x = lines;	 Second.y = column;}
			BYTE = '.';
		}
		node::fields[lines][column] = BYTE;

		if (BYTE == '\n')	{ isFirstLine = false; ++lines; column = 0; }
		else	column++;
		if (isFirstLine)	{++columns;}
	}
    ++lines;
	
//	Root

	node*	root = new node();
	root->distance = 0;
	root->first = First;
	root->human = Human;
	root->second = Second;
// Create tree
	vector<node*> Tree;
	Tree.push_back(root);
	check.insert(state(state(Human.x, Human.y, First.x, First.y, Second.x, Second.y)));

	int i(0);
	for (;;++i)
	{
		node* current = Tree[i];

		coordinates tmpCoord;
		state tmpState;

//-------------------//
//		Number 1	 //
//-------------------//
        tmpCoord = coordinates(current->human.x - 1, current->human.y);
		tmpState = state(tmpCoord.x, tmpCoord.y, current->first.x, current->first.y, current->second.x,	current->second.y);
		if (   tmpCoord.x >= 0
			&& (node::fields[tmpCoord.x][tmpCoord.y] == '.' || node::fields[tmpCoord.x][tmpCoord.y] == '_' )
			&& !(tmpCoord == current->first)
			&& !(tmpCoord == current->second)
			&& check.find(tmpState) == check.end()
		   )
		{
			check.insert(tmpState);
			Step(current,Tree,mUp,check1,check2);
		}

//-------------------//
//		Number 2	 //
//-------------------//
		tmpCoord = coordinates(current->human.x + 1, current->human.y);
		tmpState = state(tmpCoord.x, tmpCoord.y, current->first.x, current->first.y, current->second.x,	current->second.y);
		if (   tmpCoord.x < lines
			&& (node::fields[tmpCoord.x][tmpCoord.y] == '.' || node::fields[tmpCoord.x][tmpCoord.y] == '_' )
			&& !(tmpCoord == current->first)
			&& !(tmpCoord == current->second)
			&& check.find(tmpState) == check.end()
		   )
		{
			check.insert(tmpState);
			Step(current,Tree,mDown,check1,check2);
		}

//-------------------//
//		Number 3	 //
//-------------------//
		tmpCoord = coordinates(current->human.x, current->human.y - 1);
		tmpState = state(tmpCoord.x, tmpCoord.y, current->first.x, current->first.y, current->second.x,	current->second.y);
		if (   tmpCoord.y >= 0
			&& (node::fields[tmpCoord.x][tmpCoord.y] == '.' || node::fields[tmpCoord.x][tmpCoord.y] == '_' )
			&& !(tmpCoord == current->first)
			&& !(tmpCoord == current->second)
			&& check.find(tmpState) == check.end()
		   )
		{
			check.insert(tmpState);
			Step(current,Tree,mLeft,check1,check2);
		}

//-------------------//
//		Number 4	 //
//-------------------//
		tmpCoord = coordinates(current->human.x, current->human.y + 1);
		tmpState = state(tmpCoord.x, tmpCoord.y, current->first.x, current->first.y, current->second.x,	current->second.y);
		if (   tmpCoord.y < columns
			&& (node::fields[tmpCoord.x][tmpCoord.y] == '.' || node::fields[tmpCoord.x][tmpCoord.y] == '_' )
			&& !(tmpCoord == current->first)
			&& !(tmpCoord == current->second)
			&& check.find(tmpState) == check.end()
		   )
		{
			check.insert(tmpState);
			Step(current,Tree,mRight,check1,check2);
		}

//-------------------//
//		Number 5	 //
//-------------------//
		if (current->human.x-2 >= 0)
		{
			bool isFirstBox(current->first == coordinates(current->human.x-1,current->human.y));
			bool isSecondBox(current->second == coordinates(current->human.x-1,current->human.y));
			if ((isFirstBox || isSecondBox)
				&& ('.' == node::fields[current->human.x-2][current->human.y] || '_' == node::fields[current->human.x-2][current->human.y])
				)
			{
				state s	( current->human.x-1,	current->human.y
						, current->first.x - (isFirstBox? 1: 0), current->first.y
						, current->second.x - (isSecondBox? 1: 0), current->second.y
						);
				if (!(s.F == s.S) && check.find(s) == check.end() && !(isFirstBox? check1 : check2))
				{
					check.insert(s);
					Step(current,Tree,s);
					if (Tree.back()->Complete1 && Tree.back()->Complete2)
						break;
				}
			}
		}

//-------------------//
//		Number 6	 //
//-------------------//		
		if (current->human.x+2 < lines)
		{
			bool isFirstBox(current->first == coordinates(current->human.x+1,current->human.y));
			bool isSecondBox(current->second == coordinates(current->human.x+1,current->human.y));
			if ((isFirstBox || isSecondBox)
				&& ('.' == node::fields[current->human.x+2][current->human.y] || '_' == node::fields[current->human.x+2][current->human.y])
				)
			{
				state s	( current->human.x+1,	current->human.y
						, current->first.x + (isFirstBox? 1: 0), current->first.y
						, current->second.x + (isSecondBox? 1: 0), current->second.y
						);
				if (!(s.F == s.S) && check.find(s) == check.end() )
				{
					check.insert(s);
					Step(current,Tree,s);
					if (Tree.back()->Complete1 && Tree.back()->Complete2)
						break;
				}
			}
		}


//-------------------//
//		Number 7	 //
//-------------------//		
		if (current->human.y-2 >= 0)
		{
			bool isFirstBox(current->first == coordinates(current->human.x,current->human.y-1));
			bool isSecondBox(current->second == coordinates(current->human.x,current->human.y-1));
			if ((isFirstBox || isSecondBox)
				&& ('.' == node::fields[current->human.x][current->human.y-2] || '_' == node::fields[current->human.x][current->human.y-2])
				)
			{
				state s	( current->human.x,	current->human.y-1
						, current->first.x, current->first.y - (isFirstBox? 1: 0)
						, current->second.x, current->second.y - (isSecondBox? 1: 0)
						);
				if (!(s.F == s.S) && check.find(s) == check.end() )
				{
					check.insert(s);
					Step(current,Tree,s);
					if (Tree.back()->Complete1 && Tree.back()->Complete2)
						break;
				}
			}
		}

//-------------------//
//		Number 8	 //
//-------------------//		
		if (current->human.y+2 > 0)
		{
			bool isFirstBox(current->first == coordinates(current->human.x,current->human.y+1));
			bool isSecondBox(current->second == coordinates(current->human.x,current->human.y+1));
			if ((isFirstBox || isSecondBox)
				&& ('.' == node::fields[current->human.x][current->human.y+2] || '_' == node::fields[current->human.x][current->human.y+2])
				)
			{
				state s	( current->human.x,	current->human.y+1
						, current->first.x, current->first.y + (isFirstBox? 1: 0)
						, current->second.x, current->second.y + (isSecondBox? 1: 0)
						);
				if (!(s.F == s.S) && check.find(s) == check.end())
				{
					check.insert(s);
					Step(current,Tree,s);
					if (Tree.back()->Complete1 && Tree.back()->Complete2)
						break;
				}
			}
		}
	}

	
	stack<vector<vector<char>>> TempStack;

	node* current = Tree.back();
	while (current->Parent != 0)
	{
		vector<vector<char>> tmpField;
		tmpField = node::fields;
		tmpField[current->first.x][current->first.y] = '$';
		tmpField[current->second.x][current->second.y] = '$';
		tmpField[current->human.x][current->human.y] = '@';
		TempStack.push(tmpField);
		current = current->Parent;
	}
		vector<vector<char>> tmpField;
		tmpField = node::fields;
		tmpField[current->first.x][current->first.y] = '$';
		tmpField[current->second.x][current->second.y] = '$';
		tmpField[current->human.x][current->human.y] = '@';
		TempStack.push(tmpField);

	ofstream outFile("Out.txt",ios::out|ios::app);
	int Steps(0);
	while (!TempStack.empty())
	{
		tmpField = TempStack.top();
		outFile<<"Step numer : " ;
		outFile<<Steps;
		outFile.put('\n');
		for (int i(0);i<lines;++i)
		{
			for (int j(0);j<columns;++j)
			{
				outFile.put(tmpField[i][j]);
			}
			outFile.put('\n');
		}
		TempStack.pop();
		++Steps;
	}
	delete Tree[0];
	return 0;
}