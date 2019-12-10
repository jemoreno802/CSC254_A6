/*
    Starter code for assignment 6, CSC 2/454, Fall 2019

    Provides skeleton of code for a simple hierarchy of set
    abstractions.

    Everything but /main/ should be moved to a .h file, which should
    then be #included from here.
*/

#include <set>
#include <iostream>
#include <string.h>
#include <type_traits>
#include <sstream> 
#include <bitset>
using std::set;
using std::cout;
using std::string;

// Naive comparator.
// Provides a default for any type that has an operator<
// and an operator==.
//
template<typename T>
class comp {
  public:
    bool precedes(const T& a, const T& b) const {
        // replace this line 
	(void) a;  (void) b;  return a < b;
    }
    bool equals(const T& a, const T& b) const {
        // replace this line:
        (void) a;  (void) b;  return a == b;
    }
};

// Abstract base class from which all sets are derived.
//
template<typename T, typename C = comp<T>>
class simple_set {
  public:
    virtual ~simple_set<T, C>() { }
        // destructor should be virtual so that we call the right
        // version when saying, e.g.,
        // simple_set* S = new derived_set(args);
        //  ...
        // delete S;
    virtual simple_set<T, C>& operator+=(const T item) = 0;
        // inserts item into set
        // returns a ref so you can say, e.g.
        // S += a += b += c;
    virtual simple_set<T, C>& operator-=(const T item) = 0;
        // removes item from set, if it was there (otherwise does nothing)
    virtual bool contains(const T& item) const = 0;
        // indicates whether item is in set
};

//---------------------------------------------------------------

// Example of a set that implements the simple_set interface.
// Borrows the balanced tree implementation of the standard template
// library.  Note that you are NOT to use any standard library
// collections in your code (though you may use strings and streams).
//
template<typename T>
class std_simple_set : public virtual simple_set<T>, protected set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
  public:
    virtual ~std_simple_set<T>() { }  // will invoke std::~set<T>()
    virtual std_simple_set<T>& operator+=(const T item) {
        set<T>::insert(item);
        return *this;
    }
    virtual std_simple_set<T>& operator-=(const T item) {
        (void) set<T>::erase(item);
        return *this;
    }
    virtual bool contains(const T& item) const {
        return (set<T>::find(item) != set<T>::end());
    }
};


template<typename T>
static std::string toBinaryString(const T& x)
{
    std::stringstream ss;
    ss << std::bitset<sizeof(T) * 8>(x);
    return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

// Characteristic array implementation of set.
// Requires instantiation with guaranteed low and one-more-than-high
// bounds on elements that can be placed in the set.  Should compile
// and run correctly for any element class T that can be cast to int.
// Throws out_of_bounds exception when appropriate.
//
class out_of_bounds { };    // exception
template<typename T>
class carray_simple_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
    T low;
    T high;
    unsigned long int arr [3] = {};

  public:
    carray_simple_set(const T l, const T h) {   // constructor
        low = l;
        high = h;
    }
    virtual ~carray_simple_set() {              // destructor
        delete this; //idk what to put in a destructor 
    }
    virtual carray_simple_set<T>& operator+=(const T item) {
        if(item < low || item >= high){
            throw out_of_bounds();
        }
        cout << "inserting " << item << std::endl;
        unsigned long int number;
        if(item < 64){
            //item is in first word
            arr[0] |= 1UL << item;            
        }
        return *this;
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        // int nthBit = item;
        if(item < low || item >= high){
            throw out_of_bounds();
        }
        cout << "removing " << item << std::endl;
        unsigned long int number;
        if(item < 64){
            arr[0] &= ~(1UL << item);// item is in first word
        }
        return *this;
        
    }
    virtual bool contains(const T& item) const {
        bool contained = ((arr[0] >> item) & 1U) == 1; //change to depend on which word
        return contained;
    }
    void print(){
        for(int i = 0; i<3; i++){
            //cout << i << ": " << arr[i] << std::endl;
            cout << toBinaryString(arr[i]) << std::endl;
        }
    }
};

//---------------------------------------------------------------

// Naive hash function object.
// Provides a default for any type that can be cast to int.
//
template<typename T>
class cast_to_int {
  public:
int operator()(const T n) {
	printf("Yee");
	return (int) n;
}
};

// Hash table implementation of set.
// Requires instantiation with guaranteed upper bound on number of elements
// that may be placed in set.  Throws overflow if bound is exceeded.
// Can be instantiated without second generic parameter if element type
// can be cast to int; otherwise requires hash function object.
//
class overflow { };         // exception
template<typename T, typename F = cast_to_int<T>>
class hashed_simple_set : public virtual simple_set<T> {
// 'virtual' on simple_set ensures single copy if multiply inherited
// You'll need some data members here.
// I recommend you pick a hash table size p that is a prime
// number >= n, use F(e) % p as your hash function, and rehash
// with kF(e) % p after the kth collision.  (But make sure that
// F(e) is never 0.)
struct hashnode {
T value;
hashnode* next;
};

struct hashnode* createHashNode(T value)
{
struct hashnode* newNode = (struct hashnode*)malloc(sizeof(struct hashnode));
newNode->value = value;
newNode->next = NULL;
return newNode;
};

hashnode** hashtable;
int max_count;
int current_count;
int table_size;	

public:
//Constructor
hashed_simple_set(const int n) {   
hashtable = new hashnode*[n];
table_size = n;
for (int i = 0; i < table_size; i++)
{
  hashtable[i] = NULL;
}

(void) n;
}

//Destructor
virtual ~hashed_simple_set() 
{
for (int i = 0; i < table_size; ++i)
{
hashnode* each = hashtable[i];
while(each)
{
hashnode* prev = each;
each = each->next;
delete prev;
}
}
delete[] hashtable;
} 

/* Data structure methods */
int hash(int value) const
{
return value % table_size;
}

virtual hashed_simple_set<T, F>& operator+=(const T item) {
	int hashed = hash(item);//Passive call to operator?
	hashnode* last_node = NULL;
	hashnode* current_node = hashtable[hashed];//Find node at correct bucket
	while(current_node)
	{
	  last_node = current_node;
	  current_node = current_node->next;
	}

	if(!current_node)
	{
	  current_node = createHashNode(item);
	  if (!last_node)
	  {
	    hashtable[hashed] = current_node;
	  }
	  else 
  	  {
	    last_node->next = current_node;
	  }
	} 
	else 
	{
	  current_node->value = item;
	}

        (void) item;  return *this;
    }

virtual hashed_simple_set<T, F>& operator-=(const T item) {
	int hashed = hash(item);//Passive call to operator?
	hashnode* last_node = NULL;
	hashnode* current_node = hashtable[hashed];//Find node at correct bucket
	
	while(current_node)
	{
	  if (current_node->value == item)
	  {
	    if(last_node)
	    {
	      last_node->next = current_node->next;
	    }
	   delete current_node;
           (void) item; return *this;	  }
	   current_node = current_node->next;
	}  
	(void) item;  return *this;
    }

    virtual bool contains(const T& item) const { 
	int hashed = hash(item);
	hashnode* current_node = hashtable[hashed];
	while (current_node)
	{
	  if(current_node->value == item) { (void) item; return true; }
	  current_node = current_node->next;
	}	
        (void) item;  return false;
    }
};

//---------------------------------------------------------------

// Sorted array implementation of set; supports binary search.
// Requires instantiation with guaranteed upper bound on number of
// elements that may be placed in set.  Throws overflow if bound is
// exceeded.
//
template<typename T, typename C = comp<T>>
class bin_search_simple_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
    // You'll need some data members here.
	struct node {
		T value;
		node* left;
		node* right;
	};
	
	C cmp;
	int current_count = 0;
	int max_count;

  public:
    struct node* root; 

    bin_search_simple_set(const double n){
            max_count = n;
	    (void) n;
    }
    virtual ~bin_search_simple_set<T, C>() { }

    /* Data structure methods */
    
    //Node creation
    struct node* createNode(T value)
    {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));
        newNode->value = value;
        newNode->left = NULL;
       	newNode->right = NULL;
        return newNode;
    }

    //In-order print of the binary search tree
    void printBST(struct node* root)
    {
        if (root)
        {
            printBST(root->left);
            printf("%d ", root->value);
            printBST(root->right);
        }
    }
    
    //Attempts to insert a new node at a given subtree
    struct node* r_insert(struct node* node, T value)
    {
        if (!node)
        {
	    //Check bounds for insertion
	    if(current_count == max_count) throw overflow();
            current_count++;
	    struct node* newnode = createNode(value);
	    return newnode; //Empty case
        }

        if (cmp.precedes(value, node->value))
            node->left = r_insert(node->left, value);
        else
            node->right = r_insert(node->right, value);

        return node;
    }

    //Finds a subtree's smallest value node, used for deletion mostly
    struct node* smallestNode(struct node* node)
    {
        struct node* current = node;

        while (current && current->left)//Iterates until it's at the bottom left
            current = current->left;

        return current;
    }

    //Recursivley attempts to remove a node of given value within the subtree
    struct node* r_deleteNode(struct node* root, T value)
    {
        // No item found, do nothing
        if (!root) return root;

        //At target value
        if (cmp.equals(value, root->value)) 
        { 
	    current_count--;//Removing item reduces count
            if (!root->left)//Only right child present
            {
                struct node* temp = root->right;
                free(root);
                return temp;
            }
            else if (!root->right)//Only left child present
            {
                struct node* temp = root->left;
                free(root);
                return temp;
            }

            //Both nodes present
            struct node* temp = smallestNode(root->right);//Find replacement value
            root->value = temp->value;//Replace value
            root->right = r_deleteNode(root->right, temp->value);//Remove the old ref to replacer
        }
        //Smaller value
        else if (cmp.precedes(value, root->value)){ 
		root->left = r_deleteNode(root->left, value);
	}
        //Must be larger value
        else
        {
           root->right = r_deleteNode(root->right, value);
        }
        return root;
    }

    bool r_contains(struct node* root, T value) const
    {
	
	if (!root) return false;
	if (cmp.equals(root->value, value)) return true;
	
	if (cmp.precedes(value, root->value))
	   return r_contains(root->left, value);
    	else return r_contains(root->right, value);
    }

	// inserts item into set
	// returns a ref so you can say, e.g.
	// S += a += b += c;
    virtual bin_search_simple_set<T, C>& operator+=(const T item){
        if (root)
        {
            r_insert(root, item);
        }
        else //If there's no root yet, assign it
        {
	    root = r_insert(root, item);
        }

        (void) item; return *this;
    }

	// removes item from set, if it was there (otherwise does nothing)
    virtual bin_search_simple_set<T, C>& operator-=(const T item) {
        if (root)
        {
            r_deleteNode(root, item);
        }

        (void) item; return *this;
    }

	// and some methods
    virtual bool contains(const T& item) const {
        (void) item; return r_contains(root, item);  
    }

};

//===============================================================
// RANGE SETS

// Function object for incrementing.
// Provides a default for any integral type.
//
template<typename T>
class increment {
    static_assert(std::is_integral<T>::value, "Integral type required.");
  public:
    T operator()(T a) const {
        return ++a;
    }
};

// Range type.  Uses comp<T> by default, but you can provide your
// own replacement if you want, e.g. for C strings.
//
static const struct { } empty_range;    // exception

template<typename T, typename C = comp<T>>
class range {
    T L;        // represents all elements from L
    bool Linc;  // inclusive?
    T H;        // through H
    bool Hinc;  // inclusive?
    C cmp;      // can't be static; needs explicit instantiation
  public:
    range(const T l, const bool linc, const T h, const bool hinc)
            : L(l), Linc(linc), H(h), Hinc(hinc), cmp() {
        if (cmp.precedes(h, l)
            || (cmp.equals(l, h) && (!Linc || !Hinc))) throw empty_range;
    }
    // no destructor needed
    T low() const { return L; }
    bool closed_low() const { return Linc; }
    T high() const { return H; }
    bool closed_high() const {return Hinc; }
    bool contains(const T& item) const {
        return ((cmp.precedes(L, item) || (Linc && cmp.equals(L, item)))
            && (cmp.precedes(item, H) || (Hinc && cmp.equals(item, H))));
    }
    // You may also find it useful to define the following:
    // bool precedes(const range<T, C>& other) const { ...
    // bool overlaps(const range<T, C>& other) const { ...
};

// You may find it useful to define derived types with two-argument
// constructors that embody the four possible combinations of open and
// close-ended:
//
// template<typename T, typename C = comp<T>>
// class CCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class COrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OOrange : public range<T, C> { ...

// This is the abstract class from which all range-supporting sets are derived.
//
template<typename T, typename C = comp<T>>
class range_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
  public:
    virtual range_set<T, C>& operator+=(const range<T, C> r) = 0;
    virtual range_set<T, C>& operator-=(const range<T, C> r) = 0;
};

//---------------------------------------------------------------

// As implemented in the standard library, sets contain individual
// elements, not ranges.  (There are range insert and erase operators, but
// (a) they use iterators, (b) they take time proportional to the number of
// elements in the range, and (c) they require, for deletion, that the
// endpoints of the range actually be in the set.  An std_range_set, as
// defined here, avoids shortcomings (a) and (c), but not (b).  Your
// bin_search_range_set should avoid (b), though it will have slow insert
// and remove operations.  A tree_range_set (search tree -- extra credit)
// would have amortized log-time insert and remove for individual elements
// _and_ ranges.
//
template<typename T, typename C = comp<T>, typename I = increment<T>>
class std_range_set : public virtual range_set<T, C>,
                      public std_simple_set<T> {
    // 'virtual' on range_set ensures single copy if multiply inherited
    static_assert(std::is_integral<T>::value, "Integral type required.");
    I inc;
  public:
    // The first three methods below tell the compiler to use the
    // versions of the simple_set methods already found in std_simple_set
    // (given true multiple inheritance it can't be sure it should do that
    // unless we tell it).
    virtual std_simple_set<T>& operator+=(const T item) {
        return std_simple_set<T>::operator+=(item);
    }
    virtual std_simple_set<T>& operator-=(const T item) {
        return std_simple_set<T>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return std_simple_set<T>::contains(item);
    }
    virtual range_set<T>& operator+=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this += i;
        }
        return *this;
    }
    virtual range_set<T>& operator-=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this -= i;
        }
        return *this;
    }
};

//---------------------------------------------------------------

// insert an appropriate carray_range_set declaration here

//---------------------------------------------------------------

// insert an appropriate hashed_range_set declaration here

//---------------------------------------------------------------

// insert an appropriate bin_search_range_set declaration here

//===============================================================

// comparator for C strings
//
class lexico_less {
  public:
    bool precedes(const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }
    bool equals(const char *a, const char *b) const {
        return strcmp(a, b) == 0;
    }
};

typedef enum{mon, tue, wed, thu, fri} weekday;

int main() {

    // Some miscellaneous code to get you started on testing your sets.
    // The following should work:
/*
    std_simple_set<int> R;
    R += 3;
    
    cout << "3 is " << (R.contains(3) ? "" : "not ") << "in R\n";
    cout << "5 is " << (R.contains(5) ? "" : "not ") << "in R\n";

    simple_set<double>* S = new std_simple_set<double>();
    *S += 3.14;
    cout << "pi is " << (S->contains(3.14) ? "" : "not ") << "in S\n";
    cout << "e is " << (S->contains(2.718) ? "" : "not ") << "in S\n";

    std_simple_set<string> U;
    U += "hello";
    cout << "\"hello\" is " << (U.contains("hello") ? "" : "not ") << "in U\n";
    cout << "\"foo\" is " << (U.contains("foo") ? "" : "not ") << "in U\n";

    range<string> r1("a", true, "f", true);
    cout << "\"b\" is " << (r1.contains("b") ? "" : "not ") << "in r1\n";
    cout << "\"aaa\" is " << (r1.contains("aaa") ? "" : "not ") << "in r1\n";
    cout << "\"faa\" is " << (r1.contains("faa") ? "" : "not ") << "in r1\n";

    range<const char*, lexico_less> r2("a", true, "f", true);
    cout << "\"b\" is " << (r2.contains("b") ? "" : "not ") << "in r2\n";
    cout << "\"aaa\" is " << (r2.contains("aaa") ? "" : "not ") << "in r2\n";
    cout << "\"faa\" is " << (r2.contains("faa") ? "" : "not ") << "in r2\n";

    // The following will not work correctly yet:

    range_set<int>* X = new std_range_set<int>();
    *X += range<int>(5, true, 8, false);
    if (X->contains(4)) cout << "4 is in X\n";
    if (X->contains(5)) cout << "5 is in X\n";      // should print
    if (X->contains(6)) cout << "6 is in X\n";      // should print
    if (X->contains(7)) cout << "7 is in X\n";      // should print
    if (X->contains(8)) cout << "8 is in X\n";
    if (X->contains(9)) cout << "9 is in X\n";
    *X -= range<int>(6, true, 10, false);
    if (X->contains(4)) cout << "4 is now in X\n";
    if (X->contains(5)) cout << "5 is now in X\n";      // should print
    if (X->contains(6)) cout << "6 is now in X\n";
    if (X->contains(7)) cout << "7 is now in X\n";
    if (X->contains(8)) cout << "8 is now in X\n";
    if (X->contains(9)) cout << "9 is now in X\n";

    simple_set<weekday>* V = new carray_simple_set<weekday>(mon, (weekday)5);

    hashed_simple_set<int, cast_to_int<int>> H(100);

    bin_search_simple_set<double> J(100);
*/
    /*	
    carray_simple_set<int>* test = new carray_simple_set<int>(1,5);
    //(*test).print();

    //*test += 2; // * required
   // *test += 1;
    //*test += 10;
    //test->
    //(*test).print();

    carray_simple_set<weekday>* V = new carray_simple_set<weekday>(mon, (weekday)5);
   // *V += mon;
    *V += tue;
   // *V += wed;
    *V += thu;
    //*V += (weekday)5; //OUT OF BOUNDS
    V->print();
    cout << "Tuesday: " << V->contains(tue) << std::endl;
    *V -= tue;
    *V -= thu;
    V->print();
    cout << "Tuesday: " << V->contains(tue) << std::endl;
    */
/*
    bin_search_simple_set<int>* test = new bin_search_simple_set<int>(5);
    *test += 2;
    *test += -1;
    *test += 5;
    *test += 3;
    *test += 4;
    *test -= 3;
    *test += 6;
    *test -= 3;
    
    test->printBST(test->root);
    printf("%d", test->contains(3));
*/
    hashed_simple_set<double>* hashtest = new hashed_simple_set<double>(10);
   *hashtest += 5.f;
   *hashtest += 4.f;
   *hashtest += 4.f;
   *hashtest -= 4.f;
   
   printf("%d", hashtest->contains(4.f));
}
