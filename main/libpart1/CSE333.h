/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef HW1_PUBLIC_CSE333_H_
#define HW1_PUBLIC_CSE333_H_

// A wrapper for assert that permits side-effects within the `Verify333()`
// statement.
// Reference: http://www.acm.uiuc.edu/sigops/roll_your_own/2.a.html
void VerificationFailure(const char* exp, const char* file,
                         const char* basefile, int line);

// `Verify333` is the function used like `exit` to stop execution and
// indicate failure to users
#define Verify333(exp) if (!(exp)) \
  VerificationFailure(#exp, __FILE__, __BASE_FILE__, __LINE__)

#endif  // HW1_PUBLIC_CSE333_H_
