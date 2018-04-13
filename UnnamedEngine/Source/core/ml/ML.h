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

#ifndef CORE_ML_ML_H_
#define CORE_ML_ML_H_

#include <string>
#include <vector>

#include "../../utils/Utils.h"

/*****************************************************************************
 * The MLAttribute class stores data about an attribute
 *****************************************************************************/

class MLAttribute {
private:
	/* The name and data */
	std::string name;
	std::string data;
public:
	/* The constructor */
	MLAttribute();
	MLAttribute(std::string name, std::string data);

	/* Various methods to convert the data into another data type before
	 * returning it */
	inline unsigned int getDataAsUInt()  { return utils_string::strToUInt(data);  }
	inline int  		getDataAsInt()   { return utils_string::strToInt(data);   }
	inline bool 		getDataAsBool()  { return utils_string::strToBool(data);  }
	inline float 		getDataAsFloat() { return utils_string::strToFloat(data); }

	/* Various setters and getters */
	inline void setName(std::string name) { this->name = name; }
	inline void setData(std::string data) { this->data = data; }
	inline std::string& getName() { return name; }
	inline std::string& getData() { return data; }

	/* Method used to generate the text representation of this attribute */
	std::string toString();
};

/*****************************************************************************
 * The MLElement class stores all data about a tag, including any attributes
 * supplied
 *****************************************************************************/

class MLElement {
private:
	/* The name of the tag */
	std::string name;

	/* The attributes of this tag */
	std::vector<MLAttribute> attributes;

	/* The child tags */
	std::vector<MLElement> children;
public:
	/* The constructor */
	MLElement();
	MLElement(std::string name);

	/* Methods to add attributes/child tags */
	inline void add(MLAttribute attribute) { attributes.push_back(attribute); }
	inline void add(MLElement child) { children.push_back(child); }

	/* Method used to find an element given its name and return the index of the
	 * first occurrence if it exists - otherwise it will return -1 */
	int find(std::string name);

	/* Various setters and getters */
	inline void setName(std::string name) { this->name = name; }
	inline void setChild(unsigned int index, MLElement& element) { children[index] = element; }
	inline std::string& getName() { return name; }
	inline std::vector<MLAttribute>& getAttributes() { return attributes; }
	inline std::vector<MLElement>& getChildren() { return children; }
	inline MLElement& getChild(unsigned int index) { return children[index]; }

	/* Method used to generate the text representation of this element */
	std::string toString(std::string prefix = "");
};

/*****************************************************************************
 * The MLDocument class stores all data in an associated ML document
 *****************************************************************************/

class MLDocument {
private:
	/* The root element */
	MLElement root;
public:
	/* The constructor */
	MLDocument();
	MLDocument(MLElement root);

	/* Method used to read a file and load its contents into this document
	 * instance */
	void load(std::string path);

	/* Method used to write the string representation of this document to a
	 * file */
	void save(std::string path);

	/* Various setters and getters */
	inline void setRoot(MLElement& root) { this->root = root; }
	inline MLElement& getRoot() { return root; }

	/* Method used to generate the text representation of this document */
	std::string toString();
};

/*****************************************************************************
 * The MLParser class is used to parse text to produce an MLDocument
 *****************************************************************************/

class MLParser {
private:
	/* The currently loaded elements that are still open, so are being
	 * added to */
	std::vector<MLElement> elements;

	/* States whether the parser is currently parsing a comment */
	bool inComment;
public:
	/* The constructor */
	MLParser();

	/* Method used to parse a line of text */
	void parse(std::string text);

	/* Method used to parse an element */
	MLElement parseElement(std::string line);

	/* Method used to return a document */
	MLDocument createDocument();

	/* Method used to a return a document's root element, after the finishing
	 * the parsing */
	MLElement getDocumentRoot();
};

#endif /* CORE_ML_ML_H_ */
