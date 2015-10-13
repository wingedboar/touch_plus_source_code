#include "stereo_processor.h"

struct BlobPairOverlap
{
	BlobNew* blob0;
	BlobNew* blob1;

	int overlap;
	int index0;
	int index1;
	int y_diff_diff;

	BlobPairOverlap(BlobNew* _blob0, BlobNew* _blob1, int _overlap, int _index0, int _index1, int _y_diff_diff)
	{
		blob0 = _blob0;
		blob1 = _blob1;
		overlap = _overlap;
		index0 = _index0;
		index1 = _index1;
		y_diff_diff = _y_diff_diff;
	};
};

struct compare_blob_pair_overlap
{
	bool operator() (const BlobPairOverlap& blob_pair0, const BlobPairOverlap& blob_pair1)
	{
		return (blob_pair0.overlap > blob_pair1.overlap);
	}
};

void StereoProcessor::compute(MonoProcessorNew& mono_processor0, MonoProcessorNew& mono_processor1,
							  PointResolver& point_resolver, PointerMapper& pointer_mapper, Mat& image0, Mat& image1)
{
	Point pt_y_max0 = get_y_max_point(mono_processor0.fingertip_points);
	Point pt_y_max1 = get_y_max_point(mono_processor1.fingertip_points);
	int alignment_y_diff = pt_y_max0.y - pt_y_max1.y;
	int alignment_x_diff = mono_processor0.pt_alignment.x - mono_processor1.pt_alignment.x;

	//------------------------------------------------------------------------------------------------------------------------

	vector<BlobPairOverlap> blob_pair_vec;

	int index0 = 0;
	for (BlobNew& blob0 : mono_processor0.fingertip_blobs)
	{
		int index1 = 0;
		for (BlobNew& blob1 : mono_processor1.fingertip_blobs)
		{
			float y_diff = blob0.y_max - blob1.y_max;
			float y_diff_diff = abs(y_diff - alignment_y_diff) + 1;
			float dist = get_distance(blob0.pt_tip, Point(blob1.pt_tip.x + alignment_x_diff, blob1.pt_tip.y + alignment_y_diff), true) + 1;
			float overlap = blob0.compute_overlap(blob1, alignment_x_diff, alignment_y_diff, 1) * 1000 / y_diff_diff / dist;
			blob_pair_vec.push_back(BlobPairOverlap(&blob0, &blob1, overlap, index0, index1, y_diff_diff));
			++index1;
		}
		++index0;
	}
	sort(blob_pair_vec.begin(), blob_pair_vec.end(), compare_blob_pair_overlap());

	//------------------------------------------------------------------------------------------------------------------------

	vector<BlobPairOverlap> blob_pair_vec_filtered;

	bool checker0[100] = { 0 };
	bool checker1[100] = { 0 };
	for (BlobPairOverlap& blob_pair : blob_pair_vec)
	{
		if (checker0[blob_pair.index0] == true || checker1[blob_pair.index1] == true)
			continue;

		checker0[blob_pair.index0] = true;
		checker1[blob_pair.index1] = true;

		if (blob_pair.y_diff_diff >= 10)
			continue;

		blob_pair_vec_filtered.push_back(blob_pair);
	}

	//------------------------------------------------------------------------------------------------------------------------

	Mat image_visualization = Mat::zeros(HEIGHT_LARGE, WIDTH_LARGE, CV_8UC1);

	Point pt_resolved_pivot0 = point_resolver.reprojector->remap_point(mono_processor0.pt_palm, 0, 4);
	Point pt_resolved_pivot1 = point_resolver.reprojector->remap_point(mono_processor1.pt_palm, 1, 4);

	for (BlobPairOverlap& blob_pair : blob_pair_vec_filtered)
	{
		BlobNew* blob0 = blob_pair.blob0;
		BlobNew* blob1 = blob_pair.blob1;

		Point2f pt_resolved0 = point_resolver.compute(blob0->pt_tip, image0, 0);
		Point2f pt_resolved1 = point_resolver.compute(blob1->pt_tip, image1, 1);

#if 0
		circle(image_visualization, pt_resolved0, 5, Scalar(127), 2);
		circle(image_visualization, pt_resolved1, 5, Scalar(254), 2);
		circle(image_visualization, pt_resolved_pivot0, 10, Scalar(127), 2);
		circle(image_visualization, pt_resolved_pivot1, 10, Scalar(254), 2);
#endif

#if 1
		if (pt_resolved0.x != 9999 && pt_resolved1.x != 9999)
		{
			Point3f pt3d = point_resolver.reprojector->reproject_to_3d(pt_resolved0.x, pt_resolved0.y,
																	   pt_resolved1.x, pt_resolved1.y);
			
			circle(image_visualization, Point(320 + pt3d.x, 240 + pt3d.y), pow(1000 / pt3d.z, 2), Scalar(127), 1);
		}
#endif

#if 0
		blob0->fill(image_visualization, 127);
		for (Point& pt : blob1->data)
			image_visualization.ptr<uchar>(pt.y, pt.x +	100)[0] = 254;

		line(image_visualization, blob0->pt_y_max, Point(blob1->pt_y_max.x + 100, blob1->pt_y_max.y), Scalar(127), 1);
#endif
	}

	imshow("image_visualizationkladhflkjasdhf", image_visualization);
}