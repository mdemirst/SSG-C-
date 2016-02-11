#include "graphmatch.h"
#include "hungarian.h"
#include "utils.h"
#include <dlib/optimization/max_cost_assignment.h>

GraphMatch::GraphMatch(int img_width, int img_height) 
{
    this->img_width = img_width;
    this->img_height = img_height;

    this->color_weight = COLOR_WEIGHT;
    this->pos_weight = POS_WEIGHT;
    this->area_weight = AREA_WEIGHT;
}

//Calculates matching cost and show matching results on the window
float GraphMatch::drawMatches(vector<NodeSig> ns1, vector<NodeSig> ns2,
                                Mat img1, Mat img2)
{
    Mat assignment, cost;
    //Construct [rowsxcols] cost matrix using pairwise
    //distance between node signature
    int rows = ns1.size();
    int cols = ns2.size();

    int** cost_matrix = (int**)calloc(rows,sizeof(int*));
    for(int i = 0; i < rows; i++)
    {
        cost_matrix[i] = (int*)calloc(cols,sizeof(int));
        for(int j = 0; j < cols; j++)
        {
            //Multiplied by constant because hungarian algorithm accept integer defined cost
            //matrix. We later divide by constant for correction
            cost_matrix[i][j] = GraphMatch::calcN2NDistance(ns1[i], ns2[j])*MULTIPLIER_1000;
        }
    }

    hungarian_problem_t p;

    // initialize the hungarian_problem using the cost matrix
    hungarian_init(&p, cost_matrix , rows, cols, HUNGARIAN_MODE_MINIMIZE_COST);

    // solve the assignment problem
    hungarian_solve(&p);

    //Convert results to OpenCV format
    assignment = array2Mat8U(p.assignment,rows,cols);
    cost = array2Mat32F(cost_matrix,rows,cols);
    //Divide for correction
    cost = cost / MULTIPLIER_1000;

    //free variables
    hungarian_free(&p);

    // Calculate match score and
    // show matching results given assignment and cost matrix
    Mat img_merged;
    float matching_cost = 0;

    //Concatenate two images
    hconcat(img1, img2, img_merged);

    //Get non-zero indices which defines the optimal match(assignment)
    vector<Point> nonzero_locs;
    findNonZero(assignment,nonzero_locs);

    //Draw optimal match
    for(int i = 0; i < nonzero_locs.size(); i++)
    {
        Point p1 = ns1[nonzero_locs[i].y].center;
        Point p2 = ns2[nonzero_locs[i].x].center+Point(img1.size().width,0);
        circle(img_merged,p1,MATCH_CIRCLE_RADIUS,MATCH_LINE_COLOR);
        circle(img_merged,p2,MATCH_CIRCLE_RADIUS,MATCH_LINE_COLOR);
        line(img_merged,p1,p2,MATCH_LINE_COLOR,MATCH_LINE_WIDTH);

        float dist = cost.at<float>(nonzero_locs[i].y, nonzero_locs[i].x);

        matching_cost = matching_cost + dist;

        //Draw cost on match image
        stringstream ss;
        ss << dist;
        string cost_str = ss.str();
        Point center_pt((p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0);
        //putText(img_merged, cost_str, center_pt, cv::FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0), 1);
    }

    return matching_cost;

}

//Calculates matching cost and show matching results on the window
float GraphMatch::matchTwoImages(vector<NodeSig> ns1, vector<NodeSig> ns2,
                                 Mat& assignment, Mat& cost)
{
    //Construct [rowsxcols] cost matrix using pairwise
    //distance between node signature
    int rows = ns1.size();
    int cols = ns2.size();

    int** cost_matrix = (int**)calloc(rows,sizeof(int*));
    for(int i = 0; i < rows; i++)
    {
        cost_matrix[i] = (int*)calloc(cols,sizeof(int));
        for(int j = 0; j < cols; j++)
            cost_matrix[i][j] = GraphMatch::calcN2NDistance(ns1[i], ns2[j])*MULTIPLIER_1000;
    }

    hungarian_problem_t p;

    // initialize the hungarian_problem using the cost matrix
    hungarian_init(&p, cost_matrix , rows, cols, HUNGARIAN_MODE_MINIMIZE_COST);

    // solve the assignment problem
    hungarian_solve(&p);

    //Convert results to OpenCV format
    assignment = array2Mat8U(p.assignment,rows,cols);
    cost = array2Mat32F(cost_matrix,rows,cols);
    //Divide for correction
    cost = cost / MULTIPLIER_1000;

    //free variables
    hungarian_free(&p);

    // Calculate match score
    float matching_cost = 0;


    //Get non-zero indices which defines the optimal match(assignment)
    vector<Point> nonzero_locs;
    findNonZero(assignment,nonzero_locs);

    //Find optimal match cost
    for(int i = 0; i < nonzero_locs.size(); i++)
    {
        matching_cost = matching_cost + cost.at<float>(nonzero_locs[i].y, nonzero_locs[i].x);
    }

    return matching_cost;

}

double GraphMatch::calcN2NDistance(NodeSig s1, NodeSig s2)
{
    //Note: all individual sums must be normalized to 1
    double dist = 0;

    //difference between centers
    double max_dist = sqrt(img_width*img_width+img_height*img_width);   //for normalization purposes.
                                                                        //must be diagonal distance
                                                                        //of image = max distance

    dist = dist + pos_weight*pow(sqrt(pow(s1.center.x-s2.center.x,2)+pow(s1.center.y-s2.center.y,2))/max_dist, 2);


    //difference between colors
    double color_dist = (fabs(s1.colorR-s2.colorR)+
                         fabs(s1.colorG-s2.colorG)+
                         fabs(s1.colorB-s2.colorB))/255.0/3.0;


    dist = dist + color_weight*pow(color_dist,2);


    //difference between areas
    double area_dist = fabs((double)s1.area-s2.area) / (double)(img_width*img_height);

    dist = dist + area_weight*pow(area_dist,2);

    return dist;


//    //find edge differences
//    if(use_edge_permutation)
//        % EDGE DIFF CALC. First method
//        % FIND PERMUTATION MATRIX THAT OUTPUTS MINIMUM EDGE DIFFERENCE

//        nr_edges1 = s1{1,2};
//        nr_edges2 = s2{1,2};

//        if(nr_edges1 >= nr_edges2)
//            perm1 = perms(s1{1,3});
//            perm1 = perm1(:,1:nr_edges2);
//            diff = perm1 - ones(factorial(nr_edges1),1)*s2{1,3}';
//            diff = edge_weight*diff / 255; %max edge value can be 255
//            inrc = min(sum(diff.^2,2));
//            inrc = inrc + (nr_edges1-nr_edges2)*missingEdgeWeight;
//        else
//            perm2 = perms(s2{1,3});
//            perm2 = perm2(:,1:nr_edges1);
//            diff = perm2 - ones(factorial(nr_edges2),1)*s1{1,3}';
//            diff = edge_weight*diff / 255;
//            inrc = min(sum(diff.^2,2));
//            inrc = inrc + (nr_edges2-nr_edges1)*missing_edge_weight;
//        end
//        dist = dist + inrc;

//    else

//        % EDGE DIFF CALC - 2nd Method
//        % ORDER EDGE WEIGHTS AND FIND DIFFERENCE
//        %
//        %difference between edge weights
//        e1 = sort(s1{1,3},'descend');
//        e2 = sort(s2{1,3},'descend');
//        maxSizeAttr = max(s1{1,2},s2{1,2});
//        for i = 1 : maxSizeAttr
//            if(s1{1,2} < i || s2{1,2} < i)
//                dist = dist + missing_edge_weight;    %if one of the edges is missing.
//            else
//                diff = abs(e1(i) - e2(i)) / 255;
//                dist = dist + edge_weight*diff^2;
//            end
//        end
//    end
}
