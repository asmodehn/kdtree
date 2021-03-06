/*This is a test/benchmark program for the KDTree implementation*/

#include "KDTree.hh"
#include <math.h>

//Sanity checks
//We are usually interested by performances
#ifndef CHECK
#define PERF
#endif
//If we are looking for errors, we dont care about the performances
#ifdef CHECK
#undef PERF
#endif

#include <iostream>
#include <string>
#ifdef CHECK
#include <fstream> // needed only for checking purpose...
#endif
using namespace std;



//Test Node Classes
class Voxel
{
public:
	static const Voxel ERROR;
	float x,y,z;//Not needed, already in KDObject, just for testing purpose
	Voxel (float a=0, float b=0, float c=0) : x(a), y(b), z(c) {}
	
	bool operator == (const Voxel& v)
	{
		return ((v.x == x) && (v.y == y) && (v.z == z));
	}	
	friend ostream & operator << (ostream &out, const Voxel &v)
	{
		out << v.x << ' ' << v.y << ' ' << v.z;
		return out; //needed for chaining
	}
	friend istream & operator >> (istream &in, Voxel &v)
	{
		in >> v.x >> v.y >> v.z;
		return in; //needed for chaining
	}
};


// Test program
//If we are building on Windows with mingw32 (devcpp). version 3.3 (devcpp5) is
//necessary since remainderf is not in 3.0

#ifdef  __MINGW32__
#define srandom srand
#define random rand
#endif

#include <time.h>
time_t begin;
time_t end;

#define MAX 500000 //maximum number of voxels
#define MAXQ 1000//(64*48)//max number of requests
#define MAXC 1000.0f //rem coordinates -> coord [ -MAXC/2 , MAXC/2 ]
#define RAYON 10.0f//search RAYON

#define FILENAME_LIST_RES "list.res"
#define FILENAME_TREE_RES "tree.res"

#define SIZECOORDSUP 3*(1+ 1+ static_cast<int> (logf((MAXC/2.f)/logf(10.f))))
#define SIZEFLOAT 7
#define SIZENEWLINE 1 /*unix*/

float distance(const float* coords, const Voxel& v)
{
	return sqrtf((coords[0]-v.x) * (coords[0]-v.x) + (coords[1]-v.y)* (coords[1]-v.y) + (coords[2]-v.z) * (coords[2]-v.z));
}

void printpercent(int curPct,const int i)
{
	if (curPct<(i*100)/MAXQ)
	{
		curPct =(i*100)/MAXQ;
		cout << "\b\b\b\b\t" <<curPct << "%";
		cout.flush();
	}
}

int main (int argc, char** argv)
{
	cout << endl << "This is a test program for the KDTree Imlementation." << endl;
	
	const string grabbed="Grabbed";
	const string dist="distance";
	const string wanted="Wanted";
	
	//TODO : this could be better estimated...
	cout << "At least "<< MAX * sizeof(Voxel) * 2 / (1024*1024) << " MB of memory are required to process."<<endl;
#ifdef CHECK
	//This seems to be quite good
	cout << "And about "<< MAXQ * ((sizeof(wanted) +SIZECOORDSUP+SIZENEWLINE) + static_cast<int> (MAX/MAXC *RAYON) * (sizeof(grabbed)+SIZECOORDSUP+sizeof(dist)+SIZEFLOAT+SIZENEWLINE/*lf*/)) /(1024*1024) <<" MB of disk space to store the results " << endl;
#endif
	cout << "Press Ctrl-C to abort..." << endl;
	//TODO : grab this signal...
	cout << endl;
	
	KDTree<Voxel> t;
	srandom(time(NULL));
	vector<Voxel> list;
	cout << "Creating list of " << MAX << " Voxels...";flush(cout);
	begin=time(NULL);
	for( int i = 0; i < MAX; i++ )
	{
    	const Voxel v(remainderf(random(), MAXC),remainderf(random(), MAXC),remainderf(random(), MAXC));
		list.push_back(v);
	}
	end=time(NULL);
	cout << "Done" << endl;
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;
	
    cout << "Inserting " << MAX<< " Voxels in a KDTree...";flush(cout);
    begin=time(NULL);
	for( int i = 0; i < MAX; i++ )
	{
		//cout << "Inserting " << v <<endl;
		KDObject<Voxel> obj(list[i]);
		obj[0]=list[i].x;obj[1]=list[i].y;obj[2]=list[i].z;
		t.insert( obj );
    }
	end=time(NULL);
	cout << "Done" << endl;
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;
	
	//Displaying stats about the KDTree
	cout << endl << "KDTree statistics : " << endl;
	begin=time(NULL);
	t.stats();
	end=time(NULL);
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;

	//Optimizing the KDTree for research (theorically not exactly balanced)
	cout << endl << "Balancing...";flush(cout);
	t.balance();
	cout << "Done" << endl;

	//Displaying stats about the KDTree
	cout << "KDTree statistics : " << endl;
	begin=time(NULL);
	t.stats();
	end=time(NULL);
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;

	
	cout << "Preparing the test..." << endl;
	vector <float*> testlist;
	float* coord;
	for (int i=0;i<MAXQ;i++)
	{
		coord=new float[3];
		coord[0]=remainderf(random(), MAXC);
		coord[1]=remainderf(random(), MAXC);
		coord[2]=remainderf(random(), MAXC);
		testlist.push_back(coord);
	}
	cout << "Done." << endl;
		
	//Queriing MAXQ times
	float tmp=0.0;
	int curPct;
	
#ifdef CHECK	
	cout << endl << "Querying List...\t";
	//First we will search in the list
	vector<Voxel> lresult;
	vector<float> ldists;
	//File for results storage
	fstream reslist;
	reslist.open(FILENAME_LIST_RES, ios::out | ios::trunc);
	if (!reslist.is_open()){ cerr << "Unable to open the list results file"<<endl; exit(1);}
	float lsum=0.0;
	curPct=-1;
	for (int i=0;i<MAXQ;i++)
	{
		//Display of the current percentage
		printpercent(curPct,i);
		reslist << ' ' << wanted << ' ' << testlist.at(i)[0] << ' ' << testlist.at(i)[1] <<' ' << testlist.at(i)[2] << "\n"; //not endl to use the full buffer and speed up execution
		int ind=lresult.size();
		//Search in list
		int lfound=0;
		begin=time(NULL);
		for ( unsigned int k=0;k<list.size();k++)
		{
			if (distance(testlist.at(i),list[k])<=RAYON)
			{
				lfound++;
				lresult.push_back(list[k]);
				ldists.push_back(distance(testlist.at(i),list[k]));
			}
		}
		end=time(NULL);
		//We sum the duration to compute a mean later
		tmp=difftime(end,begin);
		lsum+=(tmp<0)?0:tmp;
		for ( int k=ind;k<ind+lfound;k++)
		{
			reslist << ' ' << grabbed << ' ' << lresult[k];
			reslist << ' ' << dist << ' ' << ldists[k] << endl;
		}
	}		 
	reslist.close();
	cout << "\b\b\b\b\t100%" << endl;
	cout << MAXQ <<" queries done..." << endl;
	cout << "List Duration : Total = " << lsum << " Mean = " << lsum/MAXQ << endl;
#endif	
	cout << endl << "Querying Tree...\t";
	//Second we will search in the tree
	vector<Voxel> result;
	vector<float> dists;
#ifdef CHECK
	//File for results storage
	fstream restree;
	restree.open(FILENAME_TREE_RES, ios::out | ios::trunc);
	if (!restree.is_open()){ cerr << "Unable to open the tree results file"<<endl; exit(1);}
#endif	
	float sum=0.0;
	curPct=-1;
	for (int i=0;i<MAXQ;i++)
	{
		//Display of the current percentage
		printpercent(curPct,i);
#ifdef CHECK
		restree << ' ' << wanted << ' ' << testlist.at(i)[0] << ' ' << testlist.at(i)[1] <<' ' << testlist.at(i)[2] << "\n";//not endl to use the full buffer and speed up execution
		int ind=result.size();
#endif
		//Search in the tree
		begin=time(NULL);
		vector<KDObjDist<Voxel> > res=t.findNear(testlist[i],RAYON);
		int found=res.size();
		for (int k=0;k<found;k++)
		{
			result.push_back(res[k].object);
			dists.push_back(res[k].dist);
			//cout << res[k].object << endl;
		}
		end=time(NULL);
		tmp=difftime(end,begin);
		sum+=(tmp<0)?0:tmp; //to avoide some negative time measures on short duration
#ifdef CHECK
		for (int k=ind;k<ind+found;k++)
		{
			restree << ' ' << grabbed << ' ' << result[k];
			restree << ' ' << dist << ' ' << dists[k] << endl;
		}
#endif
	}		 
	
#ifdef CHECK	
	restree.close();
#endif	
	cout << "\b\b\b\b\t100%" << endl;
	//Then we can print KDTree results
	cout << MAXQ <<" queries done..." << endl;
	cout << "KDTree Duration : Total = " << sum << " Mean = "<< sum / MAXQ << endl;
	cout << endl << "Mean number of query results = " << result.size()/MAXQ << endl;
	
#ifdef CHECK
	//We need to compare the results
	cout << "You can now compare the results in the .res file..." << endl;
	//INWORK special diff program (same solutions, but not in the same order...)
	//1 - Open the files restree & reslist
	reslist.open(FILENAME_LIST_RES, ios::in);
	restree.open(FILENAME_TREE_RES, ios::in);
	if( ! (reslist.is_open() && restree.is_open() ) ) exit(1);
	//2 - Testing the size of the files, must be exactly the same...
	long l,m, sizl, sizt;
	
	l = reslist.tellg();reslist.seekg (0, ios::end); 
	m = reslist.tellg();
    cout << "size of " << FILENAME_LIST_RES << " is " << (sizl=m-l) << " bytes.\n";
	reslist.seekg(ios::beg);
	
	l = restree.tellg();restree.seekg (0, ios::end); 
	m = restree.tellg();
    cout << "size of " << FILENAME_TREE_RES << " is " << (sizt=m-l) << " bytes.\n";
	restree.seekg(ios::beg);
	
	//3 - Test ARE in the same order -> compare requests results one by one
	Voxel voxtmpl,voxtmpt;
	string tmpl,tmpt;
	float disttmpl,disttmpt;
	vector<Voxel> voxl,voxt;
	vector<float> distl,distt;
	bool found=false;
	
	// checking results files
	//UGLY
	while (! (reslist.eof() || restree.eof()) )
	{	
		//TODO : print current line...
		/*UGLY*/
		reslist >> tmpl >> voxtmpl ; //cout << tmpl << endl;
		restree >> tmpt >> voxtmpt ; //cout << tmpt <<endl;
		if ((tmpl == wanted) && (tmpt == wanted))
		{
			//If this is the end of a grabbed list, we must compare results
			if (found)
			{
				//We compare elements one by one
				if ( voxl.size() != voxt.size() ) 
				{
					cerr << "ERROR : Not the same number of results found in tree and list"<< endl;
					cerr << "Please check the " << FILENAME_LIST_RES << " and " << FILENAME_TREE_RES << " files." << endl;
					exit(1);
				}
				for (unsigned int i=0; i<voxl.size() ; i++)
				{
					for (unsigned int j=0; j<voxt.size(); j++)
					{	//found... go on to next one
						if (voxl.at(i) == voxt.at(j))
						{
							found=true;
							break;
						}
					}
					if (found==false)
					{	//not found
						cerr << "ERROR : The voxel " << voxl.at(i) << " found in list, was not found in tree search results" << endl;
						cerr << "Please check the " << FILENAME_LIST_RES << " and " << FILENAME_TREE_RES << " files." << endl;
						exit(1);
					}
				}
			//clear the vectors
			voxl.clear();voxt.clear();distl.clear();distt.clear();

			}
			
			//cout << wanted << ' ' <<  voxtmpl << "\t";
			//cout << wanted << ' ' <<  voxtmpt << endl;

			found=false;
		}
		else if ((tmpl == grabbed) && (tmpt == grabbed))
		{
			//cout << grabbed << ' ' <<  voxtmpl << "\t";
			//cout << grabbed << ' ' <<  voxtmpt << endl;
			found=true;
			//Add voxel to vectors
			voxl.push_back(voxtmpl);
			voxt.push_back(voxtmpt);
			//pick up the distances
			reslist >> tmpl >> disttmpl; //cout << dist << ' ' << disttmpl << "\t";
			restree >> tmpt >> disttmpt; //cout << dist << ' ' << disttmpt << endl;
			//Add distances to vectors
			distl.push_back(disttmpl);
			distt.push_back(disttmpt);

		}
		else
		{ //Must not happen
			cerr << "ERROR while reading results files. Check the syntax" << endl;
			exit(2);
		}
	}
	/*UGLY*/
	//4 - Close the files restree & reslist
	reslist.close();
	restree.close();
#endif	
	
		
	cout << "End : Freeing Memory..." << endl;
	//KDTree is deleted at the end, because it is a variable in this main function.
	//Just check the memory to see if the delete function is working well ;)
    return 0;
}
