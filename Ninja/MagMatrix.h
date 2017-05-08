//MagMatrix.h
#ifndef MAGMATRIX_H
#define MAGMATRIX_H


//This class is Magnar's template data structure, 2-dimensional data structure
template <typename T>
class Matrix
{
public:
	Matrix();//constructor
	Matrix(int x, int y);//create matrix with dimensions 
	~Matrix();//need to deallocate memory when finished
	void setDimensions(int x, int y);//reset the dimensions
	void clear();//empties the array
	T& get(int x, int y);//returns the dimensions given, can also be used to set values
	T& operator[] (int a);//uses matrix as a single-dimensional array
	long int size();//returns the size of the memory allocation
	int width();//returns the width of the matrix
	int height();//returns the height of the matrix
	bool valid(int x, int y);//returns true if the x,y cordinates are within valid bounds
private:
	int mX;//X, the width of the matrix
	int mY;//Y, the height of the matrix
	T* mData;
};

template <typename T>
Matrix<T>::Matrix()
{
	mData = NULL;
	setDimensions(0, 0);
}

template <typename T>
Matrix<T>::Matrix(int x, int y)
{	mData = NULL;
	mX = 0;
	mY = 0;//just in case other memories may conflict
	setDimensions(x, y);
}

template <typename T>
Matrix<T>::~Matrix()
{
	clear();
}


template <typename T>
void Matrix<T>::setDimensions(int x, int y)
{
	//if the current dimensions are the same as what you want, do nothing
	if (x == mX && y == mY)
		return;

	//if there is data to delete, delete it
	if (this->size() > 0)
		delete mData;

	//use the given arguments to replace the current size 
	mX = x;
	mY = y;
	
	if (this->size() > 0)//data will only be allocated if it is of non-zero size
		mData = new T[x * y];	//new, more like neo, amirite? 
								//get it? matrix? ehehehehe
	else 
	{//if the size() is 0, it could be like 1024*0 or whatever, I prefer it just be 0*0
		mX = 0;
		mY = 0;
	}
}

template <typename T>
void Matrix<T>::clear()
{
	if (this->size() > 0)//if there is data to delete, delete it,
		delete mData;

	//then set the dimensions to 0
	mX = 0;
	mY = 0;
}

template <typename T>
T& Matrix<T>::get(int x, int y)
{
	return mData[(x + (mX * y))];
}

template <typename T>
long int Matrix<T>::size()
{
	return (mX * mY);
}

template<typename T>
int Matrix<T>::width()
{
	return mX;
}

template<typename T>
int Matrix<T>::height()
{
	return mY;
}

template<typename T>
bool Matrix<T>::valid(int x, int y)
{
	return (x < mX
		&& x >= 0
		&& y < mY
		&& y >= 0);
}

template <typename T>
T& Matrix<T>::operator[](int a)
{
	return mData[a];
}



#endif // !MAGMATRIX_H

