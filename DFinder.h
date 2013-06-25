#ifndef _DFINDER_H_
#define _DFINDER_H_

#include "SoftClip.h"
#include "Interval.h"
#include "TargetRegion.h"
#include "Consensus.h"
#include "api/BamReader.h"


template<typename T>
class DeletePtr {
 public:
  void operator() (T* ptr) {
    delete ptr;
  }
};

class DFinder
{
 public:
  DFinder(const std::string& filename, int meanInsertSize, int stdInsertSize, int minOverlapLength, double maxMismatchRate);
  ~DFinder();
  void callToFile(const std::string& filename);
  void callToVcf(const std::string& filename);
  
 private: 
  void call(const std::string& filename, std::vector<Deletion>& calls);
  
  void loadFrom(const std::string& filename);
  
  void identifyTargetRegions(int referenceId, std::vector<TargetRegion>& regions);

  // void computeConsensuses(int referenceId, std::vector<Consensus>& consensuses1, std::vector<Consensus>& consensuses2);

  template <typename T, typename Compare, typename Compare1, typename Compare2>
  void callAllDeletions(const std::vector<TargetRegion>& regions,
                        const T& consensuses1,
                        const T& consensuses2,
                        Compare comp,
                        Compare1 comp1,
                        Compare2 comp2,
                        std::vector<Deletion>& calls);

  template <typename ForwardIterator, typename Compare>
  void callOneDeletion(ForwardIterator first1,
                       ForwardIterator last1,
                       ForwardIterator first2,
                       ForwardIterator last2,
                       Compare comp,
                       const TargetRegion& region,
                       std::vector<Deletion>& calls);
  
  int meanInsertSize;
  int stdInsertSize;
  int minOverlapLength;
  double maxMismatchRate;
  BamTools::RefVector references;
  int size;
  
  std::vector<std::vector<SoftClip*> > leftClips;
  std::vector<std::vector<SoftClip*> > rightClips;
  std::vector<std::vector<Interval*> > intervals;
  std::vector<std::vector<TargetRegion> > lRegions;
};

template <typename T, typename Compare, typename Compare1, typename Compare2>
void DFinder::callAllDeletions(const std::vector<TargetRegion>& regions,
                               const T& consensuses1,
                               const T& consensuses2,
                               Compare comp,
                               Compare1 comp1,
                               Compare2 comp2,
                               std::vector<Deletion>& calls) {
  for (auto itr = regions.begin(); itr != regions.end(); ++itr) {
    auto first1 = lower_bound(consensuses1.begin(), consensuses1.end(), (*itr).start, comp1);
    auto last1 = upper_bound(consensuses1.begin(), consensuses1.end(), (*itr).end, comp2);
    auto first2 = upper_bound(consensuses2.begin(), consensuses2.end(), (*itr).start, comp2);
    auto last2 = lower_bound(consensuses2.begin(), consensuses2.end(), (*itr).end, comp1);
    // if ((*itr).start == 2802991 && (*itr).end == 2806403) {
    //   for (auto itr2 = first1; itr2 != last1 + 1; ++itr2)
    //     std::cout << **itr2 << std::endl;
    //   std::cout << "2222222222222" << std::endl;
    //   for (auto itr2 = first2; itr2 != last2; ++itr2) {
    //     std::cout << **itr2 << std::endl;
    //   }
    // }
    callOneDeletion(first1, last1 + 1, first2, last2, comp, *itr, calls);
  }
}

template <typename ForwardIterator, typename Compare>
void DFinder::callOneDeletion(ForwardIterator first1,
                              ForwardIterator last1,
                              ForwardIterator first2,
                              ForwardIterator last2,
                              Compare comp,
                              const TargetRegion& region,
                              std::vector<Deletion>& calls) {
  for (auto itr1 = first2; itr1 != last2; ++itr1) {
    first1 = upper_bound(first1, last1, *itr1, comp);
    for (auto itr2 = first1; itr2 != last1; ++itr2) {
      // std::cout << (*itr1).getClipPosition() << "\t" << (*itr2).getClipPosition() << std::endl;
      if ((*itr1)->minDeletionLength(**itr2) < region.minDeletionLength) continue;
      if ((*itr1)->maxDeletionLength(**itr2) > region.maxDeletionLength) break;
      Overlap overlap;
      if((*itr1)->overlaps(**itr2, minOverlapLength, maxMismatchRate, overlap)) {
        calls.push_back(overlap.getDeletion());
        // std::cout << overlap << std::endl;
        return;
      }
    }
    break;
  }
}

#endif /* _DFINDER_H_ */