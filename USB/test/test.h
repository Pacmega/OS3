#ifndef _TEST_H_
#define _TEST_H_

class Scan
{
private:
    int serialNumber;
    int timesRecycled;
    Scan* next;

public:
	Scan(int number);
	/*	pre :
		post: serialNumber == number and timesRecycled == 0
	*/

	virtual ~Scan();
    /*  pre :
        post: object Scan was deleted
    */

    int getSerialNumber() const;
	/*	pre :
		post: return serial value
	*/

    Scan* getNext();
	/*	pre : -
		post: return next
	*/

    void setNext(Scan* nextScan);
	/*	pre : -
		post: next points to nextScan
	*/

    void recycle();
    // pre: -
    // post: timesRecycled has been increase by one

    int getTimesRecycled() const;
    // pre: -
    // post: timesRecycled has been returned
};

#endif
