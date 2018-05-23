/**
 * @brief: core of ED
 * @author: hongxinliu <github.com/hongxinliu> <hongxinliu.com>
 * @date: Mar. 05, 2018
 */

#include "ED.h"

#define GAUSS_SIZE	(5)
#define GAUSS_SIGMA	(1.0)
#define SOBEL_ORDER	(1)
#define SOBEL_SIZE	(3)

int ED::detectEdges(const cv::Mat &image, 
					std::vector<std::list<cv::Point>> &edges, 
					const int proposal_thresh, 
					const int anchor_interval, 
					const int anchor_thresh)
{
	// 0.preparation
    cv::Mat gray;
    if(image.empty())
    {
        std::cout<<"Empty image input!"<<std::endl;
        return -1;
    }
    if(image.type() == CV_8UC1)
        gray = image.clone();
    else if(image.type() == CV_8UC3)
        cv::cvtColor(image, gray, CV_BGR2GRAY);
    else
    {
        std::cout<<"Unknow image type!"<<std::endl;
        return -2;
    }

    // 1.Gauss blur
    cv::GaussianBlur(gray, gray, cv::Size(GAUSS_SIZE, GAUSS_SIZE), GAUSS_SIGMA, GAUSS_SIGMA);

    // 2.get gradient magnitude and orientation
    cv::Mat M, O;
    getGradient(gray, M, O);

    // 3.get anchors
    std::vector<cv::Point> anchors;
    getAnchors(M, O, proposal_thresh, anchor_interval, anchor_thresh, anchors);

    // 4.trace edges from anchors
    cv::Mat status(gray.rows, gray.cols, CV_8UC1, cv::Scalar(STATUS_UNKNOWN)); //Init all status to STATUS_UNKNOWN
    edges.clear();
    for(const auto &anchor : anchors)
        traceFromAnchor(M, O, proposal_thresh, anchor, status, edges);
    
    return int(edges.size());
}

void ED::getGradient(const cv::Mat &gray, 
					 cv::Mat &M, 
					 cv::Mat &O)
{
	cv::Mat Gx, Gy;
    cv::Sobel(gray, Gx, CV_16SC1, SOBEL_ORDER, 0, SOBEL_SIZE);
    cv::Sobel(gray, Gy, CV_16SC1, 0, SOBEL_ORDER, SOBEL_SIZE);
    
    M.create(gray.rows, gray.cols, CV_16SC1);
    O.create(gray.rows, gray.cols, CV_8UC1);

    for(int r=0; r<gray.rows; ++r)
    {
        for(int c=0; c<gray.cols; ++c)
        {
            short dx = abs(Gx.at<short>(r, c));
            short dy = abs(Gy.at<short>(r, c));

            M.at<short>(r, c) = dx + dy;
            O.at<uchar>(r, c) = (dx > dy ? EDGE_VER : EDGE_HOR);
        }
    }
}

void ED::getAnchors(const cv::Mat &M, 
					const cv::Mat &O, 
					const int proposal_thresh, 
					const int anchor_interval, 
					const int anchor_thresh, 
					std::vector<cv::Point> &anchors)
{
	anchors.clear();

    for(int r = 1; r < M.rows - 1; r += anchor_interval)
    {   
        for(int c = 1; c < M.cols - 1; c += anchor_interval)
        {
            // ignore non-proposal pixels
            if(M.at<short>(r, c) < proposal_thresh)
                continue;
            
            // horizontal edge
            if(O.at<uchar>(r, c) == EDGE_HOR)
            {
                if(M.at<short>(r, c) - M.at<short>(r-1, c) >= anchor_thresh &&
                   M.at<short>(r, c) - M.at<short>(r+1, c) >= anchor_thresh)
                   anchors.emplace_back(c, r);
            }

            // vertical edge
            else
            {
                if(M.at<short>(r, c) - M.at<short>(r, c-1) >= anchor_thresh &&
                   M.at<short>(r, c) - M.at<short>(r, c+1) >= anchor_thresh)
                   anchors.emplace_back(c, r);
            }
        }
    }
}

void ED::traceFromAnchor(const cv::Mat &M, 
						 const cv::Mat &O, 
						 const int proposal_thresh, 
						 const cv::Point &anchor, 
						 cv::Mat &status, 
						 std::vector<std::list<cv::Point>> &edges)
{
	// if this anchor point has already been visited
    if(status.at<uchar>(anchor.y, anchor.x) != STATUS_UNKNOWN)
        return;
    
    std::list<cv::Point> edge;
    cv::Point pt_last;
    TRACE_DIR dir_last;

    // if horizontal edge, go left and right
    if(O.at<uchar>(anchor.y, anchor.x) == EDGE_HOR)
    {
        // go left first
		// sssume the last visited point is the right hand side point and TRACE_LEFT to current point, the same below
		pt_last = cv::Point(anchor.x + 1, anchor.y);
		dir_last = TRACE_LEFT;
        trace(M, O, proposal_thresh, pt_last, anchor, dir_last, false, status, edge);
        
        // reset anchor point
		// it has already been set in the previous traceEdge(), reset it to satisfy the initial while condition, the same below */
        status.at<uchar>(anchor.y, anchor.x) = STATUS_UNKNOWN;
        
        // go right then
		pt_last = cv::Point(anchor.x - 1, anchor.y);
		dir_last = TRACE_RIGHT;
		trace(M, O, proposal_thresh, pt_last, anchor, dir_last, true, status, edge);
    }

    // vertical edge, go up and down
    else
    {
        // go up first
		pt_last = cv::Point(anchor.x, anchor.y + 1);
		dir_last = TRACE_UP;
		trace(M, O, proposal_thresh, pt_last, anchor, dir_last, false, status, edge);

		// reset anchor point
		status.at<uchar>(anchor.y, anchor.x) = STATUS_UNKNOWN;

		// go down then
		pt_last = cv::Point(anchor.x, anchor.y - 1);
		dir_last = TRACE_DOWN;
		trace(M, O, proposal_thresh, pt_last, anchor, dir_last, true, status, edge);
    }

    edges.push_back(edge);
}

void ED::trace(const cv::Mat &M, 
			   const cv::Mat &O, 
			   const int proposal_thresh, 
			   cv::Point pt_last, 
			   cv::Point pt_cur, 
			   TRACE_DIR dir_last, 
			   bool push_back, 
			   cv::Mat &status, 
			   std::list<cv::Point> &edge)
{
	// current direction
    TRACE_DIR dir_cur;
    
    // repeat until reaches the visited pixel or non-proposal
	while (true)
	{   
        // terminate trace if that point has already been visited
        if(status.at<uchar>(pt_cur.y, pt_cur.x) != STATUS_UNKNOWN)
            break;

        // set it to background and terminate trace if that point is not a proposal edge
        if(M.at<short>(pt_cur.y, pt_cur.x) < proposal_thresh)
        {
            status.at<uchar>(pt_cur.y, pt_cur.x) = STATUS_BACKGROUND;
            break;
        }

        // set point pt_cur as edge
        status.at<uchar>(pt_cur.y, pt_cur.x) = STATUS_EDGE;
        if (push_back)
			edge.push_back(pt_cur);
		else
			edge.push_front(pt_cur);
        
        // if its direction is EDGE_HOR, trace left or right
		if (O.at<uchar>(pt_cur.y, pt_cur.x) == EDGE_HOR)
		{
            // calculate trace direction
			if (dir_last == TRACE_UP || dir_last == TRACE_DOWN)
			{
				if (pt_cur.x < pt_last.x)
					dir_cur = TRACE_LEFT;
				else
					dir_cur = TRACE_RIGHT;
			}
			else
				dir_cur = dir_last;

            // update last state
			pt_last = pt_cur;
            dir_last = dir_cur;
            
            // go left
			if (dir_cur == TRACE_LEFT)
			{
				auto leftTop = M.at<short>(pt_cur.y - 1, pt_cur.x - 1);
				auto left = M.at<short>(pt_cur.y, pt_cur.x - 1);
				auto leftBottom = M.at<short>(pt_cur.y + 1, pt_cur.x - 1);

				if (leftTop >= left && leftTop >= leftBottom)
					pt_cur = cv::Point(pt_cur.x - 1, pt_cur.y - 1);
				else if (leftBottom >= left && leftBottom >= leftTop)
					pt_cur = cv::Point(pt_cur.x - 1, pt_cur.y + 1);
				else
					pt_cur.x -= 1;

				// break if reaches the border of image, the same below
				if (pt_cur.x == 0 || pt_cur.y == 0 || pt_cur.y == M.rows - 1)
					break;
            }
            
            // go right
			else
			{
				auto rightTop = M.at<short>(pt_cur.y - 1, pt_cur.x + 1);
				auto right = M.at<short>(pt_cur.y, pt_cur.x + 1);
				auto rightBottom = M.at<short>(pt_cur.y + 1, pt_cur.x + 1);

				if (rightTop >= right && rightTop >= rightBottom)
					pt_cur = cv::Point(pt_cur.x + 1, pt_cur.y - 1);
				else if (rightBottom >= right && rightBottom >= rightTop)
					pt_cur = cv::Point(pt_cur.x + 1, pt_cur.y + 1);
				else
					pt_cur.x += 1;

				if (pt_cur.x == M.cols - 1 || pt_cur.y == 0 || pt_cur.y == M.rows - 1)
					break;
			}
        }

        // its direction is EDGE_VER, trace up or down
        else
        {
            // calculate trace direction
			if (dir_last == TRACE_LEFT || dir_last == TRACE_RIGHT)
			{
				if (pt_cur.y < pt_last.y)
					dir_cur = TRACE_UP;
				else
					dir_cur = TRACE_DOWN;
			}
			else
				dir_cur = dir_last;

			// update last state
			pt_last = pt_cur;
			dir_last = dir_cur;

			// go up
			if (dir_cur == TRACE_UP)
			{
				auto leftTop = M.at<short>(pt_cur.y - 1, pt_cur.x - 1);
				auto top = M.at<short>(pt_cur.y - 1, pt_cur.x);
				auto rightTop = M.at<short>(pt_cur.y - 1, pt_cur.x + 1);

				if (leftTop >= top && leftTop >= rightTop)
					pt_cur = cv::Point(pt_cur.x - 1, pt_cur.y - 1);
				else if (rightTop >= top && rightTop >= leftTop)
					pt_cur = cv::Point(pt_cur.x + 1, pt_cur.y - 1);
				else
					pt_cur.y -= 1;

				if (pt_cur.y == 0 || pt_cur.x == 0 || pt_cur.x == M.cols - 1)
					break;
			}

			// go down
			else
			{
				auto leftBottom = M.at<short>(pt_cur.y + 1, pt_cur.x - 1);
				auto bottom = M.at<short>(pt_cur.y + 1, pt_cur.x);
				auto rightBottom = M.at<short>(pt_cur.y + 1, pt_cur.x + 1);

				if (leftBottom >= bottom && leftBottom >= rightBottom)
					pt_cur = cv::Point(pt_cur.x - 1, pt_cur.y + 1);
				else if (rightBottom >= bottom && rightBottom >= leftBottom)
					pt_cur = cv::Point(pt_cur.x + 1, pt_cur.y + 1);
				else
					pt_cur.y += 1;

				if (pt_cur.y == M.rows - 1 || pt_cur.x == 0 || pt_cur.x == M.cols - 1)
					break;
			}
        }
    }
}
