#include "SequenceAligner.h"

#include <array>        // std::array

using namespace std;

// todo - make the matrix hold tuples with prev and punish longer skipping

std::vector<std::vector<float>> SequenceAligner::getAlignmentMatrix(std::vector<float> A, std::vector<float> B)
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

	return matrix;

}


std::tuple< std::vector<int>, std::vector<int>>
SequenceAligner::getBestAlignment(std::vector<float> A, std::vector<float> B) 
{
	std::vector<std::vector<float>> matrix = getAlignmentMatrix(A, B);
	std::vector<int> alignmentA, alignmentB;

	int i = A.size()-1;
	int j = B.size()-1;

	while (i > 0 || j > 0) {
		if (i > 0 && j > 0 &&
			matrix[i][j] == 
			matrix[i - 1][j - 1] + 
			matchScoreFunction(A.at(i), B.at(j)) )
		{
				alignmentA.push_back(i);
				alignmentB.push_back(j);
				i--;
				j--;
		}
		else if (i > 0 &&
			matrix[i][j] == matrix[i - 1][j] + gapScoreFunction(A.at(i)))
		{
			i--;
		}
		else
			//if (j >= 0 &&
			//matrix[i][j] == matrix[i][j - 1] + gapScoreFunction(B.at(i)))
		{
			j--;
		}
	}

	alignmentA.push_back(i);
	alignmentB.push_back(j);


	std::tuple<std::vector<int>, std::vector<int>> alignments;
	alignments = std::make_tuple(alignmentA, alignmentB);
	return alignments;

}


float SequenceAligner::getTotalScore(std::vector<float> A, std::vector<float> B) 
{
	std::vector<std::vector<float>> matrix = getAlignmentMatrix(A,B);
	int lengthA = A.size();
	int lengthB = B.size();
	return matrix[lengthA - 1][lengthB - 1];
}
