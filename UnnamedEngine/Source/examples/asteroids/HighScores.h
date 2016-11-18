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

#ifndef EXAMPLES_ASTEROIDS_HIGHSCORES_H_
#define EXAMPLES_ASTEROIDS_HIGHSCORES_H_

#include <string>
#include <vector>

/*****************************************************************************
 * The HighScores class manages/loads and saves high scores
 *****************************************************************************/

class HighScores {
private:
	/* The high scores file path */
	std::string filePath;
	/* The names and scores */
	std::vector<std::string> names;
	std::vector<unsigned int> scores;
	/* The maximum number of high scores stored */
	unsigned int maxHighScores;
public:
	/* The constructor */
	HighScores();

	/* The destructor */
	virtual ~HighScores();

	/* Used to assing the file path */
	inline void setPath(std::string path) { filePath = path; }

	/* Method used to save the high scores to a file */
	void save();

	/* Method used to load the high scores from a file */
	void load();

	/* Method used to add a high score to the table */
	void add(std::string name, unsigned int score);

	/* Method used to check whether a score is a new high score */
	bool isHighScore(unsigned int score);

	/* Method used to check whether a score is on the table of high scores */
	bool isOnTable(unsigned int score);

	/* Getters */
	inline unsigned int getNumHighScores() { return names.size(); }
	inline std::string getName(unsigned int index) { return names[index]; }
	inline unsigned int getScore(unsigned int index) { return scores[index]; }
};

#endif /* EXAMPLES_ASTEROIDS_HIGHSCORES_H_ */
