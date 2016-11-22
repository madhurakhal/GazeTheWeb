//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline for text selection

#ifndef TEXTSELECTIONPIPELINE_H_
#define TEXTSELECTIONPIPELINE_H_

#include "Pipeline.h"

class TextSelectionPipeline : public Pipeline
{
public:

    // Constructor. Takes start coordinate in pixel screen space
	TextSelectionPipeline(TabInteractionInterface* pTab, glm::vec2 startCoordinate);
};

#endif // TEXTSELECTIONPIPELINE_H_
