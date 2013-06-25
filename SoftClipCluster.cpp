#include "SoftClipCluster.h"
#include <algorithm>
#include <sstream>
#include <cassert>

// std::ostream& operator <<(std::ostream& stream, SingleClippedCluster& self) {
//   stream << self.str();
//   return stream;
// }

SoftClipCluster::SoftClipCluster(const SoftClip* clip) {
  clips.push_back(clip);
}

void SoftClipCluster::add(const SoftClip* clip) {
  clips.push_back(clip);
}

size_t SoftClipCluster::size() const { return clips.size(); }

int SoftClipCluster::referenceId() const { return clips[0]->referenceId(); }

int SoftClipCluster::clipPosition() const { return clips[0]->position(); }

// std::string SingleClippedCluster::str() {
//   std::stringstream sstream;
//   sstream << anchor << "\t[" << size() << "]" << std::endl;
//   sstream << contig().sequence();
//   return sstream.str();
// }

std::string SoftClipCluster::consensusSequence() const {
  assert(size() > 0);
  if (size() == 1) return clips[0]->sequence();
  
  int  N1 = localClipPosition();
  std::vector<int> diffs(size());
  for (int i = 0; i < diffs.size(); i++) {
    diffs[i] = clips[i]->lengthOfLeftPart() - N1;
  }
  std::vector<int> lens2(size());
  for (int i = 0; i < lens2.size(); i++)
    lens2[i] = clips[i]->lengthOfRightPart();
  
  int N = N1 + secondLargest(lens2);
  std::string seq;
  for (int i = 0; i < N; i++) {
    std::map<char, int> bases;
    std::map<char, int> quals;
    for (int j = 0; j < size(); j++) {
      int ind = diffs[j] + i;
      if (ind < 0 || ind >= clips[j]->length()) continue;
      ++bases[clips[j]->at(ind)];
      quals[clips[j]->at(ind)] += clips[j]->qual(ind) - '!';
    }
    for (std::map<char, int>::iterator it = bases.begin(); it != bases.end(); ++it)
      quals[it->first] /= it->second;
    seq += correctBase(bases, quals);
  }
  return seq;
}

int SoftClipCluster::localClipPosition() const {
  std::vector<int> lens1(size());
  for (int i = 0; i < lens1.size(); i++) lens1[i] = clips[i]->lengthOfLeftPart();
  return secondLargest(lens1);  
}

Consensus SoftClipCluster::getConsensus() const {
  return Consensus(referenceId(), clipPosition(), localClipPosition(), consensusSequence(), size());
}

char SoftClipCluster::correctBase(const std::map<char, int>& bases, const std::map<char, int>& quals) {
  int maxnum = 0;
  for (std::map<char, int>::const_iterator it = bases.begin(); it != bases.end(); ++it)
    if (maxnum < it->second) maxnum = it->second;
  int maxqual= 0;
  char ch;
  for (std::map<char, int>::const_iterator it = bases.begin(); it != bases.end(); ++it) {
    if (it->second == maxnum && maxqual < quals.at(it->first)) {
      maxqual = quals.at(it->first);
      ch = it->first;
    }
  }
  return ch;
}

int SoftClipCluster::secondLargest(const std::vector<int>& lens) {
  assert(lens.size() > 1);
  int max = lens[0];
  int secondMax = lens[1];
  if (max < secondMax) std::swap(max, secondMax);

  for (int i = 2; i < lens.size(); i++) {
    if (max <= lens[i]) {
      secondMax = max;
      max = lens[i];
    } else if (secondMax < lens[i]) {
      secondMax = lens[i];
    }
  }
  return secondMax;
}