#include "document.hpp"


int Document::getMatchRow(size_t i, size_t j)
{
    for (int k = 0; k < pairMat.rows; k++)
    {
        size_t currI = pairMat.at<int>(k, 0);
        size_t currJ = pairMat.at<int>(k, 1);

        if (currI == i && currJ == j)
            return k;
    }
    return -1;
}

// not really erase to avoid conflicts in serialization
// just clear the correspondig vector
void Document::eraseMatchRow(size_t k)
{
    if (k >= static_cast<size_t>(pairMat.rows))
        return;

    //cv::Mat truncPairMat = cv::Mat::zeros(pairMat.rows - 1, 2, pairMat.type());
    //for (int i = 0, j = 0; i < pairMat.rows; i++)
    //{
    //    if (i != static_cast<int>(k))
    //    {
    //        truncPairMat.at<int>(j, 0) = truncPairMat.at<int>(i, 0);
    //        truncPairMat.at<int>(j, 1) = truncPairMat.at<int>(i, 1);
    //        j++;
    //    }
    //}
    //pairMat = truncPairMat;
    matches[k].clear();
}


