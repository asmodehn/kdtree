/*This is a test/benchmark program for the KDTree implementation*/

//temporary
#define CHECK

#include "KDTree.hh"
#include <math.h>

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
	friend ostream & operator << (ostream &out, const Voxel &v)
	{
		out << v.x << ' ' << v.y << ' ' << v.z;
		return out;
	}
};


// Test program
#include <time.h>
time_t begin;
time_t end;

#define MAX 100000 //maximum number of voxels
#define MAXQ 200//(64*48)//max number of requests
#define MAXC 1000.0f //rem coordinates -> coord [ -MAXC/2 , MAXC/2 ]
#define RAYON 10.0f//search RAYON

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
	cout << "This is a test program for the KDTree Imlementation." << endl;
	
	string grabbed="Grabbed : ";
	string dist=" | distance = ";
	string wanted="Wanted near : ";
	//TODO : this could be better estimated...
	cout << "At least "<< MAX * sizeof(Voxel) * 2 / (1024*1024) << " MB of memory are required to process."<<endl;
#ifdef CHECK
	//This seems to be quite good
	cout << "And about "<< MAXQ * ((sizeof(wanted) +SIZECOORDSUP+SIZENEWLINE) + static_cast<int> (MAX/MAXC *RAYON) * (sizeof(grabbed)+SIZECOORDSUP+sizeof(dist)+SIZEFLOAT+SIZENEWLINE/*lf*/)) /(1024*1024) <<" MB of disk space to store the results " << endl;
#endif
	cout << "Press Ctrl-C to abort..." << endl;
	//TODO : grab this signal...
	
	KDTree<Voxel> t;
	srandom(1);
	vector<Voxel> list;
	cout << "Creating list of " << MAX << " Voxels..." << endl;
	begin=time(NULL);
	for( int i = 0; i < MAX; i++ )
	{
    	const Voxel v(remainderf(random(), MAXC),remainderf(random(), MAXC),remainderf(random(), MAXC));
		list.push_back(v);
	}
	end=time(NULL);
	cout << "Done" << endl;
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;
	
    cout << "Creating tree of " << MAX<< " Voxels..." << endl;
    begin=time(NULL);
	for( int i = 0; i < MAX; i++ )
	{
		//cout << "Inserting " << v <<endl;
		KDObject<Voxel> obj(list.at(i));
		obj[0]=list.at(i).x;obj[1]=list.at(i).y;obj[2]=list.at(i).z;
		t.insert( obj );
    }
	end=time(NULL);
	cout << "Done" << endl;
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;
	
	//Affichage des stats sur le KDTree
	cout << "KDTree statistics : " << endl;
	begin=time(NULL);
	t.stats();
	end=time(NULL);
	cout<<"Computing Time spend = "<<difftime(end,begin)<< " seconds"<<endl;

	//on balance
	cout << "Balancing...";flush(cout);
	t.balance();
	cout << "Done" << endl;

	//Affichage des stats sur le KDTree
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
	
	
	cout << "Querying List...\t";
	//First we will search in the list
	vector<Voxel> lresult;
	vector<float> ldists;
	//File for results storage
	ofstream reslist("list.res");
	if (!reslist){ cerr << "Unable to open the list results file"<<endl; exit(1);}
	float lsum=0.0;
	float lnbressum=0.0;
	curPct=-1;
	for (int i=0;i<MAXQ;i++)
	{
		//Display of the current percentage
		printpercent(curPct,i);
		
		reslist << wanted << testlist.at(i)[0] << ' ' << testlist.at(i)[1] <<' ' << testlist.at(i)[2] << endl;
		
		//Search in list
		int ind=lresult.size();
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
			reslist << grabbed << lresult[k];
			reslist << dist << ldists[k] << endl;
		}
								
		//We sum results number to compute a mean later...
		lnbressum+=lresult.size();
	}		 
	reslist.close();
	
	cout << "\b\b\b\b\t100%" << endl;
	cout << MAXQ <<" queries done..." << endl;
	cout << "List Duration : Total = " << lsum << " Mean = " << lsum/MAXQ << endl;
	cout << endl;
	
	cout << "Querying Tree...\t";
	//Second we will search in the tree
	vector<Voxel> result;
	vector<float> dists;
	//File for results storage
	ofstream restree("tree.res");
	if (!restree){ cerr << "Unable to open the tree results file"<<endl; exit(1);}
	
	float sum=0.0;
	float nbressum=0.0;
	curPct=-1;
	for (int i=0;i<MAXQ;i++)
	{
		//Display of the current percentage
		printpercent(curPct,i);
	
		restree << wanted << testlist.at(i)[0] << ' ' << testlist.at(i)[1] <<' ' << testlist.at(i)[2] << endl;
				
		//Search in the tree
		int ind=result.size();
		begin=time(NULL);
		vector<KDObjDist<Voxel> > res=t.findNear(testlist.at(i),RAYON);
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
		for (int k=ind;k<ind+found;k++)
		{
			restree << grabbed << result[k];
			restree << dist << dists[k] << endl;
		}
		//We sum results number to compute a mean later...
		nbressum+=result.size();
	}		 
	restree.close();
	cout << "\b\b\b\b\t100%" << endl;
	
#ifdef CHECK
	//We need to compare the results
	cout << "You can now compare the results in the .res file..." << endl;
	//TODO special diff program (same solutions, but not in the same order...)
	
#endif	
	
	//Then we can print KDTree results
	cout << MAXQ <<" queries done..." << endl;
	cout << "KDTree Duration : Total = " << sum << " Mean = "<< sum / MAXQ << endl;
	cout << "Mean number of query results = " << nbressum/MAXQ << endl;
		
	cout << "End : Freeing Memory..." << endl;
    return 0;
}
