// #include <iostream>
#include <iomanip>
#include <cassert>
#include <cctype>
#include "Overlap.h"
#include "SoftClip.h"

Overlap::Overlap() {
}

Overlap::Overlap(const SoftClip *first, const SoftClip *second, int length, int numMismatches, int offset) :
    first(first),
    second(second),
    length(length),
    numMismatches(numMismatches),
    offset(offset) {
    assert(first != NULL && second != NULL && first->getReferenceId() == second->getReferenceId());
    // std::cout << offset << std::endl;
}

// int Overlap::getLength() const { return length; }

// int Overlap::getNumMismatches() const { return numMismatches; }

int Overlap::deletionLength() const {
    return first->isLeftPartClipped() ? first->getClipPosition() + offset - second->getClipPosition() :
                                        second->getClipPosition() + offset - first->getClipPosition();
}

int Overlap::start() const { return first->getClipPosition(); }

int Overlap::end() const { return second->getClipPosition() + offset; }

double Overlap::score() const {
    return numMismatches / float(length);
}

Deletion Overlap::getDeletion() const {
    // return Deletion(referenceId, clipPosition1, clipPosition2 + offset, clipPosition1-offset, clipPosition2);
    int start = first->isLeftPartClipped() ? second->getClipPosition() : first->getClipPosition();
    int end = first->isLeftPartClipped() ? first->getClipPosition() : second->getClipPosition();
    return Deletion(first->getReferenceId(), start, end, offset);
}

bool Overlap::equals(const std::string& s1, const std::string& s2, int maxMismatches, int& numMismatches) {
    int cnt = 0;
    if (s1.size() != s2.size()) return false;
    // assert(s1.size() == s2.size());
    for (size_t i = 0; i < s1.size(); ++i) {
        if (toupper(s1[i]) == 'N' || toupper(s2[i] == 'N')) continue;
        if (s1[i] != s2[i]) ++cnt;
        if (cnt > maxMismatches) return false;
    }
    numMismatches = cnt;
    return true;
}

// bug to fix in getBestOverlap
bool Overlap::getHighScoreOverlap(const std::vector<Overlap>& overlaps, Overlap& ov) {
    if (overlaps.size() == 0) return false;
    ov = overlaps.front();
    // if (ov.first->getClipPosition() == 30255998) {
    //     std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    //     for (auto itr = overlaps.begin(); itr != overlaps.end(); ++itr)
    // 	std::cout << *itr << std::endl;
    // }
    double min = ov.score();
    for (auto itr = overlaps.begin() + 1; itr != overlaps.end(); ++itr) {
        // std::cout << *itr << std::endl;
        if (min > (*itr).score()) {
            min = (*itr).score();
            ov = *itr;
        }
    }
    // if (ov.first->getClipPosition() == 30255998) std::cout << ov << std::endl;
    return true;
}

std::ostream& operator <<(std::ostream& stream, const Overlap& o) {
    stream << "[CALL] " << o.first->getReferenceId() << ":" << o.first->getClipPosition()
           << "-" << o.second->getClipPosition() << "+(" << o.offset << ")\t" << o.deletionLength() << "\t"
           << o.score() << std::endl;
    if (o.first->lengthOfLeftPart() >= o.second->lengthOfLeftPart() + o.offset) {
        stream << std::string(o.first->lengthOfLeftPart(), ' ') << '+' << std::endl;
        stream << o.first->getSequence() << std::endl;
        stream << std::string(o.first->lengthOfLeftPart() - o.second->lengthOfLeftPart() - o.offset, ' ') << o.second->getSequence() << std::endl;
        stream << std::string(o.first->lengthOfLeftPart() - o.offset, ' ') << '^' << std::endl;
    } else {
        stream << std::string(o.second->lengthOfLeftPart() + o.offset, ' ') << '+' << std::endl;
        stream << std::string(o.second->lengthOfLeftPart() - o.first->lengthOfLeftPart() + o.offset, ' ') << o.first->getSequence() << std::endl;
        stream << o.second->getSequence() << std::endl;
        stream << std::string(o.second->lengthOfLeftPart(), ' ') << '^' << std::endl;
    }
    return stream;
}
