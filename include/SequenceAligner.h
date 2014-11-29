#pragma once
#include "common.h"


class SequenceAligner
{
protected:

	std::map<int, std::map<int, float>> * matchScoreMatrix_;  
	float matchScoreFunction(int a, int b) {

		if (abs(a - b) > 18) {
			return (float)-abs(a + b);
		}

		return 5.0 - (float)abs(a - b);
	};
	float gapScoreFunction(int a) {
		return (float)-abs(a);
	};

	// todo: work with templates!
	float gapScore_;

	void clearMatchScore() {
		for (std::map<int, std::map<int, float>>::iterator it = matchScoreMatrix_->begin(); it != matchScoreMatrix_->end(); ++it)
		{
			it->second.clear();
		}
		matchScoreMatrix_->clear();
	};

	

public:

	SequenceAligner(){};
	~SequenceAligner(){
		clearMatchScore();
	};

	void setGapScore(float gapScore) { gapScore_ = gapScore; };

	void setMatchScore(std::map<int, std::map<int, float>> * matchScoreMatrix)  {
		matchScoreMatrix_ = matchScoreMatrix;
	};


	float getTotalScore(std::vector<int> A, std::vector<int> B);

};

