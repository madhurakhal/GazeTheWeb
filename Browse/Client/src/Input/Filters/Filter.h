//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract interface for eye gaze filters.

#ifndef FILTER_H_
#define FILTER_H_

#include "plugins/Eyetracker/Interface/EyetrackerSampleData.h"
#include <vector>

class Filter
{
public:

	// Update. Takes samples in window pixel coordinates
	virtual void Update(std::vector<SampleData> samples,
		double& rGazeX,
		double& rGazeY,
		bool& rSaccade) = 0;


};

#endif FILTER_H_