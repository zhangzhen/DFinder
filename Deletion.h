#ifndef _DELETION_H_
#define _DELETION_H_

#include <string>
#include <iostream>

class Deletion {
public:
    Deletion(const std::string& referenceName, int start1, int end1, int start2, int end2, int length);

    virtual ~Deletion();

    std::string getReferenceName() const { return referenceName; }

    int getStart1() const { return start1; }

    int getEnd1() const { return end1; }

    int getStart2() const { return start2; }

    int getEnd2() const { return end2; }

    int getLength() const { return length; }

    std::string toBedpe() const;

    friend std::ostream& operator <<(std::ostream& stream, const Deletion& del);

    bool contains(const Deletion &other);
    bool dovetailsTo(const Deletion &other);

private:
    std::string referenceName;
    int start1;
    int end1;
    int start2;
    int end2;
    int length;

    bool checkRep() const;

};

#endif /* _DELETION_H_ */
