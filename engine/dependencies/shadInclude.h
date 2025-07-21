#pragma once

#include <string>
#include <fstream>
#include <iostream>

//	===========
//	Shadinclude
//	===========
/*
LICENCE
MIT License
Copyright (c) [2018] [Tahar Meijs]
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
INTRODUCTION
The sole purpose of this class is to load a file and extract the text that is in it.
In theory, this class could be used for a variety of text-processing purposes, but
it was initially designed to be used to load shader source code.
USING THIS CLASS
Since the entire class is a static class, you only have to add this single line of
code to your project:
--------------------------------------------------------------------------------------
std::string shaderSource = Shadinclude::load("./path/to/shader.extension");
--------------------------------------------------------------------------------------
This will (recursively) extract the source code from the first shader file.
Now, you might be wondering, what is the point of using your code for something
so trivial as to loading a file and calling the "std::getline()" function on it?
Well, besides loading the shader source code from a single file, the loader also
supports custom keywords that allow you to include external files inside your
shader source code!
PARAMETERS OF THE LOAD FUNCTION
- std::string	path				path to the "main" shader file
- std::string	includeIdentifier		keyword to look for when scanning for files
MISCELLANEOUS
- Author	:	Tahar Meijs
- Date		:	10th - 12th of April 2018
- Language	:	C++ (can easily be converted into other languages)
*/

/*Edited to fit the needs */
class Shadinclude {
public:
	// Return the source code of the complete shader
	static bool load(const std::filesystem::path& filePath, std::string& fullSourceCode, std::string& logOutput, bool isRecursiveCall = false) {
		std::ifstream file(filePath);

		if (!file.is_open()) {
			logOutput = filePath.string();
			return false;
		}

		const std::string includeToken = "#include ";

		std::string lineBuffer;
		while (std::getline(file, lineBuffer)) {
			// Look for the new shader include identifier
			std::size_t pos = lineBuffer.find(includeToken);
			if (pos != std::string::npos) {
				// Remove the "#include " part
				lineBuffer.erase(pos, includeToken.size());

				// Remove any surrounding quotes
				lineBuffer.erase(std::remove(lineBuffer.begin(),
					lineBuffer.end(), '\"'),
					lineBuffer.end());

				// Construct the include path relative to the parent directory of filePath
				std::filesystem::path includePath = filePath.parent_path() / lineBuffer;

				// Recursively load the included file
				std::string tempSource;
				if (!load(includePath, tempSource, logOutput, true)) {
					return false;
				}
				fullSourceCode += tempSource;
				// Do not add this line to the shader source code, as the include
				// path would generate a compilation issue in the final source code
				continue;
			} else {
				// Normal line: just append to the source code
				fullSourceCode += lineBuffer + '\n';
			}
		}

		// Only add the null terminator at the end of the complete file,
		// essentially skipping recursive function calls this way
		if (!isRecursiveCall) {
			fullSourceCode.push_back('\0');
		}

		file.close();

		return true;
	}
};