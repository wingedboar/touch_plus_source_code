/*
 * Touch+ Software
 * Copyright (C) 2015
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Aladdin Free Public License as
 * published by the Aladdin Enterprises, either version 9 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Aladdin Free Public License for more details.
 *
 * You should have received a copy of the Aladdin Free Public License
 * along with this program.  If not, see <http://ghostscript.com/doc/8.54/Public.htm>.
 */

#pragma once

#include "hand_splitter_new.h"
#include "mat_functions.h"
#include "contour_functions.h"
#include "permutation.h"
#include "id_point.h"
#include "value_store.h"
#include "compare_structs.h"

class MonoProcessorNew
{
public:
	ValueStore value_store;

	BlobDetectorNew blob_detector_image_hand;
	BlobDetectorNew blob_detector_image_hand_rotated;

	LowPassFilter low_pass_filter;

	Point pt_palm;
	Point pt_palm_rotated;

	Point pt_index;
	Point pt_thumb;

	int dest_diff_x = 0;
	int dest_diff_y = 0;

	float angle_final = 0;
	float palm_radius = 1;

	vector<Point> points_unwrapped_result;

	bool compute(HandSplitterNew& hand_splitter, const string name, bool visualize, bool secondary = false);
	void bresenham_line(int x1_in, int y1_in, int const x2_in, int const y2_in, vector<Point>& result_out, const uchar count_in);
	void sort_contour(vector<Point>& points, vector<Point>& points_sorted, Point& pivot);
	void compute_extension_line(Point pt_start, Point pt_end, const uchar length, vector<Point>& line_points, const bool reverse);
	BlobNew* find_parent_blob_before_rotation(BlobNew* blob);
	Point rotate(Point pt, float angle = 9999, Point anchor_in = Point(9999, 9999));
	Point unrotate(Point pt, float angle = 9999, Point anchor_in = Point(9999, 9999));
	Point find_pt_extremum(BlobNew* blob, Point pt_anchor);

private:
	struct compare_point_angle
	{
		Point anchor;

		compare_point_angle(Point& anchor_in)
		{
			anchor = anchor_in;
		}

		bool operator() (const Point& point0, const Point& point1)
		{
			float theta0 = get_angle(point0.x, point0.y, anchor.x, anchor.y);
			float theta1 = get_angle(point1.x, point1.y, anchor.x, anchor.y);

			return theta0 > theta1;
		}
	};

	struct compare_point_dist
	{
		Point anchor;

		compare_point_dist(Point& anchor_in)
		{
			anchor = anchor_in;
		}

		bool operator() (Point& point0, Point& point1)
		{
			float dist0 = get_distance(point0, anchor);
			float dist1 = get_distance(point1, anchor);

			return dist0 < dist1;
		}
	};

	struct compare_id_point_y
	{
		bool operator() (const IDPoint& pt0, const IDPoint& pt1)
		{
			return (pt0.y > pt1.y);
		}
	};

	struct compare_point_x
	{
		bool operator() (const Point& pt0, const Point& pt1)
		{
			return (pt0.x < pt1.x);
		}
	};

	struct compare_point_y
	{
		bool operator() (const Point& pt0, const Point& pt1)
		{
			return (pt0.y < pt1.y);
		}
	};

	struct compare_extension_dist_to_point
	{
		Point pt;

		compare_extension_dist_to_point(Point& pt_in)
		{
			pt = pt_in;
		}

		bool operator() (vector<Point>& extension0, vector<Point>& extension1)
		{
			float dist0 = get_distance(extension0[0], pt);
			float dist1 = get_distance(extension1[0], pt);

			return dist0 < dist1;
		}
	};

	struct compare_extension_angle
	{
		bool operator() (vector<Point>& extension0, vector<Point>& extension1)
		{
			float angle0 = get_angle(extension0[0], extension0[extension0.size() - 1], Point(0, extension0[0].y));
			if (extension0[0].y > extension0[extension0.size() - 1].y)
				angle0 = 360 - angle0;

			float angle1 = get_angle(extension1[0], extension1[extension1.size() - 1], Point(0, extension1[0].y));
			if (extension1[0].y > extension1[extension1.size() - 1].y)
				angle1 = 360 - angle1;

			return angle0 < angle1;
		}
	};

	struct compare_extension_y
	{
		bool operator() (vector<Point>& extension0, vector<Point>& extension1)
		{
			return extension0[0].y > extension1[0].y;
		}
	};

	struct compare_extension_y_rotated
	{
		float angle;
		Point anchor;

		Point _rotate(Point pt)
		{
			pt = rotate_point(angle, pt, anchor);
			return pt;
		}

		compare_extension_y_rotated(float angle_in, Point& anchor_in)
		{
			angle = angle_in;
			anchor = anchor_in;	
		}

		bool operator() (vector<Point>& extension0, vector<Point>& extension1)
		{
			Point pt_rotated0 = _rotate(extension0[0]);
			Point pt_rotated1 = _rotate(extension1[0]);

			return pt_rotated0.y > pt_rotated1.y;
		}
	};
};