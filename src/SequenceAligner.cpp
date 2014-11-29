#include "SequenceAligner.h"

#include <array>        // std::array

using namespace std;


float SequenceAligner::getTotalScore(std::vector<int> A, std::vector<int> B) 
{
	int lengthA = A.size();
	int lengthB = B.size();

	std::vector<std::vector<float>> matrix(lengthA, std::vector<float>(lengthB));

	matrix[0][0] = 0.0f;

	for (int i = 1; i < lengthA; i++) {
		matrix[i][0] = matrix[i - 1][0] + gapScoreFunction(A.at(i));
	}

	for (int j = 1; j < lengthB; j++) {
		matrix[0][j] = matrix[0][j - 1] + gapScoreFunction(B.at(j));
	}

	for (int i = 1; i < lengthA; i++) {
		for (int j = 1; j < lengthB; j++) {
//			float matchScore = matchScoreMatrix_->at(A.at(i)).at(B.at(j));
			float matchScore = matchScoreFunction(A.at(i), B.at(j));
			float match = matrix[i - 1][j - 1] + matchScore;
			float skipA = matrix[i - 1][j] + gapScoreFunction(A.at(i));
			float skipB = matrix[i][j - 1] + gapScoreFunction(B.at(j));

			matrix[i][j] = max(max(match, skipA), skipB);
		}
	}

	return matrix[lengthA - 1][lengthB - 1];
}
