#include "document.hpp"

/* int Document::getMatchRow(int i, int j) */
/* { */
/*     for (int k = 0; k < pairMat.rows; k++) */
/*     { */
/*         auto currI = pairMat.at<int>(k, 0); */
/*         auto currJ = pairMat.at<int>(k, 1); */

/*         if (currI == i && currJ == j) */
/*             return k; */
/*     } */
/*     return -1; */
/* } */

// not really erase to avoid conflicts in serialization
// just clear the correspondig vector
/* void Document::eraseMatchRow(int k) */
/* { */
/*     if (k >= pairMat.rows || k < 0) */
/*         return; */
/*     matches[k].clear(); */
/* } */


