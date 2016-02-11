#include "utils.h"

//Convert 2 dim-vector to unsigned char Mat
Mat vec2mat(vector< vector<int> > vec_matrix)
{
    Mat matrix = Mat::zeros(vec_matrix.size(),vec_matrix[0].size(), CV_8U);

    for(int i = 0; i < vec_matrix.size(); i++)
    {
        for(int j = 0; j < vec_matrix[0].size(); j++)
        {
            matrix.at<unsigned char>(i,j) = vec_matrix[i][j];
        }
    }

    return matrix;
}

//Convert 2 dim-vector to integer Mat
Mat vec2matInt(vector< vector<int> > vec_matrix)
{
    Mat matrix = Mat::zeros(vec_matrix.size(),vec_matrix[0].size(), CV_32F);

    for(int i = 0; i < vec_matrix.size(); i++)
    {
        for(int j = 0; j < vec_matrix[0].size(); j++)
        {
            matrix.at<float>(i,j) = vec_matrix[i][j];
        }
    }

    return matrix;
}



//Convert 2 dim array to floating Mat
Mat array2Mat32F(int** C, int rows, int cols){
    Mat matrix = Mat::zeros(rows,cols, CV_32F);
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            matrix.at<float>(i,j) =  C[i][j];
        }
    }
    return matrix;
}

//Convert 2 dim array to unsigned char Mat
Mat array2Mat8U(int** C, int rows, int cols){
    Mat matrix = Mat::zeros(rows,cols, CV_8U);
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) {
            matrix.at<unsigned char>(i,j) =  C[i][j];
        }
    }
    return matrix;
}

//Returns permuted index of j given permutation matrix
//Give column index -> Returns row index
int getPermuted(Mat& P, int c)
{
    for(int r = 0; r < P.col(c).size().height; r++)
    {
        if(P.at<unsigned char>(r,c) == 1)
            return r;
    }
    return -1;
}

//Get index of value at particular colum col
int getIndexByCol(Mat& M, int col, int val)
{
    //size() returns in [col,row] index order

    for(int i = 0; i < M.col(col).size().height; i++)
    {
        if(M.at<int>(i,col) == val)
            return i;
    }
    return -1;
}

// Given frame number returns associated file path of that frame
std::string getFilePath(std::string dir, std::string folder, std::string prefix, int frame_no)
{
    std::stringstream path;

    if(frame_no < 10)
        path << dir << "/" << folder << "/" << prefix << "00" << frame_no << ".jpg";
    else if(frame_no < 100)
        path << dir << "/" << folder << "/" << prefix << "0" << frame_no << ".jpg";
    else if(frame_no < 1000)
        path << dir << "/" << folder << "/" << prefix << "" << frame_no << ".jpg";
    else
        path << dir << "/" << folder << "/" << prefix << frame_no << ".jpg";

    return path.str();
}

// Scales up the given image by given factor in vertical axis only
// Sample-and-hold interpolation is used
void scaleUpMap(Mat &img, Mat &img_scaled, int factor_x, int factor_y)
{
    img_scaled = Mat::zeros(img.size().height*factor_y, img.size().width*factor_x, CV_8UC3);
    for(int i = 0; i < img.size().height; i++)
    {
        for(int j = 0; j < img.size().width; j++)
        {
            for(int a = 0; a < factor_x; a++)
                for(int b = 0; b < factor_y; b++)
                    img_scaled.at<Vec3b>(i*factor_y+b, j*factor_x+a) = img.at<Vec3b>(i,j);
        }
    }
}


