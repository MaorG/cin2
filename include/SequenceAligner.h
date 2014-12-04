#pragma once
#include "common.h"


class SequenceAligner
{
protected:

	std::map<int, std::map<int, float>> * matchScoreMatrix_;  

	float(*matchScoreFunction)(float, float);
	float(*gapScoreFunction)(float);

	// todo: work with templates!
	float gapScore_;

	void clearMatchScore() {
		for (std::map<int, std::map<int, float>>::iterator it = matchScoreMatrix_->begin(); it != matchScoreMatrix_->end(); ++it)
		{
			it->second.clear();
		}
		matchScoreMatrix_->clear();
	};

	std::vector<std::vector<float>> getAlignmentMatrix(std::vector<float> A, std::vector<float> B);


public:

	SequenceAligner() {};
	//SequenceAligner(float(*matchScoreFunction)(float, float),
	//				float(*gapScoreFunction)(float)) :
	//	matchScoreFunction(matchScoreFunction),
	//	gapScoreFunction(gapScoreFunction) {};

	void setMatchScoreFunction(float(*aMatchScoreFunction)(float, float)) {
		matchScoreFunction = aMatchScoreFunction;
	};

	void setGapScoreFunction(float(*aGapScoreFunction)(float)) {
		gapScoreFunction = aGapScoreFunction;
	};

	~SequenceAligner(){
		clearMatchScore();
	};

	void setGapScore(float gapScore) { gapScore_ = gapScore; };

	void setMatchScore(std::map<int, std::map<int, float>> * matchScoreMatrix)  {
		matchScoreMatrix_ = matchScoreMatrix;
	};


	float getTotalScore(std::vector<float> A, std::vector<float> B);
	std::tuple< std::vector<int>, std::vector<int>>
		getBestAlignment(std::vector<float> A, std::vector<float> B);

};

