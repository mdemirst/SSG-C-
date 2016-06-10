#ifndef SSGPROC_H
#define SSGPROC_H

#include <QObject>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/flann/flann.hpp>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QDate>
#include "utils.h"
#include "defs.h"
#include "utilTypes.h"

using namespace std;
using namespace cv;

class SSGProc : public QObject
{
    Q_OBJECT
public:
    SSGProc();
    static void updateSSG(SSG& ssg, vector<NodeSig>& ns, Mat& map_col);
    static Mat drawSSG(SSG& ssg, Mat& input);
    static void updateNodeSig(pair<NodeSig, int>& ns, NodeSig new_ns);
    static void filterSummarySegments(SSG& ssg, float tau_p);
    static void updateSSGInvariants(SSG& ssg, Mat& current_image, Parameters* params);
    static void updateSSGInvariantsFromDB(SSG& ssg, Mat& current_image, Parameters* params, FrameDesc& frame_desc);

public:

private:
    //QPen pen;

signals:
    //void showImg1(QImage img);

public slots:

};

#endif // SSGPROC_H
