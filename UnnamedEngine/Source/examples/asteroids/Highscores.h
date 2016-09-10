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
 * The Highscores class manages/loads and saves highscores
 *****************************************************************************/

class Highscores {
private:
	/* The highscores file path */
	std::string filePath;
	/* The names and scores */
	std::vector<std::string> names;
	std::vector<unsigned int> scores;
	/* The maximum number of highscores stored */
	unsigned int maxHighscores;
public:
	/* The constructor */
	Highscores();

	/* The destructor */
	virtual ~Highscores();

	/* Used to assing the file path */
	inline void setPath(std::string path) { filePath = path; }

	/* Method used to save the highscores to a file */
	void save();

	/* Method used to load the highscores from a file */
	void load();

	/* Method used to add a highscore to the table */
	void add(std::string name, unsigned int score);

	/* Method used to check whether a score is a new highscore */
	bool isHighscore(unsigned int score);

	/* Method used to check whether a score is on the table of highscores */
	bool isOnTable(unsigned int score);

	/* Getters */
	inline unsigned int getNumHighscores() { return names.size(); }
	inline std::string getName(unsigned int index) { return names[index]; }
	inline unsigned int getScore(unsigned int index) { return scores[index]; }
};

#endif /* EXAMPLES_ASTEROIDS_HIGHSCORES_H_ */
