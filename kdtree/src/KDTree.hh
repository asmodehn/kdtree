#ifndef KDTREE_HH
#define KDTREE_HH 1

#define DIMENSIONS 3

#include <vector>
#include <iostream>
using namespace std;

//Classe Template pour les objets a classer dans le KDTree
//Cette classe doit disposer d'un code d'erreur, pour indiquer une recherche sans resultat par exemple

//Classe pour les objets a classer dans le KDTree
template <class Object> class KDObject
{
	//Meme nombre de dimensions pour tous les objets, attention a la coherence
	static const int dimensions = DIMENSIONS;
	
	public:
	//Tableau de coordonnées, Attention a la dimensions lors des acces aux coords
	float* coords;
	//Stockage de l'objet
	const Object obj;
	
	KDObject(const Object& o=Object::ERROR) : obj(o) {coords=new float[dimensions];}
	KDObject(const Object& o,float* c) : obj(o) {coords=c;}
	KDObject(const KDObject& toStore) : obj(toStore.obj)
	{
		coords=new float[dimensions];
		for(int i=0;i<dimensions;i++)
		{
			coords[i]=toStore.coords[i];
		}
	}
	~KDObject() {delete coords;}
	
	//Accesseurs aux coordonnees
	inline float operator [] (int i) const { return *(coords + i % dimensions); }
	inline float &operator [] (int i) { return *(coords + i % dimensions); }
	
	//Calcul des distances
	inline float sqdist(const float* point) const
	{
		float sum=0.0f;
		for (int i=0;i<dimensions;i++)
		{
			sum+=(point[i]-coords[i])*(point[i]-coords[i]);
		}
		return sum;
	}
	inline float dist(const float* point) const {return sqrtf(sqdist(point));}
	
};

//Type retourné lors d'une requete de recherche au KDTree
template <class Object> class KDObjDist
{
	public:
	float dist;
	Object object; //Pour recopier la valeur et laisser le KDTree intact

	
	KDObjDist(const Object & o=Object::ERROR, float d=-1.0f) : dist(d) , object(o) {}
	KDObjDist(const KDObject<Object> & o, float d=-1.0f) : dist(d), object(o->obj) {}
};

template <class Object> class KDTree
{
	static const int dimensions=DIMENSIONS;
			
	class KDNode
	{
		private:
		const KDObject<Object> _data;
		public:
		KDNode* left;
		KDNode* right;
#ifndef REC
		KDNode* parent;
#endif
		//Constructeur et Destructeur
		KDNode(const KDObject<Object>& d=KDObject<Object>(Object::ERROR)) : _data(d) 
		{
			left=NULL;right=NULL;
#ifndef	REC
			parent=NULL;
#endif
		}
		~KDNode() {delete left; delete right;}
		//Accesseur
		const KDObject<Object>& data(void) const { return _data;}
	};
	
	KDNode* root;
	
	//Fonctions de manipulation internes
#ifdef REC
	bool insert(KDNode*& start,short dimstart, KDNode* node);
#else
	bool insert(KDNode* start,short dimstart, KDNode* node);
#endif
	//suppr(KDNode* start,int dimstart);
	bool minmax(KDNode* start,short dimstart,float* min,float* max);
	bool findNN(KDNode* start,short dimstart,const float* point,const KDNode*& neighbor, float& dist);
	bool findNear(KDNode* start,short dimstart,const float* point, float radius,vector<const KDNode*>& neighbor, vector<float>& dist);
	//findinAABox(KDNode* start,short dimstart,const float*& min,const float*& max);
#ifndef REC
		KDNode* pivot(const KDNode* start, short dimstart);
#endif
#ifdef REC
	bool balance(KDNode*& start,short dimstart);
#else
	bool balance(KDNode* start,short dimstart);
#endif
	long count(KDNode* start);
		
	public:
			
	//Constructeurs et Destructeurs
	KDTree() {root=NULL;}
	~KDTree() {delete root;}
	
	//Fonctions de manipulation globales
	bool insert(const KDObject<Object>& data);
	//suppr(const KDObject* data);
	bool minmax(float* min,float* max);
	KDObjDist<Object> findNN(const float* point);
	vector< KDObjDist<Object> > findNear(const float* point,const float radius);
	//findinAABox(const float*& min,const float*& max);
	bool balance(void);
	long count(void);
	void stats(void);
	
};

//Because of the template class, implementation must be here :(
#pragma implementation
#include "KDTree.cc"

#endif /* !KDTREE_HH */
