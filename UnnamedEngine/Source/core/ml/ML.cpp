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

#include "ML.h"

#include <iostream>
#include <fstream>

#include "../../utils/Logging.h"

/*****************************************************************************
 * The MLAttribute class
 *****************************************************************************/

MLAttribute::MLAttribute() {

}

MLAttribute::MLAttribute(std::string name, std::string data) {
	this->name = name;
	this->data = data;
}

/*****************************************************************************
 * The MLElement class
 *****************************************************************************/

MLElement::MLElement() {

}

MLElement::MLElement(std::string name) {
	this->name = name;
}

/*****************************************************************************
 * The MLDocument class
 *****************************************************************************/

MLDocument::MLDocument() {

}

MLDocument::MLDocument(MLElement root) {
	this->root = root;
}


void MLDocument::load(std::string path) {
	//The input stream for the file
	std::ifstream input;

	//Open the file
	input.open(path.c_str());

	//Ensure the file was opened successfully
	if (input.is_open()) {
		//The current line
		std::string line;
		//The parser
		MLParser parser;
		//Go though each line
		while (getline(input, line))
			//Parse the current line
			parser.parse(line);
		//Close the file
		input.close();
		//Assign the root element of this document
		root = parser.getDocumentRoot();
	} else
		Logger::log("Couldn't open the file with the path '" + path + "'", "MLDocument", Logger::Error);
}

void MLDocument::printData(MLElement& element, std::string prefix) {
	std::cout << prefix + "Element: " + element.getName() << std::endl;
	for (MLAttribute& attrib : element.getAttributes())
		std::cout << prefix + "\tAttribute: " + attrib.getName() + "     " + attrib.getData() << std::endl;
	for (MLElement& child : element.getChildren())
		printData(child, prefix + "\t");
}

/*****************************************************************************
 * The MLParser class
 *****************************************************************************/

MLParser::MLParser() {

}

void MLParser::parse(std::string line) {
	//Try and locate the start of a tag
	std::size_t startLoc = line.find("<");
	//Ensure the sequence has been found
	if (startLoc != std::string::npos) {

		//Locate the end of the current tag
		std::size_t endLoc = line.find(">");

		//Ensure the end was found
		if (endLoc != std::string::npos) {
			//Now extract the tag
			std::string tag = line.substr(startLoc + 1, endLoc - (startLoc + 1));
			//Now extract any remaining data within the line
			std::string remaining = line.substr(endLoc + 1);

			//std::cout << tag << std::endl;

			//Check for a comment
			if (! StrUtils::strStartsWith(tag, "!--")) {
				//Ensure the current tag is not closing a previous  one
				if (! StrUtils::strStartsWith(tag, "/")) {
					//A new element has been declared so parse it
					MLElement element = parseElement(tag);

					//Add the element onto the currently opened elements
					elements.push_back(element);
				} else {
					//An old element has been closed
					//Get it's name
					std::string name = tag.substr(1);
					//Remove the last element, but ensure it is the intended one
					//otherwise print a warning message
					if (elements.at(elements.size() - 1).getName() == name) {
						//Check the size of the array
						if (elements.size() > 1) {
							//Add the last element, to the one before it
							elements.at(elements.size() - 2).add(elements.at(elements.size() - 1));
							//Remove the last element - (as long as it is not the root)
							elements.pop_back();
						}
					} else
						Logger::log("Element '" + name + "' cannot be closed as another element is still open", "MLDocument", Logger::Warning);
				}
			}


			//Check whether anything else is on the same line
			if (remaining.size() > 0)
				//Parse the remaining text
				parse(remaining);
		}
	}
}

MLElement MLParser::parseElement(std::string line) {
	//The element
	MLElement element;
	//Locate a space (if there are any attributes this is the end of the element name)
	std::size_t nameEndLoc = line.find(" ");
	//Assign the name
	element.setName(line.substr(0, nameEndLoc));
	//Now check for any attributes in the element declaration
	if (nameEndLoc != std::string::npos) {
		//Get a string containing all of the attribute declarations
		std::string attributes = line.substr(nameEndLoc + 1);

		//Go through each attribute
		while (attributes.length() > 0) {
			//Find the end of the current attribute
			std::size_t endOfAttrib = attributes.find(" ");
			//Get the current attribute declaration
			std::string currentAttrib = attributes.substr(0, endOfAttrib);

			//At this point currentAttrib will contain a single attribute
			//declaration e.g. name="Default"

			//Get the location of the = sign
			std::size_t equalLoc = currentAttrib.find("=");
			//Get the start/end location of the data (substring using these should
			//give data without '"' at the start and end)
			std::size_t dataStartLoc = currentAttrib.find("\"") + 1;
			std::size_t dataEndLoc   = currentAttrib.find("\"", dataStartLoc);
			//Create the attribute
			MLAttribute attribute;
			attribute.setName(currentAttrib.substr(0, equalLoc));
			attribute.setData(currentAttrib.substr(dataStartLoc, dataEndLoc - dataStartLoc));

			//Add the attribute to the element
			element.add(attribute);

			//Re-assign the rest of the attributes
			if (endOfAttrib == std::string::npos)
				attributes = "";
			else
				attributes = attributes.substr(endOfAttrib + 1);
		}
	}
	//Return the element
	return element;
}

MLDocument MLParser::createDocument() {
	return MLDocument(getDocumentRoot());
}

MLElement MLParser::getDocumentRoot() {
	//Check whether the document is valid
	if (elements.size() != 1)
		//More than one root node - possibly left one element open, so print an error
		Logger::log("Invalid document - Can only have one root element (are any elements not closed properly?)", "MLParser", Logger::Error);
	//Return the first element
	return elements.at(0);
}
