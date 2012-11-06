#include <string>
#include <vector>
#include <algorithm>
#include "gtest/gtest.h"
#include "api/BamReader.h"
#include "clip-sv.h"

// TEST(SingleClipTest, GetSoftClips) {
//   std::string filename = "toy2.bam";
//   BamTools::BamReader reader;
//   if (!reader.Open(filename)) {
//     FAIL() << "Could not open input BAM file.";
//   }
//   BamTools::BamAlignment al;
//   std::vector<int> clipSizes, readPositions, genomePositions;
  
//   reader.GetNextAlignment(al);
//   EXPECT_FALSE(al.GetSoftClips(clipSizes, readPositions, genomePositions));
//   EXPECT_EQ(0, clipSizes.size());
//   EXPECT_EQ(0, readPositions.size());
//   EXPECT_EQ(0, genomePositions.size());
 
//   reader.GetNextAlignment(al);
//   EXPECT_TRUE(al.GetSoftClips(clipSizes, readPositions, genomePositions));
//   EXPECT_EQ("AAAAGATAAGGATA", al.QueryBases);
//   EXPECT_EQ("AGATAA*GGATA", al.AlignedBases);
//   EXPECT_EQ(8, al.Position);
//   EXPECT_EQ(0, al.RefID);
//   EXPECT_EQ(1, clipSizes.size());
//   EXPECT_EQ(1, readPositions.size());
//   EXPECT_EQ(1, genomePositions.size());
//   EXPECT_EQ(3, clipSizes[0]);
//   EXPECT_EQ(3, readPositions[0]);
//   EXPECT_EQ(8, genomePositions[0]);

//   reader.GetNextAlignment(al);
//   clipSizes.clear();
  
//   readPositions.clear();
//   genomePositions.clear();
//   EXPECT_TRUE(al.GetSoftClips(clipSizes, readPositions, genomePositions));
//   EXPECT_EQ(8, al.Position);  
//   EXPECT_EQ(1, clipSizes.size());
//   EXPECT_EQ(1, readPositions.size());
//   EXPECT_EQ(1, genomePositions.size());
//   EXPECT_EQ(3, clipSizes[0]);
//   EXPECT_EQ(11, readPositions[0]);
//   EXPECT_EQ(18, genomePositions[0]);

//   reader.Close();

// }

TEST(SingleClipTest, equals) {
  std::string s1 = "AGGTACT";
  std::string s2 = "ACGTACT";
  EXPECT_FALSE(equals(s1, s2));
  EXPECT_TRUE(equals(s1, s2, 1));
}

TEST(SingleClipTest, toString) {
  LeftClip lcl = LeftClip(0, 27, 3, 3, "ATAGTAGGCA");
  EXPECT_EQ("Clip: 0, 27, 3, 3, ATAGTAGGCA - [L]", lcl.toString());
}

// TEST(SingleClipTest, reverseIterator) {
//   std::vector<int> myvector;
//   for (int i=1; i<=5; i++) myvector.push_back(i);
//   std::vector<int>::reverse_iterator rit;
//   for ( rit=myvector.rbegin() ; rit < myvector.rend();) {
//     if (*rit == 4) {
//       std::vector<int>::iterator tempIt = myvector.erase(--rit.base());
//       rit = std::vector<int>::reverse_iterator(tempIt);
//     }
//     else ++rit;
//   }
//   EXPECT_EQ(4, myvector.size());
//   EXPECT_EQ(3, myvector[2]);
//   EXPECT_EQ(5, myvector[3]);
// }

void createGraph(std::vector<std::vector<int> >& g) {
  std::vector<int> nodes;
  nodes.push_back(0);
  nodes.push_back(3);
  g.push_back(nodes);
  nodes.clear();
  nodes.push_back(2);
  nodes.push_back(4);
  g.push_back(nodes);
  nodes.clear();
  nodes.push_back(0);
  nodes.push_back(3);
  g.push_back(nodes);
  nodes.clear();
  nodes.push_back(1);
  nodes.push_back(2);
  nodes.push_back(4);
  g.push_back(nodes);
}

TEST(SingleClipTest, match) {
  std::vector<std::vector<int> > g;
  int m = 4, n = 5;
  createGraph(g);
  Matching mat(g, m, n);
  EXPECT_EQ(4, mat.match());
  EXPECT_EQ(3, mat.getMateL(0));
  EXPECT_EQ(2, mat.getMateL(1));
  EXPECT_EQ(0, mat.getMateL(2));
  EXPECT_EQ(1, mat.getMateL(3));  
  EXPECT_EQ(2, mat.getMateR(0));
}

TEST(SingleClipTest, findOverlaps) {
  StructVar t1("22", 30, 80);
  StructVar t2("22", 340, 410);
  std::vector<StructVar> cs1;
  StructVar c1("22", 200, 220);
  cs1.push_back(c1);
  StructVar c2("22", 10, 40);
  cs1.push_back(c2);
  StructVar c3("22", 10, 30);
  cs1.push_back(c3);
  StructVar c4("22", 410, 500);
  cs1.push_back(c4);
  StructVar c5("22", 80, 150);
  cs1.push_back(c5);
  StructVar c6("22", 300, 340);
  cs1.push_back(c6);
  std::vector<StructVar> cs2 = cs1;
  sort(cs1.begin(), cs1.end(), cmp1);
  sort(cs2.begin(), cs2.end(), cmp2);
  std::set<StructVar> s1 = findOverlaps(t1, cs1, cs2);
  EXPECT_EQ(3, s1.size());
  EXPECT_EQ(1, s1.count(c2));
  EXPECT_EQ(0, s1.count(c4));
}

TEST(SingleClipTest, groupBreakpoints) {
  std::vector<Breakpoint> bps;
  bps.push_back(Breakpoint(10, 30));
  bps.push_back(Breakpoint(10, 40));
  bps.push_back(Breakpoint(30, 80));
  bps.push_back(Breakpoint(80, 150));
  bps.push_back(Breakpoint(200, 220));
  bps.push_back(Breakpoint(340, 410));
  bps.push_back(Breakpoint(380, 500));
  std::vector<std::vector<Breakpoint> > groups;
  groupBreakpoints(bps, groups);
  EXPECT_EQ(4, groups.size());
  EXPECT_EQ(3, groups[0].size());
  EXPECT_EQ(1, groups[1].size());
  EXPECT_EQ(1, groups[2].size());
  EXPECT_EQ(2, groups[3].size());
}
