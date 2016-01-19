#lang slideshow

(require "codify.rkt")

(codify #:size 20 "
int main ( void )  
{
    return 0;
}
")

(slide 
(codify "
static inline char *duplicateStringValue(const char *value,
                                         unsigned int length = unknown) {
  if (length == unknown)
    length = (unsigned int)strlen(value);

  // Avoid an integer overflow in the call to malloc below by limiting length lenth.....
  // to a sane value.
  if (length >= (unsigned)Value::maxInt)
    length = Value::maxInt - 1;

  char *newString = static_cast<char *>(malloc(length + 1));
  JSON_ASSERT_MESSAGE(newString != 0,
                      \"in Json::Value::duplicateStringValue(): \"
                      \"Failed to allocate string value buffer\");
  memcpy(newString, value, length);
  newString[length] = 0;
  return newString;
}
")
)



