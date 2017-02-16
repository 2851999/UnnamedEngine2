/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *****************************************************************************/

#include "HighScores.h"

#include "../../utils/Utils.h"

/*****************************************************************************
 * The HighScores class
 *****************************************************************************/

HighScores::HighScores() {

}

HighScores::~HighScores() {
	names.clear();
	scores.clear();
}

void HighScores::save() {
	std::vector<std::string> fileText;
	//Go through each name and add their score
	for (unsigned int i = 0; i < names.size(); i++)
		fileText.push_back(names[i] + ": " + StrUtils::str(scores[i]));
	//Write to the high scores file
	FileUtils::writeFile(filePath, fileText);
}

void HighScores::load() {
	//Ensure the names and scores are clear
	names.clear();
	scores.clear();

	std::vector<std::string> fileText = FileUtils::readFile(filePath);
	//Go through each line in the file
	for (unsigned int i = 0; i < fileText.size(); i++) {
		//Split up the current line
		std::vector<std::string> split = StrUtils::strSplit(fileText[i], ": ");
		//Add the name and score
		names.push_back(split[0]);
		scores.push_back(StrUtils::strToUInt(split[1]));
	}
}

void HighScores::add(std::string name, unsigned int score) {
	bool added = false;
	//Ensure the name will not break anything
	name = StrUtils::replaceAll(name, ": ", "");

	for (unsigned int i = 0; i < scores.size(); i++) {
		//Check the current score
		if (scores[i] < score && ! added) {
			//Add the score (as it is now in the correct place)
			names.insert(names.begin() + i, name);
			scores.insert(scores.begin() + i, score);
			added = true;
		}
	}
	//Check whether the score was added
	if (! added) {
		//Check the length of the scores
		if (scores.size() < MAX_HIGH_SCORES) {
			names.push_back(name);
			scores.push_back(score);
		}
	} else {
		//Check whether the number of scores stored is now too big
		if (scores.size() > MAX_HIGH_SCORES) {
			//Remove the last ones
			names.pop_back();
			scores.pop_back();
		}
	}
}

bool HighScores::isHighScore(unsigned int score) {
	if (scores.size() > 0) {
		return score > scores[0];
	} else
		return true;
}

bool HighScores::isOnTable(unsigned int score) {
	//Check how many scores are stored
	if (scores.size() < MAX_HIGH_SCORES)
		return true;
	else {
		//Go through the scores and check whether the given score is higher than any of them
		for (unsigned int i = 0; i < MAX_HIGH_SCORES; i++) {
			if (score > scores[i])
				return true;
		}
	}
	return false;
}
