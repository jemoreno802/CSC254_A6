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
        // replace this line:
        (void) a;  (void) b;  return true;
    }
    bool equals(const T& a, const T& b) const {
        // replace this line:
        (void) a;  (void) b;  return true;
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
    T arr [5] = {};
    int nextOpen = 0;
    int numElements = 5;

  public:
    carray_simple_set(const T l, const T h) {   // constructor
        low = l;
        high = h;
    }
    virtual ~carray_simple_set() {              // destructor
        delete this; //idk what to put in a destructor 
    }
    virtual carray_simple_set<T>& operator+=(const T item) {
        cout << "inserting: " << item << std::endl;
        if(item < low || item >= high){
            throw out_of_bounds();
        }
        arr[nextOpen] = item;
        nextOpen++; //redefine for inserting in middle of array if nextOpen is a prev. deleted space
        return *this;
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        if(item < low || item >= high){
            throw out_of_bounds();
        }
        //todo: search array for element and then delete it 
        return *this;
    }
    virtual bool contains(const T& item) const {
        // replace this line:
        (void) item;  return true;
    }

    void print(){
        for(int i = 0; i<numElements; i++){
            cout << arr[i] << std::endl;
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
  public:
    hashed_simple_set(const int n) {    // constructor
        // replace this line:
        (void) n;
    }
    virtual ~hashed_simple_set() { }    // destructor
    virtual hashed_simple_set<T, F>& operator+=(const T item) {
        // replace this line:
        (void) item;  return *this;
    }
    virtual hashed_simple_set<T, F>& operator-=(const T item) {
        // replace this line:
        (void) item;  return *this;
    }
    virtual bool contains(const T& item) const {
        // replace this line:
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
	
	struct node* root;
	C cmp;

  public:
    bin_search_simple_set(const double n){
        (void) n;
    }
    virtual ~bin_search_simple_set<T, C>() { }

	// inserts item into set
	// returns a ref so you can say, e.g.
	// S += a += b += c;
    virtual bin_search_simple_set<T, C>& operator+=(const T item){
        if (!root) //No root yet
		{
			root->value = item;
		}

		struct node* current = root;
		bool placed = false;
		while (!placed)
		{
			//Check if already in set
			if (cmp.equals(item, current->value))
			{
				(void)item; return *this; //Return if already in set
			} 
			else if (cmp.precedes(item, current->value))//Less than current
			{
				//Does it have a left child? No -> make it left child, Yes -> loop back
				if (!current->left)
				{
					struct node* newnode;
					newnode->value = item;
					current->left = newnode;
					placed = true;
				}
				else
				{
					current = current->left;
				}
			}
			else { //Greater than current
				//Does it have a right child? No -> make it right child, Yes -> loop back
				if (!current->right)
				{
					struct node* newnode;
					newnode->value = item;
					current->right = newnode;
					placed = true;
				}
				else
				{
					current = current->right;
				}
			}
		}
		(void) item; return *this;
    }

	// removes item from set, if it was there (otherwise does nothing)
    virtual bin_search_simple_set<T, C>& operator-=(const T item) {
		if (!root || cmp.equals(item, root->value) //No items -> return
		{
			root = NULL;
			(void)item; return *this;
		}

		struct node* current = root;
		struct node* current_parent;
		bool removed = false;
		while (!removed)
		{
			//Check current for leaf condition
			if (cmp.equals(item, current->value))
			{
				//TODO Remove item and set children
				if (current->left && !current->right) //Only right is null
				{
					current = current->left;
				} 
				else if (current->right && !current->left) //Only left is null
				{
					current = current->right;
				}
				else if (!current->left && !current->right)// Both children null
				{
					free(current);
				}
				else //Both nodes present
				{
					struct node* current_search = current->right;

					// Find left-most leaf from current
					while (current_search && current_search->left != NULL)
						current_search = current_search->left;

					current = current_search;
				}
				removed = true;
			}
			else if (cmp.precedes(item, current->value))//Less than current
			{
				//Does it have a left child? No -> return success, Yes -> goto left child
				if (!current->left)
				{
					removed = true;
				}
				else
				{
					current_parent = current;
					current = current->left;
				}
			}
			else { //Greater than current
				   //Does it have a right child? No -> return success, Yes -> goto right child
				if (!current->right)
				{
					removed = true;
				}
				else
				{
					current_parent = current;
					current = current->right;
				}
			}
		}
        (void) item; return *this;

    }

	// and some methods
    virtual bool contains(const T& item) const {
        (void) item; return false;  
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
    carray_simple_set<int>* test = new carray_simple_set<int>(1,5);
    //(*test).print();

    //*test += 2; // * required
   // *test += 1;
    //*test += 10;
    //test->
    //(*test).print();

    carray_simple_set<weekday>* V = new carray_simple_set<weekday>(mon, (weekday)5);
    *V += mon;
    *V += tue;
    *V += wed;
    *V += thu;
    //*V += (weekday)5; //OUT OF BOUNDS
    V->print();
    

}