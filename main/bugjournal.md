# Bug 1

## A) How is your program acting differently than you expect it to?
- For binary file, FileReader::ReadFile() returns contents with only size 3
  instead of 43.

## B) Brainstorm a few possible causes of the bug
- The read_size argument passed to ReadFileToString() could be incorrect
- Might be some special case for binary files not unhandled in ReadFileToString()
- ReadFileToString() returns correct output but could be something wrong when constructing the resulting string

## C) How you fixed the bug and why the fix was necessary
- Changed the second argument to string constructor from strlen(read_content) to the output argument read_size
  we got from ReadFileToString(). There is very likely some bytes in the file string that got interpreted
  as an end-string character, so we cannot use strlen() to determine the length of data, and instead just use
  the output argument that tells how much data we read.


# Bug 2

## A) How is your program acting differently than you expect it to?
- TestHttpConnetionBasic() causes segafult

## B) Brainstorm a few possible causes of the bug
- We are not parsing the request correctly. Maybe not extracting the URI the way we're supposed to?
- If we are extracting the URI correctly, we are probably not handling the buffer_ code in GetNextRequest() properly.
- we are not handling the " Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
next time the caller invokes GetNextRequest()!" part corretly

## C) How you fixed the bug and why the fix was necessary
- gdb wasn't particularly helpful but we figured out we were using substr() incorrectly. Substr(x) means sub string from x till the end instead of from 0 till x as we thought. We fixed this bug and made some progress (before encoutring another)


# Bug 3

## A) How is your program acting differently than you expect it to?
- TestHttpUtilsIsPathSafeIntegration() returns false when it should return true

## B) Brainstorm a few possible causes of the bug
- IsPathSafe has an error
- we are not storing the dir path/ file path correctly
- we are testing for something we shouldn't be testing for (?)

## C) How you fixed the bug and why the fix was necessary
- turns out, I'd been running the tests incorrectly the whole quarter. I used to cd to tests and then run the test suite. The "." and ".." are relative to the terminal and not the executable. I thought the unit test was incorrect because "./../hw4/tests" = "hw4/hw4/tests" since "." would equate to the tests folder where the test suite executable is located.
